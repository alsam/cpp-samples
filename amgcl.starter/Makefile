# sample build & docgen targets

debug_build:
	mkdir -p build_debug1 && cd build_debug1 && CC=gcc-10 CXX=g++-10 cmake .. -DFLOAT_TYPE=double -DCMAKE_BUILD_TYPE=Debug -DCUDA_TARGET_ARCH=Turing  -DCUDA_TOOLKIT_ROOT_DIR=/opt/cuda && make -j4

release_build_double:
	mkdir -p build_release_double && cd build_release_double && CC=gcc-10 CXX=g++-10 cmake .. -DFLOAT_TYPE=double -DCMAKE_BUILD_TYPE=Release -DCUDA_TARGET_ARCH=Turing  -DCUDA_TOOLKIT_ROOT_DIR=/opt/cuda && make -j4

release_build_float:
	mkdir -p build_release_float && cd build_release_float && CC=gcc-10 CXX=g++-10 cmake .. -DFLOAT_TYPE=float -DCMAKE_BUILD_TYPE=Release -DCUDA_TARGET_ARCH=Turing  -DCUDA_TOOLKIT_ROOT_DIR=/opt/cuda && make -j4

gen_compile_commands:
	mkdir -p build_compile_commands && cd build_compile_commands && CMAKE_EXPORT_COMPILE_COMMANDS=yes cmake .. -DFLOAT_TYPE=double -DCMAKE_BUILD_TYPE=Debug -DCUDA_TARGET_ARCH=Turing && mv compile_commands.json ..

debug_build0:
	mkdir -p build && cd build && cmake .. -DCMAKE_BUILD_TYPE=Debug && make -j4

ninja_build_debug:
	mkdir -p build_ninja && cd build_ninja && cmake .. -DCMAKE_BUILD_TYPE=Debug -G Ninja && ninja -j4

install_vexcl:
	mkdir -p 3dparty/vexcl/build && cd 3dparty/vexcl/build && cmake .. -DCMAKE_INSTALL_PREFIX=$(HOME)/data/local && make install

dox:
	doxygen Doxyfile && cd docs/latex && make && okular refman.pdf

clean:
	rm -rf build* docs/
