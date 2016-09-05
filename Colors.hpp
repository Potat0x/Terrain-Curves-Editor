#ifndef colors_hpp
#define color_hpp
#include <SFML/Graphics.hpp>
#include <memory>

using namespace std;

class Colors
{
    unique_ptr<sf::Color[]>color;
    Colors();
    Colors(const Colors & c) {}

public:
    static Colors & get()
    {
        static Colors singleton;
        return singleton;
    }

    enum Item
    {
        CURVE,
        LINK,
        PIX,
        POINT,
        POINT_PRESSED,
        POINT_SELECTED,
        AREA_PRESSED,
        AREA_SELECTED,
        NODE,
        GRID,
        BACKGROUND,
        COLORS_COUNT
    };

    sf::Color getColor(const Item & item);
};

#endif // colors_hpp
