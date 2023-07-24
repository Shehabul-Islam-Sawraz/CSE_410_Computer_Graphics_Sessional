#include <bits/stdc++.h>

using namespace std;

#define MATRIX_SIZE 4

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

    Matrix(const Matrix m)
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

    Matrix translateMatrix(double translateX, double translateY, double translateZ)
    {
        Matrix translate = identityMatrix();
        translate.matrix[0][MATRIX_SIZE - 1] = translateX;
        translate.matrix[1][MATRIX_SIZE - 1] = translateY;
        translate.matrix[2][MATRIX_SIZE - 1] = translateZ;

        return translate;
    }

    Matrix scaleMatrix(double scaleX, double scaleY, double scaleZ)
    {
        Matrix scale = identityMatrix();
        scale.matrix[0][0] = scaleX;
        scale.matrix[1][1] = scaleY;
        scale.matrix[2][2] = scaleZ;

        return scale;
    }
};