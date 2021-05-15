// #include "imageops.h"
#include <opencv2/core.hpp>
#include "opencv2/opencv.hpp"
#include "filter.h"

#include "HalideBuffer.h"

using namespace cv;
using namespace std;

Halide::Runtime::Buffer<uchar> wrap_interleaved(Mat mat)
{
    return Halide::Runtime::Buffer<uchar>::make_interleaved(mat.data, mat.rows, mat.cols, 3);
}

Halide::Runtime::Buffer<uchar> wrap_output(Mat mat)
{
    return Halide::Runtime::Buffer<uchar>(mat.data, mat.rows, mat.cols, 3);
}

int main(int argc, char **argv)
{

    // representada como 3 dimensões: x,y,c, onde c varia de 0 até 2, representando cada canal
    // BGR
    // Buffer<uint8_t> input = Halide::Tools::load_image("images/Gramado_72k.jpg");

    // auto output = Buffer<uint8_t>(input.width(), input.height(), 3);
    // auto equalized = histogram_equalization(input);
    // equalized.realize(output);

    // Halide::Tools::save_image(output, "equalized.png");
    // Buffer<uint8_t> equalized_img = Halide::Tools::load_image("equalized.png");

    // auto filtered = filter_3x3(input);
    // filtered.realize(output);
    // Halide::Tools::save_image(output, "filtered.png");

    // auto hist = normalized_histogram(equalized_img);
    // auto hist_image = histogram_image(hist, equalized_img);

    // Halide::Tools::save_image(hist_image, "hist.png");

    // If the input is the web camera, pass 0 instead of the video file name
    VideoCapture cap(0);

    // Check if camera opened successfully
    if (!cap.isOpened())
    {
        cout << "Error opening video stream or file" << endl;
        return -1;
    }

    Mat input;
    Mat output;
    cap >> input;
    output = input;

    while (1)
    {
        cap >> input;
        auto in = wrap_interleaved(input);
        auto out = wrap_interleaved(output);

        filter(in, out);

        // If the frame is empty, break immediately
        if (input.empty())
            break;

        // Display the resulting frame
        imshow("Frame", output);

        // Press  ESC on keyboard to exit
        char c = (char)waitKey(25);
        if (c == 27)
            break;
    }

    // When everything done, release the video capture object
    cap.release();
    // Closes all the frames
    destroyAllWindows();
    printf("Success!\n");
    return 0;
}
