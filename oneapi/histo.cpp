#include <CL/sycl.hpp>
#include <array>
#include <iostream>

// dpcpp simple2.cpp -o simple2

using namespace sycl;
using namespace sycl::ONEAPI;

int main() {

  constexpr unsigned N = 256;
  constexpr unsigned M = 16;

  // Create queue on implementation-chosen default device
  queue Q;

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

  return 0;
}
