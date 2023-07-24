#include <bits/stdc++.h>

using namespace std;

class PointVector
{
public:
    double x, y, z, w;

    PointVector()
    {
        this->x = 0;
        this->y = 0;
        this->z = 0;
        this->w = 1.0;
    }

    PointVector(double x, double y, double z, double w)
    {
        this->x = x;
        this->y = y;
        this->z = z;
        this->w = w;
    }

    PointVector(double x, double y, double z)
    {
        this->x = x;
        this->y = y;
        this->z = z;
        this->w = 1.0;
    }

    PointVector(const PointVector &point)
    {
        this->x = point.x;
        this->y = point.y;
        this->z = point.z;
        this->w = point.w;
    }

    void normalize()
    {
        double vectorValue = sqrt(x * x + y * y + z * z);
        if (vectorValue == 0.0)
        {
            cout << "Can't normalize the point vector!!" << endl;
            return;
        }

        this->x /= vectorValue;
        this->y /= vectorValue;
        this->z /= vectorValue;
    }
};