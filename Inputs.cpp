#include "Inputs.hpp"
sf::RenderWindow * Inputs::win;

void Inputs::update()
{
    mouse_pos = sf::Vector2f(sf::Mouse::getPosition(* win));

    for(int i = 0; i < sf::Mouse::ButtonCount; i++)
    {
        if(sf::Mouse::isButtonPressed(sf::Mouse::Button(i)))
        {
            mouse_state[i] = true;
        }
        else
        {
            mouse_state[i] = false;
        }
    }
}

bool Inputs::get(sf::Mouse::Button button)
{
    return mouse_state[button];
}

sf::Vector2f Inputs::mousePos()
{
    sf::Vector2f mp;
    mp = sf::Vector2f(win->mapPixelToCoords(sf::Vector2i(mouse_pos)));
    //sf::Vector2f x(mouse_pos);
    //sf::Vector2i d = win->mapCoordsToPixel(x, win->getView());
    return mp;
}

sf::Vector2f Inputs::Inputs::mousePosOnWindow()
{
    return mouse_pos;
}
