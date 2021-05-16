#include "Halide.h"
#include "halide_image_io.h"
#include "imageops.h"
using namespace Halide;

int main(int argc, char **argv)
{

    // representada como 3 dimensões: x,y,c, onde c varia de 0 até 2, representando cada canal
    // BGR
    Buffer<uint8_t> input = Halide::Tools::load_image("images/Gramado_72k.jpg");
    Func input_fn;
    auto output = Buffer<uint8_t>(input.width(), input.height(), 3);
    auto equalized = histogram_equalization(input);
    equalized.realize(output);

    Halide::Tools::save_image(output, "equalized.png");
    Buffer<uint8_t> equalized_img = Halide::Tools::load_image("equalized.png");

    auto filtered = filter_3x3(input);
    filtered.realize(output);
    Halide::Tools::save_image(output, "filtered.png");

    auto hist = normalized_histogram(equalized_img);
    auto hist_image = histogram_image(hist, equalized_img);

    Halide::Tools::save_image(hist_image, "hist.png");

    Var x("x"), y("y"), c("c");
    Func grayscale("grayscale");

    grayscale(x, y, c) = to_grayscale(input, x, y);

    Var c_inner("c_inner"), c_outer("c_outer");
    Var x_inner("x_inner"), x_outer("x_outer");
    Var y_inner("y_inner"), y_outer("y_outer");
    Var tile_index("tile_index");

    grayscale.split(y, y_outer, y_inner, 4);
    grayscale.split(x, x_outer, x_inner, 4);
    grayscale.reorder(c, x_inner, y_inner, x_outer, y_outer);
    grayscale.fuse(y_outer, x_outer, tile_index);
    grayscale.parallel(tile_index);
    grayscale.print_loop_nest();
    grayscale.realize(output);
    Halide::Tools::save_image(output, "gray.png");

    return 0;
}
