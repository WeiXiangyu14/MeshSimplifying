#include "edge.h"

edge::edge(int rr, int ss)
{
    u = rr;
    v = ss;
    if (u > v)
    {
        int tmp = u;
        u = v;
        v = tmp;
    }
    Delta = 0;
}

edge::~edge()
{

}

bool edge::operator <(const edge&a) const
{
    if (Delta < a.Delta) return true;
    if (Delta == a.Delta)
    {
        if(u < a.u) return true;
        if(u == a.u && v < a.v) return true;
    }
    return false;
}
