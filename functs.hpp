#ifndef functs_hpp
#define functs_hpp
#include <SFML/Graphics.hpp>
#include <math.h>
#include <sstream>
#include <iostream>
#include <fstream>
#include <sys/stat.h>

using namespace std;

template <class Arg>
string number_to_string(const Arg & x)
{
    stringstream ss;
    ss<<x;
    return ss.str();
}

template <class Ret>
Ret string_to_number(const string & x)
{
    stringstream ss;
    ss<<x;
    Ret t;
    ss>>t;
    return t;
}

bool create_new_file(const string & filename);
sf::Color rand_color();
unsigned int r_255();
#endif // functs_hpp
