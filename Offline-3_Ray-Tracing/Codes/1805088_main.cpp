#include <bits/stdc++.h>
#include <GL/glut.h>
#include <windows.h>
#include <string.h>
#include "1805088_utils.h"

using namespace std;

GLdouble near_distance;
GLdouble far_distance;
GLdouble fov_x;
GLdouble fov_y;
GLdouble aspect_ratio;
int level_of_recursion = 0;
int screen_size = 0;
bool draw_axes, texture_mode, texture_loaded;

PointVector pos, l, r, u;
Object *checkBoard;

vector<Object *> objects;

#define GL_WINDOW_HEIGHT 700
#define GL_WINDOW_WIDTH 700
#define INPUT_FILE "../Assignment-RayTracer/scene.txt"

/* Draw axes: X in Red, Y in Green and Z in Blue */
void drawAxes()
{
    if (draw_axes)
    {
        glLineWidth(3);
        glBegin(GL_LINES);
        glColor3f(1, 0, 0); // Red
        // X axis
        glVertex3f(GL_WINDOW_WIDTH, 0, 0);
        glVertex3f(-GL_WINDOW_WIDTH, 0, 0);

        glColor3f(0, 1, 0); // Green
        // Y axis
        glVertex3f(0, GL_WINDOW_HEIGHT, 0);
        glVertex3f(0, -GL_WINDOW_HEIGHT, 0);

        glColor3f(0, 0, 1); // Blue
        // Z axis
        glVertex3f(0, 0, GL_WINDOW_HEIGHT);
        glVertex3f(0, 0, -GL_WINDOW_HEIGHT);
        glEnd();
    }
}

void load_data()
{
    ifstream input;
    input.open(INPUT_FILE);

    if (!input.is_open())
    {
        cout << "ERROR: Can't open file " << INPUT_FILE << endl;
        exit(0);
    }

    input >> near_distance >> far_distance >> fov_y >> aspect_ratio;
    input >> level_of_recursion >> screen_size;

    fov_x = aspect_ratio * fov_y;

    double checkboard_width, cb_ambient, cb_diffuse, cb_reflection;
    input >> checkboard_width >> cb_ambient >> cb_diffuse >> cb_reflection;

    if (cb_ambient + cb_diffuse + cb_reflection != 1.0)
    {
        cout << "'cb_ambient + cb_diffuse + cb_reflection' must be equal to 1.0" << endl;
        exit(0);
    }

    Object *object;
    int noOfObjects;
    string objectType;

    input >> noOfObjects;

    for (int i = 0; i < noOfObjects; i++)
    {
        input >> objectType;

        if (objectType == "sphere")
        {
            PointVector center;
            double radius;
            Color sphereColor;
            double ambient, diffuse, specular, reflection;
            int shininess;

            input >> center.x >> center.y >> center.z;
            input >> radius;
            input >> sphereColor.r >> sphereColor.g >> sphereColor.b;
            input >> ambient >> diffuse >> specular >> reflection;
            input >> shininess;

            object = new Sphere(center, radius);
            object->setColor(sphereColor);
            object->setCoEfficients(ambient, diffuse, specular, reflection);
            object->setShine(shininess);
            objects.push_back(object);
        }
    }
}

void animate()
{
    // codes for any changes in Models, Camera
    glutPostRedisplay();
}

void initGL(void)
{
    draw_axes = true;
    texture_mode = false;
    texture_loaded = false;

    glClearColor(0, 0, 0, 0); // Set background color to Black

    glEnable(GL_POINT_SMOOTH);
    glEnable(GL_LINE_SMOOTH);

    // load the PROJECTION matrix
    glMatrixMode(GL_PROJECTION);

    // initialize the matrix
    glLoadIdentity();

    // give PERSPECTIVE parameters
    gluPerspective(fov_y, aspect_ratio, near_distance, far_distance);
}

void display()
{
    // clear the display
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(0, 0, 0, 0); // color black
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // load the correct matrix -- MODEL-VIEW matrix
    glMatrixMode(GL_MODELVIEW);

    // initialize the matrix
    glLoadIdentity();

    // now give three info
    // 1. where is the camera (viewer)?
    // 2. where is the camera looking?
    // 3. Which direction is the camera's UP direction?

    gluLookAt(pos.x, pos.y, pos.z,
              pos.x + l.x, pos.y + l.y, pos.z + l.z,
              u.x, u.y, u.z);

    // again select MODEL-VIEW
    glMatrixMode(GL_MODELVIEW);

    drawAxes();

    glutSwapBuffers();
}

int main(int argc, char **argv)
{
    pos.x = 200;
    pos.y = 200;
    pos.z = 0;

    l.x = -1 / sqrt(2.00);
    l.y = -1 / sqrt(2.00);
    l.z = 0;

    u.x = 0;
    u.y = 0;
    u.z = 1;

    r.x = -1 / sqrt(2.00);
    r.y = 1 / sqrt(2.00);
    r.z = 0;

    load_data();

    glutInit(&argc, argv);
    glutInitWindowSize(GL_WINDOW_WIDTH, GL_WINDOW_HEIGHT);
    glutInitWindowPosition(0, 0);
    glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGB); // Depth, Double buffer, RGB color

    glutCreateWindow("1805088 - Ray Tracing");
    initGL();

    glEnable(GL_DEPTH_TEST); // enable Depth Testing

    glutDisplayFunc(display); // display callback function
    glutIdleFunc(animate);    // what you want to do in the idle time (when no drawing is occuring)

    // glutKeyboardFunc(keyboardListener);
    // glutSpecialFunc(specialKeyListener);
    // glutMouseFunc(mouseListener);

    glutMainLoop(); // The main loop of OpenGL

    return 0;
}
