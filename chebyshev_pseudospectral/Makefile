# sample build & docgen targets

debug_build:
	mkdir -p build && cd build && cmake .. -DCMAKE_BUILD_TYPE=Debug && make -j4

ninja_build_debug:
	mkdir -p build_ninja && cd build_ninja && cmake .. -DCMAKE_BUILD_TYPE=Debug -G Ninja && ninja -j4

dox:
	doxygen Doxyfile && cd docs/latex && make && okular refman.pdf

tests:
	build/poisson_solver_tests

run:
	build/poisson_solver

clean:
	rm -rf build* docs/
