#include "Link.hpp"

Link::Link(const shared_ptr<Point>&ptr1, const shared_ptr<Point>&ptr2)
{
    node_ptr1 = ptr1;
    node_ptr2 = ptr2;
}
