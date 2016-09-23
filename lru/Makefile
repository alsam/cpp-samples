all: bitops_test lru_test

bitops_test: bitops.cpp bitops.hpp
	g++ -g -std=c++11 -DBIT_SLICE_TEST bitops.cpp -o bitops_test

lru_test: cache.cpp cache.hpp bitops.hpp
	g++ -g -std=c++11 -DCACHE_TEST cache.cpp -o lru_test

clean:
	rm -rf bitops_test lru_test *.o
