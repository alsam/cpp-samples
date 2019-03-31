#include <stdio.h>
#include <iostream>
#include <cuda_runtime.h>
#include <string>

#define THREADBLOCK_SIZE 128
#define WORKING_SET_SIZE_ELEM_BITS 21
#define WORKING_SET_SIZE_ELEMS (1 << WORKING_SET_SIZE_ELEM_BITS)
#define ITERATION_COUNT 5

// FNV-1a released into public domain
#define INITIAL_HASH 14695981039346656037ULL
__forceinline__ __device__ unsigned long long hash(unsigned long long current_hash, int val)
{
   return (current_hash ^ (unsigned long long)val) * 1099511628211ULL;
}

template<bool write>
__launch_bounds__(THREADBLOCK_SIZE) 
__global__ void
access_random(float * in_buf, int coalescing_mask)
{
	int elem_id = blockIdx.x * THREADBLOCK_SIZE + threadIdx.x;
	int base_read_elem_id = elem_id & coalescing_mask;
	int thread_local_id = base_read_elem_id - elem_id;

	float res = 0.0F;
	unsigned long long current_hash = INITIAL_HASH;
	for(int i = 0; i < ITERATION_COUNT; ++i)
	{
		current_hash = hash(current_hash, base_read_elem_id + i);
		int offset = (current_hash + thread_local_id * 8) & ((1 << WORKING_SET_SIZE_ELEM_BITS) - 1);
		res += __ldg(in_buf + offset);
		if (write)
			in_buf[offset] = 1.0F;
	}
	if (elem_id == res)
		in_buf[elem_id & ((1 << WORKING_SET_SIZE_ELEM_BITS) - 1)] = res;
}

int main(int argc, char *argv[])
{
	float * d_buffer_in;

	int coalescing_mask = ~0;
	int threadblock_count = 65536 * 4;
	std::string mode = "read";

	if (argc > 1)
		coalescing_mask = ~(atol(argv[1]) - 1);
	if (argc > 2)
		mode = argv[2];
	if (argc > 3)
		threadblock_count = atol(argv[3]);

	std::cout << "coalescing mask = " << coalescing_mask << ", threadblock_count = " << threadblock_count
		<< ", mode = " << mode << std::endl;
	cudaMalloc((void **)&d_buffer_in, sizeof(float) * WORKING_SET_SIZE_ELEMS);

	if (mode == "read")
		access_random<false><<<threadblock_count, THREADBLOCK_SIZE>>>(d_buffer_in, coalescing_mask);
	else if (mode == "write")
		access_random<true><<<threadblock_count, THREADBLOCK_SIZE>>>(d_buffer_in, coalescing_mask);
	cudaDeviceSynchronize();

	cudaFree(d_buffer_in);

	cudaDeviceReset();

	return 0;
}

