# ECS160Linux

## Requirements

Development packages for:
- Cmake
- GTK+ 2.0
- PortAudio v19
- Fluidsynth
- libsndfile

For Ubuntu install the following packages

`sudo apt-get install build-essential cmake libgtk2.0-dev portaudio19-dev libsndfile1-dev libfluidsynth-dev`

## Building

Make a build directory and run `cmake` and `make`

    mkdir build
    cd build
    cmake -DCMAKE_BUILD_TYPE=Debug .. && make && ./ECS160Linux

Alternatively, just use `./make_and_run.sh`


