#!/bin/zsh

. /etc/X11/xinit/xinitrc.d/50-systemd-user.sh

setxkbmap -option grp:win_space_toggle us,ru

xsetroot -cursor_name left_ptr

/usr/lib/polkit-gnome/polkit-gnome-authentication-agent-1 &

exec bspwm
