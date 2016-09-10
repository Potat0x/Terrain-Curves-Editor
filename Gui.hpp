#ifndef gui_hpp
#define gui_hpp
#include <SFGUI/SFGUI.hpp>
#include <fstream>
#include "Editor.hpp"
#include "FileDialog/FileDialog.hpp"
#include "functs.hpp"
#include <functional>
#include "App.hpp"

using namespace sfg;
using namespace std;

class Editor;
class Point;

class Gui
{
    SFGUI sfgui;
    ////
    Desktop desktop;
    sf::Clock update_clock;
    ////
    Box::Ptr main_menu_box;
    Button::Ptr file, edit, view, settings, help, about;
    //
    Box::Ptr file_box;
    Button::Ptr create, open, save, save_as, generate;
    void show_filemenu(bool s);
    //
    Box::Ptr edit_box;
    Button::Ptr undo, redo, copy, paste, clear_history;
    void show_editmenu(bool s);
    //
    Box::Ptr view_box;
    CheckButton::Ptr link, curve, points, segments, grid;
    void show_viewmenu(bool s);
    Window::Ptr view_window;
    //
    Window::Ptr settings_window;
    Box::Ptr settings_box;
    Box::Ptr settings_lines[5];
    CheckButton::Ptr use_grid;
    Entry::Ptr grid_value;

    Label::Ptr curve_step, segment_step;
    Entry::Ptr curve_step_entry, segment_step_entry;
    Button::Ptr s_apply, s_close;
    Separator::Ptr s_sep;
    void show_settingsmenu(bool s);
    //
    Window::Ptr info_window;
    Box::Ptr info_box;
    Label::Ptr info_label;
    Button::Ptr info_button;
    void hide_info();
    //
    Window::Ptr status_window;
    Box::Ptr status_box;
    Label::Ptr status_label[4];//0=filename   1=points    2=segments    3=position
    Label::Ptr fps_label;
    //
    Window::Ptr uch_window;//unsaved changes window
    Box::Ptr uch_box;
    Box::Ptr uch_box2;
    Label::Ptr uch_label;
    Button::Ptr uch_buttons[3];//0 - yes, 1 - no, 2 - cancel

    bool tmp_use_grid;//tmp_ variables to restore when user discard changes
    float tmp_segment_step;
    int tmp_grid;
    float last_nonzero_step;

    void update_settings();
    void load_tmp_settings();

    void restore_settings_from_tmp();
    void apply_settings();
    //
    Window::Ptr about_window;
    Box::Ptr about_box;
    Label::Ptr about_label_t;
    Label::Ptr about_label;
    void show_about_dialog(bool s);
    //
    Button::Ptr testb;
    Window::Ptr help_window;
    Box::Ptr help_box;
    Label::Ptr help_label;
    void show_help_dialog(bool s);
    //
    void load_b_view();
    /////////// File menu
    Window::Ptr new_file_window;
    Entry::Ptr new_file_entry;
    Label::Ptr new_file_label;
    void set_label();

    Box::Ptr new_file_box;
    Box::Ptr new_file_box_lines[4];
    Button::Ptr nf_apply, nf_close, nf_browse;
    Separator::Ptr nf_sep;

    void create_new_file_apply();
    void create_new_file();
    void save_file_as();
    void save_and_exit_app();

public:
    FileDialog filedialog;
    Gui(sf::RenderWindow & sfml_window);
    void update_view();
    void update(const unsigned int & points, const unsigned int & segments, const string & filename);
    void update_fps(const int & fps);
    void window_resize();
    void draw();
    void handle_event(sf::Event & event);
    void show_unsaved_ch_window(const string & filename);
    void show_info(const string & title, const string & message);
    void create_new_file_after_dialog();

    void switch_grid_usage();
    void save_file();
    void show_new_file_window(bool s);
    void open_file();
    void export_to_file();
};

#endif // gui_hpp
