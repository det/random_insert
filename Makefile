CFLAGS=-O3 -Wall -Wextra -pedantic -Wno-unused-parameter -std=c++11

all: bench_vector bench_avl_array bench_btree_array

bench_vector: bench_vector.cpp bench.hpp
	${CXX} -o bench_vector bench_vector.cpp ${CFLAGS}

bench_avl_array: bench_avl_array.cpp bench.hpp
	${CXX} -o bench_avl_array bench_avl_array.cpp ${CFLAGS}

bench_btree_array: bench_btree_array.cpp bench.hpp
	${CXX} -o bench_btree_array bench_btree_array.cpp ${CFLAGS}

clean:
	rm -rf bench_vector bench_avl_array bench_btree_array

run: run_vector run_avl_array run_btree_array

run_vector: bench_vector
	perf stat -r3 ./bench_vector 10
	perf stat -r3 ./bench_vector 100
	perf stat -r3 ./bench_vector 1000
	perf stat -r3 ./bench_vector 10000
	perf stat -r3 ./bench_vector 100000
	perf stat -r3 ./bench_vector 1000000

run_avl_array: bench_avl_array
	perf stat -r3 ./bench_avl_array 10
	perf stat -r3 ./bench_avl_array 100
	perf stat -r3 ./bench_avl_array 1000
	perf stat -r3 ./bench_avl_array 10000
	perf stat -r3 ./bench_avl_array 100000
	perf stat -r3 ./bench_avl_array 1000000
	perf stat -r3 ./bench_avl_array 10000000

run_btree_array: bench_btree_array
	perf stat -r3 ./bench_btree_array 10
	perf stat -r3 ./bench_btree_array 100
	perf stat -r3 ./bench_btree_array 1000
	perf stat -r3 ./bench_btree_array 10000
	perf stat -r3 ./bench_btree_array 100000
	perf stat -r3 ./bench_btree_array 1000000
	perf stat -r3 ./bench_btree_array 10000000
	perf stat -r3 ./bench_btree_array 100000000
