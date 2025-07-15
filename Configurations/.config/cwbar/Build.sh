#!/bin/sh
clear
g++ -Wno-write-strings -Ofast -lnkk -lcairo -lwayland-egl -lwayland-client cwbar.cpp -o cwbar
