project('freq', ['c', 'cpp'],
        version: '0.1',
        meson_version: '>= 0.46',
        default_options: ['cpp_std=c++20',
                          'default_library=static',
                         ])

cc = meson.get_compiler('c')

# add_global_arguments('-DTEST_FREQ_LOCKER=1', language : 'cpp')
add_project_link_arguments(['-pthread', ], language: 'cpp')

dl_dep = cc.find_library('dl', required: false)
fmt_lib = static_library('fmt', sources: ['3dparty/fmt/src/format.cc',
                                          '3dparty/fmt/src/os.cc'],
                         include_directories: ['3dparty/fmt/include'])

src = [
  'src/greenfield.cpp',

]
# add_project_link_arguments(['-pthread', '-static'], language: 'cpp')
exe = executable('sample', sources: src,
                           include_directories: ['3dparty/fmt/include',
                                                 '3dparty/cxxopts/include',],
                           dependencies: [dl_dep],
                           link_with: [fmt_lib])
