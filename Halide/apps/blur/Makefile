include ../support/Makefile.inc

auto: clean_auto test_auto

man3: test_man3

halide_blur: halide_blur.cpp
	$(CXX) $(CXXFLAGS) halide_blur.cpp $(LIB_HALIDE) -o halide_blur $(LLVM_LDFLAGS)

halide_blur_ref.o: halide_blur
	./halide_blur 0

halide_blur_auto.o: halide_blur
	./halide_blur -1

halide_blur_man1.o: halide_blur
	HL_MAN_TYPE=1 ./halide_blur 1

halide_blur_man2.o: halide_blur
	HL_MAN_TYPE=2 ./halide_blur 2

halide_blur_man3.o: halide_blur
	HL_MAN_TYPE=3 ./halide_blur 3 512

halide_blur_naive.o: halide_blur
	HL_AUTO_NAIVE=1 ./halide_blur -1

halide_blur_naive_gpu.o: halide_blur
	HL_AUTO_NAIVE=1 HL_AUTO_GPU=1 ./halide_blur -2

halide_blur_auto_gpu.o: halide_blur
	HL_AUTO_GPU=1 ./halide_blur -2

halide_blur_sweep.o: halide_blur
	HL_AUTO_SWEEP=1 HL_AUTO_PARALLELISM=12 HL_AUTO_VEC_LEN=16 HL_AUTO_BALANCE=10 HL_AUTO_FAST_MEM_SIZE=262144 ./halide_blur -1

halide_blur_rand.o: halide_blur
	HL_AUTO_RAND=1 HL_AUTO_PARALLELISM=12 HL_AUTO_VEC_LEN=16 HL_AUTO_BALANCE=40 HL_AUTO_FAST_MEM_SIZE=131072 ./halide_blur -1
	#HL_AUTO_RAND=1 HL_AUTO_PARALLELISM=16 HL_AUTO_VEC_LEN=8 HL_AUTO_BALANCE=20 HL_AUTO_FAST_MEM_SIZE=262144 ./halide_blur -1

test_%: test.cpp halide_blur_%.o
	$(CXX) $(CXXFLAGS) $^ -fopenmp -o $@ -march=native -lpthread -ldl $(PNGFLAGS)

clean: clean_bench
	rm -f test halide_blur halide_blur.h halide_blur*.o test_ref \
    test_auto test_sweep test_naive test_rand test_auto_gpu test_naive_gpu
