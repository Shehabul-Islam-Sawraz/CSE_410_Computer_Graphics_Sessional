#include <bits/stdc++.h>
#include "Triangle.h"
#include "bitmap_image.hpp"

using namespace std;

#define SCENE_FILE "../Test-Cases/3/scene.txt"
#define CONFIG_FILE "../Test-Cases/3/config.txt"
#define STAGE1_FILE "../Test-Cases/3/mystage1.txt"
#define STAGE2_FILE "../Test-Cases/3/mystage2.txt"
#define STAGE3_FILE "../Test-Cases/3/mystage3.txt"
#define Z_BUFFER_FILE "../Test-Cases/3/my_z_buffer.txt"
#define BMP_FILE "../Test-Cases/3/myout.bmp"

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
        // Set color for each triangle
        triangle.setColor(random(), random(), random());

        double tri_max_y = triangle.getMaxY();
        double tri_min_y = triangle.getMinY();
        // cout << tri_max_y << " " << tri_min_y << endl;

        // Clipping of y value Start
        // -------------------------
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
        // cout << top_scanline_index << " " << bottom_scanline_index << endl;

        // Clipping End
        // ------------

        for (int row = top_scanline_index; row < bottom_scanline_index; row++)
        {
            double middle_val_y = Top_Y - row * dy;

            PointVector intersectionPoints[3];
            // Initialize intersectiong points with dummy values where point.y=middle_val_y
            for (int x = 0; x < 3; x++)
            {
                intersectionPoints[x] = PointVector(INFINITY, middle_val_y, INFINITY);
            }

            // Find the values of x,z of intersectiong points of the three side of triangles with current row
            for (int ind = 0; ind < 3; ind++)
            {
                PointVector vertex1 = triangle.vertex[ind % 3];
                PointVector vertex2 = triangle.vertex[(ind + 1) % 3];

                // If values of y of two vertex is same then no intersection will be found
                if (vertex1.y != vertex2.y)
                {
                    /*
                        We know x,y,z of two vertex. We want to know the value of x & z, where y = middle_val_y
                                              x1 - x2
                        x = x1 + (y - y1) x __________
                                              y1 - y2
                    */
                    double val_x = vertex1.x + (middle_val_y - vertex1.y) * ((vertex1.x - vertex2.x) / (vertex1.y - vertex2.y));
                    double val_z = vertex1.z + (middle_val_y - vertex1.y) * ((vertex1.z - vertex2.z) / (vertex1.y - vertex2.y));

                    intersectionPoints[ind].x = val_x;
                    intersectionPoints[ind].z = val_z;
                }
            }

            // Now, check for invalid values of intersecting points
            for (int ind = 0; ind < 3; ind++)
            {
                PointVector vertex1 = triangle.vertex[ind % 3];
                PointVector vertex2 = triangle.vertex[(ind + 1) % 3];

                if (intersectionPoints[ind].x != INFINITY)
                {
                    if ((intersectionPoints[ind].x > max(vertex1.x, vertex2.x)) || (intersectionPoints[ind].x < min(vertex1.x, vertex2.x)))
                    {
                        intersectionPoints[ind].x = INFINITY;
                    }
                }

                if (intersectionPoints[ind].z != INFINITY)
                {
                    if ((intersectionPoints[ind].z > max(vertex1.z, vertex2.z)) || (intersectionPoints[ind].z < min(vertex1.z, vertex2.z)))
                    {
                        intersectionPoints[ind].z = INFINITY;
                    }
                }
            }

            int min_x_index = -1, max_x_index = -1;
            double tri_max_x = -INFINITY, tri_min_x = INFINITY;

            for (int ind = 0; ind < 3; ind++)
            {
                if (intersectionPoints[ind].x != INFINITY)
                {
                    if (intersectionPoints[ind].x < tri_min_x)
                    {
                        min_x_index = ind;
                        tri_min_x = intersectionPoints[ind].x;
                    }

                    if (intersectionPoints[ind].x > tri_max_x)
                    {
                        max_x_index = ind;
                        tri_max_x = intersectionPoints[ind].x;
                    }
                }
            }

            // cout << tri_max_x << " " << tri_min_x << endl;
            // cout << Left_X << " " << Right_X << endl;

            // Clipping of x value start
            // -------------------------

            int left_intersecting_col, right_intersecting_col;

            if (tri_min_x <= Left_X)
            {
                left_intersecting_col = 0;
            }
            else
            {
                left_intersecting_col = int(round((tri_min_x - Left_X) / dx));
            }

            if (tri_max_x >= Right_X)
            {
                right_intersecting_col = screen_width;
            }
            else
            {
                right_intersecting_col = screen_width - int(round((Right_X - tri_max_x) / dx));
            }

            // cout << left_intersecting_col << " " << right_intersecting_col << endl;

            // Clipping End
            // ------------

            // Calculating the constant term for change by dx
            // ----------------------------------------------

            double x1 = intersectionPoints[min_x_index].x;
            double x2 = intersectionPoints[max_x_index].x;
            double z1 = intersectionPoints[min_x_index].z;
            double z2 = intersectionPoints[max_x_index].z;

            double const_change_val = ((z1 - z2) / (x1 - x2)) * dx;

            // ----------------------------------------------

            for (int col = left_intersecting_col; col < right_intersecting_col; col++)
            {
                double val_x = Left_X + col * dx;
                double val_z;

                if (col == left_intersecting_col)
                {
                    val_z = z1 + (z1 - z2) * ((val_x - x1) / (x1 - x2));
                }
                else
                {
                    val_z += const_change_val;
                }

                if (val_z > z_front_limit)
                {
                    if (val_z < z_buffer[row][col])
                    {
                        z_buffer[row][col] = val_z;
                        frame_buffer[row][col] = triangle.triangleColor;
                    }
                }
            }
        }
    }

    // Creating bitmap image

    bitmap_image image(screen_width, screen_height);

    for (int row = 0; row < screen_height; row++)
    {
        for (int col = 0; col < screen_width; col++)
        {
            if (z_buffer[row][col] < z_rear_limit)
            {
                stage << fixed << setprecision(6) << z_buffer[row][col] << "\t";
            }

            image.set_pixel(col, row, frame_buffer[row][col].r, frame_buffer[row][col].g, frame_buffer[row][col].b);
        }
        stage << endl;
    }

    image.save_image(BMP_FILE);

    scene.close();
    config.close();
    stage.close();

    // Stage 4 Ends
    // ------------

    // Free Memory Starts
    // ------------------

    for (int i = 0; i < screen_height; i++)
    {
        z_buffer[i].clear();
        z_buffer[i].shrink_to_fit();
        frame_buffer[i].clear();
        frame_buffer[i].shrink_to_fit();
    }

    z_buffer.clear();
    z_buffer.shrink_to_fit();
    frame_buffer.clear();
    frame_buffer.shrink_to_fit();

    // Free Memory Ends
    // ----------------

    return 0;
}