#ifndef functs_hpp
#define functs_hpp
#include <SFML/Graphics.hpp>
#include <math.h>
#include <sstream>
#include <fstream>
#include <sys/stat.h>

using namespace std;

template <class Arg>
string number_to_string(Arg x)
{
    stringstream ss;
    ss<<x;
    return ss.str();
}

template <class Ret>
Ret string_to_number(string x)
{
    stringstream ss;
    ss<<x;
    Ret t;
    ss>>t;
    return t;
}

bool create_new_file(const string filename);
unsigned int r();

sf::Color rand_color();

void sforc(sf::RectangleShape & shape);
#endif // functs_hpp
