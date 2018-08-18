#include "get_gpu_context.hh"
#include <iostream>

using namespace cl_tutorial;

std::tuple<std::vector<cl::Device>, cl::Context>
    cl_tutorial::get_default_gpu_context()
{
    console
        .msg("\033[32m☷ \033[m OpenCL initialisation ...")
        .push("\033[32m│\033[m  ");

    std::vector<cl::Platform> platform_list;
    cl::Platform::get(&platform_list);
    checkErr(platform_list.size() != 0 ? CL_SUCCESS : -1,
        " cl::Platform::get failed to retrieve a platform;\n"
        "  is OpenCL correctly installed?"
        "\n(fail)\n");

    cl::Platform default_platform = platform_list[0];
    console.msg(
        "Using platform: ",
        default_platform.getInfo<CL_PLATFORM_NAME>());

    std::vector<cl::Device> device_list;
    //default_platform.getDevices(CL_DEVICE_TYPE_GPU, &device_list);
    default_platform.getDevices(CL_DEVICE_TYPE_ALL, &device_list);
    checkErr(device_list.size() != 0 ? CL_SUCCESS : -1,
        " cl::Platform::getDevices failed to find a GPU device;\n"
        "  do you have the correct device loader installed?"
        "\n(fail)\n");

    std::cout << "found devices: " << device_list.size() << std::endl;
    cl::Device default_device = device_list[2];
    console.msg(
        "Using device:   ",
        default_device.getInfo<CL_DEVICE_NAME>());

    console
        .pop("\033[32m┕\033[m (ok)");
    //cl::Context context({default_device});
    cl::Context context(device_list);
    return std::make_tuple(device_list, context);
}

