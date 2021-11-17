#include <CL/sycl.hpp>
#include <array>
#include <vector>
#include <iostream>
#include <algorithm>
#include <cmath>

// dpcpp simple2.cpp -o simple2

using namespace sycl;
using namespace sycl::ONEAPI;

void histogramCPU(unsigned int* input, unsigned int* res, unsigned int n)
{
    for (unsigned i=0; i<n; ++i) {
        res[input[i]]++;
    }
}

int main()
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
  }).wait();

  // copy back
  host_accessor h_result_back{d_result};

  histogramCPU(h_input.data(), h_result.data(), h_result.size());

  for (size_t i = 0; i < h_result_back.size(); ++i) {
    std::cout << "d_result[" << i << "] = " << h_result_back[i] << "\n";
  }

  for (size_t i = 0; i < h_result_back.size(); ++i) {
    std::cout << "h_result[" << i << "] = " << h_result[i] << "\n";
  }

  auto compare_vals = [](std::string const& label,
                         auto const& arr1,
                         auto const& arr2) {
         std::cout << "checking values: " << label << std::endl;
         size_t mismatches = 0;
         for (size_t i = 0; i < arr1.size(); ++i) {
             if (arr1[i] != arr2[i]) {
                 ++mismatches;                                   
                 std::cout << "mismatch for index: " << i << " got:" << arr1[i] << " expected: " << arr2[i] << std::endl;
             }
         }
         std::cout << mismatches << " mismatches for " << label << std::endl;

  };
  compare_vals("h_result vs. d_result", h_result, h_result_back);

  return 0;
}
