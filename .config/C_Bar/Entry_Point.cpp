#include <cstdio>

#include <vector>

#include <cstring>

#include <xcb/xcb.h>

#define Color_0 725009

#include <xcb/xcb_ewmh.h>

#include <cairo-xcb.h>

#include <X11/XKBlib.h>

#define Color_1 11 / 255.f, 16 / 255.f, 17 / 255.f

void Shell_Output_Multiple(char* Command, std::vector<char*>& Output_List)
{
  char* Line = nullptr;

  size_t Line_Length;

  FILE* File = popen(Command, "r");

  Get_Line_Label:
  {
    if (getline(&Line, &Line_Length, File) != -1)
    {
      char Line_Length = strlen(Line);

      Line[Line_Length - 1] = 0;

      char* Allocated_Line = (char*)malloc(Line_Length);

      memcpy(Allocated_Line, Line, Line_Length);

      Output_List.push_back(Allocated_Line);

      goto Get_Line_Label;
    }
  }

  free(Line);

  pclose(File);
}

char* Shell_Output(char* Command)
{
  char* Line = nullptr;

  size_t Line_Length;

  FILE* File = popen(Command, "r");

  getline(&Line, &Line_Length, File);

  Line[strlen(Line) - 1] = 0;

  pclose(File);

  return Line;
}

#include <algorithm>

int8_t Compare(char* A, char* B)
{
  if (atoi(A) >= atoi(B))
  {
    return 0;
  }

  return 1;
}

#define Color_2 196 / 255.f, 196 / 255.f, 196 / 255.f

#define Color_3 75 / 255.f, 82 / 255.f, 81 / 255.f

ssize_t Shell_Output_Multiple_Count(char* Command, char* Ignored_Line)
{
  char* Line = nullptr;

  size_t Line_Length;

  FILE* File = popen(Command, "r");

  ssize_t Lines_Amount = 0;

  Get_Line_Label:
  {
    if (getline(&Line, &Line_Length, File) != -1)
    {
      char Line_Length = strlen(Line);

      Line[Line_Length - 1] = 0;

      if (Line != Ignored_Line)
      {
        Lines_Amount += 1;
      }

      goto Get_Line_Label;
    }
  }

  free(Line);

  pclose(File);

  return Lines_Amount;
}

#include <thread>

int32_t main()
{
  xcb_connection_t* X_Connection = xcb_connect(nullptr, nullptr);

  xcb_screen_t* X_Screen = xcb_setup_roots_iterator(xcb_get_setup(X_Connection)).data;

  uint8_t X_Screen_Depth = X_Screen->root_depth;

  xcb_gcontext_t X_Graphics_Context_Pixel_Map = xcb_generate_id(X_Connection);

  xcb_drawable_t X_Drawable = xcb_generate_id(X_Connection);

  xcb_window_t X_Screen_Root = X_Screen->root;

  uint16_t X_Screen_Width = X_Screen->width_in_pixels;

  constexpr uint16_t X_Screen_Height = 33;

  xcb_visualid_t X_Screen_Visual = X_Screen->root_visual;

  constexpr uint32_t Value_List[2] =
  {
    Color_0,

    XCB_EVENT_MASK_BUTTON_PRESS | XCB_EVENT_MASK_EXPOSURE | XCB_EVENT_MASK_SUBSTRUCTURE_NOTIFY
  };

  xcb_create_window(X_Connection, X_Screen_Depth, X_Drawable, X_Screen_Root, 0, 0, X_Screen_Width, X_Screen_Height, 0, XCB_WINDOW_CLASS_INPUT_OUTPUT, X_Screen_Visual, XCB_CW_BACK_PIXEL | XCB_CW_EVENT_MASK, Value_List);

  xcb_ewmh_connection_t X_Extended_Window_Manager_Hints_Connection;

  xcb_ewmh_init_atoms_replies(&X_Extended_Window_Manager_Hints_Connection, xcb_ewmh_init_atoms(X_Connection, &X_Extended_Window_Manager_Hints_Connection), nullptr);

  xcb_atom_t X_Atom = X_Extended_Window_Manager_Hints_Connection._NET_WM_WINDOW_TYPE_DOCK;

  xcb_ewmh_set_wm_window_type(&X_Extended_Window_Manager_Hints_Connection, X_Drawable, sizeof(X_Atom) / sizeof(xcb_atom_t), &X_Atom);

  X_Atom = X_Extended_Window_Manager_Hints_Connection._NET_WM_STATE_ABOVE;

  xcb_ewmh_set_wm_state(&X_Extended_Window_Manager_Hints_Connection, X_Drawable, sizeof(X_Atom) / sizeof(xcb_atom_t), &X_Atom);

  xcb_ewmh_wm_strut_partial_t X_Extended_Window_Manager_Hints_Window_Manager_Strut_Partial =
  {
    0,

    0,

    X_Screen_Height,

    0,

    0,

    0,

    0,

    0,

    0,

    X_Screen_Width,

    0,

    0
  };

  xcb_ewmh_set_wm_strut_partial(&X_Extended_Window_Manager_Hints_Connection, X_Drawable, X_Extended_Window_Manager_Hints_Window_Manager_Strut_Partial);

  xcb_map_window(X_Connection, X_Drawable);

  xcb_flush(X_Connection);

  xcb_visualtype_t* X_Visual_Type;

  xcb_depth_iterator_t X_Depth_Iterator = xcb_screen_allowed_depths_iterator(X_Screen);

  X_Depth_Iterator_Label:
  {
    if (X_Depth_Iterator.rem != 0)
    {
      xcb_visualtype_iterator_t X_Visual_Type_Iterator = xcb_depth_visuals_iterator(X_Depth_Iterator.data);

      X_Visual_Type_Iterator_Label:
      {
        if (X_Visual_Type_Iterator.rem != 0)
        {
            if (X_Screen_Visual == X_Visual_Type_Iterator.data->visual_id)
            {
              X_Visual_Type = X_Visual_Type_Iterator.data;

              goto Stop_Searching_Visual_Type_Label;
            }

            xcb_visualtype_next(&X_Visual_Type_Iterator);

            goto X_Visual_Type_Iterator_Label;
        }
      }

      xcb_depth_next(&X_Depth_Iterator);

      goto X_Depth_Iterator_Label;
    }
  }

  Stop_Searching_Visual_Type_Label:
  {

  }

  cairo_surface_t* Cairo_Surface = cairo_xcb_surface_create(X_Connection, X_Drawable, X_Visual_Type, X_Screen_Width, X_Screen_Height);

  cairo_t* Cairo = cairo_create(Cairo_Surface);

  cairo_font_options_t* Cairo_Font_Options = cairo_font_options_create();

  cairo_font_options_set_hint_style(Cairo_Font_Options, CAIRO_HINT_STYLE_FULL);

  cairo_font_options_set_antialias(Cairo_Font_Options, CAIRO_ANTIALIAS_GRAY);

  cairo_set_font_options(Cairo, Cairo_Font_Options);

  cairo_select_font_face(Cairo, "notosansmono", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL);

  cairo_set_font_size(Cairo, 13);

  cairo_set_line_width(Cairo, 1);

  xcb_intern_atom_reply_t* X_Intern_Atom_Reply = xcb_intern_atom_reply(X_Connection, xcb_intern_atom(X_Connection, 0, sizeof("_NET_SYSTEM_TRAY_S0") - 1, "_NET_SYSTEM_TRAY_S0"), nullptr);

  X_Atom = X_Intern_Atom_Reply->atom;

  xcb_set_selection_owner(X_Connection, X_Drawable, X_Atom, 0);

  xcb_get_selection_owner_reply(X_Connection, xcb_get_selection_owner(X_Connection, X_Atom), nullptr);

  std::vector<xcb_window_t> X_Window_Icon_List;

  Display* X_Display = XOpenDisplay(nullptr);

  int8_t Time_Representation_Variant = 0;

  Draw_Label:
  {
    xcb_generic_event_t* X_Generic_Event = xcb_poll_for_event(X_Connection);

    if (X_Generic_Event != nullptr)
    {
      cairo_set_source_rgb(Cairo, Color_1);

      cairo_paint(Cairo);

      //Left
      {
        size_t Occupied_Desktop_Number = 0;

        std::vector<char*> Occupied_Desktop_List;

        Shell_Output_Multiple((char*)"bspc query -D -d .occupied --names", Occupied_Desktop_List);

        size_t Occupied_Desktops_Amount = Occupied_Desktop_List.size();

        char* Focused_Desktop_Number = Shell_Output((char*)"bspc query -D -d --names");

        int32_t Converted_Focused_Desktop_Number = atoi(Focused_Desktop_Number);

        Remove_Focused_Desktop_From_Occupied_Desktop_List_Label:
        {
          if (Occupied_Desktop_Number != Occupied_Desktops_Amount)
          {
            char* To_Draw_Occupied_Desktop_Number = Occupied_Desktop_List.at(Occupied_Desktop_Number);

            if (Converted_Focused_Desktop_Number == atoi(To_Draw_Occupied_Desktop_Number))
            {
              free(To_Draw_Occupied_Desktop_Number);

              Occupied_Desktop_List.erase(Occupied_Desktop_List.begin() + Occupied_Desktop_Number);
            }
            else
            {
              Occupied_Desktop_Number += 1;

              goto Remove_Focused_Desktop_From_Occupied_Desktop_List_Label;
            }
          }
        }

        Occupied_Desktop_List.push_back(Focused_Desktop_Number);

        std::sort(Occupied_Desktop_List.begin(), Occupied_Desktop_List.end(), Compare);

        size_t Desktop_Number = 0;

        size_t Desktops_Amount = Occupied_Desktop_List.size();

        Draw_Desktops_Label:
        {
          if (Desktop_Number != Desktops_Amount)
          {
            char* To_Draw_Desktop_Number = Occupied_Desktop_List.at(Desktop_Number);

            int32_t Converted_To_Draw_Desktop_Number = atoi(To_Draw_Desktop_Number);

            if (Converted_Focused_Desktop_Number == Converted_To_Draw_Desktop_Number)
            {
              cairo_set_source_rgb(Cairo, Color_2);
            }
            else
            {
              cairo_set_source_rgb(Cairo, Color_3);
            }

            cairo_move_to(Cairo, 7 + Desktop_Number * 46, X_Screen_Height / 2 + 3);

            char* Formatted_To_Draw_Desktop_Number = (char*)malloc(strlen(To_Draw_Desktop_Number) + 5);

            free(To_Draw_Desktop_Number);

            sprintf(Formatted_To_Draw_Desktop_Number, "[ %i ]", Converted_To_Draw_Desktop_Number);

            cairo_show_text(Cairo, Formatted_To_Draw_Desktop_Number);

            cairo_text_extents_t Cairo_Text_Extents;

            cairo_text_extents(Cairo, Formatted_To_Draw_Desktop_Number, &Cairo_Text_Extents);

            free(Formatted_To_Draw_Desktop_Number);

            cairo_move_to(Cairo, 10 + Desktop_Number * 46, X_Screen_Height);

            cairo_line_to(Cairo, 10 + Desktop_Number * 46 + Cairo_Text_Extents.width, X_Screen_Height);

            cairo_stroke(Cairo);

            Desktop_Number += 1;

            goto Draw_Desktops_Label;
          }
        }
      }

      uint8_t X_Generic_Event_Identifier = X_Generic_Event->response_type & ~128;

      //Center
      {
        constexpr uint16_t Icon_Size = 16;

        double X_Screen_Width_Icon_Center = X_Screen_Width / 2 - Icon_Size / 2;

        constexpr uint16_t Icon_Spacing = 13;

        double X_Screen_Height_Icon_Center = X_Screen_Height / 2 - Icon_Size / 2;

        if (X_Generic_Event_Identifier == XCB_DESTROY_NOTIFY)
        {
          size_t X_Window_Icon_Number = 0;

          size_t X_Window_Icon_List_Size = X_Window_Icon_List.size();

          xcb_window_t X_Window_Icon = ((xcb_unmap_notify_event_t*)X_Generic_Event)->window;

          Remove_X_Window_Icon_Label:
          {
            if (X_Window_Icon_Number != X_Window_Icon_List_Size)
            {
              if (X_Window_Icon_List.at(X_Window_Icon_Number) == X_Window_Icon)
              {
                X_Window_Icon_List.erase(X_Window_Icon_List.begin() + X_Window_Icon_Number);

                X_Window_Icon_List_Size -= 1;
              }
              else
              {
                X_Window_Icon_Number += 1;

                goto Remove_X_Window_Icon_Label;
              }
            }
          }

          if (X_Window_Icon_List_Size != 0)
          {
            if (X_Window_Icon_List_Size == 1)
            {
              X_Window_Icon = X_Window_Icon_List.at(0);

              xcb_reparent_window(X_Connection, X_Window_Icon, X_Drawable, X_Screen_Width_Icon_Center, X_Screen_Height_Icon_Center);
            }
            else
            {
              X_Window_Icon_Number = 0;

              Destroy_Rearrange_X_Window_Icon_List_Label:
              {
                if (X_Window_Icon_Number != X_Window_Icon_List_Size)
                {
                  X_Window_Icon = X_Window_Icon_List.at(X_Window_Icon_Number);

                  if (X_Window_Icon_Number % 2 == 0)
                  {
                    xcb_reparent_window(X_Connection, X_Window_Icon, X_Drawable, X_Screen_Width_Icon_Center - Icon_Spacing * (X_Window_Icon_List_Size - X_Window_Icon_Number - 1), X_Screen_Height_Icon_Center);
                  }
                  else
                  {
                    xcb_reparent_window(X_Connection, X_Window_Icon, X_Drawable, X_Screen_Width_Icon_Center + Icon_Spacing * (X_Window_Icon_List_Size - X_Window_Icon_Number), X_Screen_Height_Icon_Center);
                  }

                  X_Window_Icon_Number += 1;

                  goto Destroy_Rearrange_X_Window_Icon_List_Label;
                }
              }
            }
          }
        }
        else
        {
          if (X_Generic_Event_Identifier == XCB_CLIENT_MESSAGE)
          {
            xcb_window_t X_Window_Icon = ((xcb_client_message_event_t*)X_Generic_Event)->data.data32[2];

            xcb_query_tree_reply_t* X_Query_Tree_Reply = xcb_query_tree_reply(X_Connection, xcb_query_tree(X_Connection, X_Window_Icon), nullptr);

            if (X_Query_Tree_Reply != nullptr)
            {
              free(X_Query_Tree_Reply);

              X_Window_Icon_List.push_back(X_Window_Icon);

              constexpr uint32_t Value_List[2] =
              {
                Icon_Size,

                Icon_Size
              };

              xcb_configure_window(X_Connection, X_Window_Icon, XCB_CONFIG_WINDOW_WIDTH | XCB_CONFIG_WINDOW_HEIGHT, Value_List);

              size_t X_Window_Icon_List_Size = X_Window_Icon_List.size();

              if (X_Window_Icon_List_Size == 1)
              {
                xcb_reparent_window(X_Connection, X_Window_Icon, X_Drawable, X_Screen_Width_Icon_Center, X_Screen_Height_Icon_Center);

                xcb_map_window(X_Connection, X_Window_Icon);
              }
              else
              {
                size_t X_Window_Icon_Number = 0;

                Create_Rearrange_X_Window_Icon_List_Label:
                {
                  if (X_Window_Icon_Number != X_Window_Icon_List_Size)
                  {
                    X_Window_Icon = X_Window_Icon_List.at(X_Window_Icon_Number);

                    if (X_Window_Icon_Number % 2 == 0)
                    {
                      xcb_reparent_window(X_Connection, X_Window_Icon, X_Drawable, X_Screen_Width_Icon_Center - Icon_Spacing * (X_Window_Icon_List_Size - X_Window_Icon_Number - 1), X_Screen_Height_Icon_Center);
                    }
                    else
                    {
                      xcb_reparent_window(X_Connection, X_Window_Icon, X_Drawable, X_Screen_Width_Icon_Center + Icon_Spacing * (X_Window_Icon_List_Size - X_Window_Icon_Number), X_Screen_Height_Icon_Center);
                    }

                    X_Window_Icon_Number += 1;

                    goto Create_Rearrange_X_Window_Icon_List_Label;
                  }
                }

                xcb_map_window(X_Connection, X_Window_Icon);
              }
            }
          }
        }

        size_t X_Window_Icon_List_Size = X_Window_Icon_List.size();

        if (X_Window_Icon_List_Size == 0)
        {
          cairo_set_source_rgb(Cairo, Color_3);

          cairo_move_to(Cairo, X_Screen_Width_Icon_Center, X_Screen_Height);

          cairo_line_to(Cairo, X_Screen_Width / 2 + Icon_Size / 2, X_Screen_Height);

          cairo_stroke(Cairo);
        }
        else
        {
          cairo_set_source_rgb(Cairo, Color_2);

          if (X_Window_Icon_List_Size == 1)
          {
            double X = X_Screen_Width_Icon_Center;

            cairo_move_to(Cairo, X, X_Screen_Height);

            cairo_line_to(Cairo, X + Icon_Size, X_Screen_Height);

            cairo_stroke(Cairo);
          }
          else
          {
            size_t X_Window_Icon_Number = 0;

            Underline_X_Window_Icon_List_Label:
            {
              if (X_Window_Icon_Number != X_Window_Icon_List_Size)
              {
                if (X_Window_Icon_Number % 2 == 0)
                {
                  double X = X_Screen_Width_Icon_Center - Icon_Spacing * (X_Window_Icon_List_Size - X_Window_Icon_Number - 1);

                  cairo_move_to(Cairo, X, X_Screen_Height);

                  cairo_line_to(Cairo, X + Icon_Size, X_Screen_Height);

                  cairo_stroke(Cairo);
                }
                else
                {
                  double X = X_Screen_Width_Icon_Center + Icon_Spacing * (X_Window_Icon_List_Size - X_Window_Icon_Number);

                  cairo_move_to(Cairo, X, X_Screen_Height);

                  cairo_line_to(Cairo, X + Icon_Size, X_Screen_Height);

                  cairo_stroke(Cairo);
                }

                X_Window_Icon_Number += 1;

                goto Underline_X_Window_Icon_List_Label;
              }
            }
          }
        }
      }

      //Right
      {
        uint32_t X_Indicator_State;

        XkbGetIndicatorState(X_Display, XkbUseCoreKbd, &X_Indicator_State);

        fflush(stdout);

        if ((X_Indicator_State & 1) == 0)
        {
          cairo_set_source_rgb(Cairo, Color_3);
        }
        else
        {
          cairo_set_source_rgb(Cairo, Color_2);
        }

        cairo_text_extents_t Previous_Cairo_Text_Extents;

        if ((X_Indicator_State & 4096) == 0)
        {
          cairo_text_extents(Cairo, "[ English ]", &Previous_Cairo_Text_Extents);

          cairo_move_to(Cairo, X_Screen_Width - Previous_Cairo_Text_Extents.width - 13, X_Screen_Height / 2 + 3);

          cairo_show_text(Cairo, "[ English ]");
        }
        else
        {
          cairo_text_extents(Cairo, "[ Russian ]", &Previous_Cairo_Text_Extents);

          cairo_move_to(Cairo, X_Screen_Width - Previous_Cairo_Text_Extents.width - 13, X_Screen_Height / 2 + 3);

          cairo_show_text(Cairo, "[ Russian ]");
        }

        cairo_move_to(Cairo, X_Screen_Width - Previous_Cairo_Text_Extents.width - 10, X_Screen_Height);

        cairo_line_to(Cairo, X_Screen_Width - 10, X_Screen_Height);

        cairo_stroke(Cairo);

        char* System_Time;

        cairo_text_extents_t Cairo_Text_Extents;

        if (X_Generic_Event_Identifier == XCB_BUTTON_PRESS)
        {
          xcb_button_press_event_t* X_Button_Press_Event = (xcb_button_press_event_t*)X_Generic_Event;

          if (X_Button_Press_Event->detail == 1)
          {
            if (Time_Representation_Variant == 0)
            {
              cairo_set_source_rgb(Cairo, Color_3);

              System_Time = Shell_Output((char*)"echo \"[ $(date +\"%T\") ]\"");

              cairo_text_extents(Cairo, System_Time, &Cairo_Text_Extents);

              free(System_Time);
            }
            else
            {
              cairo_set_source_rgb(Cairo, Color_2);

              System_Time = Shell_Output((char*)"echo \"[ $(date +\"%d.%m.%Y\") ]\"");

              cairo_text_extents(Cairo, System_Time, &Cairo_Text_Extents);

              free(System_Time);
            }

            if (X_Button_Press_Event->event_x >= X_Screen_Width - Previous_Cairo_Text_Extents.width - Cairo_Text_Extents.width - 20)
            {
              if (X_Button_Press_Event->event_x <= X_Screen_Width - Previous_Cairo_Text_Extents.width - 20)
              {
                Time_Representation_Variant ^= 1;
              }
            }
          }
        }

        if (Time_Representation_Variant == 0)
        {
          cairo_set_source_rgb(Cairo, Color_3);

          System_Time = Shell_Output((char*)"echo \"[ $(date +\"%T\") ]\"");
        }
        else
        {
          cairo_set_source_rgb(Cairo, Color_2);

          System_Time = Shell_Output((char*)"echo \"[ $(date +\"%d.%m.%Y\") ]\"");
        }

        cairo_text_extents(Cairo, System_Time, &Cairo_Text_Extents);

        cairo_move_to(Cairo, X_Screen_Width - Previous_Cairo_Text_Extents.width - Cairo_Text_Extents.width - 23, X_Screen_Height / 2 + 3);

        cairo_show_text(Cairo, System_Time);

        free(System_Time);

        cairo_move_to(Cairo, X_Screen_Width - Previous_Cairo_Text_Extents.width - Cairo_Text_Extents.width - 20, X_Screen_Height);

        cairo_line_to(Cairo, X_Screen_Width - Previous_Cairo_Text_Extents.width - 20, X_Screen_Height);

        cairo_stroke(Cairo);
      }

      cairo_surface_flush(Cairo_Surface);

      xcb_flush(X_Connection);

      char* Previous_Focused_Desktop_Number = Shell_Output((char*)"bspc query -D -d --names");

      ssize_t Previous_Occupied_Desktops_Amount = Shell_Output_Multiple_Count((char*)"bspc query -D -d .occupied --names", Previous_Focused_Desktop_Number);

      uint32_t Previous_X_Indicator_State;

      XkbGetIndicatorState(X_Display, XkbUseCoreKbd, &Previous_X_Indicator_State);

      time_t Previous_Time = time(nullptr);

      Wait_For_Change_Label:
      {
        char* Focused_Desktop_Number = Shell_Output((char*)"bspc query -D -d --names");

        if (Focused_Desktop_Number[0] + Focused_Desktop_Number[1] != Previous_Focused_Desktop_Number[0] + Previous_Focused_Desktop_Number[1])
        {
          free(Focused_Desktop_Number);

          xcb_expose_event_t* X_Expose_Event = (xcb_expose_event_t*)malloc(sizeof(xcb_expose_event_t));

          X_Expose_Event->response_type = XCB_EXPOSE;

          xcb_send_event(X_Connection, 0, X_Drawable, XCB_EVENT_MASK_EXPOSURE, (char*)X_Expose_Event);

          free(X_Expose_Event);

          xcb_flush(X_Connection);

          goto Stop_Waiting_For_Change_Label;
        }

        free(Focused_Desktop_Number);

        if (Shell_Output_Multiple_Count((char*)"bspc query -D -d .occupied --names", Previous_Focused_Desktop_Number) != Previous_Occupied_Desktops_Amount)
        {
          xcb_expose_event_t* X_Expose_Event = (xcb_expose_event_t*)malloc(sizeof(xcb_expose_event_t));

          X_Expose_Event->response_type = XCB_EXPOSE;

          xcb_send_event(X_Connection, 0, X_Drawable, XCB_EVENT_MASK_EXPOSURE, (char*)X_Expose_Event);

          free(X_Expose_Event);

          xcb_flush(X_Connection);

          goto Stop_Waiting_For_Change_Label;
        }

        xcb_generic_event_t* X_Generic_Event = xcb_poll_for_event(X_Connection);

        if (X_Generic_Event != nullptr)
        {
          X_Generic_Event_Identifier = X_Generic_Event->response_type & ~128;

          if (X_Generic_Event_Identifier == XCB_BUTTON_PRESS)
          {
            if (((xcb_button_press_event_t*)X_Generic_Event)->detail == 1)
            {
              xcb_send_event(X_Connection, 0, X_Drawable, XCB_EVENT_MASK_BUTTON_PRESS, (char*)X_Generic_Event);

              free(X_Generic_Event);

              xcb_flush(X_Connection);

              goto Stop_Waiting_For_Change_Label;
            }
            else
            {
              free(X_Generic_Event);
            }
          }
          else
          {
            if (X_Generic_Event_Identifier == XCB_EXPOSE)
            {
              xcb_send_event(X_Connection, 0, X_Drawable, XCB_EVENT_MASK_EXPOSURE, (char*)X_Generic_Event);

              free(X_Generic_Event);

              xcb_flush(X_Connection);

              goto Stop_Waiting_For_Change_Label;
            }
            else
            {
              if (X_Generic_Event_Identifier == XCB_DESTROY_NOTIFY)
              {
                xcb_send_event(X_Connection, 0, X_Drawable, XCB_EVENT_MASK_SUBSTRUCTURE_NOTIFY, (char*)X_Generic_Event);

                free(X_Generic_Event);

                xcb_flush(X_Connection);

                goto Stop_Waiting_For_Change_Label;
              }
              else
              {
                if (X_Generic_Event_Identifier == XCB_CLIENT_MESSAGE)
                {
                  xcb_send_event(X_Connection, 0, X_Drawable, XCB_EVENT_MASK_SUBSTRUCTURE_NOTIFY, (char*)X_Generic_Event);

                  free(X_Generic_Event);

                  xcb_flush(X_Connection);

                  goto Stop_Waiting_For_Change_Label;
                }
                else
                {
                  free(X_Generic_Event);
                }
              }
            }
          }
        }

        uint32_t X_Indicator_State;

        XkbGetIndicatorState(X_Display, XkbUseCoreKbd, &X_Indicator_State);

        if (X_Indicator_State != Previous_X_Indicator_State)
        {
          xcb_expose_event_t* X_Expose_Event = (xcb_expose_event_t*)malloc(sizeof(xcb_expose_event_t));

          X_Expose_Event->response_type = XCB_EXPOSE;

          xcb_send_event(X_Connection, 0, X_Drawable, XCB_EVENT_MASK_EXPOSURE, (char*)X_Expose_Event);

          free(X_Expose_Event);

          xcb_flush(X_Connection);

          goto Stop_Waiting_For_Change_Label;
        }

        if (time(nullptr) != Previous_Time)
        {
          xcb_expose_event_t* X_Expose_Event = (xcb_expose_event_t*)malloc(sizeof(xcb_expose_event_t));

          X_Expose_Event->response_type = XCB_EXPOSE;

          xcb_send_event(X_Connection, 0, X_Drawable, XCB_EVENT_MASK_EXPOSURE, (char*)X_Expose_Event);

          free(X_Expose_Event);

          xcb_flush(X_Connection);

          goto Stop_Waiting_For_Change_Label;
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(100));

        goto Wait_For_Change_Label;
      }

      Stop_Waiting_For_Change_Label:
      {

      }

      free(Previous_Focused_Desktop_Number);

      free(X_Generic_Event);
    }

    goto Draw_Label;
  }

  return EXIT_SUCCESS;
}
