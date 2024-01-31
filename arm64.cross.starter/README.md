see [Lightweight C++ command line option parser](https://github.com/jarro2783/cxxopts) for modern c++-11 one-header command line parser.

```shell
# sudo ln -s /usr/aarch64-linux-gnu/lib/ld-linux-aarch64.so.1 /lib/ld-linux-aarch64.so.1
# https://stackoverflow.com/questions/64547306/cannot-open-lib-ld-linux-aarch64-so-1-in-qemu-or-gem5

export LD_LIBRARY_PATH=/usr/aarch64-linux-gnu/lib64/:/usr/aarch64-linux-gnu/lib/
meson build --cross-file cross_file.txt
pushd build
ninja
qemu-aarch64 ./greenfield
qemu-aarch64 ./greenfield -d
qemu-aarch64 ./greenfield --gpu 900
g++ -pthread -DCXXOPTS_NO_EXCEPTIONS=1 -I. -g greenfield.cpp -o greenfield
```
