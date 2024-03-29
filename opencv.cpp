// #include "imageops.h"
#include <opencv2/core.hpp>
#include "opencv2/opencv.hpp"
#include "HalideBuffer.h"
#include "equalize.h"
#include "grayscale.h"

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
    // If the input is the web camera, pass 0 instead of the video file name
    VideoCapture cap(0);

    // Check if camera opened successfully
    if (!cap.isOpened())
    {
        cout << "Error opening video stream or file" << endl;
        return -1;
    }

    Mat input;
    Mat grayscale_output;
    Mat eq_output;
    Mat grayscale_eq_output;

    cap >> input;

    grayscale_output = input.clone();
    grayscale_eq_output = input.clone();
    eq_output = input.clone();

    while (1)
    {
        cap >> input;
        auto in = wrap_interleaved(input);
        auto gray_out = wrap_interleaved(grayscale_output);
        auto eq_gray_out = wrap_interleaved(grayscale_eq_output);
        auto eq_out = wrap_interleaved(eq_output);

        equalize(in, eq_out);
        grayscale(in, gray_out);
        equalize(gray_out, eq_gray_out);

        // If the frame is empty, break immediately
        if (input.empty())
            break;

        // Display the resulting frame
        imshow("src", input);

        imshow("Grayscale", grayscale_output);
        imshow("Grayscale_eq", grayscale_eq_output);
        imshow("eq", eq_output);

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
