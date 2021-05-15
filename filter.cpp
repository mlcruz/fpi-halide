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
    Var h;

    // We then define a method that constructs and return the Halide
    // pipeline:
    void generate()
    {
        Func filter_3x3("filter_3x3");
        Func src_int;

        auto gray = to_grayscale(input, x, y);
        Func histogram("histogram");
        auto x_y_domain = RDom(0, input.width(), 0, input.height(), "x_y");

        output(x, y, c) = gray;

        input.dim(0)
            .set_stride(3);         // stride in dimension 0 (x) is three
        input.dim(2).set_stride(1); // stride in dimension 2 (c) is one

        output.dim(0).set_stride(3); // stride in dimension 0 (x) is three
        output.dim(2).set_stride(1); // stride in dimension 2 (c) is one
    }
};

HALIDE_REGISTER_GENERATOR(Filter, filter)

// Var x_outer("x_outer"), y_outer("y_outer"), x_inner("x_inner"), y_inner("y_inner"), tile_index("tile_index");

// // Processamos parelelamente em blocos de 64x64
// histogram_eq.tile(x, y, x_outer, y_outer, x_inner, y_inner, 64, 64)
//     .fuse(x_outer, y_outer, tile_index)
//     .parallel(tile_index);

// // Separamos cada bloco de 64 em blocos de 4x4
// // Vetorizando o loop externo tamanho 4
// // e realizando unroll no loop interno tamanho 4
// Var x_inner_outer("x_inner_outer"), y_inner_outer("y_inner_outer"), x_vectors("x_vectorts"), y_pairs("y_pairs");
// histogram_eq
//     .tile(x_inner, y_inner, x_inner_outer, y_inner_outer, x_vectors, y_pairs, 4, 4)
//     .vectorize(x_vectors)
//     .unroll(y_pairs);

// histogram_eq.print_loop_nest();
