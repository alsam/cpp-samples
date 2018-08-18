#include "../base.hh"
#include "../cl-util/info.hh"

using namespace cl_tutorial;

int main(int argc, char **argv)
{
    std::vector<cl::Platform> platform_list;
    cl::Platform::get(&platform_list); 
    checkErr(platform_list.size() != 0 ? CL_SUCCESS : -1, "cl::Platform::get");
    print_opencl_info(platform_list);
    return EXIT_SUCCESS;
}

