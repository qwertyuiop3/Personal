. /usr/share/zsh/plugins/zsh-vi-mode/zsh-vi-mode.plugin.zsh
HISTFILE=~/.histfile
HISTSIZE=1024
SAVEHIST=1024
unsetopt autocd beep extendedglob nomatch notify
if [ $UID -ne 0 ]; then
	rm -rf ~/.cache/ranger
	rm -rf ~/.local/share/ranger
	cat ~/.cache/wal/sequences
	if [ $TERM == "rxvt-unicode-256color" ]; then
		reboot()
		{
			pkill qbittorrent
			while pgrep qbittorrent > /dev/null; do
				sleep 1
			done
			command reboot
		}
		poweroff()
		{
			pkill qbittorrent
			while pgrep qbittorrent > /dev/null; do
				sleep 1
			done
			command poweroff
		}
		alias mpv="mpv --audio-file-auto=all --sub-auto=all --no-osd-bar $@"
		alias mpvu="command mpv --profile=low-latency --untimed $@"
		alias ncmpcpp="ncmpcpp;printf \"\e]20;;0x0+1920+1080\a\""
		run()
		{
			screen -dms -X $@
			exit
		}
		alias discord="run discord"
		firejail()
		{
			command firejail --private=/Storage/Software/firejail $@
		}
		alias wine=""
		neofetch
	fi
fi
