#include <cstdio>
#include <cstdlib>
#include <cassert>

#include "harris.h"

#include "benchmark.h"
#include "halide_image.h"
#include "halide_image_io.h"

using namespace Halide::Tools;

int main(int argc, char **argv) {

    Image<float> input = load_image(argv[1]);
    Image<float> output(input.width()-6, input.height()-6);

    harris(input, output);

    // Timing code. Timing doesn't include copying the input data to
    // the gpu or copying the output back.
    double min_t = benchmark(10, 10, [&]() {
        harris(input, output);
    }, [&]() {output.device_sync();});
    printf("runtime: %g\n", min_t * 1e3);

    //save_image(output, argv[2]);

    return 0;
}
