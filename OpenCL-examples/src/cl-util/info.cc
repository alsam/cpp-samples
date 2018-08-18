#include "info.hh"

using namespace cl_tutorial;

/*! \brief Prints some information on the available OpenCL runtime.
 */
void cl_tutorial::print_opencl_info(
    std::vector<cl::Platform> const &platform_list)
{
    console.msg("Number of platforms: ", platform_list.size()).msg();

    for (unsigned i = 0; i < platform_list.size(); ++i)
    {
        auto const &platform = platform_list[i];
        console
            .msg("== \033[1mPlatform Nº ", i+1, 
			     "\033[m =========================================================")
            .push("  ")
            .msg("Name:      ", platform.getInfo<CL_PLATFORM_NAME>())
            .msg("Vendor:    ", platform.getInfo<CL_PLATFORM_VENDOR>());

        std::vector<cl::Device> device_list;
        platform.getDevices(CL_DEVICE_TYPE_ALL, &device_list);

        console
			.msg("# devices: ", device_list.size())
			.msg();

        for (unsigned j = 0; j < device_list.size(); ++j)
        {
            auto const &device = device_list[j];
            console
				.msg("-- \033[1mDevice Nº ", j+1,
				     "\033[m ---------------------------------------------------------")
				.push("  ")
                .msg("Name:             ", device.getInfo<CL_DEVICE_NAME>())
                .msg("Device version:   ", device.getInfo<CL_DEVICE_VERSION>())
				.msg("Driver version:   ", device.getInfo<CL_DRIVER_VERSION>())
				.msg("OpenCL version:   ", device.getInfo<CL_DEVICE_OPENCL_C_VERSION>())
				.msg("# Compute units:  ", device.getInfo<CL_DEVICE_MAX_COMPUTE_UNITS>())
				.pop();
        }

		console.pop();
    }
}

