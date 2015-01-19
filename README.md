# ECS160Linux

## Documentation

View documentation at https://ucdclassnitta.github.io/ECS160Linux/

## Requirements

Development packages for:
- Cmake
- GTK+ 2.0
- PortAudio v19
- Fluidsynth
- libsndfile

Documentation:
- Doxygen
- Graphviz

For Ubuntu install the following packages:

`sudo apt-get install build-essential doxygen graphviz cmake libgtk2.0-dev portaudio19-dev libsndfile1-dev libfluidsynth-dev`

For Fedora install the following packages:

`yum install doxygen graphviz cmake gtk2-devel portaudio-devel libsndfile-devel fluidsynth-devel`

## Building

Make a build directory and run `cmake` and `make`

    mkdir build
    cd build
    cmake -DCMAKE_BUILD_TYPE=Debug .. && make && ./ECS160Linux

Alternatively, just use `./make_and_run.sh`

To build docs, run `doxygen` or `./doxygen_and_open.sh`

## File Organization

    /
    |- source - Source files
      |- .cpp files
      |- .h files
    |- build - Project build location used with cmake
    |- docs - Docs generated from Doxygen
    |- data - Data describing assets
    |- pngs - Graphical assets
    |- sounds - Sound effects
    |- CMakeLists.txt - Described project for cmake
    |- make_and_run.sh - Script for building project
    |- README.md - This file

## Working on Issues

- Clone the repo

        git clone git@github.com:UCDClassNitta/ECS160Linux.git

- Start a new branch for the issue

        git checkout -b issue-name

- Work on fixing issue
- Commit changes

        git commit -m "<what you did> fixes #<issue number>"

- Push new branch up

        git push --set-upstream origin issue-name

- Create a new pull request from master to your branch
  - Name it `<what you did>. Fixes #<issue number>`
  - Include description
  - Tag @ricmatsui @wotwell and anyone else that should review it

## Coding Convention

See the [coding convention document](coding_convention.md)

## Network

    - For files with dependencies on the ZeroMQ library, add '-lzmq' when building.
    - The server must be running before the game.


