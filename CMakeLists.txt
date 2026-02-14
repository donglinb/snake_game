cmake_minimum_required(VERSION 3.16)

project(snake_game VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets Multimedia)

qt_add_executable(snake_game
    src/main.cpp
    src/snakegame.h
    src/snakegame.cpp
    src/resources.qrc
)

target_link_libraries(snake_game PRIVATE
    Qt6::Widgets
    Qt6::Multimedia
)

set_target_properties(snake_game PROPERTIES
    WIN32_EXECUTABLE ON
    MACOSX_BUNDLE ON
)
