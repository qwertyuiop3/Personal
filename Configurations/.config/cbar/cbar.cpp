#include <vector>
#include <sys/socket.h>
timeval bspwm_timeout;
#include <sys/un.h>
#include <unistd.h>
#include "xcb/xkb.h"
#include "xcb/xcb_ewmh.h"
#include "colors-cbar.hpp"
#include "cairo/cairo-xcb.h"
#include <poll.h>
#include "X11/XKBlib.h"
#include <cstdio>
void bspc(char* command, size_t length, std::vector<char*>& output)
{
	int32_t bspwm_socket = socket(AF_UNIX, SOCK_STREAM, 0);
	setsockopt(bspwm_socket, SOL_SOCKET, SO_RCVTIMEO, &bspwm_timeout, sizeof(bspwm_timeout));
	sockaddr_un bspwm_address = { AF_UNIX, "/tmp/bspwm_0_0-socket" };
	connect(bspwm_socket, (sockaddr*)&bspwm_address, sizeof(sockaddr_un));
	send(bspwm_socket, command, length, 0);
	{
		char stack[BUFSIZ];
		ssize_t size = recv(bspwm_socket, stack, sizeof(stack), 0);
		{
			char* field = stack;
			do
			{
				char* next_field = strchr(field, '\n');
				*next_field = '\0';
				output.push_back(strcpy((char*)malloc(strlen(field) + 1), field));
				field = next_field + 1;
			} while (field != stack + size);
		}
	}
	close(bspwm_socket);
}
int32_t main()
{
	xcb_connection_t* x_connection = xcb_connect(nullptr, nullptr);
	xcb_xkb_use_extension(x_connection, XCB_XKB_MAJOR_VERSION, XCB_XKB_MINOR_VERSION);
	xcb_xkb_select_events(x_connection, XCB_XKB_ID_USE_CORE_KBD, XCB_XKB_EVENT_TYPE_INDICATOR_STATE_NOTIFY, 0, XCB_XKB_EVENT_TYPE_INDICATOR_STATE_NOTIFY, 0, 0, nullptr);
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
	int32_t bspwm_socket = socket(AF_UNIX, SOCK_STREAM, 0);
	bspwm_timeout.tv_sec = 60;
	setsockopt(bspwm_socket, SOL_SOCKET, SO_RCVTIMEO, &bspwm_timeout, sizeof(bspwm_timeout));
	sockaddr_un bspwm_address = { AF_UNIX, "/tmp/bspwm_0_0-socket" };
	connect(bspwm_socket, (sockaddr*)&bspwm_address, sizeof(sockaddr_un));
	send(bspwm_socket, "subscribe\0report", sizeof("subscribe\0report"), 0);
	pollfd stream[2] = { { xcb_get_file_descriptor(x_connection), POLLIN }, { bspwm_socket, POLLIN } };
	cairo_surface_t* surface = cairo_xcb_surface_create(x_connection, window, xcb_depth_visuals_iterator(xcb_screen_allowed_depths_iterator(screen).data).data, bar_width, bar_height);
	cairo_t* cairo = cairo_create(surface);
	cairo_select_font_face(cairo, "notosansmono", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL);
	cairo_set_font_size(cairo, 13);
	std::vector<xcb_window_t> icons;
	int8_t show_date = 0;
	while (Display* display = XOpenDisplay(nullptr))
	{
		xcb_generic_event_t* event = nullptr;
		if (poll(stream, sizeof(stream) / sizeof(pollfd), 1000) > 0)
		{
			if (stream[0].revents == POLLIN) event = xcb_poll_for_event(x_connection);
			char stack[BUFSIZ];
			if (stream[1].revents == POLLIN) recv(bspwm_socket, stack, sizeof(stack), 0);
		}
		if (event == nullptr) event = (xcb_generic_event_t*)calloc(1, sizeof(xcb_generic_event_t));
		while (event != nullptr)
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
				bspc("query\0-D\0-d\0.!focused.!occupied\0--names", sizeof("query\0-D\0-d\0.!focused.!occupied\0--names"), workspaces);
				int8_t workspace_flag[11] = { };
				for (char* workspace : workspaces)
				{
					workspace_flag[atoi(workspace)] = 1;
					free(workspace);
				}
				std::vector<char*> focused_workspace;
				bspc("query\0-D\0-d\0--names", sizeof("query\0-D\0-d\0--names"), focused_workspace);
				int32_t focused_workspace_number = atoi(focused_workspace.back());
				free(focused_workspace.back());
				int32_t workspace_number = 1;
				double text_x = item_spacing;
				do
				{
					if (workspace_flag[workspace_number] == 0)
					{
						if (workspace_number == focused_workspace_number)
							cairo_set_source_rgb(cairo, cbar_color(cbar_active));
						else
							cairo_set_source_rgb(cairo, cbar_color(cbar_inactive));
						cairo_move_to(cairo, text_x - text_bearing[0], text_y);
						sprintf(formatted, "[ %d ]", workspace_number);
						cairo_show_text(cairo, formatted);
						cairo_move_to(cairo, text_x, bar_height);
						cairo_text_extents(cairo, formatted, &text_extents);
						cairo_line_to(cairo, text_x + text_extents.width, bar_height);
						cairo_stroke(cairo);
						text_x += text_extents.width + item_spacing;
					}
				} while (++workspace_number != 11);
			}
			uint8_t event_identifier = event->response_type & ~128;
			//center
			{
				if (event_identifier == XCB_DESTROY_NOTIFY)
					std::erase(icons, ((xcb_unmap_notify_event_t*)event)->window);
				constexpr uint16_t icon_size = 16;
				if (event_identifier == XCB_CLIENT_MESSAGE)
				{
					xcb_query_tree_reply_t* tree_reply = xcb_query_tree_reply(x_connection, xcb_query_tree(x_connection, ((xcb_client_message_event_t*)event)->data.data32[2]), nullptr);
					if (free(tree_reply), tree_reply != nullptr)
					{
						icons.push_back(((xcb_client_message_event_t*)event)->data.data32[2]);
						xcb_configure_window(x_connection, icons.back(), XCB_CONFIG_WINDOW_WIDTH | XCB_CONFIG_WINDOW_HEIGHT, (uint32_t[2]){ icon_size, icon_size });
					}
				}
				double icon_center_x = bar_width / 2 - icon_size / 2;
				constexpr uint16_t icon_spacing = icon_size / 2 + item_spacing / 2;
				size_t icon_number = 0;
				double icon_center_y = bar_height / 2 - icon_size / 2;
				if (icons.size() == 0)
					cairo_set_source_rgb(cairo, cbar_color(cbar_inactive));
				else
					cairo_set_source_rgb(cairo, cbar_color(cbar_active));
				do
				{
					double icon_x = icon_center_x;
					if (icons.size() != 0)
					{
						if (icons.size() > 1)
						{
							if (icon_number % 2 == 0)
								icon_x -= icon_spacing * (icons.size() - icon_number - 1);
							else
								icon_x += icon_spacing * (icons.size() - icon_number);
						}
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
				time_t seconds = time(nullptr);
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
			event = xcb_poll_for_event(x_connection);
		}
		XCloseDisplay(display);
	}
}
