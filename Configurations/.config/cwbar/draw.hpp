#include <pango/pangocairo.h>
void draw_bitmap(cairo_t* context, int32_t x, uint8_t* bitmap)
{
	cairo_surface_t* surface = cairo_get_group_target(context);
	uint8_t* data = cairo_image_surface_get_data(surface) + (uintptr_t)cwbar_width * 4 * (uintptr_t)((cwbar_height - 1.) / 2. - cwbar_icon_size / 2);
	int32_t vertical = 0;
	traverse_vertical_label:
	{
		int32_t horizontal = 0;
		traverse_horizontal_label:
		{
			uint8_t* destination = &data[(x + horizontal) * 4];
			uint8_t* source = &bitmap[(vertical * cwbar_icon_size + horizontal) * 4];
			uint8_t blend = 255 - source[3];
			destination[0] = (destination[0] * blend + source[0] * source[3]) / 255;
			destination[1] = (destination[1] * blend + source[1] * source[3]) / 255;
			destination[2] = (destination[2] * blend + source[2] * source[3]) / 255;
			destination[3] = 255;
			if (++horizontal != cwbar_icon_size) goto traverse_horizontal_label;
		}
		data += (uintptr_t)cwbar_width * 4;
		if (++vertical != cwbar_icon_size) goto traverse_vertical_label;
	}
}
int8_t within_box(double start_x, double end_x)
{
	int8_t result = mouse_x >= start_x && mouse_x <= end_x;
	mouse_x *= result ^ 1;
	return result;
}
void draw_underline(cairo_t* context, uint32_t color, double start_x, double end_x)
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
	if (right == 0) draw_underline(context, color, x + extents.width - 3, x + 3);
	else draw_underline(context, color, x - extents.width + 3, x - 3);
	cairo_move_to(context, right == 0 ? x + extents.width - 3 : x - extents.width + 3, y);
}
