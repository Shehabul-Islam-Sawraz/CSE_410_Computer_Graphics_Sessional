#include <bits/stdc++.h>
#include "PointVector.h"

using namespace std;

#define MATRIX_SIZE 4
#define PI acos(-1)
#define degToRad(angle) (PI * angle / 180.0)
#define radToDeg(angle) ((180 * angle) / PI)

class Matrix
{
public:
    double matrix[MATRIX_SIZE][MATRIX_SIZE];

    Matrix()
    {
        for (int i = 0; i < MATRIX_SIZE; i++)
        {
            for (int j = 0; j < MATRIX_SIZE; j++)
            {
                this->matrix[i][j] = 0.0;
            }
        }
    }

    Matrix(const Matrix &m)
    {
        for (int i = 0; i < MATRIX_SIZE; i++)
        {
            for (int j = 0; j < MATRIX_SIZE; j++)
            {
                this->matrix[i][j] = m.matrix[i][j];
            }
        }
    }
    static Matrix identityMatrix()
    {
        Matrix temp;
        for (int i = 0; i < 4; i++)
        {
            temp.matrix[i][i] = 1.0;
        }
        return temp;
    }

    static Matrix translateMatrix(double translateX, double translateY, double translateZ)
    {
        Matrix translate = identityMatrix();
        translate.matrix[0][MATRIX_SIZE - 1] = translateX;
        translate.matrix[1][MATRIX_SIZE - 1] = translateY;
        translate.matrix[2][MATRIX_SIZE - 1] = translateZ;

        return translate;
    }

    static Matrix scaleMatrix(double scaleX, double scaleY, double scaleZ)
    {
        Matrix scale = identityMatrix();
        scale.matrix[0][0] = scaleX;
        scale.matrix[1][1] = scaleY;
        scale.matrix[2][2] = scaleZ;

        return scale;
    }

    static PointVector RodriguezFormula(PointVector x, PointVector a, double angle)
    {
        angle = degToRad(angle);
        PointVector point = x * cos(angle) + a * ((a.dotProduct(x)) * (1 - cos(angle))) + (a.crossProduct(x)) * sin(angle);
        return point;
    }

    static Matrix rotationMatrix(double angle, double ax, double ay, double az)
    {
        Matrix temp = identityMatrix();

        PointVector point(ax, ay, az);
        point.normalize();

        PointVector point1 = RodriguezFormula(PointVector(1, 0, 0), point, angle);
        PointVector point2 = RodriguezFormula(PointVector(0, 1, 0), point, angle);
        PointVector point3 = RodriguezFormula(PointVector(0, 0, 1), point, angle);

        temp.matrix[0][0] = point1.x;
        temp.matrix[1][0] = point1.y;
        temp.matrix[2][0] = point1.z;

        temp.matrix[0][1] = point2.x;
        temp.matrix[1][1] = point2.y;
        temp.matrix[2][1] = point2.z;

        temp.matrix[0][2] = point3.x;
        temp.matrix[1][2] = point3.y;
        temp.matrix[2][2] = point3.z;

        return temp;
    }

    PointVector operator*(PointVector point)
    {
        double ans[MATRIX_SIZE];

        for (int i = 0; i < MATRIX_SIZE; i++)
        {
            ans[i] = 0;
            for (int j = 0; j < MATRIX_SIZE; j++)
            {
                if (j == 0)
                {
                    ans[i] += this->matrix[i][j] * point.x;
                }
                else if (j == 1)
                {
                    ans[i] += this->matrix[i][j] * point.y;
                }
                else if (j == 2)
                {
                    ans[i] += this->matrix[i][j] * point.z;
                }
                else if (j == 3)
                {
                    ans[i] += this->matrix[i][j] * point.w;
                }
            }
        }

        return PointVector(ans[0], ans[1], ans[2], ans[3]);
    }

    Matrix operator*(Matrix mat)
    {
        Matrix temp;
        for (int i = 0; i < 4; i++)
        {
            for (int j = 0; j < 4; j++)
            {
                for (int k = 0; k < 4; k++)
                {
                    temp.matrix[i][j] += this->matrix[i][k] * mat.matrix[k][j];
                }
            }
        }
        return temp;
    }

    Matrix viewTransformationMatrix(PointVector look, PointVector eye, PointVector up)
    {
        PointVector l = look - eye;
        l.normalize();
        PointVector r = l.crossProduct(up);
        r.normalize();
        PointVector u = r.crossProduct(l);

        Matrix T = translateMatrix(-eye.x, -eye.y, -eye.z);
        this->matrix[0][0] = r.x;
        this->matrix[0][1] = r.y;
        this->matrix[0][2] = r.z;

        this->matrix[1][0] = u.x;
        this->matrix[1][1] = u.y;
        this->matrix[1][2] = u.z;

        this->matrix[2][0] = -l.x;
        this->matrix[2][1] = -l.y;
        this->matrix[2][2] = -l.z;

        return *this * T; // V = RT
    }

    Matrix projectionMatrix(double fovY, double aspectRatio, double near, double far)
    {
        double fovX = fovY * aspectRatio;
        double t = near * tan(degToRad(fovY / 2));
        double r = near * tan(degToRad(fovX / 2));

        this->matrix[0][0] = near / r;
        this->matrix[1][1] = near / t;
        this->matrix[2][2] = -(far + near) / (far - near);
        this->matrix[2][3] = -(2 * far * near) / (far - near);
        this->matrix[3][2] = -1;
        this->matrix[3][3] = 0;

        return *this;
    }
};