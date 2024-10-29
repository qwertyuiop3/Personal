. /usr/share/zsh/plugins/zsh-vi-mode/zsh-vi-mode.plugin.zsh

HISTFILE=~/.histfile

HISTSIZE=1024

SAVEHIST=1024

unsetopt autocd beep extendedglob nomatch notify

if [ $UID -ne 0 ]; then
	if [ $TERM == "rxvt-unicode-256color" ]; then
		rm -rf ~/.cache/ranger

		rm -rf ~/.local/share/ranger

		rm -rf ~/.cache/ueberzugpp

		cat ~/.cache/wal/sequences

		neofetch

		alias mpv="mpv --audio-file-auto=all --sub-auto=all $@"

		alias mpvu="command mpv --profile=low-latency --untimed $@"

		alias ncmpcpp="ncmpcpp;printf \"\e]20;;0x0+1920+1080\a\""

		run()
		{
			screen -dms -X $@

			exit
		}

		alias discord="run discord --proxy-server=socks5://127.0.0.1:1081"

		firejail()
		{
			command firejail --private=/Storage/Software/firejail $@
		}

		alias wine=""
	fi
fi
