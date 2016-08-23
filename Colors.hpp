#ifndef colors_hpp
#define color_hpp
#include <SFML/Graphics.hpp>

class Colors
{
    sf::Color * color;//array
    Colors();
    Colors(const Colors & c){}
    ~Colors();

public:
    static Colors & get()
    {
        static Colors singleton;
        return singleton;
    }

    enum Item
    {
        CURVE,
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

    sf::Color getColor(Item item);

};

#endif // colors_hpp
