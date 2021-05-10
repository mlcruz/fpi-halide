#include "Halide.h"
#include "halide_image_io.h"
using namespace Halide;

const float HISTOGRAM_ROWS = 512.0f;

Expr clamp(Expr expr)
{
    return Halide::cast<u_int8_t>(Halide::min(255, expr));
}

Expr adjust_brightness(Buffer<u_int8_t> buf, Var x, Var y, Var c, u_int8_t val)
{
    return clamp(buf(x, y, c) + val);
}

Expr to_grayscale(Buffer<u_int8_t> buf, Var x, Var y)
{
    Expr to_grayscale = buf(x, y, 0) * 0.114f + buf(x, y, 1) * 0.587f + buf(x, y, 2) * 0.299f;
    return clamp(to_grayscale);
}

Func normalized_histogram(Buffer<u_int8_t> src)
{
    float alpha = HISTOGRAM_ROWS / (src.width() * src.height() / 3);
    Var x("x"), y("y"), c("c");
    Var h("h");

    Func grayscale("grayscale");
    Func histogram("histogram");

    // Tons de cinza
    grayscale(x, y, c) = to_grayscale(src, x, y);

    // reduction domain variando em 2d
    auto x_y_domain = RDom(0, src.width(), 0, src.height(), "x_y");

    // Histograma da imagem
    histogram(h) = 0.0f;
    histogram(grayscale(x_y_domain.x, x_y_domain.y, 0)) += 1.0f;
    histogram(h) = round(histogram(h) * alpha);

    return histogram;
}

Buffer<uint8_t> histogram_image(Func histogram, Buffer<u_int8_t> src)
{
    Buffer<uint8_t> result(256, static_cast<int>(HISTOGRAM_ROWS), 3);
    Func histogram_out("histogram_out");

    Var x("x"), y("y"), c("c");
    Var h("h");

    // Imagem do histograma
    Expr out_expr = mux(c,
                        {select(histogram(x) > (static_cast<int>(HISTOGRAM_ROWS) - y), 180, 128),
                         select(histogram(x) > (static_cast<int>(HISTOGRAM_ROWS) - y), 30, 128),
                         select(histogram(x) > (static_cast<int>(HISTOGRAM_ROWS) - y), 100, 128)});

    out_expr = cast<uint8_t>(out_expr);
    histogram_out(x, y, c) = out_expr;

    //Var x_outer, x_inner, y_outer, y_inner, c_outer, c_inner;
    // histogram_out.reorder(y, x, c)
    // histogram_out.tile(x, y, x_outer, y_outer, x_inner, y_inner, 4, 4);
    // histogram_out.vectorize(x, 4);
    //histogram_out.unroll(x, 8);

    //    Var x_outer, y_outer, x_inner, y_inner, tile_index;
    // histogram_out.tile(x, y, x_outer, y_outer, x_inner, y_inner, 4, 4)
    //     .fuse(x_outer, y_outer, tile_index)
    //     .parallel(tile_index);

    // We'll process 64x64 tiles in parallel.
    Var x_outer, y_outer, x_inner, y_inner, tile_index;
    histogram_out
        .tile(x, y, x_outer, y_outer, x_inner, y_inner, 64, 64)
        .fuse(x_outer, y_outer, tile_index)
        .parallel(tile_index);

    // We'll compute two scanlines at once while we walk across
    // each tile. We'll also vectorize in x. The easiest way to
    // express this is to recursively tile again within each tile
    // into 4x2 subtiles, then vectorize the subtiles across x and
    // unroll them across y:
    Var x_inner_outer, y_inner_outer, x_vectors, y_pairs;
    histogram_out
        .tile(x_inner, y_inner, x_inner_outer, y_inner_outer, x_vectors, y_pairs, 4, 2)
        .vectorize(x_vectors)
        .unroll(y_pairs);

    histogram_out.print_loop_nest();
    histogram_out.realize(result);
    return result;
}

int main(int argc, char **argv)
{

    Buffer<uint8_t> input = Halide::Tools::load_image("images/Space_187k.jpg");

    // representada como 3 dimensões: x,y,c, onde c varia de 0 até 2, representando cada canal
    // BGR
    Var x, y, c;
    //Func main_pipeline;
    //main_pipeline(x, y, c) = to_grayscale(input, x, y);

    auto hist = normalized_histogram(input);
    hist.compute_root();
    //hist.print_loop_nest();
    auto hist_image = histogram_image(hist, input);
    // Halide::Buffer<uint8_t> output =
    //     main_pipeline.realize(input.width(), input.height(), input.channels());

    Halide::Tools::save_image(hist_image, "hist.jpeg");
    printf("Success!\n");

    return 0;
}
