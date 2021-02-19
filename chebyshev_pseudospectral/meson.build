project('poisson_solver', ['c', 'cpp'],
        default_options: ['c_std=c17',])

src = [
  'src/cft.cpp',
  'src/PoissonProblem.cpp',
]

pymod = import('python')
py = pymod.find_installation('python3')
py.extension_module('psm',
                    sources: src,
                    include_directories: ['/usr/include/eigen3'],
                    dependencies: dependency('python3'))
