project('poisson_solver', ['c', 'cpp'],
        default_options: ['c_std=c17',])

src = [
  'src/FastCosineTransform.cpp',
  'src/PoissonProblem.cpp',
  'src/BartelsStewart.cpp',
  'src/ChebyshevDifferentiate.cpp',
]

pymod = import('python')
py = pymod.find_installation('python3')
py.extension_module('psm',
                    sources: src,
                    include_directories: ['/usr/include/eigen3'],
                    dependencies: dependency('python3'))
