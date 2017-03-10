#ifndef EDGE_H
#define EDGE_H
#include "point.h"

class edge
{
public:
    int u, v;
    double Delta;
    int f1;
    int f2;

    edge(int rr, int ss);
    ~edge();
    bool operator < (const edge&a) const;
};

#endif // EDGE_H
