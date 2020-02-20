project('greenfield', ['c', 'cpp'],
        version: '0.1',
        meson_version: '>= 0.46',
        default_options: ['c_std=c11',
#                          'default_library=static',
                         ])

cc = meson.get_compiler('c')

add_global_arguments('-DTEST_FREQ_LOCKER=1', language : 'cpp')

src = [
  'greenfield.cpp',

]
# add_project_link_arguments(['-pthread', '-static'], language: 'cpp')
add_project_link_arguments(['-pthread', ], language: 'cpp')
exe = executable('freqlock', sources: src)
