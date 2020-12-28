cmake_minimum_required(VERSION 3.18.2)

set(CMAKE_CXX_STANDARD 17)

find_package(
    Qt5
    COMPONENTS
    Core
    Widgets
    Sql
    Multimedia
    MultimediaWidgets
    Network
    REQUIRED
)

file(GLOB SRC *.cpp)

add_library(app ${SRC})

target_link_libraries(
    app
    tag
    Qt5::Core
    Qt5::Widgets
    Qt5::Sql
    Qt5::Multimedia
    Qt5::MultimediaWidgets
    Qt5::Network
)
