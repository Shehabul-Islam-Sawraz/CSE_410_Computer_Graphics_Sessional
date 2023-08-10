#include "Matrix.h"

class Triangle
{
public:
    PointVector vertex[3];

    Triangle() {}

    Triangle(PointVector v1, PointVector v2, PointVector v3)
    {
        vertex[0] = v1;
        vertex[1] = v2;
        vertex[2] = v3;
    }

    double getMaxX()
    {
        return max(max(vertex[0].x, vertex[1].x), vertex[2].x);
    }

    double getMinX()
    {
        return min(min(vertex[0].x, vertex[1].x), vertex[2].x);
    }

    double getMaxY()
    {
        return max(max(vertex[0].y, vertex[1].y), vertex[2].y);
    }

    double getMinY()
    {
        return min(min(vertex[0].y, vertex[1].y), vertex[2].y);
    }
};