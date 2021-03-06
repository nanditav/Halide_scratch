HALIDE_BIN_PATH ?= ../..
HALIDE_SRC_PATH ?= ../..

CXX ?= g++
GXX ?= g++
CXX += -Wall -g

CFLAGS += -I $(HALIDE_BIN_PATH)/include/ -I $(HALIDE_SRC_PATH)/tools/ -I $(HALIDE_SRC_PATH)/apps/support/
CXXFLAGS += -std=c++11 -I $(HALIDE_BIN_PATH)/include/ -I $(HALIDE_SRC_PATH)/tools/ -I $(HALIDE_SRC_PATH)/apps/support/ 

ifeq ($(UNAME), Darwin)
CXXFLAGS += -fvisibility=hidden
endif

LIB_HALIDE = $(HALIDE_BIN_PATH)/lib/libHalide.a

GENERATOR_DEPS ?= $(HALIDE_BIN_PATH)/bin/libHalide.a $(HALIDE_BIN_PATH)/include/Halide.h $(HALIDE_SRC_PATH)/tools/GenGen.cpp

LLVM_CONFIG ?= llvm-config
LLVM_VERSION_TIMES_10 = $(shell $(LLVM_CONFIG) --version | cut -b 1,3)
LLVM_LDFLAGS = $(shell $(LLVM_CONFIG) --ldflags --system-libs)
LLVM_FULL_VERSION = $(shell $(LLVM_CONFIG) --version)
LLVM_LIBDIR = $(shell $(LLVM_CONFIG) --libdir)

ifeq ($(USE_LLVM_SHARED_LIB), )
LLVM_STATIC_LIBS = -L $(LLVM_LIBDIR) $(shell $(LLVM_CONFIG) --libs bitwriter bitreader linker ipo mcjit $(LLVM_OLD_JIT_COMPONENT) $(X86_LLVM_CONFIG_LIB) $(ARM_LLVM_CONFIG_LIB) $(OPENCL_LLVM_CONFIG_LIB) $(NATIVE_CLIENT_LLVM_CONFIG_LIB) $(PTX_LLVM_CONFIG_LIB) $(AARCH64_LLVM_CONFIG_LIB) $(MIPS_LLVM_CONFIG_LIB))
LLVM_SHARED_LIBS =
else
LLVM_STATIC_LIBS =
LLVM_SHARED_LIBS = -L $(LLVM_LIBDIR) -lLLVM-$(LLVM_FULL_VERSION)
endif


LIBPNG_LIBS_DEFAULT = $(shell libpng-config --ldflags)
LIBPNG_CXX_FLAGS ?= $(shell libpng-config --cflags)
# Workaround for libpng-config pointing to 64-bit versions on linux even when we're building for 32-bit
ifneq (,$(findstring -m32,$(CXX)))
ifneq (,$(findstring x86_64,$(LIBPNG_LIBS_DEFAULT)))
LIBPNG_LIBS ?= -lpng
endif
endif
LIBPNG_LIBS ?= $(LIBPNG_LIBS_DEFAULT)
PNGFLAGS=$(LIBPNG_LIBS) $(LIBPNG_CXX_FLAGS)

UNAME = $(shell uname)

PLATFORM_OPENGL_LDFLAGS=-lGL -lX11
ifeq ($(UNAME), Darwin)
PLATFORM_OPENGL_LDFLAGS=-framework OpenGL
endif

ifneq (, $(findstring opengl,$(HL_TARGET)))
  OPENGL_LDFLAGS=$(PLATFORM_OPENGL_LDFLAGS)
endif

print-%:
	@echo '$*=$($*)'

# BENCHMARKING
.NOTPARALLEL: # important not to parallelize the bench runs accidentally

TESTS ?= test_ref test_auto test_naive test_sweep test_rand test_man1 test_man2 test_man3# overload before include if desired!
THREADS_TO_TEST ?= 1 6 12
REF_BENCHES=$(patsubst %,.bench_ref.%.txt,$(THREADS_TO_TEST))
AUTO_BENCHES=$(patsubst %,.bench_auto.%.txt,$(THREADS_TO_TEST))
SWEEP_BENCHES=$(patsubst %,.bench_sweep.%.txt,$(THREADS_TO_TEST))
RAND_BENCHES=$(patsubst %,.bench_rand.%.txt,$(THREADS_TO_TEST))
NAIVE_BENCHES=$(patsubst %,.bench_naive.%.txt,$(THREADS_TO_TEST))
BENCHES=$(REF_BENCHES) $(AUTO_BENCHES) $(NAIVE_BENCHES) $(SWEEP_BENCHES) $(RAND_BENCHES)

gen: $(TESTS)

cleangen:
	rm -f $(TESTS) *.o $(BENCHES)

clean_%:
	rm -rf *.dSYM*
	rm -f $(patsubst clean_%,*_%*,$@)

clean_variants: clean_auto clean_ref clean_naive clean_sweep clean_rand

regen: cleangen gen

.bench_%.txt: $(TESTS)
	$(eval NTHREADS := $(word 2,$(subst ., ,$@)))
	$(eval VER := $(word 2,$(subst _, ,$(word 1,$(subst ., ,$@)))))
	echo "[$@]" > $@
	echo "version: $(VER)" >> $@
	echo "threads: $(NTHREADS)" >> $@
	./test.sh $(VER) $(NTHREADS) >> $@

ref_perf.txt: $(REF_BENCHES)
	cat $^ > $@
bench_ref: ref_perf.txt
auto_perf.txt: $(AUTO_BENCHES)
	cat $^ > $@
bench_auto: auto_perf.txt
sweep_perf.txt: $(SWEEP_BENCHES)
	cat $^ > $@
bench_sweep: sweep_perf.txt
rand_perf.txt: $(RAND_BENCHES)
	cat $^ > $@
bench_rand: rand_perf.txt
naive_perf.txt: $(NAIVE_BENCHES)
	cat $^ > $@
bench_naive: naive_perf.txt
bench: bench_ref bench_auto bench_naive bench_sweep bench_rand

clean_bench:
	rm -f $(BENCHES) auto_perf.txt ref_perf.txt naive_perf.txt sweep_perf.txt rand_perf.txt
