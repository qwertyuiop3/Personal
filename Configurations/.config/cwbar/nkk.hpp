#ifndef __NKK_H__
#define __NKK_H__

#include <stdbool.h>
#include <xkbcommon/xkbcommon.h>
#include <linux/input-event-codes.h>
#include <cairo/cairo.h>

#define NKK_MOD_SHIFT  0x01
#define NKK_MOD_CTRL   0x02
#define NKK_MOD_ALT    0x04
#define NKK_MOD_MOD4   0x08
#define NKK_MOD_SUPER  0x08
#define NKK_MOD_LOGO   0x08

#define NkkLayerAnchorTop      1
#define NkkLayerAnchorBottom   2
#define NkkLayerAnchorLeft     4
#define NkkLayerAnchorRight    8

#define NkkLayerBackground     0
#define NkkLayerBelow          1
#define NkkLayerAbove          2
#define NkkLayerOverlay        3

typedef struct nkk_window nkk_win;
typedef struct nkk_display nkk_display;

struct nkk_layer_config {
	uint32_t layer;
	uint32_t width, height;
	uint32_t anchor;
	int32_t exclusive_zone;
	struct { int32_t top, right, bottom, left; } margin;
	bool accepts_kb_focus;
};

struct nkk_printf_args {
	const char *name;
	const char *fmt;
	const union {
		const char *s;
		int d;
		unsigned int u;
		double f;
		void *v;
	} arg;
};

extern "C" {
	nkk_display *nkk_display_open          (const char *, void *);
	void         nkk_display_close         (nkk_display *);
	void         nkk_display_main_loop     (nkk_display *);
	void         nkk_display_add_event_fd  (nkk_display *, int, int (*)(void *), void *);

	void         nkk_text_extents          (const char *, const char *, int, int *, int *);
	int          nkk_printf                (cairo_t *, float, const char *, struct nkk_printf_args *);

	nkk_win     *nkk_xdg_window_create     (nkk_display *, uint32_t, uint32_t);
	nkk_win     *nkk_layer_window_create   (nkk_display *, struct nkk_layer_config *);
	nkk_win     *nkk_egl_window_create     (nkk_win *win);
	nkk_win     *nkk_sub_window_create     (nkk_win *win, int32_t, int32_t, uint32_t, uint32_t);

	void         nkk_window_destroy        (nkk_win *);
	void        *nkk_window_get_user_data  (nkk_win *);
	void         nkk_window_move           (nkk_win *, int32_t, int32_t);
	void         nkk_window_redraw         (nkk_win *);
	void         nkk_window_resize         (nkk_win *, uint32_t, uint32_t);
	void         nkk_window_set_cursor     (nkk_win *, const char *);
	void         nkk_window_set_font       (nkk_win *, const char *);
	void         nkk_window_set_fullscreen (nkk_win *, bool);
	void         nkk_window_set_size       (nkk_win *, uint32_t, uint32_t);
	void         nkk_window_set_title      (nkk_win *, const char *);
	void         nkk_window_set_user_data  (nkk_win *, void *);

	void         nkk_window_on_draw        (nkk_win *, void (*)(nkk_win *, cairo_t *));
	void         nkk_window_on_keydown     (nkk_win *, void (*)(nkk_win *, uint32_t, xkb_keysym_t));
	void         nkk_window_on_keyup       (nkk_win *, void (*)(nkk_win *, uint32_t, xkb_keysym_t));
	void         nkk_window_on_mousedown   (nkk_win *, void (*mousedown)(nkk_win *, uint32_t, uint32_t, double, double));
	void         nkk_window_on_mouseup     (nkk_win *, void (*mouseup)(nkk_win *, uint32_t, uint32_t, double, double));
	void         nkk_window_on_mousewheel  (nkk_win *, void (*mousewheel)(nkk_win *, double, double));
	void         nkk_window_on_mousemotion (nkk_win *, void (*mousemotion)(nkk_win *, double, double));
	void         nkk_window_on_mouseenter  (nkk_win *, void (*mouseenter)(nkk_win *, double, double));
	void         nkk_window_on_mouseleave  (nkk_win *, void (*mouseleave)(nkk_win *));
};

#endif /* __NKK_H__ */
