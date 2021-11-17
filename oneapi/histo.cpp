#include <CL/sycl.hpp>
#include <array>
#include <vector>
#include <iostream>
#include <algorithm>
#include <cmath>

// dpcpp simple2.cpp -o simple2

using namespace sycl;
using namespace sycl::ONEAPI;

int main() {

  constexpr unsigned N = 256;
  constexpr unsigned M = 16;

  // Create queue on implementation-chosen default device
  queue Q{gpu_selector{}};

  unsigned* data = malloc_shared<unsigned>(N, Q);
  std::fill(data, data + N, 0);

  // Launch exactly one work-group
  // Number of work-groups = global / local
  range<1> global{N};
  range<1> local{N};

  Q.parallel_for(nd_range<1>{global, local}, [=](nd_item<1> it) {
    int i = it.get_global_id(0);
    int j = i % M;
    for (int round = 0; round < N; ++round) {
      // Allow exactly one work-item update per round
      if (i == round) {
        data[j] += 1;
      }
      it.barrier();
    }
  }).wait();


  std::array<unsigned, N> data2;
  buffer buf{ data2 };
  Q.submit([&](handler& h) {
    atomic_accessor acc(buf, h, relaxed_order, system_scope);
    h.parallel_for(N, [=](id<1> i) {
      int j = i % M;
      acc[j] += 1;
    });
  });

  for (int i = 0; i < N; ++i) {
    std::cout << "data [" << i << "] = " << data[i] << " data2 [" << i << "] = " << data2[i] << "\n";
  }
  
  {
    unsigned N = 1 << 16;
    constexpr unsigned NUM_BINS = 256;
    std::vector<unsigned> h_input(N);
    std::vector<unsigned> h_result(NUM_BINS);
    std::srand(42);
    std::generate(std::begin(h_input), std::end(h_input), [] { return  (unsigned)rand() % NUM_BINS; } );

    // Create queue on implementation-chosen default device
    queue Q{gpu_selector{}};
    std::cout << "Selected device: " <<
      Q.get_device().get_info<info::device::name>() << "\n";
    std::cout << "-> Device vendor: " <<
      Q.get_device().get_info<info::device::vendor>() << "\n";

    buffer d_input{ h_input };
    buffer d_result{ h_result };
    Q.submit([&](handler& h) {
      atomic_accessor acc(d_result, h, relaxed_order, system_scope);
      accessor acc2(d_input, h);
      h.parallel_for(N, [=](id<1> i) {
        unsigned j = acc2[i];
        acc[j] += 1;
      });
    });

    // copy back
    host_accessor h_result_back{d_result};
    for (size_t i = 0; i < h_result_back.size(); ++i) {
      std::cout << "h_result[" << i << "] = " << h_result_back[i] << "\n";
    }

  }

  return 0;
}
