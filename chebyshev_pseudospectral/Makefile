# sample build & docgen targets

debug_build:
	mkdir -p build && cd build && cmake .. -DCMAKE_BUILD_TYPE=Debug && make -j4

ninja_build_debug:
	mkdir -p build_ninja && cd build_ninja && cmake .. -DCMAKE_BUILD_TYPE=Debug -G Ninja && ninja -j4

gen_compile_commands:
	mkdir -p build_compile_commands && cd build_compile_commands && CMAKE_EXPORT_COMPILE_COMMANDS=yes cmake .. && mv compile_commands.json ..

dox:
	doxygen Doxyfile && cd docs/latex && make && okular refman.pdf

tests:
	build/poisson_solver_tests

run:
	build/poisson_solver

# integration mintest
minitest:
	build/poisson_solver --M 4 --N 4 --verbose

clean:
	rm -rf build* docs/
