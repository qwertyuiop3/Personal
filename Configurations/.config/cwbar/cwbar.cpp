#include "_nkk.h"
#include <wayland-client-protocol.h>
#include <bits/stdc++.h>
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
int8_t initialized;
int8_t show_date;
void draw(nkk_win* window, cairo_t* context)
{
	cairo_set_source_rgb(context, 0x0B / 255.f, 0x10 / 255.f, 0x11 / 255.f);
	cairo_paint(context);
	nkk_window_set_font(window, "notosansmono 10");
	char formatted[128];
	char* workspaces = shell_output("echo $(hyprctl workspaces | grep -oP 'ID \\K.{2}' | sort -n)");
	char* workspace = shell_output("echo $(hyprctl activeworkspace | grep -oP 'ID \\K.{2}')");
	int32_t active_workspace = atoi(workspace);
	free(workspace);
	workspace = workspaces;
	parse_workspace_label:
	{
		int32_t workspace_parse = atoi(workspace);
		if (workspace_parse == active_workspace)
		{
			sprintf(formatted, "{fg:C4C4C4} [ %d ]", workspace_parse);
		}
		else
		{
			sprintf(formatted, "{fg:4B5251} [ %d ]", workspace_parse);
		}
		nkk_printf(context, 1, formatted, nullptr);
		workspace = strchr(workspace + 1, ' ');
		if (workspace != nullptr)
		{
			goto parse_workspace_label;
		}
	}
	free(workspaces);
	//td: systray, manual cairo
	time_t seconds = time(&seconds);
	tm* local_time = localtime(&seconds);
	if (show_date == 0)
	{
		sprintf(formatted, "{fg:4B5251} [ %.2d:%.2d:%.2d ]", local_time->tm_hour, local_time->tm_min, local_time->tm_sec);
	}
	else
	{
		sprintf(formatted, "{fg:C4C4C4} [ %.2d.%.2d.%.2d ]", local_time->tm_mday, 1 + local_time->tm_mon, 1900 + local_time->tm_year);
	}
	nkk_printf(context, 1, formatted, nullptr);
	char* capital = shell_output("cat /sys/class/leds/input*::capslock/brightness");
	char* language = shell_output("echo $(hyprctl devices | grep -o Russian)");
	if (capital[0] == '0')
	{
		sprintf(formatted, "{fg:4B5251} [ %s ]", language[0] == 0 ? "English" : language);
	}
	else
	{
		sprintf(formatted, "{fg:C4C4C4} [ %s ]", language[0] == 0 ? "English" : language);
	}
	nkk_printf(context, 1, formatted, nullptr);
	free(language);
	free(capital);
	initialized = 1;
}
void mouse_down(nkk_win* window, uint32_t modifiers, uint32_t button, double x, double y)
{
	show_date ^= 1;
}
nkk_win* window;
nkk_display* display;
void* routine(void* parameter)
{
	redraw_label:
	{
		if (initialized == 1)
		{
			nkk_window_redraw(window);
			wl_display_flush(*(wl_display**)display);
		}
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
		goto redraw_label;
	}
	return nullptr;
}
int32_t main()
{
	nkk_layer_config layer = { NkkLayerAbove, 0, 33, NkkLayerAnchorTop | NkkLayerAnchorLeft | NkkLayerAnchorRight, 33 };
	window = nkk_layer_window_create(display = nkk_display_open(nullptr, nullptr), &layer);
	nkk_window_on_draw(window, draw);
	nkk_window_on_mousedown(window, mouse_down);
	pthread_t thread;
	pthread_create(&thread, nullptr, routine, nullptr);
	pthread_detach(thread);
	nkk_display_main_loop(display);
	nkk_display_close(display);
	return 0;
}
