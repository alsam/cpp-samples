#include <cmath>
#include <algorithm>
#include <tuple>

#include "../base.hh"
#include "../cl_wrap.hh"
#include "../save_png.hh"

#include "../cl-util/get_gpu_context.hh"
#include "../cl-util/compile.hh"
#include "../cl-util/timing.hh"
#include "../cl-util/set_args.hh"

using namespace cl_tutorial;

void render_julia_set(
    unsigned width, unsigned height, unsigned max_it, float *data)
{
    Timer time;
    OpenCL clcx;

    std::vector<std::string> const source_files = {
        "src/kernels/mandel.cl" };
    clcx.use_default_gpu_context()
        .build_program_from_sources(source_files);

    size_t size = width * height;
    size_t mandel_bytesize = size * sizeof(float);
    cl::Buffer mandel_d(clcx.context, CL_MEM_READ_WRITE, mandel_bytesize);

    float res = 2.4f / height;
    float x0 = -res * width / 2.0,
          y0 = -1.2;

    auto julia_k = clcx.kernel<
        cl::Buffer, float, float, float, float, float, unsigned>("julia");

    cl::EnqueueArgs julia_args(clcx.queue, cl::NDRange(width, height), cl::NullRange);

    auto julia_event = julia_k(
        julia_args,
        mandel_d, x0, y0, res, -0.03f, 0.7f, max_it);

    std::vector<cl::Event> wait_list = {
        julia_event };

    clcx.queue.enqueueReadBuffer(mandel_d, CL_TRUE, 0, mandel_bytesize,
        data, &wait_list, NULL);
    clcx.queue.finish();

    //print_runtime_msg(get_runtime(julia_event), "render time without overhead");
}

int main(int argc, char **argv)
{
    Timer time;

    size_t width = 3840, height = 2160;
    size_t size = width * height;

    std::vector<float> mandel_h(size);

    std::fill(mandel_h.begin(), mandel_h.end(), 0.0);
    time.start("render with opencl");
    render_julia_set(width, height, 512, mandel_h.data());
    time.stop();

    save_png("julia3.png", width, height, mandel_h, colour_map::rainbow);
}

