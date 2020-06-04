see [Lightweight C++ command line option parser](https://github.com/jarro2783/cxxopts) for modern c++-11 one-header command line parser.

```shell
export LD_LIBRARY_PATH=/usr/aarch64-linux-gnu/lib64/:/usr/aarch64-linux-gnu/lib/
meson build --cross-file cross_file.txt
pushd build
ninja
qemu-aarch64 ./greenfield
qemu-aarch64 ./greenfield -d
qemu-aarch64 ./greenfield --gpu 900
g++ -pthread -DCXXOPTS_NO_EXCEPTIONS=1 -I. -g greenfield.cpp -o greenfield
```
