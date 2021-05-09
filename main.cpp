#include "Halide.h"
// Include some support code for loading pngs.
#include "halide_image_io.h"
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
using namespace Halide;

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

Buffer<uint8_t> histogram(Buffer<u_int8_t> src)
{
    float alpha = 255.0f / src.width() * src.height();
    Buffer<uint8_t> result(256, 256, 3);

    Var x, y, c;
    Var h;

    Func grayscale;
    Func histogram;
    Func histogram_out;

    // Tons de cinza
    grayscale(x, y, c) = to_grayscale(src, x, y);

    // reduction domain variando em 2d
    auto x_y_domain = RDom(0, src.width(), 0, src.height());

    // Histograma da imagem
    histogram(h) = 0.0f;
    histogram(grayscale(x_y_domain.x, x_y_domain.y, 0)) += 1.0f;
    histogram(h) = histogram(h) * alpha;

    // Imagem do histograma
    Expr out_expr = result(x, y, c);
    out_expr = x, y, mux(c, {select(histogram(x) <= y, 0, 255), select(histogram(x) <= y, 0, 255), select(histogram(x) <= y, 0, 255)});
    out_expr = cast<uint8_t>(out_expr);
    histogram_out(x, y, c) = out_expr;

    histogram_out.realize(result);
    return result;
}

int main(int argc, char **argv)
{

    Buffer<uint8_t> input = Halide::Tools::load_image("images/Gramado_72k.jpg");

    // representada como 3 dimensões: x,y,c, onde c varia de 0 até 2, representando cada canal
    // BGR
    Var x, y, c;
    Func main_pipeline;
    main_pipeline(x, y, c) = to_grayscale(input, x, y);
    auto hist = histogram(input);

    // Halide::Buffer<uint8_t> output =
    //     main_pipeline.realize(input.width(), input.height(), input.channels());

    Halide::Tools::save_image(hist, "hist.jpeg");
    printf("Success!\n");

    return 0;
}
