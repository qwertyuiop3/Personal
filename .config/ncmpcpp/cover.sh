#!/usr/bin/env zsh

printf "\e]20;;0x0+1920+1080\a"

song=$(ncmpcpp --current-song /Storage/Music/%D/%f 2>/dev/null)

external_cover=$(find "$(ncmpcpp --current-song /Storage/Music/%D 2>/dev/null)" -maxdepth 1 -iname \*.j\* | head -1)

internal_cover=$(ffmpeg -i $song -c:v copy /tmp/cover.jpg -loglevel -8 -y && echo /tmp/cover.jpg)

if [[ $external_cover || $internal_cover ]]; then
	cover=$(printf `echo $song | cksum` | sed "s/^/\/tmp\//")

	convert ${internal_cover:=$external_cover} -fill black -colorize 80 $cover

	printf "\e]20;$cover;style=stretched\a"
fi
