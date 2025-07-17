#define cwbar_background 0x{color0.strip}
#define cwbar_active 0x{foreground.strip}
#define cwbar_inactive 0x{color1.strip}
#define cwbar_red(c) (c>>16)
#define cwbar_green(c) (c>>8&255)
#define cwbar_blue(c) (c&255)
