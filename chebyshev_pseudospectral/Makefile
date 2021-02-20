# sample build & docgen targets

debug_build:
	mkdir -p build && cd build && cmake .. -DCMAKE_BUILD_TYPE=Debug && make

ninja_build_debug:
	mkdir -p build_ninja && cd build_ninja && cmake .. -DCMAKE_BUILD_TYPE=Debug -G Ninja && ninja

dox:
	doxygen Doxyfile && cd docs/latex && make

clean:
	rm -rf build* docs/
