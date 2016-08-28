#include "Colors.hpp"

Colors::Colors()
{
    unique_ptr<sf::Color[]> tmp (new sf::Color[COLORS_COUNT]);
    color = move(tmp);
    //color = unique_ptr<sf::Color[]>(new sf::Color[COLORS_COUNT]);

    /*color[CURVE] = sf::Color(5, 155, 190);
    color[LINK] = sf::Color(40, 159, 88, 100);
    color[PIX] = sf::Color(5, 155, 190);
    color[POINT] = sf::Color(40, 159, 88);
    color[POINT_PRESSED] = sf::Color(249, 55, 45);
    color[POINT_SELECTED] = sf::Color(216, 237, 42);
    color[AREA_PRESSED] = color[POINT_PRESSED]*sf::Color(255, 255, 255, 180);
    color[AREA_SELECTED] = color[POINT_SELECTED]*sf::Color(255, 255, 255, 180);
    color[NODE] = sf::Color(255, 0, 139);
    color[GRID] = sf::Color(77, 66, 110, 140);
    color[BACKGROUND] = sf::Color(17, 16, 23);*/

    color[CURVE] = sf::Color(255, 242, 230);
    color[LINK] = sf::Color(82, 255, 82, 100);
    color[PIX] = sf::Color(5, 155, 190);
    color[POINT] = sf::Color(82, 255, 82, 140);
    color[POINT_PRESSED] = sf::Color(255, 51, 0, 200);
    color[POINT_SELECTED] = sf::Color(255, 255, 51, 200);
    color[AREA_PRESSED] = color[POINT_PRESSED];
    color[AREA_SELECTED] = color[POINT_SELECTED];
    color[NODE] = sf::Color(255, 0, 139, 200);
    color[GRID] = sf::Color(231, 242, 236, 40);
    color[BACKGROUND] = sf::Color(3, 5, 5);
}

sf::Color Colors::getColor(Item item)
{
    return color[item];
}

Colors::~Colors()
{
    //delete [] color;
}
