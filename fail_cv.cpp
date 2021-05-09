// #include "Halide.h"
// // Include some support code for loading pngs.
// #include "halide_image_io.h"
// #include <opencv2/core.hpp>
// #include "opencv2/opencv.hpp"

// using namespace Halide;
// using namespace cv;
// using namespace std;

// Buffer<uchar> wrap_mat(Mat mat)
// {

//     return Buffer<uchar>::make_interleaved(mat.data, mat.rows, mat.cols, 3);
// }

// int main(int argc, char **argv)
// {
//     // Buffer<uint8_t> input = Halide::Tools::load_image("images/Gramado_72k.jpg");

//     // // representada como 3 dimensões: x,y,c, onde c varia de 0 até 2, representando cada canal
//     // // rgb
//     Var x, y, c;
//     Func main_pipeline;
//     main_pipeline(x, y, c) = input(x, y, c);

//     // Halide::Buffer<uint8_t> output =
//     //     brighter.realize(input.width(), input.height(), input.channels());

//     // Halide::Tools::save_image(output, "brighter.png");
//     // printf("Success!\n");
//     // Create a VideoCapture object and open the input file
//     // If the input is the web camera, pass 0 instead of the video file name
//     VideoCapture cap(0);

//     // Check if camera opened successfully
//     if (!cap.isOpened())
//     {
//         cout << "Error opening video stream or file" << endl;
//         return -1;
//     }

//     while (1)
//     {

//         Mat frame;
//         cap >> frame;
//         auto buf = wrap_mat(frame);

//         // If the frame is empty, break immediately
//         if (frame.empty())
//             break;

//         // Display the resulting frame
//         imshow("Frame", frame);

//         // Press  ESC on keyboard to exit
//         char c = (char)waitKey(25);
//         if (c == 27)
//             break;
//     }

//     // When everything done, release the video capture object
//     cap.release();
//     // Closes all the frames
//     destroyAllWindows();
//     return 0;
// }

// Func to_grayscale(Func f)
// {
//     Func gray;
//     Var x, y, c;
//     gray(x, y, c) = f(x, y, mux(c, {c, c, c}));
//     return gray;
// }