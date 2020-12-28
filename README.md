<h1 align="center"> Uamp </h1> <br>

<p align="center">
    <a href="https://github.com/yohrudkov/Utag">
        <img alt="Uamp"
        title="Uamp"
        src="https://github.com/yohrudkov/Uamp/blob/main/app/resources/exe.svg?raw=true"
        width="250">
    </a>
</p>

<p align="center"> Your personal music player. Build with Cmake. </p>

<p align="center">
    <a href="https://github.com/yohrudkov">
        <img alt="yohrudkov GitHub main page"
        title="Main page"
        src="https://github.githubassets.com/images/modules/logos_page/GitHub-Logo.png"
        width="140">
    </a>
</p>

## Table of Contents

- [Introduction](#Introduction)
- [Features](#Features)
- [Build Process](#Build-Process)
    - [Dependencies](#Dependencies)
    - [Preparation](#Preparation)
    - [Compilation](#Compilation)
    - [Initiation](#Initiation)
- [Authors](#Authors)
- [License](#License)
- [Acknowledgments](#Acknowledgments)

## Introduction

Desktop application for to playing audio and video files, reading and editing metadata of several popular audio formats.\
Uamp uses a graphical interface to make it easy to modify tags and is completely free to use.

<p align="center">
    <img alt="Uamp"
    title="Uamp"
    src="https://github.com/yohrudkov/Utag/blob/main/app/resources/img/2.jpg?raw=true"
    width="400">
</p>

## Features

A few of the things you can do with Uamp:

* supports several popular audio file formats;
* support such standard options:\
    *`play`*, *`pause`*, *`stop`*, *`fast forward`*, *`rewind`*, *`next`*, *`previous`*.
* control sound volume;
* autoplay the next song in the Queue;
* create, read, update and delete personal playlists;
* import, export playlists in *`m3u`* standard playlist format;
* add and remove songs from the Queue;
* loop (repeat) a single song or Queue;
* shuffle songs listed in the Queue;
* sort songs listed in the Queue;
* edit songs album art using pictures on the computer;
* display and edit the audio tags;
* ability to download album art through the internet;
* displaying and editing the text of the songs;
* support auto-shutdown of the program after a while;
* multiuser mode support;
* support radio;
* add and delete pictures for playlists from the computer.

## Build Process

### Dependencies

- [CMake](https://cmake.org) - *an open-source, cross-platform family of tools designed to build, test and package software.*
- [Qt](https://www.qt.io) - *a free and open-source widget toolkit for creating graphical user interfaces as well as cross-platform applications that run on various software and hardware platforms.*
- [TagLib](https://taglib.org) - *a library for reading and editing the meta-data of several popular audio formats.*

### Preparation

For find_package to be successful, CMake must find the Qt installation in one of the following ways:

- Set your CMAKE_PREFIX_PATH environment variable to the Qt 5 installation prefix:
    - uncomment and change CMAKE_PREFIX_PATH path in *`Utag/CMakeLists.txt`* file;
    - or just run *`export CMAKE_PREFIX_PATH=/path_to_qt_directory`* commend in shell.
- Set the Qt5_DIR in the CMake cache to the location of the Qt5Config.cmake file.

### Compilation

To build Uamp project, run the following *`./build.sh`* script.

### Initiation

To begin to use Uamp project, run the following *`./uamp`* exe file.

## Authors

- Yaroslav Ohrudkov - *Application development* - [yohrudkov](https://github.com/yohrudkov)

## License

Collision is an open-sourced software licensed under the [MIT license](https://en.wikipedia.org/wiki/MIT_License). \
[Copyright (c) 2020 yohrudkov](https://github.com/yohrudkov/Utag/blob/main/LICENSE).

## Acknowledgments

Thanks [Ucode IT academy](https://ucode.world/ru/) for the training program provided.