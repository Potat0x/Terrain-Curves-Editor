#ifndef inputs_hpp
#define inputs_hpp

#include <SFML/System.hpp>
#include <SFML/Graphics.hpp>

/*
    class storing informations about inputs
    for better performance
*/

class Inputs
{
    Inputs() {};
    Inputs(const Inputs & ) {};
    sf::Vector2f mouse_pos;

public:
    static Inputs & getInputs()
    {
        static Inputs singleton;
        return singleton;
    }

    static sf::RenderWindow * win;
    bool mouse_state[sf::Mouse::ButtonCount];
    /*enum Mouse
    {
        MOUSE_LEFT = sf::Mouse::Left,
        MOUSE_RIGHT = sf::Mouse::Right,
        MOUSE_MIDDLE = sf::Mouse::Middle
    };*/

    /*
    enum
    {
        MOUSE_LEFT,
        MOUSE_MID,
        MOUSE_RIGHT,
        KEY_X,
        KEY_DEL,
        KEY_N,
        KEY_L,
        KEY_LCTRL,
        KEY_LSHIFT,
    };*/

    void update();
    bool get(sf::Mouse::Button button);
    sf::Vector2f mousePos();
    sf::Vector2f mousePosOnWindow();

};

#endif // inputs_hpp
