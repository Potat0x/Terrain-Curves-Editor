#include <SFML/Graphics.hpp>
#include <iostream>
#include <fstream>
using namespace std;

class Segment
{
    sf::Vector2f p1, p2;
    sf::RectangleShape line;
    float angle, length;

public:
    Segment(){}
    Segment(sf::Vector2f pa, sf::Vector2f pb):p1(pa), p2(pb)
    {
        float a = abs(p1.x-p2.x);
        float b = abs(p1.y-p2.y);
        length = sqrt(pow(a, 2) + pow(b, 2));
        angle = atan2(p1.y-p2.y,p1.x-p2.x);

        line.setSize(sf::Vector2f(length, 1));
        line.setOrigin(line.getLocalBounds().width/2.0f, line.getLocalBounds().height/2.0f);
        line.setPosition((p1.x+p2.x)/2.0f, (p1.y+p2.y)/2.0f);
        line.setRotation(angle* 180.0f / 3.1415926f);
        line.setFillColor(sf::Color(247,217,159));
    }

    void draw(sf::RenderWindow & window)
    {
        window.draw(line);
    }
};

void load(vector<Segment> & segments)
{
    float ax, ay, bx, by;
    fstream file;
    file.open("exported.txt");

    if(file.good())
    {
        while(!file.eof())
        {
            file>>ax>>ay>>bx>>by;
            segments.push_back(Segment(sf::Vector2f(ax, ay), sf::Vector2f(bx, by)));
        }
        file.close();
    }
    else cout<<"fail"<<endl;
}
int main()
{
    sf::RenderWindow window(sf::VideoMode(800, 600), "");

    vector<Segment>segments;
    load(segments);

    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        if (event.type == sf::Event::Closed) window.close();

        window.clear();

        for(int i = 0; i < segments.size(); i++)
        {
            segments.at(i).draw(window);
        }
        window.display();
        sf::sleep(sf::milliseconds(10));
    }
}
