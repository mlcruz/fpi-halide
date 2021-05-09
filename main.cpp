#include "Halide.h"

// Include some support code for loading pngs.
#include "halide_image_io.h"

int main(int argc, char **argv) {

    Halide::Buffer<uint8_t> input = Halide::Tools::load_image("images/Gramado_72k.jpg");
    Halide::Func brighter;
    Halide::Var x, y, c;
    Halide::Expr value = input(x, y, c);

    value = Halide::cast<float>(value);
    value = value * 1.5f;
    
    //clamp
    value = Halide::min(value, 255.0f);

    // Cast it back to an 8-bit unsigned integer.
    value = Halide::cast<uint8_t>(value);

    // Define the function.
    brighter(x, y, c) = value;

    Halide::Buffer<uint8_t> output =
        brighter.realize(input.width(), input.height(), input.channels());

    //Halide::Tools::save_image(output, "brighter.png");
    printf("Success!\n");
 
    return 0;
}