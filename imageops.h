#include "Halide.h"
#include "halide_image_io.h"
using namespace Halide;

const int HISTOGRAM_ROWS = 255;

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

Func histogram(Buffer<u_int8_t> src)
{
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
    return histogram;
}

Func normalized_histogram(Buffer<u_int8_t> src)
{
    float alpha = static_cast<float>(HISTOGRAM_ROWS) / (src.width() * src.height() / 3);
    Var x("x"), y("y"), c("c");
    Var h("h");

    Func normalized("normalized_hist");
    Func hist = histogram(src);
    hist.compute_root();
    normalized(h) = clamp(round(hist(h) * alpha));
    return normalized;
}

Buffer<uint8_t> histogram_image(Func histogram, Buffer<u_int8_t> src)
{
    Buffer<uint8_t> result(256, HISTOGRAM_ROWS, 3);
    Func histogram_out("histogram_out");

    Var x("x"), y("y"), c("c");
    Var h("h");

    // Imagem do histograma
    Expr out_expr = mux(c,
                        {select(histogram(x) > (HISTOGRAM_ROWS - y), 180, 128),
                         select(histogram(x) > (HISTOGRAM_ROWS - y), 30, 128),
                         select(histogram(x) > (HISTOGRAM_ROWS - y), 100, 128)});

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

    //histogram_out.print_loop_nest();
    histogram_out.realize(result);
    return result;
}

Func histogram_equalization(Buffer<u_int8_t> src)
{
    Var x("x"), y("y"), c("c");
    Var h("h");

    float alpha = 255.0f / (src.height() * src.width() / 3);

    Func histogram_fn = histogram(src);
    histogram_fn.compute_root();

    Func hist_cum("hist cum");

    hist_cum(h) = alpha * histogram_fn(h);

    auto h_dom = RDom(1, 255, "h");

    hist_cum(h_dom.x) =
        hist_cum(h_dom.x - 1) +
        ((alpha * histogram_fn(h_dom.x)) / 3);

    hist_cum.compute_root();
    hist_cum.trace_stores();

    Func histogram_eq("histogram eq");
    histogram_eq(x, y, c) = clamp(hist_cum(src(x, y, c)));

    return histogram_eq;
}