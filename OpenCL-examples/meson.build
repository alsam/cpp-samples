project('OpenCL-tutorial', 'cpp')
subdir('src')

opencl_dep = dependency('OpenCL')
libpng_dep = dependency('libpng')
#libyuv_dep = dependency('libyuv')

local_include = include_directories('./include')

cl_util_static = static_library('cl-util', cl_util_files,
    cpp_args: ['-std=c++17', '-g'], 
    dependencies: opencl_dep,
    include_directories: local_include)

executable('cl-info', cl_info_files,
    cpp_args: ['-std=c++17', '-g'],
    dependencies : opencl_dep,
    link_with: [cl_util_static],
    include_directories: local_include)

executable('mandel', mandel_files,
    dependencies : [opencl_dep, libpng_dep],
    cpp_args: ['-std=c++17', '-g'],
    link_args: ['-g'],
    link_with: [cl_util_static],
    include_directories: local_include)

#executable('yuv2rgb', yuv2rgb_files,
#    dependencies : [opencl_dep, libpng_dep, libyuv_dep],
#    cpp_args: ['-std=c++17', '-g'],
#    link_args: ['-g'],
#    link_with: [cl_util_static],
#    include_directories: local_include)
