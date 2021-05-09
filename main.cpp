#include "Halide.h"
// Include some support code for loading pngs.
#include "halide_image_io.h"
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
using namespace Halide;

int main(int argc, char **argv)
{

    Buffer<uint8_t> input = Halide::Tools::load_image("images/Gramado_72k.jpg");

    // representada como 3 dimensões: x,y,c, onde c varia de 0 até 2, representando cada canal
    // BGR
    Var x, y, c;
    Func main_pipeline;
    Expr to_grayscale = input(x, y, 0) * 0.114f + input(x, y, 1) * 0.587f + input(x, y, 1) * 0.299f;
    main_pipeline(x, y, c) = Halide::cast<u_int8_t>(Halide::min(255, to_grayscale));

    Halide::Buffer<uint8_t> output =
        main_pipeline.realize(input.width(), input.height(), input.channels());

    Halide::Tools::save_image(output, "gray.png");
    printf("Success!\n");

    return 0;
}