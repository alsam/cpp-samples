#include <CL/sycl.hpp>
#include <array>
#include <iostream>
#include <memory>
#include <list>

// dpcpp simple2.cpp -o simple2

using namespace sycl;

template <typename TSelector>
std::unique_ptr<cl::sycl::device> makeDevice()
{
    try
    {
        return std::unique_ptr<cl::sycl::device>(new cl::sycl::device(TSelector()));
    }
    catch (...)
    {
        return std::unique_ptr<cl::sycl::device>();
    }
}

std::list<std::pair<std::string, cl::sycl::device> > getListOfDevices()
{
    std::list<std::pair<std::string, cl::sycl::device> > selects;
    std::unique_ptr<cl::sycl::device> device;

    device = makeDevice<cl::sycl::gpu_selector>();
    if (device) selects.emplace_back("GPU", *device);

    device = makeDevice<cl::sycl::cpu_selector>();
    if (device) selects.emplace_back("CPU", *device);

    device = makeDevice<cl::sycl::host_selector>();
    if (device) selects.emplace_back("HOST", *device);

    return selects;
}

int main() {

  constexpr int size=16;
  std::array<int, size> data;

  for (const auto & deviceSelector : getListOfDevices()) {
    const auto & nameDevice = deviceSelector.first;
    const auto & device     = deviceSelector.second;
    cl::sycl::queue queue(device);
    std::cout << "Running on " << nameDevice << "\n\n";
    std::cout << "device: " <<
    queue.get_device().get_info<info::device::name>() << "\n";

    std::cout << "-> vendor: " <<
    queue.get_device().get_info<info::device::vendor>() << "\n";
  }

  // Create queue on implementation-chosen default device
  queue Q{ gpu_selector{} };

  std::cout << "Selected device: " <<
    Q.get_device().get_info<info::device::name>() << "\n";

  std::cout << "-> Device vendor: " <<
    Q.get_device().get_info<info::device::vendor>() << "\n";

  // Create buffer using host allocated "data" array
  buffer B { data };

  Q.submit([&](handler& h) {
    accessor A{B, h};

    h.parallel_for(size , [=](auto& idx) {
      A[idx] = idx;
    });
  });

  // Obtain access to buffer on the host
  // Will wait for device kernel to execute to generate data
  host_accessor A{B};
  for (int i = 0; i < size; i++)
    std::cout << "data[" << i << "] = " << A[i] << "\n";

  return 0;
}
