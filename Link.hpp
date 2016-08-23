#ifndef link_hpp
#define link_hpp

#include "Point.hpp"
class Point;
class Link
{
    public:
    Point *node_ptr1;
    Point *node_ptr2;

    Link(){};
    Link(Point*ptr1, Point*ptr2);
};

#endif // link_hpp
