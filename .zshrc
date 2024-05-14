. /usr/share/zsh/plugins/zsh-vi-mode/zsh-vi-mode.plugin.zsh

HISTFILE=~/.histfile

HISTSIZE=1000

SAVEHIST=1000

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
	fi
fi
