#include "FPSCounter.hpp"

FPSCounter::FPSCounter()
{
    tmpfps = 0;
    fps = 0;
    clock.restart();
}

void FPSCounter::update()
{
    if(clock.getElapsedTime().asSeconds() >= 1.0f)
    {
        fps = tmpfps;
        tmpfps = 0;
        clock.restart();
    }
    else
    {
        tmpfps++;
    }
}

int FPSCounter::get()
{
    return fps;
}
