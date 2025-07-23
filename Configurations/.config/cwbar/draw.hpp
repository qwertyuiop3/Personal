#include <pango/pangocairo.h>
void draw_bitmap(cairo_t* context, int32_t x, uint8_t* bitmap, int32_t width, int32_t height)
{
	cairo_surface_t* surface = cairo_get_group_target(context);
	uint8_t* data = cairo_image_surface_get_data(surface) + (uintptr_t)cwbar_width * 4 * (uintptr_t)((cwbar_height - 1.) / 2. - height / 2);
	int32_t y = 0;
	traverse_bitmap_label:
	{
		memcpy(&data[x * 4], bitmap + y * width, width * 4);
		data += (uintptr_t)cwbar_width * 4;
		if (++y != height) goto traverse_bitmap_label;
	}
}
int8_t within_box(double start_x, double end_x)
{
	int8_t result = mouse_x >= start_x && mouse_x <= end_x;
	mouse_x *= result ^ 1;
	return result;
}
void draw_underline(cairo_t* context, double start_x, double end_x, uint32_t color)
{
	cairo_set_source_rgb(context, cwbar_red(color) / 255., cwbar_green(color) / 255., cwbar_blue(color) / 255.);
	cairo_set_line_width(context, 1);
	cairo_move_to(context, start_x, cwbar_height);
	cairo_line_to(context, end_x, cwbar_height);
	cairo_stroke(context);
}
PangoRectangle get_text_extents(PangoLayout* layout, char* text)
{
	pango_layout_set_text(layout, text, strlen(text));
	PangoRectangle extents;
	pango_layout_get_extents(layout, nullptr, &extents);
	pango_extents_to_pixels(&extents, nullptr);
	return extents;
}
extern cairo_user_data_key_t USER_DATA_PANGO_LAYOUT;
int8_t within_text_extents(cairo_t* context, char* text)
{
	if (mouse_x == 0) return 0;
	double x, y;
	cairo_get_current_point(context, &x, &y);
	PangoRectangle extents = get_text_extents((PangoLayout*)cairo_get_user_data(context, &USER_DATA_PANGO_LAYOUT), text);
	return within_box(x - 4 - extents.width, x - 10);
}
void draw_text(cairo_t* context, char* text, int8_t right, uint32_t color)
{
	double x, y;
	cairo_get_current_point(context, &x, &y);
	PangoLayout* layout = (PangoLayout*)cairo_get_user_data(context, &USER_DATA_PANGO_LAYOUT);
	PangoRectangle extents = get_text_extents(layout, text);
	if (right == 0)
	{
		x += 7;
		cairo_move_to(context, x, y + 5);
	}
	else
	{
		x = right == 1 ? cwbar_width - 7 : x - 7;
		y *= right == 2;
		cairo_move_to(context, x - extents.width, y + 5);
	}
	cairo_set_source_rgb(context, cwbar_red(color) / 255., cwbar_green(color) / 255., cwbar_blue(color) / 255.);
	cairo_save(context);
	pango_cairo_show_layout(context, layout);
	cairo_restore(context);
	if (right == 0) draw_underline(context, x + extents.width - 3, x + 3, color);
	else draw_underline(context, x - extents.width + 3, x - 3, color);
	cairo_move_to(context, right == 0 ? x + extents.width - 3 : x - extents.width + 3, y);
}
