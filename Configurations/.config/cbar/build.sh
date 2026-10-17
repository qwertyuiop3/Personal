#!/usr/bin/env sh
clear
clang++ -I$HOME/.cache/wal -lxcb -lxcb-xkb -lxcb-ewmh -lcairo -lX11 -O3 -Wno-writable-strings -std=c++20 cbar.cpp -o cbar
