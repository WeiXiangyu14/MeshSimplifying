#include "point.h"

point::point()
{
    exist=true;
    for(int i=0;i<4;i++)
        for(int j=0;j<4;j++)
            Q[i][j] = 0;
}

point::~point()
{

}

void point::setpos(double xx, double yy, double zz)
{
    x = xx, y = yy, z = zz;
}

void point::addFace(int f1)
{
    neighborFace.insert(f1);
}

void point::addPoint(int p1)
{
    neighborPoint.insert(p1);
}

