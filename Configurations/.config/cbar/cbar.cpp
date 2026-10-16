#include <vector>
#include <cstring>
#include "xcb/xcb.h"
#include "xcb/xcb_ewmh.h"
#include "colors-cbar.hpp"
#include "cairo/cairo-xcb.h"
#include "X11/XKBlib.h"
#include <algorithm>
#include <thread>
//td: replace with https://github.com/baskerville/bspwm/blob/master/src/bspc.c
char* shell_output(char* command)
{
	char* line = nullptr;
	size_t length;
	FILE* file = popen(command, "r");
	getline(&line, &length, file);
	pclose(file);
	return line;
}
void shell_output_multiple(char* command, std::vector<char*>& output)
{
	char* line = nullptr;
	size_t length;
	FILE* file = popen(command, "r");
	while (getline(&line, &length, file) != -1)
	{
		length = strlen(line);
		char* allocated_line = (char*)malloc(length);
		memcpy(allocated_line, line, length);
		output.push_back(allocated_line);
	}
	free(line);
	pclose(file);
}
int32_t main()
{
	xcb_connection_t* x_connection = xcb_connect(nullptr, nullptr);
	xcb_screen_t* screen = xcb_setup_roots_iterator(xcb_get_setup(x_connection)).data;
	xcb_window_t window = xcb_generate_id(x_connection);
	uint16_t bar_width = screen->width_in_pixels;
	constexpr uint16_t bar_height = 33;
	xcb_create_window(x_connection, screen->root_depth, window, screen->root, 0, 0, bar_width, bar_height, 0, XCB_WINDOW_CLASS_INPUT_OUTPUT, screen->root_visual, XCB_CW_BACK_PIXEL | XCB_CW_EVENT_MASK, (uint32_t[2]){ cbar_background, XCB_EVENT_MASK_BUTTON_PRESS | XCB_EVENT_MASK_EXPOSURE | XCB_EVENT_MASK_SUBSTRUCTURE_NOTIFY });
	xcb_ewmh_connection_t ewmh_connection;
	xcb_ewmh_init_atoms_replies(&ewmh_connection, xcb_ewmh_init_atoms(x_connection, &ewmh_connection), nullptr);
	xcb_ewmh_set_wm_window_type(&ewmh_connection, window, 2, (xcb_atom_t[2]){ ewmh_connection._NET_WM_WINDOW_TYPE_DOCK, ewmh_connection._NET_WM_STATE_ABOVE });
	xcb_ewmh_set_wm_strut_partial(&ewmh_connection, window, (xcb_ewmh_wm_strut_partial_t){ .top = bar_height, .top_end_x = bar_height });
	xcb_intern_atom_reply_t* atom_reply = xcb_intern_atom_reply(x_connection, xcb_intern_atom(x_connection, 0, sizeof("_NET_SYSTEM_TRAY_S0") - 1, "_NET_SYSTEM_TRAY_S0"), nullptr);
	xcb_set_selection_owner(x_connection, window, atom_reply->atom, XCB_CURRENT_TIME);
	free(atom_reply);
	xcb_map_window(x_connection, window);
	cairo_surface_t* surface = cairo_xcb_surface_create(x_connection, window, xcb_depth_visuals_iterator(xcb_screen_allowed_depths_iterator(screen).data).data, bar_width, bar_height);
	cairo_t* cairo = cairo_create(surface);
	cairo_select_font_face(cairo, "notosansmono", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL);
	cairo_set_font_size(cairo, 13);
	std::vector<xcb_window_t> icons;
	int8_t show_date = 0;
	while (Display* display = XOpenDisplay(nullptr))
	{
		xcb_generic_event_t* event = xcb_poll_for_event(x_connection); //td: replace with wait_for, bspc subscribe, xkbevents
		if (event != nullptr)
		{
			cairo_set_source_rgb(cairo, cbar_color(cbar_background));
			cairo_paint(cairo);
			char formatted[sizeof("[ ??.??.???? ]")];
			static cairo_text_extents_t text_extents = { -1 };
			if (text_extents.x_bearing == -1) cairo_text_extents(cairo, "[", &text_extents);
			static double text_bearing[2] = { text_extents.x_bearing, text_extents.y_bearing + text_extents.height };
			static double text_y = (bar_height - 1) / 2 - text_bearing[1] + text_extents.height / 2.;
			constexpr uint16_t item_spacing = 10;
			//left
			{
				std::vector<char*> workspaces;
				char* focused_workspace_string = shell_output("bspc query -D -d --names");
				int32_t focused_workspace_number = atoi(focused_workspace_string);
				workspaces.push_back(focused_workspace_string);
				shell_output_multiple("bspc query -D -d .occupied.!focused --names", workspaces);
				std::sort(workspaces.begin(), workspaces.end(), [](char* x, char* y) -> int8_t { return atoi(x) < atoi(y); });
				size_t entry_number = 0;
				do
				{
					char* workspace_string = workspaces.at(entry_number);
					int32_t workspace_number = atoi(workspace_string);
					if (workspace_number == focused_workspace_number)
						cairo_set_source_rgb(cairo, cbar_color(cbar_active));
					else
						cairo_set_source_rgb(cairo, cbar_color(cbar_inactive));
					free(workspace_string);
					cairo_text_extents(cairo, "[ 0 ]", &text_extents);
					double workspace_spacing = entry_number * (text_extents.width + item_spacing);
					cairo_move_to(cairo, item_spacing - text_bearing[0] + workspace_spacing, text_y);
					sprintf(formatted, "[ %i ]", workspace_number);
					cairo_show_text(cairo, formatted);
					cairo_move_to(cairo, item_spacing + workspace_spacing, bar_height);
					cairo_line_to(cairo, item_spacing + workspace_spacing + text_extents.width, bar_height);
					cairo_stroke(cairo);
				} while (++entry_number != workspaces.size());
			}
			uint8_t event_identifier = event->response_type & ~128;
			//center
			{
				constexpr uint16_t icon_size = 16;
				double icon_center_x = bar_width / 2 - icon_size / 2;
				constexpr uint16_t icon_spacing = icon_size / 2 + item_spacing / 2;
				double icon_center_y = bar_height / 2 - icon_size / 2;
				if (event_identifier == XCB_DESTROY_NOTIFY)
					std::erase(icons, ((xcb_unmap_notify_event_t*)event)->window);
				if (event_identifier == XCB_CLIENT_MESSAGE)
				{
					xcb_query_tree_reply_t* tree_reply = xcb_query_tree_reply(x_connection, xcb_query_tree(x_connection, ((xcb_client_message_event_t*)event)->data.data32[2]), nullptr);
					if (free(tree_reply), tree_reply != nullptr)
					{
						icons.push_back(((xcb_client_message_event_t*)event)->data.data32[2]);
						xcb_configure_window(x_connection, icons.back(), XCB_CONFIG_WINDOW_WIDTH | XCB_CONFIG_WINDOW_HEIGHT, (uint32_t[2]){ icon_size, icon_size });
					}
				}
				if (icons.size() == 0)
					cairo_set_source_rgb(cairo, cbar_color(cbar_inactive));
				else
					cairo_set_source_rgb(cairo, cbar_color(cbar_active));
				size_t icon_number = 0;
				do
				{
					double icon_x = icon_center_x;
					if (icons.size() > 1)
					{
						if (icon_number % 2 == 0)
							icon_x -= icon_spacing * (icons.size() - icon_number - 1);
						else
							icon_x += icon_spacing * (icons.size() - icon_number);
					}
					if (icons.size() > 0)
					{
						if (event_identifier == XCB_DESTROY_NOTIFY)
							xcb_reparent_window(x_connection, icons.at(icon_number), window, icon_x, icon_center_y);
						if (event_identifier == XCB_CLIENT_MESSAGE)
						{
							xcb_reparent_window(x_connection, icons.at(icon_number), window, icon_x, icon_center_y);
							xcb_map_window(x_connection, icons.at(icon_number));
						}
					}
					cairo_move_to(cairo, icon_x, bar_height);
					cairo_line_to(cairo, icon_x + icon_size, bar_height);
					cairo_stroke(cairo);
				} while (++icon_number < icons.size());
			}
			time_t seconds = time(&seconds);
			//right
			{
				uint32_t indicator_state;
				XkbGetIndicatorState(display, XkbUseCoreKbd, &indicator_state);
				if ((indicator_state & 1) == 0)
					cairo_set_source_rgb(cairo, cbar_color(cbar_inactive));
				else
					cairo_set_source_rgb(cairo, cbar_color(cbar_active));
				sprintf(formatted, indicator_state & 4096 ? "[ Russian ]" : "[ English ]");
				cairo_text_extents(cairo, formatted, &text_extents);
				double text_x = bar_width - item_spacing - text_extents.width;
				cairo_move_to(cairo, text_x - text_bearing[0], text_y);
				cairo_show_text(cairo, formatted);
				cairo_move_to(cairo, text_x, bar_height);
				cairo_line_to(cairo, text_x + text_extents.width, bar_height);
				cairo_stroke(cairo);
				tm* local_time = localtime(&seconds);
				if (event_identifier == XCB_BUTTON_PRESS)
				{
					xcb_button_press_event_t* button = (xcb_button_press_event_t*)event;
					if (button->detail == XCB_BUTTON_INDEX_1)
					{
						if (show_date == 0)
							sprintf(formatted, "[ %.2d:%.2d:%.2d ]", local_time->tm_hour, local_time->tm_min, local_time->tm_sec);
						else
							sprintf(formatted, "[ %.2d.%.2d.%.2d ]", local_time->tm_mday, 1 + local_time->tm_mon, 1900 + local_time->tm_year);
						cairo_text_extents(cairo, formatted, &text_extents);
						show_date ^= button->event_x >= text_x - item_spacing - text_extents.width && button->event_x <= text_x - item_spacing;
					}
				}
				if (show_date == 0)
					cairo_set_source_rgb(cairo, cbar_color(cbar_inactive)), sprintf(formatted, "[ %.2d:%.2d:%.2d ]", local_time->tm_hour, local_time->tm_min, local_time->tm_sec);
				else
					cairo_set_source_rgb(cairo, cbar_color(cbar_active)), sprintf(formatted, "[ %.2d.%.2d.%.2d ]", local_time->tm_mday, 1 + local_time->tm_mon, 1900 + local_time->tm_year);
				cairo_text_extents(cairo, formatted, &text_extents);
				text_x -= item_spacing + text_extents.width;
				cairo_move_to(cairo, text_x - text_bearing[0], text_y);
				cairo_show_text(cairo, formatted);
				cairo_move_to(cairo, text_x, bar_height);
				cairo_line_to(cairo, text_x + text_extents.width, bar_height);
				cairo_stroke(cairo);
			}
			cairo_surface_flush(surface);
			xcb_flush(x_connection);
			free(event);
		}
		else
		{
			xcb_expose_event_t event = { XCB_EXPOSE };
			xcb_send_event(x_connection, 0, window, XCB_EVENT_MASK_EXPOSURE, (char*)&event);
			xcb_flush(x_connection);
			std::this_thread::sleep_for(std::chrono::milliseconds(100));
		}
		XCloseDisplay(display);
	}
}
