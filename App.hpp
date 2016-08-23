#ifndef app_hpp
#define app_hpp

#include "Editor.hpp"
#include "FPSCounter.hpp"

class Gui;
class App
{
    sf::RenderWindow window;
    string app_title;
    FPSCounter fps_counter;
    shared_ptr<Gui>gui;
    sf::View view;
    sf::Vector2f pan_start;
    sf::Vector2f pan_diff;
    bool pan_mouse_mid_released;
    void pan_view();
    void zoom_view(float delta);

    App();
    App(const App&){}

public:
    static App & getApp();
    void run();
    string get_app_title();
    sf::RenderWindow & get_sfml_window();
    void close(bool confirmed = false);
};

#endif // app_hpp
