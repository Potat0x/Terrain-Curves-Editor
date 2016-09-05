#ifndef fpscounter_hpp
#define fpscounter_hpp

#include <SFML/System.hpp>

class FPSCounter
{
    unsigned int tmpfps;
    unsigned int fps;
    sf::Clock clock;

public:
    FPSCounter();
    unsigned int get();
    void update();
};

#endif // fpscounter_hpp
