#!/usr/bin/env zsh
. /etc/X11/xinit/xinitrc.d/50-systemd-user.sh
xsetroot -cursor_name left_ptr
/usr/lib/polkit-gnome/polkit-gnome-authentication-agent-1 &
exec bspwm
