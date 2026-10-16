#define cbar_background 0x{color0.strip}
#define cbar_active 0x{foreground.strip}
#define cbar_inactive 0x{color1.strip}
#define cbar_color(c) (c>>16)/255.,(c>>8&255)/255.,(c&255)/255.
