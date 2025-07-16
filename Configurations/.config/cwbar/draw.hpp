#include <pango/pangocairo.h>
extern cairo_user_data_key_t USER_DATA_PANGO_LAYOUT;
void draw_text(cairo_t* context, char* text, int8_t right, double red, double green, double blue)
{
	double x, y;
	PangoLayout* layout = (PangoLayout*)cairo_get_user_data(context, &USER_DATA_PANGO_LAYOUT);
	pango_layout_set_text(layout, text, strlen(text));
	PangoRectangle extents;
	pango_layout_get_extents(layout, nullptr, &extents);
	pango_extents_to_pixels(&extents, nullptr);
	cairo_get_current_point(context, &x, &y);
	if (right == 0)
	{
		x += 7;
		cairo_move_to(context, x, y + 5);
	}
	else
	{
		x = right == 1 ? 1920 - 7 : x - 7;
		y *= right == 2;
		cairo_move_to(context, x - extents.width, y + 5);
	}
	cairo_set_source_rgb(context, red / 255., green / 255., blue / 255.);
	cairo_save(context);
	pango_cairo_show_layout(context, layout);
	cairo_restore(context);
	if (right == 0)
	{
		cairo_move_to(context, x + 3, 33);
		cairo_line_to(context, x + extents.width - 3, 33);
	}
	else
	{
		cairo_move_to(context, x - 3, 33);
		cairo_line_to(context, x - extents.width + 3, 33);
	}
	cairo_set_line_width(context, 1);
	cairo_stroke(context);
	cairo_move_to(context, right == 0 ? x + extents.width - 3 : x - extents.width + 3, y);
}
