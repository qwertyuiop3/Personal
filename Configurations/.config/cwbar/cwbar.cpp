#include <bits/stdc++.h>
#include <wayland-client-protocol.h>
#include <sdbus-c++/sdbus-c++.h>
#include "nkk.hpp"
double mouse_x;
constexpr double cwbar_width = 1920;
constexpr double cwbar_height = 33;
#include "colors-cwbar.hpp"
#include "draw.hpp"
char* shell_output(char* command)
{
	char* line = nullptr;
	size_t length;
	FILE* file = popen(command, "r");
	getline(&line, &length, file);
	line[strlen(line) - 1] = 0;
	pclose(file);
	return line;
}
int8_t show_date;
int8_t should_redraw;
void draw(nkk_win* window, cairo_t* context)
{
	cairo_set_source_rgb(context, cwbar_red(cwbar_background) / 255., cwbar_green(cwbar_background) / 255., cwbar_blue(cwbar_background) / 255.);
	cairo_paint(context);
	nkk_window_set_font(window, "notosansmono Medium 13px");
	char formatted[128];
	char* workspaces = shell_output("echo $(hyprctl --batch 'activeworkspace;workspaces' | grep -oP 'ID \\K.{2}')");
	int8_t active_workspaces[11] = { };
	char* workspace = strchr(workspaces, ' ');
	parse_workspaces_label:
	{
		active_workspaces[atoi(workspace)] = 1;
		workspace = strchr(workspace + 1, ' ');
		if (workspace != nullptr) goto parse_workspaces_label;
	}
	int32_t active_workspace = atoi(workspaces);
	free(workspaces);
	int32_t current_workspace = 1;
	draw_workspaces_label:
	{
		if (active_workspaces[current_workspace] == 1)
		{
			sprintf(formatted, "[ %d ]", current_workspace);
			if (current_workspace == active_workspace) draw_text(context, formatted, 0, cwbar_active);
			else draw_text(context, formatted, 0, cwbar_inactive);
		}
		if (++current_workspace != 11) goto draw_workspaces_label;
	}
	//td: systray
	char* capital = shell_output("cat /sys/class/leds/input*::capslock/brightness");
	char* language = shell_output("echo $(hyprctl devices | grep -o Russian)");
	sprintf(formatted, "[ %s ]", language[0] == 0 ? "English" : language);
	if (capital[0] == '0') draw_text(context, formatted, 1, cwbar_inactive);
	else draw_text(context, formatted, 1, cwbar_active);
	free(language);
	free(capital);
	time_t seconds = time(&seconds);
	tm* local_time = localtime(&seconds);
	show_date_label:
	{
		if (show_date == 0)
		{
			sprintf(formatted, "[ %.2d:%.2d:%.2d ]", local_time->tm_hour, local_time->tm_min, local_time->tm_sec);
			if (within_text_extents(context, formatted) == 1)
			{
				show_date = mouse_x = 1;
				goto show_date_label;
			}
			draw_text(context, formatted, 2, cwbar_inactive);
		}
		else
		{
			sprintf(formatted, "[ %.2d.%.2d.%.2d ]", local_time->tm_mday, 1 + local_time->tm_mon, 1900 + local_time->tm_year);
			if (within_text_extents(context, formatted) == 1)
			{
				show_date = mouse_x = 0;
				goto show_date_label;
			}
			draw_text(context, formatted, 2, cwbar_active);
		}
	}
	should_redraw = 1;
}
void mouse_down(nkk_win* window, uint32_t modifiers, uint32_t button, double x, double y)
{
	mouse_x = x;
}
void* routine(void* window)
{
	redraw_label:
	{
		if (should_redraw == 1)
		{
			nkk_window_redraw((nkk_win*)window);
			wl_display_flush(**(wl_display***)((uintptr_t)window + 24));
		}
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
		goto redraw_label;
	}
	return nullptr;
}
sdbus::IObject* g_concatenator{};
void concatenate(sdbus::MethodCall call)
{
	printf("RegisterStatusNotifierItem\n");
	fflush(stdout);
}
int32_t main()
{
	//td: systray
	if (1) //note: dbus-monitor to debug
	{
		sdbus::ServiceName serviceName{"org.kde.StatusNotifierWatcher"};
		auto connection = sdbus::createBusConnection(serviceName);
		sdbus::ObjectPath objectPath{"/StatusNotifierWatcher"};
		auto concatenator = sdbus::createObject(*connection, std::move(objectPath));
		g_concatenator = concatenator.get();
		sdbus::InterfaceName interfaceName{"org.kde.StatusNotifierWatcher"};
		concatenator->addVTable(sdbus::MethodVTableItem{sdbus::MethodName{"RegisterStatusNotifierItem"}, sdbus::Signature{"s"}, {}, sdbus::Signature{"s"}, {}, &concatenate, {}}).forInterface(interfaceName);
		connection->enterEventLoop();
	}
	nkk_layer_config layer = { NkkLayerBelow, (uint32_t)cwbar_width, (uint32_t)cwbar_height, NkkLayerAnchorTop | NkkLayerAnchorLeft | NkkLayerAnchorRight, (int32_t)cwbar_height };
	nkk_display* display = nkk_display_open(nullptr, nullptr);
	nkk_win* window = nkk_layer_window_create(display, &layer);
	nkk_window_on_draw(window, draw);
	nkk_window_on_mousedown(window, mouse_down);
	pthread_t thread;
	pthread_create(&thread, nullptr, routine, window);
	pthread_detach(thread);
	nkk_display_main_loop(display);
	nkk_display_close(display);
}
