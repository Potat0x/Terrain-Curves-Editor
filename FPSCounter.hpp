#ifndef fpscounter_hpp
#define fpscounter_hpp

#include <SFML/System.hpp>

class FPSCounter
{
    int tmpfps;
    int fps;
    sf::Clock clock;

public:
    FPSCounter();
    int get();
    void update();
};

#endif // fpscounter_hpp
