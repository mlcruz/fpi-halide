#include "Halide.h"
#include "halide_image_io.h"
#include "imageops.h"
using namespace Halide;

int main(int argc, char **argv)
{

    // representada como 3 dimensões: x,y,c, onde c varia de 0 até 2, representando cada canal
    // BGR
    Buffer<uint8_t> input = Halide::Tools::load_image("gray.png");

    auto output = Buffer<uint8_t>(input.width(), input.height(), 3);
    auto equalized = histogram_equalization(input);
    equalized.realize(output);

    Halide::Tools::save_image(output, "equalized.png");
    Buffer<uint8_t> equalized_img = Halide::Tools::load_image("equalized.png");

    auto hist = normalized_histogram(equalized_img);
    auto hist_image = histogram_image(hist, equalized_img);

    Halide::Tools::save_image(hist_image, "hist.png");

    // //hist.print_loop_nest();
    // // Halide::Buffer<uint8_t> output =
    //     main_pipeline.realize(input.width(), input.height(), input.channels());

    printf("Success!\n");

    return 0;
}
