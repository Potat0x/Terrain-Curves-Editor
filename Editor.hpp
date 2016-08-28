#ifndef editor_hpp
#define editor_hpp

#include <SFML/Graphics.hpp>
#include <cmath>
#include "functs.hpp"
#include "Gui.hpp"
#include "Point.hpp"
#include <utility>
#include <memory>

using namespace std;
class Gui;
class Link;

class Editor
{
    friend class Gui;
    public:
    shared_ptr<Gui>gui;
private:

    enum GuiOperations
    {
        NONE,
        OPEN_FILE,
        CREATE_NEW_FILE,
        SAVE_AS,
        EXPORT,
        SAVE_AND_EXIT
    };

    GuiOperations current_operation;

    static bool bg_pressed;
    int segments;
    bool draw_con;
    bool draw_lines;
    bool draw_gr;
    bool draw_pts;
    bool draw_cur;

    //vector<Point*>points;
    vector<shared_ptr<Point>>points;

    //vector<vector<pair<Point*, vector<Link>>>>history;//undo-redo
    vector<vector<pair<shared_ptr<Point>, vector<Link>>>>history;//undo-redo

    sf::RectangleShape area_shapes[4];
    sf::RectangleShape area_lines[4];
    int area_type;//select = 1, press = 2
    string file_pr;

    static float draw_step;
    sf::Vector2f drawarea_start;
    int selected_index;
    bool key_addnew_released;
    bool key_addnew_released_curve;
    bool selectall_released, pressall_released;
    bool drawarea_released;
    void area_contain();
    sf::RectangleShape grid_line;
    sf::Vector2i get_grid_bounds();

    float a, b, c, drl_angle;
    sf::RectangleShape line;

    Editor();
    Editor(const Editor &){}
    string current_filename;
    bool unsaved;

public:
    void create_snapshot();
    void undo();

    static Editor & get_Editor();
    void draw();
    void update();
    void unsaved_change();
    bool check_if_unsaved_changes();
    void file_dialog_event();
    void zoom_view(float delta);
    string get_current_filename();

    void select_all();
    void press_all();
    bool move_curve();
    void draw_curve();
    void draw_points();
    void draw_connections();
    void delete_points();
    void select_point();
    void delete_all();
    bool save_to_file();
    void save_segments_to_file(string filename);
    void load_from_file(string filename);
    void add_point_on_cursor();
    void add_curve_on_cursor();
    void draw_grid();
    void area();
    void draw_area();

private:
    void update_title();
    shared_ptr<Point> get_point_by_id(unsigned int id);
    void create_idents();
    void delete_point(const shared_ptr<Point> & ptr);
    void check_connection(const shared_ptr<Point> & test);
    void draw_pixel(sf::Vector2f pos, sf::Color color, float a);
    void draw(const sf::Vector2f& p1, const sf::Vector2f& p2, const sf::Vector2f& p3);
    void draw_line(const sf::Vector2f& p1, const sf::Vector2f& p2, const sf::Color & color);
};

#endif // editor_hpp
