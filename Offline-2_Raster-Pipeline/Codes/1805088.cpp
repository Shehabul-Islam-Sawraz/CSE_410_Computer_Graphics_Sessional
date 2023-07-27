#include <bits/stdc++.h>
#include "Matrix.h"

using namespace std;

#define SCENE_FILE "../Test-Cases/1/scene.txt"
#define STAGE1_FILE "../Test-Cases/1/mystage1.txt"
#define STAGE2_FILE "../Test-Cases/1/mystage2.txt"
#define STAGE3_FILE "../Test-Cases/1/mystage3.txt"

int main()
{
    ifstream scene, config;
    ofstream stage;

    double eyeX, eyeY, eyeZ;
    double lookX, lookY, lookZ;
    double upX, upY, upZ;
    double fovY, aspectRatio, near, far;

    // Stage 1 Start
    // -------------
    scene.open(SCENE_FILE);
    stage.open(STAGE1_FILE);

    scene >> eyeX >> eyeY >> eyeZ;
    scene >> lookX >> lookY >> lookZ;
    scene >> upX >> upY >> upZ;
    scene >> fovY >> aspectRatio >> near >> far;

    stack<Matrix> st;
    Matrix identity = Matrix::identityMatrix();
    st.push(identity);

    string cmd;
    while (true)
    {
        scene >> cmd;
        cout << cmd << endl;

        if (cmd == "triangle")
        {
            PointVector point1, point2, point3;
            scene >> point1.x >> point1.y >> point1.z;
            scene >> point2.x >> point2.y >> point2.z;
            scene >> point3.x >> point3.y >> point3.z;

            point1 = st.top() * point1;
            point2 = st.top() * point2;
            point3 = st.top() * point3;

            // point1.scale();
            // point2.scale();
            // point3.scale();

            stage << point1;
            stage << point2;
            stage << point3;
            stage << endl;
        }
        else if (cmd == "translate")
        {
            double tx, ty, tz;
            scene >> tx >> ty >> tz;
            Matrix matrix = Matrix::translateMatrix(tx, ty, tz);
            matrix = st.top() * matrix;
            st.pop();
            st.push(matrix);
        }
        else if (cmd == "scale")
        {
            double sx, sy, sz;
            scene >> sx >> sy >> sz;
            Matrix matrix = Matrix::scaleMatrix(sx, sy, sz);
            matrix = st.top() * matrix;
            st.pop();
            st.push(matrix);
        }
        else if (cmd == "rotate")
        {
            double rx, ry, rz, angle;
            scene >> angle >> rx >> ry >> rz;
            Matrix matrix = Matrix::rotationMatrix(angle, rx, ry, rz);
            matrix = st.top() * matrix;
            st.pop();
            st.push(matrix);
        }
        else if (cmd == "push")
        {
            st.push(st.top());
        }
        else if (cmd == "pop")
        {
            if (st.size() == 1)
            {
                cout << "Error: Stack is empty" << endl;
                break;
            }
            st.pop();
        }
        else if (cmd == "end")
        {
            break;
        }
        else
        {
            cout << "Error: Invalid Command" << endl;
            break;
        }
    }

    scene.close();
    stage.close();

    return 0;
}