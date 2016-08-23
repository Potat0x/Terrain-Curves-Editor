#include "Colors.hpp"

Colors::Colors()
{
    color = new sf::Color[COLORS_COUNT];

    color[CURVE] = sf::Color(5, 155, 190);
    color[POINT] = sf::Color(40, 159, 88);
    color[POINT_PRESSED] = sf::Color(249, 55, 45);
    color[POINT_SELECTED] = sf::Color(216, 237, 42);
    color[AREA_PRESSED] = color[POINT_PRESSED]*sf::Color(255, 255, 255, 180);
    color[AREA_SELECTED] = color[POINT_SELECTED]*sf::Color(255, 255, 255, 180);
    color[NODE] = sf::Color(255, 0, 139);
    color[GRID] = sf::Color(77, 66, 110, 140);
    color[BACKGROUND] = sf::Color(17, 16, 23);
}

sf::Color Colors::getColor(Item item)
{
    return color[item];
}

Colors::~Colors()
{
    delete [] color;
}
