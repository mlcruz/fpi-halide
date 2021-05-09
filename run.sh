#! /bin/bash
g++ main.cpp -g -lstdc++ -I /home/ubuntu/git/fpi/include -I /usr/local/include/opencv4 -L /usr/lib/x86_64-linux-gnu -L /home/ubuntu/git/fpi/lib \
-lHalide -lpng -ljpeg -lopencv_core -lopencv_video -lopencv_videoio -lopencv_highgui -o main -std=c++11
export LD_LIBRARY_PATH=/home/ubuntu/git/fpi/lib 
./main