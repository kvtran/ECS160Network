project(ECS160Linux)
cmake_minimum_required(VERSION 2.8)

set(SRC_LIST
    source/FontTileset.cpp
    source/GraphicTileset.cpp
    source/main.cpp
    source/SoundClip.cpp
    source/SoundLibraryMixer.cpp
    source/TerrainMap.cpp
    source/WallShape.cpp)

find_package(GTK2)
find_library(PORTAUDIO portaudio)
find_library(FLUIDSYNTH fluidsynth)
find_library(SNDFILE sndfile)

include_directories(${GTK2_INCLUDE_DIRS})

add_executable(${PROJECT_NAME} ${SRC_LIST})
target_link_libraries(${PROJECT_NAME} ${GTK2_LIBRARIES})
target_link_libraries(${PROJECT_NAME} ${PORTAUDIO})
target_link_libraries(${PROJECT_NAME} ${FLUIDSYNTH})
target_link_libraries(${PROJECT_NAME} ${SNDFILE})

