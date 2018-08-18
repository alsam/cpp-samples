+ HOWTO build'n'run

```sh
    meson build
    pushd build
    ninja
    popd
    build/mandel
```
+ output

```sh
    ⏲  start timer [render with opencl]
    │   ☷  OpenCL initialisation ...
    │   │  Using platform: NVIDIA CUDA
    │   │  Using device:   GeForce GTX 1060 6GB
    │   ┕ (ok)
    │   
    render time without overhead
    start:    1527009593236805600
    end:      1527009593238276064
    duration: 1.47046 ms
    
    ┕ stop timer [render with opencl]: 105.174 ms

<<< writing image to julia3.png ... >>> (ok)
```

+ HOWTO render this document to `README.pdf`

```sh
    pandoc  --pdf-engine=xelatex README.md -o README.pdf
```

