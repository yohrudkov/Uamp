cmake_minimum_required(VERSION 3.18.2)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

set(NAME uamp)
set(SOURCE_EXE main.cpp resources/resources.qrc)
set(TAGLIB_PATH "../3dparty/taglib/taglib")
set(UI src/MainWindow.ui src/SubWindow.ui src/Search.ui src/Message.ui)

project(${NAME})

add_compile_options(
    -Wall
    -Wextra
    -Werror
    -Wpedantic
)

find_library(TAGLIB libtag)
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

include_directories(
    ./src
    ../build/3dparty/taglib
    ${TAGLIB_PATH}/ape
    ${TAGLIB_PATH}/asf
    ${TAGLIB_PATH}/flac
    ${TAGLIB_PATH}/it
    ${TAGLIB_PATH}/mod
    ${TAGLIB_PATH}/mp4
    ${TAGLIB_PATH}/mpc
    ${TAGLIB_PATH}/mpeg
    ${TAGLIB_PATH}/mpeg/id3v2
    ${TAGLIB_PATH}/mpeg/id3v2/frames
    ${TAGLIB_PATH}/ogg
    ${TAGLIB_PATH}/riff
    ${TAGLIB_PATH}/s3m
    ${TAGLIB_PATH}/toolkit
    ${TAGLIB_PATH}/trueaudio
    ${TAGLIB_PATH}/wavpack
    ${TAGLIB_PATH}/xm
    ${TAGLIB_PATH}
)

add_subdirectory(src)
add_executable(${NAME} ${SOURCE_EXE})

target_link_libraries(
    ${NAME}
    tag
    Qt5::Core
    Qt5::Widgets
    Qt5::Sql
    Qt5::Multimedia
    Qt5::MultimediaWidgets
    Qt5::Network
    app
)
