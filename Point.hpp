#ifndef point_hpp
#define point_hpp
#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>
#include "Link.hpp"
#include "Inputs.hpp"
#include "Colors.hpp"
#include "Editor.hpp"

using namespace std;

class Link;
class Point : public std::enable_shared_from_this<Point>
{

public:
static bool after_group;
    static vector<shared_ptr<Point>>pressed_ptrs;
    static vector<shared_ptr<Point>>selected_ptrs;

    static shared_ptr<Point>node_ptr;
    unsigned int ident;

    vector<Link>connected;
    static bool use_grid;
    static sf::Vector2i grid;
    bool selected;
    bool pressed;
    static bool onmouse;//cursor above any point

    Point(sf::Vector2f pos);
    ~Point();
    static void init();

private:
    bool is_node;
    bool mpm_released;
    bool lpm_released;
    bool lpm_pressed;
    bool ppm_released;
    bool ppm_pressed;

    sf::Vector2f diff;
    sf::RectangleShape shape;

public:
    void draw(sf::RenderWindow * win);
    bool select_press(bool can_press = true);
    void move();
    bool mouse_contain();
    sf::Vector2f getPosition();
    void set_on_grid_position();
    void connect_this(const shared_ptr<Point>& ptr1, const shared_ptr<Point>& ptr2);
    void setPressed(bool p);
    void setSelected(bool s);

    static bool connect();
    static void move_group();
    static void calculate_diff();
    static bool check_connection();
    static void erase_pressed(const shared_ptr<Point>& ptr);
    static void erase_selected(const shared_ptr<Point>& ptr);
};

#endif // point_hpp
