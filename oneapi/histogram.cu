#include <iostream>
#include <sstream>
#include <cmath>
#include <algorithm>
#include <vector>
#include <chrono>
#include <type_traits>
#include <thrust/device_vector.h>
#include <thrust/host_vector.h>
#include <thrust/device_ptr.h>

class GpuTimer
{
    cudaEvent_t start;
    cudaEvent_t stop;

public:
    GpuTimer()
    {
        cudaEventCreate(&start);
        cudaEventCreate(&stop);
    }

    ~GpuTimer()
    {
        cudaEventDestroy(start);
        cudaEventDestroy(stop);
    }

    void Start()
    {
        cudaEventRecord(start, 0);
    }

    void Stop()
    {
        cudaEventRecord(stop, 0);
    }

    float Elapsed()
    {
        float elapsed;
        cudaEventSynchronize(stop);
        cudaEventElapsedTime(&elapsed, start, stop);
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

__global__ void histogramGPUGlobalMem(unsigned *input, unsigned *res, unsigned n)
{
    // Calculate global thread ID
    int tid = blockIdx.x * blockDim.x + threadIdx.x;

    for (size_t i = tid; i < n; i += (gridDim.x * blockDim.x)) {
        atomicAdd(&res[input[i]], 1);
    }
}

__global__ void histogramGPUSharedMem(unsigned *input, unsigned *res, unsigned n)
{
    // Calculate global thread ID
    int tid = blockIdx.x * blockDim.x + threadIdx.x;
  
    // Allocate a local histogram for each TB
    __shared__ int s_res[NUM_BINS];

    // Initalize the shared memory to 0
    if (threadIdx.x < NUM_BINS) {
        s_res[threadIdx.x] = 0;
    }

    // Wait for shared memory writes to complete
    __syncthreads();

    for (size_t i = tid; i < n; i += (gridDim.x * blockDim.x)) {
        atomicAdd(&s_res[input[i]], 1);
    }

    // Wait for shared memory writes to complete
    __syncthreads();

    // Combine the partial results
    if (threadIdx.x < NUM_BINS) {
        atomicAdd(&res[threadIdx.x], s_res[threadIdx.x]);
    }
}

int main()
{
    unsigned N = 1 << 16;

    std::vector<unsigned> h_input(N);
    std::vector<unsigned> h_result(NUM_BINS);
    thrust::host_vector<unsigned> h_result_copy(NUM_BINS);

    std::srand(42);
    std::generate(std::begin(h_input), std::end(h_input), [] { return  (unsigned)rand() % NUM_BINS; } );

    CPUTimer<> cpu_timer;
    histogramCPU(h_input.data(), h_result.data(), h_result.size());
    cpu_timer.stop("histogramCPU");

    thrust::device_vector<unsigned> d_input(h_input);
    thrust::device_vector<unsigned> d_result(h_result_copy);
    thrust::device_vector<unsigned> d_result2(h_result_copy);

    // Number of threads per threadblock
    size_t THREADS = 512;

    // Calculate the number of threadblocks
    size_t BLOCKS = N / THREADS;

    GpuTimer timer;
    timer.Start();
    CPUTimer<> cpu_timer1;
    histogramGPUGlobalMem<<<BLOCKS, THREADS>>>(thrust::raw_pointer_cast(d_input.data()),
                                               thrust::raw_pointer_cast(d_result.data()),
                                               d_result.size());
    timer.Stop();
    cpu_timer1.stop("histogramGPUGlobalMem");

    std::cout << "elapsed for histogramGPUGlobalMem: " << timer.Elapsed() << " millisecs\n";

    timer.Start();
    CPUTimer<> cpu_timer2;
    histogramGPUSharedMem<<<BLOCKS, THREADS>>>(thrust::raw_pointer_cast(d_input.data()),
                                               thrust::raw_pointer_cast(d_result2.data()),
                                               d_result2.size());
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
    h_result_copy = d_result;

    compare_vals("h_result vs. d_result", h_result, h_result_copy);
    // copy back
    h_result_copy = d_result2;

    compare_vals("h_result vs. d_result2", h_result, h_result_copy);

}
