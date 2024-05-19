#!/bin/sh

clear

g++ -Ofast -lxcb -lxcb-ewmh -lcairo -lX11 Entry_Point.cpp -o C_Bar
