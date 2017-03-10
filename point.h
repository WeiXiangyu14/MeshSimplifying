#ifndef POINT_H
#define POINT_H
#include <cstdlib>
#include <set>

using namespace std;

class point
{
public:
    double x, y, z;
    double Q[4][4];
    set<int> neighborPoint;
    set<int> neighborFace;
    bool exist;
    point();
    ~point();
    void setpos(double xx, double yy, double zz);
    void addFace(int f1);
    void addPoint(int p1);

};

#endif // POINT_H
