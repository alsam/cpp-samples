#include <oneapi/dpl/execution>
#include <oneapi/dpl/algorithm>
#include <CL/sycl.hpp>
#include <dpct/dpct.hpp>
#include <iostream>
#include <sstream>
#include <cmath>
#include <algorithm>
#include <vector>
#include <chrono>
#include <type_traits>
#include <dpct/dpl_utils.hpp>
#include <chrono>

//using namespace sycl;
//using namespace sycl::ONEAPI;

class GpuTimer
{
    sycl::event start;
    std::chrono::time_point<std::chrono::steady_clock> start_ct1;
    sycl::event stop;
    std::chrono::time_point<std::chrono::steady_clock> stop_ct1;

public:
    GpuTimer()
    {
        /*
        DPCT1026:0: The call to cudaEventCreate was removed because this call is
        redundant in DPC++.
        */
        /*
        DPCT1026:1: The call to cudaEventCreate was removed because this call is
        redundant in DPC++.
        */
    }

    ~GpuTimer()
    {
        /*
        DPCT1026:2: The call to cudaEventDestroy was removed because this call
        is redundant in DPC++.
        */
        /*
        DPCT1026:3: The call to cudaEventDestroy was removed because this call
        is redundant in DPC++.
        */
    }

    void Start()
    {
        /*
        DPCT1012:4: Detected kernel execution time measurement pattern and
        generated an initial code for time measurements in SYCL. You can change
        the way time is measured depending on your goals.
        */
        start_ct1 = std::chrono::steady_clock::now();
        start = dpct::get_default_queue().submit_barrier();
    }

    void Stop()
    {
        /*
        DPCT1012:5: Detected kernel execution time measurement pattern and
        generated an initial code for time measurements in SYCL. You can change
        the way time is measured depending on your goals.
        */
        stop_ct1 = std::chrono::steady_clock::now();
        stop = dpct::get_default_queue().submit_barrier();
    }

    float Elapsed()
    {
        float elapsed;
        stop.wait_and_throw();
        elapsed = std::chrono::duration<float, std::milli>(stop_ct1 - start_ct1)
                      .count();
        return elapsed;
    }
};

template <class Resolution = std::chrono::microseconds>
class CPUTimer {
public:
    using Clock = std::conditional_t<std::chrono::high_resolution_clock::is_steady,
        std::chrono::high_resolution_clock,
        std::chrono::steady_clock>;

private:
    const Clock::time_point mStart = Clock::now();

public:
    CPUTimer() = default;
    ~CPUTimer() {
        const auto end = Clock::now();
        std::ostringstream strStream;
        // strStream << "Destructor Elapsed: "
        //     << std::chrono::duration_cast<Resolution>(end - mStart).count()
        //     << std::endl;
        // std::cout << strStream.str() << std::endl;
    }

    void stop(std::string const& label) {
        const auto end = Clock::now();
        std::ostringstream strStream;
        strStream << "Stop Elapsed " << label << " : "
            << std::chrono::duration_cast<Resolution>(end - mStart).count()
            << std::endl;
        std::cout << strStream.str() << std::endl;
    }
};

constexpr unsigned NUM_BINS = 256;

void histogramCPU(unsigned int* input, unsigned int* res, unsigned int n)
{
    for (unsigned i=0; i<n; ++i) {
        res[input[i]]++;
    }
}

void histogramGPUGlobalMem(unsigned *input, unsigned *res, unsigned n,
                           sycl::nd_item<3> item_ct1)
{
    // Calculate global thread ID
    int tid = item_ct1.get_group(2) * item_ct1.get_local_range().get(2) +
              item_ct1.get_local_id(2);

    for (size_t i = tid; i < n; i += (item_ct1.get_group_range(2) *
                                      item_ct1.get_local_range().get(2))) {
        /*
        DPCT1039:6: The generated code assumes that "&res[input[i]]" points to
        the global memory address space. If it points to a local memory address
        space, replace "dpct::atomic_fetch_add" with
        "dpct::atomic_fetch_add<unsigned int,
        sycl::access::address_space::local_space>".
        */
        sycl::atomic<unsigned int>(
            sycl::global_ptr<unsigned int>(&res[input[i]]))
            .fetch_add(1);
    }
}

void histogramGPUSharedMem(unsigned *input, unsigned *res, unsigned n,
                           sycl::nd_item<3> item_ct1, int *s_res)
{
    // Calculate global thread ID
    int tid = item_ct1.get_group(2) * item_ct1.get_local_range().get(2) +
              item_ct1.get_local_id(2);

    // Allocate a local histogram for each TB

    // Initalize the shared memory to 0
    if (item_ct1.get_local_id(2) < NUM_BINS) {
        s_res[item_ct1.get_local_id(2)] = 0;
    }

    // Wait for shared memory writes to complete
    /*
    DPCT1065:7: Consider replacing sycl::nd_item::barrier() with
    sycl::nd_item::barrier(sycl::access::fence_space::local_space) for better
    performance if there is no access to global memory.
    */
    item_ct1.barrier();

    for (size_t i = tid; i < n; i += (item_ct1.get_group_range(2) *
                                      item_ct1.get_local_range().get(2))) {
        sycl::atomic<int, sycl::access::address_space::local_space>(
            sycl::local_ptr<int>(&s_res[input[i]]))
            .fetch_add(1);
    }

    // Wait for shared memory writes to complete
    /*
    DPCT1065:8: Consider replacing sycl::nd_item::barrier() with
    sycl::nd_item::barrier(sycl::access::fence_space::local_space) for better
    performance if there is no access to global memory.
    */
    item_ct1.barrier();

    // Combine the partial results
    if (item_ct1.get_local_id(2) < NUM_BINS) {
        /*
        DPCT1039:9: The generated code assumes that "&res[threadIdx.x]" points
        to the global memory address space. If it points to a local memory
        address space, replace "dpct::atomic_fetch_add" with
        "dpct::atomic_fetch_add<unsigned int,
        sycl::access::address_space::local_space>".
        */
        sycl::atomic<unsigned int>(
            sycl::global_ptr<unsigned int>(&res[item_ct1.get_local_id(2)]))
            .fetch_add(s_res[item_ct1.get_local_id(2)]);
    }
}

int main()
{
    dpct::device_ext &dev_ct1 = dpct::get_current_device();
    sycl::queue &q_ct1 = dev_ct1.default_queue();
    //sycl::queue q_ct1{gpu_selector{}};
    std::cout << "Selected device: " <<
    q_ct1.get_device().get_info<sycl::info::device::name>() << "\n";
  std::cout << "-> Device vendor: " <<
    q_ct1.get_device().get_info<sycl::info::device::vendor>() << "\n";

    unsigned N = 1 << 8;

    std::vector<unsigned> h_input(N);
    std::vector<unsigned> h_result(NUM_BINS);
    std::vector<unsigned> h_result_copy(NUM_BINS);

    std::srand(42);
    std::generate(std::begin(h_input), std::end(h_input), [] { return  (unsigned)rand() % NUM_BINS; } );

    CPUTimer<> cpu_timer;
    histogramCPU(h_input.data(), h_result.data(), h_result.size());
    cpu_timer.stop("histogramCPU");

    dpct::device_vector<unsigned> d_input(h_input);
    dpct::device_vector<unsigned> d_result(h_result_copy);
    dpct::device_vector<unsigned> d_result2(h_result_copy);

    // Number of threads per threadblock
    size_t THREADS = 256; //512;

    // Calculate the number of threadblocks
    size_t BLOCKS = N / THREADS;

    GpuTimer timer;
    timer.Start();
    CPUTimer<> cpu_timer1;
    /*
    DPCT1049:10: The workgroup size passed to the SYCL kernel may exceed the
    limit. To get the device limit, query info::device::max_work_group_size.
    Adjust the workgroup size if needed.
    */
    q_ct1.submit([&](sycl::handler &cgh) {
        auto thrust_raw_pointer_cast_d_input_data_ct0 =
            dpct::get_raw_pointer(d_input.data());
        auto thrust_raw_pointer_cast_d_result_data_ct1 =
            dpct::get_raw_pointer(d_result.data());
        auto d_result_size_ct2 = d_result.size();

        cgh.parallel_for(sycl::nd_range<3>(sycl::range<3>(1, 1, BLOCKS) *
                                               sycl::range<3>(1, 1, THREADS),
                                           sycl::range<3>(1, 1, THREADS)),
                         [=](sycl::nd_item<3> item_ct1) {
                             histogramGPUGlobalMem(
                                 thrust_raw_pointer_cast_d_input_data_ct0,
                                 thrust_raw_pointer_cast_d_result_data_ct1,
                                 d_result_size_ct2, item_ct1);
                         });
    });
    timer.Stop();
    cpu_timer1.stop("histogramGPUGlobalMem");

    std::cout << "elapsed for histogramGPUGlobalMem: " << timer.Elapsed() << " millisecs\n";

    timer.Start();
    CPUTimer<> cpu_timer2;
    /*
    DPCT1049:11: The workgroup size passed to the SYCL kernel may exceed the
    limit. To get the device limit, query info::device::max_work_group_size.
    Adjust the workgroup size if needed.
    */
    q_ct1.submit([&](sycl::handler &cgh) {
        sycl::accessor<int, 1, sycl::access_mode::read_write,
                       sycl::access::target::local>
            s_res_acc_ct1(sycl::range<1>(/*256*/ NUM_BINS), cgh);

        auto thrust_raw_pointer_cast_d_input_data_ct0 =
            dpct::get_raw_pointer(d_input.data());
        auto thrust_raw_pointer_cast_d_result2_data_ct1 =
            dpct::get_raw_pointer(d_result2.data());
        auto d_result2_size_ct2 = d_result2.size();

        cgh.parallel_for(sycl::nd_range<3>(sycl::range<3>(1, 1, BLOCKS) *
                                               sycl::range<3>(1, 1, THREADS),
                                           sycl::range<3>(1, 1, THREADS)),
                         [=](sycl::nd_item<3> item_ct1) {
                             histogramGPUSharedMem(
                                 thrust_raw_pointer_cast_d_input_data_ct0,
                                 thrust_raw_pointer_cast_d_result2_data_ct1,
                                 d_result2_size_ct2, item_ct1,
                                 s_res_acc_ct1.get_pointer());
                         });
    });
    timer.Stop();
    cpu_timer2.stop("histogramGPUGlobalMem");
    std::cout << "elapsed for histogramGPUSharedMem: " << timer.Elapsed() << " millisecs\n";

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

    // copy back
    h_result_copy = std::vector<unsigned>(d_result);

    compare_vals("h_result vs. d_result", h_result, h_result_copy);
    // copy back
    h_result_copy = std::vector<unsigned>(d_result2);

    compare_vals("h_result vs. d_result2", h_result, h_result_copy);

}
