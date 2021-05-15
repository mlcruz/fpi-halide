#include "Halide.h"
#include <stdio.h>
#include "imageops.h"

using namespace Halide;

class Filter : public Halide::Generator<Filter>
{
public:
    Input<Buffer<uint8_t>> input{"input", 3};
    Output<Buffer<uint8_t>> output{"output", 3};
    Var x, y, c;

    // We then define a method that constructs and return the Halide
    // pipeline:
    void generate()
    {
        Func filter_3x3("filter_3x3");
        Func src_int;

        Func clamped = BoundaryConditions::repeat_edge(input);
        src_int(x, y, c) = cast<int>(clamped(x, y, c));

        // -1, -1 => 0; 0, -1 => 1; 1, -1 -> 2;
        int k0 = 1, k1 = 1, k2 = 1, k3 = 1, k4 = 1, k5 = 1, k6 = 1, k7 = 1, k8 = 1;
        int w = (k0 + k1 + k2 + k3 + k4 + k5 + k6 + k7 + k8);

        Expr expr = src_int(x - 1, y - 1, c) * k0 + src_int(x, y - 1, c) * k1 + src_int(x + 1, y - 1, c) * k2 +
                    src_int(x - 1, y, c) * k3 + src_int(x, y, c) * k4 + src_int(x + 1, y, c) * k5 +
                    src_int(x - 1, y + 1, c) * k6 + src_int(x, y + 1, c) * k7 + src_int(x + 1, y + 1, c) * k8;

        output(x, y, c) = clamp(expr / w);

        input.dim(0).set_stride(3); // stride in dimension 0 (x) is three
        input.dim(2).set_stride(1); // stride in dimension 2 (c) is one

        output.dim(0).set_stride(3); // stride in dimension 0 (x) is three
        output.dim(2).set_stride(1); // stride in dimension 2 (c) is one
    }
};

HALIDE_REGISTER_GENERATOR(Filter, filter)