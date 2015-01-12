# ECS160Linux

## Requirements

Development packages for:
- Cmake
- GTK+ 2.0
- PortAudio v19
- Fluidsynth
- libsndfile

Documentation:
- Doxygen

For Ubuntu install the following packages:

`sudo apt-get install build-essential doxygen cmake libgtk2.0-dev portaudio19-dev libsndfile1-dev libfluidsynth-dev`

For Fedora install the following packages:

`yum install doxygen cmake gtk2-devel portaudio-devel libsndfile-devel fluidsynth-devel`

## Building

Make a build directory and run `cmake` and `make`

    mkdir build
    cd build
    cmake -DCMAKE_BUILD_TYPE=Debug .. && make && ./ECS160Linux

Alternatively, just use `./make_and_run.sh`

To build docs, run `doxygen`

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

- Start a new branch for the issue

      git checkout -b issue-name

- Work on fixing issue
- Commit changes

      git commit -m "<what you did> fixes #<issue number>"

- Push new branch up

      git push --set-upstream origin issue-name

- Create a new pull request from master to your branch
  - Name it "<what you did>. Fixes #<issue number>"
  - Include description
  - Tag @ricmatsui @wotwell and anyone else that should review it

## Source Conventions

- Indentation
  - 4 spaces per level
  - Spaces only
  - Kerninghan & Ritchi with curly on same line
- Whitespace
  - No whitespace between identifier and paren
  - Whitespace on both sides of identifier and operators
- Comments
  - Multiline comments - Single star aligned

        /**
         * ... text ...
         */

  - Single line comments - Before the line of code

        // If value is set
        if(val()){

  - Tags - One of the following
      - `// FIXME: Problem to be fixed`
      - `// NOTE: Keep this in mind`
      - `// TODO: Finish this code here`
- Naming convention
  - Pascal Case
    - Classes
    - Member Variables
  - Camel Case
    - Parameters
  - TODO: Remaining naming conventions
- Metadata Conventions
  - Classes begin with C
  - Data member variables begin with D
  - TODO: Remaining metadata conventions


### Example

TODO: Comment code with conventions

    class CRandomNumberGenerator{
        protected:
        unsigned int DRandomSeedHigh;
        unsigned int DRandomSeedLow;
        public:
        CRandomNumberGenerator(){
            DRandomSeedHigh = 0x01234567;
            DRandomSeedLow = 0x89ABCDEF;
        };
        void Seed(unsigned int high, unsigned int low){
            if((high != low) && low && high){
                DRandomSeedHigh = high;
                DRandomSeedLow = low;
            }
        };
        unsigned int Random(){
            DRandomSeedHigh = 36969 * (DRandomSeedHigh & 65535) + (DRandomSeedHigh >> 16);
            DRandomSeedLow = 18000 * (DRandomSeedLow & 65535) + (DRandomSeedLow >> 16);
            return (DRandomSeedHigh << 16) + DRandomSeedLow;
        };
    };
