#!/bin/sh
clear
g++ -I$HOME/.cache/wal -I/usr/include/pango-1.0 -I/usr/include/glib-2.0 -I/usr/lib/glib-2.0/include -I/usr/include/harfbuzz -I/usr/include/cairo -Wno-write-strings -Ofast -lnkk -lcairo -lpangocairo-1.0 -lpango-1.0 -lwayland-egl -lwayland-client -lgobject-2.0 cwbar.cpp -o cwbar
