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

        Func histogram("histogram");
        Func grayscale("grayscale");
        Func hist_cum("hist cum");

        auto alpha = 255.0f / (input.width() * input.height() / 3);
        grayscale(x, y, c) = to_grayscale(input, x, y);
        auto x_y_domain = RDom(0, input.width(), 0, input.height(), "x_y");

        histogram(h) = 0.0f;
        histogram(grayscale(x_y_domain.x, x_y_domain.y, 0)) += 1.0f;

        hist_cum(h) = alpha * histogram(h);

        auto h_dom = RDom(1, 255, "h");

        hist_cum(h_dom.x) =
            hist_cum(h_dom.x - 1) +
            ((alpha * histogram(h_dom.x)) / 3);

        output(x, y, c) = clamp(hist_cum(input(x, y, c)));

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
