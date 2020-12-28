#!/bin/zsh

export CMAKE_PREFIX_PATH="~/.linuxbrew/Cellar/qt/5.15.2/"

cmake . -Bbuild -Wdev -Werror=dev && cmake --build ./build
cp ./build/app/uamp ./uamp
