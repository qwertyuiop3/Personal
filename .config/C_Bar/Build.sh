#!/bin/sh

clear

g++ -Ofast -funroll-loops -fno-exceptions -lxcb -lxcb-ewmh -I/usr/include/cairo -lcairo -lX11 Entry_Point.cpp -o C_Bar
