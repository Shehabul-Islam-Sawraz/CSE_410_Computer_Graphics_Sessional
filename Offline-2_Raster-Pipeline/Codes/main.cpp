#include <bits/stdc++.h>
#include "Triangle.h"
#include "Color.h"

using namespace std;

#define SCENE_FILE "../Test-Cases/1/scene.txt"
#define CONFIG_FILE "../Test-Cases/1/config.txt"
#define STAGE1_FILE "../Test-Cases/1/mystage1.txt"
#define STAGE2_FILE "../Test-Cases/1/mystage2.txt"
#define STAGE3_FILE "../Test-Cases/1/mystage3.txt"
#define Z_BUFFER_FILE "../Test-Cases/1/my_z_buffer.txt"
#define BMP_FILE "../Test-Cases/1/myout.bmp"

static unsigned long int g_seed = 1;

inline int random()
{
    g_seed = (214013 * g_seed + 2531011);
    return (g_seed >> 16) & 0x7FFF;
}

int main()
{
    ifstream scene, config;
    ofstream stage;

    double eyeX, eyeY, eyeZ;
    double lookX, lookY, lookZ;
    double upX, upY, upZ;
    double fovY, aspectRatio, near, far;

    int noOfTriangles = 0;

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
        // cout << cmd << endl;

        if (cmd == "triangle")
        {
            PointVector point1, point2, point3;
            scene >> point1.x >> point1.y >> point1.z;
            scene >> point2.x >> point2.y >> point2.z;
            scene >> point3.x >> point3.y >> point3.z;

            point1 = st.top() * point1;
            point2 = st.top() * point2;
            point3 = st.top() * point3;

            point1.scale();
            point2.scale();
            point3.scale();

            stage << point1;
            stage << point2;
            stage << point3;
            stage << endl;

            noOfTriangles++;
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

    // Stage 1 End
    // -----------

    // Stage 2 Start
    // -------------

    scene.open(STAGE1_FILE);
    stage.open(STAGE2_FILE);

    PointVector look(lookX, lookY, lookZ);
    PointVector eye(eyeX, eyeY, eyeZ);
    PointVector up(upX, upY, upZ);

    Matrix viewMatrix = Matrix::identityMatrix();
    viewMatrix = viewMatrix.viewTransformationMatrix(look, eye, up);

    for (int n = 1; n <= noOfTriangles; n++)
    {
        PointVector point1, point2, point3;

        scene >> point1.x >> point1.y >> point1.z;
        scene >> point2.x >> point2.y >> point2.z;
        scene >> point3.x >> point3.y >> point3.z;

        point1 = viewMatrix * point1;
        point2 = viewMatrix * point2;
        point3 = viewMatrix * point3;

        point1.scale();
        point2.scale();
        point3.scale();

        stage << point1;
        stage << point2;
        stage << point3;
        stage << endl;
    }

    scene.close();
    stage.close();

    // Stage 2 End
    // -----------

    // Stage 3 Start
    // -------------

    scene.open(STAGE2_FILE);
    stage.open(STAGE3_FILE);

    Matrix projectMatrix = Matrix::identityMatrix();
    projectMatrix = projectMatrix.projectionMatrix(fovY, aspectRatio, near, far);

    for (int n = 1; n <= noOfTriangles; n++)
    {
        PointVector point1, point2, point3;

        scene >> point1.x >> point1.y >> point1.z;
        scene >> point2.x >> point2.y >> point2.z;
        scene >> point3.x >> point3.y >> point3.z;

        point1 = projectMatrix * point1;
        point2 = projectMatrix * point2;
        point3 = projectMatrix * point3;

        point1.scale();
        point2.scale();
        point3.scale();

        stage << point1;
        stage << point2;
        stage << point3;
        stage << endl;
    }

    scene.close();
    stage.close();

    // Stage 3 End
    // -----------

    // Stage 4 Starts
    // --------------

    config.open(CONFIG_FILE); // Input file
    scene.open(STAGE3_FILE);  // Input file

    stage.open(Z_BUFFER_FILE); // Output file

    int screen_width, screen_height;
    config >> screen_width >> screen_height;

    double min_x = -1, min_y = -1, z_front_limit = -1;
    double max_x = 1, max_y = 1, z_rear_limit = 1;

    vector<Triangle> triangles;
    for (int i = 0; i < noOfTriangles; i++)
    {
        PointVector point1, point2, point3;

        scene >> point1.x >> point1.y >> point1.z;
        scene >> point2.x >> point2.y >> point2.z;
        scene >> point3.x >> point3.y >> point3.z;

        triangles.push_back(Triangle(point1, point2, point3));
    }

    /**
        Create a pixel mapping between the x-y range values and the Screen_Width X Screen_height range.
        dx = (right limit - left limit along X-axis) / Screen_Width
        dy = (top limit - bottom limit along Y-axis) / Screen_Height
        Besides, specify Top_Y and Left_X values.
        Top_Y = top limit along Y-axis - dy/2
        Left_X = left limit along X-axis + dx/2
    */

    double dx = (max_x - min_x) / screen_width;
    double dy = (max_y - min_y) / screen_height;
    double Top_Y = (max_y - dy / 2.0);
    double Bottom_Y = (min_y + dy / 2.0);
    double Left_X = (min_x + dx / 2.0);
    double Right_X = (max_x - dx / 2.0);

    /**
     Create a z-buffer, a two dimensional array of Screen_Width X Screen_Height dimension.
     Initialize all the values in z-buffer with z_max. In the aforementioned examples, z_max = 2.0.
     The memory for z-buffer should be dynamically allocated (using STL is allowed).
    */

    vector<vector<double>> z_buffer;
    vector<vector<Color>> frame_buffer;

    for (int i = 0; i < screen_height; i++)
    {
        vector<double> temp_buffer;
        vector<Color> temp_frame;

        for (int j = 0; j < screen_width; j++)
        {
            temp_buffer.push_back(z_rear_limit);
            temp_frame.push_back(Color::setBlack());
        }

        z_buffer.push_back(temp_buffer);
        frame_buffer.push_back(temp_frame);
    }

    for (Triangle triangle : triangles)
    {
        double tri_max_y = triangle.getMaxY();
        double tri_min_y = triangle.getMinY();

        // Clipping Start
        // --------------
        int top_scanline_index, bottom_scanline_index;

        if (tri_max_y >= Top_Y)
        {
            top_scanline_index = 0;
        }
        else
        {
            top_scanline_index = int(round((Top_Y - tri_max_y) / dy));
        }

        if (tri_min_y <= Bottom_Y)
        {
            bottom_scanline_index = screen_height;
        }
        else
        {
            bottom_scanline_index = screen_height - int(round(tri_min_y - Bottom_Y) / dy);
        }

        // Clipping End
        // ------------
    }

    return 0;
}