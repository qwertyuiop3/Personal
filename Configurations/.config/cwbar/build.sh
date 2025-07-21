#!/bin/sh
clear
sdbus-c++-xml2cpp org.kde.StatusNotifierWatcher.xml --adaptor=StatusNotifierWatcher_adaptor.hpp
sdbus-c++-xml2cpp org.kde.StatusNotifierItem.xml --proxy=StatusNotifierItem_proxy.hpp
g++ -std=gnu++20 -I$HOME/.cache/wal -I/usr/include/pango-1.0 -I/usr/include/glib-2.0 -I/usr/lib/glib-2.0/include -I/usr/include/harfbuzz -I/usr/include/cairo -Wno-write-strings -Ofast -lnkk -lcairo -lpangocairo-1.0 -lpango-1.0 -lwayland-egl -lwayland-client -lgobject-2.0 -lsdbus-c++ cwbar.cpp -o cwbar
