#! /bin/bash
./build_generator.sh
g++ opencv.cpp -g -lstdc++ -I /home/ubuntu/git/fpi/include -I /usr/local/include/opencv4 -L /usr/lib/x86_64-linux-gnu -L /home/ubuntu/git/fpi/lib \
-L /home/ubuntu/git/fpi -lpthread -lHalide -lpng -ljpeg -lopencv_core -lopencv_video -lopencv_videoio -lopencv_highgui \
-l:equalize.a -l:grayscale.a -ldl -o opencv -std=c++11

export LD_LIBRARY_PATH=/home/ubuntu/git/fpi/lib 
./opencv $1