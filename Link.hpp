#ifndef link_hpp
#define link_hpp

#include "Point.hpp"
#include <memory>

using namespace std;

class Point;
class Link
{
    public:
    shared_ptr<Point>node_ptr1;
    shared_ptr<Point>node_ptr2;

    Link(){};
    Link(const shared_ptr<Point>&ptr1, const shared_ptr<Point>&ptr2);
};

#endif // link_hpp
