# sample build & docgen targets

debug_build:
	mkdir -p build && cd build && CC=gcc-10 CXX=g++-10 cmake .. -DCMAKE_BUILD_TYPE=Debug -DCUDA_TARGET_ARCH=Turing  -DCUDA_TOOLKIT_ROOT_DIR=/opt/cuda && make -j4

debug_build0:
	mkdir -p build && cd build && cmake .. -DCMAKE_BUILD_TYPE=Debug && make -j4

ninja_build_debug:
	mkdir -p build_ninja && cd build_ninja && cmake .. -DCMAKE_BUILD_TYPE=Debug -G Ninja && ninja -j4

dox:
	doxygen Doxyfile && cd docs/latex && make && okular refman.pdf

clean:
	rm -rf build* docs/
