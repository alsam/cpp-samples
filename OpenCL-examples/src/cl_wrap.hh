#pragma once

#include "cl-util/get_gpu_context.hh"
#include "cl-util/compile.hh"

namespace cl_tutorial
{

    class OpenCL
    {
        public:
            cl::Context context;
            std::vector<cl::Device> devices;
            cl::Program program;
            cl::CommandQueue queue;
    
            OpenCL &use_default_gpu_context()
            {
                std::tie(devices, context) = get_default_gpu_context();
                queue =
                    cl::CommandQueue(
                    context, devices[0], CL_QUEUE_PROFILING_ENABLE);
                return *this;
            }
    
            template <typename Span>
            OpenCL &build_program_from_sources(
                Span const &source_file_list,
                std::string const &options = "")
            {
                cl::Program::Sources sources;
                std::transform(
                    source_file_list.begin(), source_file_list.end(),
                    std::back_inserter(sources), load_source);
                program = cl::Program(context, sources);
    
                auto err_code = program.build(devices, options.c_str());
                checkErr(
                    err_code,
                    program.getBuildInfo<CL_PROGRAM_BUILD_LOG>(devices[2]));
    
                return *this;
            }
    
            template <typename ...Args>
            cl::KernelFunctor<Args...> kernel(std::string const &name)
            {
                int err_code;
                cl::Kernel k(program, name.c_str(), &err_code);
                checkErr(err_code, "at kernel construction: ", name);
                return cl::KernelFunctor<Args...>(k);
            }
    
            OpenCL() = default;
        private:
            OpenCL(OpenCL const &OpenCL) = delete;
            OpenCL &operator=(OpenCL const &other) = delete;
    };
}

