#! /bin/bash
g++ main.cpp -g -lstdc++ -I /home/ubuntu/git/fpi/include -L /usr/lib/x86_64-linux-gnu -L /home/ubuntu/git/fpi/lib \
-lHalide -lpng -ljpeg -o main -std=c++11
export LD_LIBRARY_PATH=/home/ubuntu/git/fpi/lib 
./main