#include <bits/stdc++.h>
#include <wayland-client-protocol.h>
#include <sdbus-c++/sdbus-c++.h>
double mouse_x;
constexpr double cwbar_width = 1920;
constexpr double cwbar_center = cwbar_width / 2.;
constexpr double cwbar_height = 33;
int8_t show_date;
int8_t should_redraw;
#include "StatusNotifierItem_proxy.hpp"
#include "StatusNotifierWatcher_adaptor.hpp"
#include "nkk.hpp"
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
struct status_notifier_bus
{
	std::unique_ptr<sdbus::IConnection> connection;
	struct StatusNotifierItem_client* client;
	std::vector<uint8_t> icon;
};
std::vector<status_notifier_bus*> status_notifier_buses;
//td: implement `resize_icon`
struct StatusNotifierItem_client : sdbus::ProxyInterfaces<org::kde::StatusNotifierItem_proxy>
{
	StatusNotifierItem_client(sdbus::IConnection& connection, sdbus::ServiceName service, sdbus::ObjectPath path) : ProxyInterfaces(connection, service, path)
	{
		registerProxy();
	}
	void onNewTitle(){}
	void onNewIcon()
	{
		status_notifier_bus* bus = *std::find_if(status_notifier_buses.begin(), status_notifier_buses.end(), [&](status_notifier_bus* entry){return entry->client == this;});
		bus->icon.resize(std::get<2>(IconPixmap().at(0)).size());
		memcpy(bus->icon.data(), std::get<2>(IconPixmap().at(0)).data(), bus->icon.size());
	}
	void onNewAttentionIcon(){}
	void onNewOverlayIcon(){}
	void onNewToolTip(){}
	void onNewStatus(const std::string& status){}
};
struct StatusNotifierWatcher_server : sdbus::AdaptorInterfaces<org::kde::StatusNotifierWatcher_adaptor>
{
	StatusNotifierWatcher_server(sdbus::IConnection& connection, sdbus::ObjectPath path) : AdaptorInterfaces(connection, path)
	{
		registerAdaptor();
	}
	void RegisterStatusNotifierItem(const std::string& service)
	{
		status_notifier_bus* bus = new status_notifier_bus;
		bus->connection = sdbus::createSessionBusConnection();
		bus->client = new StatusNotifierItem_client(*bus->connection, sdbus::ServiceName(service.c_str()), sdbus::ObjectPath("/StatusNotifierItem"));
		bus->icon.resize(std::get<2>(bus->client->IconPixmap().at(0)).size());
		memcpy(bus->icon.data(), std::get<2>(bus->client->IconPixmap().at(0)).data(), bus->icon.size());
		status_notifier_buses.push_back(bus);
	}
	void RegisterStatusNotifierHost(const std::string& service){}
	int32_t ProtocolVersion()
	{
		return 0;
	}
	std::vector<std::string> RegisteredStatusNotifierItems()
	{
		return {};
	}
	bool IsStatusNotifierHostRegistered()
	{
		return 1;
	}
};
void draw(nkk_win* window, cairo_t* context)
{
	cairo_set_source_rgb(context, cwbar_red(cwbar_background) / 255., cwbar_green(cwbar_background) / 255., cwbar_blue(cwbar_background) / 255.);
	cairo_paint(context);
	nkk_window_set_font(window, "notosansmono Medium 13px");
	char formatted[sizeof("[ ??.??.???? ]")];
	//left
	{
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
	}
	//center
	{
		static std::unique_ptr<sdbus::IConnection> connection = sdbus::createBusConnection(sdbus::ServiceName("org.kde.StatusNotifierWatcher"));
		static StatusNotifierWatcher_server server(*connection, sdbus::ObjectPath("/StatusNotifierWatcher"));
		auto process_events = [](sdbus::IConnection* connection) -> void
		{
			process_event_label:
			{
				if (connection->processPendingEvent() == 1) goto process_event_label;
			}
		};
		process_events(connection.get());
		if (status_notifier_buses.size() == 0)
		{
			draw_underline(context, cwbar_center - 8, cwbar_center + 8, cwbar_inactive);
		}
		else
		{
			decltype(status_notifier_buses)::size_type bus_number = 0;
			traverse_buses_label:
			{
				status_notifier_bus* bus = status_notifier_buses.at(bus_number);
				process_events(bus->connection.get());
				//td: draw bitmap
				double icon_x_left = cwbar_center - 8;
				if (status_notifier_buses.size() != 1)
				{
					if (bus_number % 2 == 0) icon_x_left -= 13 * (status_notifier_buses.size() - bus_number - 1);
					else icon_x_left += 13 * (status_notifier_buses.size() - bus_number);
				}
				double icon_x_right = icon_x_left + 16;
				draw_underline(context, icon_x_left, icon_x_right, cwbar_active);
				if (mouse_x != 0)
				{
					try
					{	
						if (within_box(icon_x_left, icon_x_right) == 1) bus->client->Activate(0, 0);
					}
					catch(...){}
				}
				if (++bus_number != status_notifier_buses.size()) goto traverse_buses_label;
				bus_number = 0;
				invalidate_buses_label:
				{
					try
					{
						status_notifier_buses.at(bus_number++)->client->Category();
					}
					catch(...)
					{
						bus = status_notifier_buses.at(bus_number);
						delete bus->client;
						bus->connection.reset();
						bus->icon.clear();
						status_notifier_buses.erase(status_notifier_buses.begin() + bus_number);
					}
					if (bus_number != status_notifier_buses.size()) goto invalidate_buses_label;
				}
			}
		}
	}
	//right
	{
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
					show_date = 1;
					goto show_date_label;
				}
				draw_text(context, formatted, 2, cwbar_inactive);
			}
			else
			{
				sprintf(formatted, "[ %.2d.%.2d.%.2d ]", local_time->tm_mday, 1 + local_time->tm_mon, 1900 + local_time->tm_year);
				if (within_text_extents(context, formatted) == 1)
				{
					show_date = 0;
					goto show_date_label;
				}
				draw_text(context, formatted, 2, cwbar_active);
			}
		}
	}
	should_redraw = 1;
}
void mouse_down(nkk_win* window, uint32_t modifiers, uint32_t button, double x, double y)
{
	mouse_x = x;
}
void* draw_routine(void* window)
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
int32_t main()
{
	nkk_layer_config layer = { NkkLayerBelow, (uint32_t)cwbar_width, (uint32_t)cwbar_height, NkkLayerAnchorTop | NkkLayerAnchorLeft | NkkLayerAnchorRight, (int32_t)cwbar_height };
	nkk_display* display = nkk_display_open(nullptr, nullptr);
	nkk_win* window = nkk_layer_window_create(display, &layer);
	nkk_window_on_draw(window, draw);
	nkk_window_on_mousedown(window, mouse_down);
	pthread_t thread;
	pthread_create(&thread, nullptr, draw_routine, window);
	pthread_detach(thread);
	nkk_display_main_loop(display);
	nkk_display_close(display);
}
