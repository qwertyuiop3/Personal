. /usr/share/zsh/plugins/zsh-vi-mode/zsh-vi-mode.plugin.zsh

HISTFILE=~/.histfile

HISTSIZE=1024

SAVEHIST=1024

unsetopt autocd beep extendedglob nomatch notify

if [ $UID -ne 0 ]; then
	if [ $TERM == "rxvt-unicode-256color" ]; then
		rm -rf ~/.cache/ranger

		rm -rf ~/.local/share/ranger

		cat ~/.cache/wal/sequences

		neofetch

		mpv()
		{
			command mpv $@ --audio-file-auto=all --sub-auto=all --profile=low-latency --untimed
		}

		ncmpcpp()
		{
			command ncmpcpp

			printf "\e]20;;0x0+1920+1080\a"
		}
	fi
fi
