#include <windows.h> // for MS Windows
#include <GL/glut.h> // GLUT, include glu.h and gl.h
#include <cmath>
#include <iostream>
#include <vector>

using namespace std;

#define NO_OF_STEP 16
#define CHANGE_PER_STEP (1.0 / NO_OF_STEP)
#define TRIANGLE_LEN 1.0
#define CYLINDER_CENTER_ANGLE (70.5287794 * M_PI / 180.0)
#define SUBDIVISION 3
#define POINTS_PER_ROW ((int)pow(2, SUBDIVISION) + 1) // compute the number of vertices per row, 2^n + 1
#define CAMERA_MOVE_STEP 0.2
#define CAMERA_ROTATE_STEP 5

/* Initialize OpenGL Graphics */
void initGL()
{
    // Set "clearing" or background color
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f); // Black and opaque
    glEnable(GL_DEPTH_TEST);              // Enable depth testing for z-culling
}

// Global variables
GLdouble rot_angle = 0;
// double d = 1.0 / sqrt(3);
double f = 0.0; // As at each step the triangles will converge to Center of Gravity, so this stores
                // how much triangles will converge now.(f = 0 to 1. So converge from 0 to 1/3)
int sphere_rec_depth = 3;
int cylinder_fragment = (1 << sphere_rec_depth);

struct Point
{
public:
    float x, y, z;
    Point() {}
};

// Global variables
struct Point pos; // position of the eye
struct Point l;   // look/forward direction
struct Point r;   // right direction
struct Point u;   // up direction

void buildUnitPositiveX()
{
    const float DEG2RAD = acos(-1) / 180.0f;

    float n1[3]; // normal of longitudinal plane rotating along Y-axis
    float n2[3]; // normal of latitudinal plane rotating along Z-axis
    float v[3];  // direction vector intersecting 2 planes, n1 x n2
    float a1;    // longitudinal angle along Y-axis
    float a2;    // latitudinal angle along Z-axis

    Point vertices[POINTS_PER_ROW + 1][POINTS_PER_ROW + 1];

    // rotate latitudinal plane from 45 to -45 degrees along Z-axis (top-to-bottom)
    for (unsigned int i = 0; i < POINTS_PER_ROW; ++i)
    {
        // normal for latitudinal plane
        // if latitude angle is 0, then normal vector of latitude plane is n2=(0,1,0)
        // therefore, it is rotating (0,1,0) vector by latitude angle a2
        a2 = DEG2RAD * (45.0f - 90.0f * i / (POINTS_PER_ROW - 1));
        n2[0] = -sin(a2);
        n2[1] = cos(a2);
        n2[2] = 0;

        // rotate longitudinal plane from -45 to 45 along Y-axis (left-to-right)
        for (unsigned int j = 0; j < POINTS_PER_ROW; ++j)
        {
            // normal for longitudinal plane
            // if longitude angle is 0, then normal vector of longitude is n1=(0,0,-1)
            // therefore, it is rotating (0,0,-1) vector by longitude angle a1
            a1 = DEG2RAD * (-45.0f + 90.0f * j / (POINTS_PER_ROW - 1));
            n1[0] = -sin(a1);
            n1[1] = 0;
            n1[2] = -cos(a1);

            // find direction vector of intersected line, n1 x n2
            v[0] = n1[1] * n2[2] - n1[2] * n2[1];
            v[1] = n1[2] * n2[0] - n1[0] * n2[2];
            v[2] = n1[0] * n2[1] - n1[1] * n2[0];

            // normalize direction vector
            float scale = 1 / sqrt(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);
            v[0] *= scale;
            v[1] *= scale;
            v[2] *= scale;

            // add a vertex into array
            Point p;
            p.x = v[0];
            p.y = v[1];
            p.z = v[2];
            vertices[i][j] = p;
        }
    }

    for (int i = 0; i < POINTS_PER_ROW - 1; i++)
    {
        for (int j = 0; j < POINTS_PER_ROW - 1; j++)
        {
            glBegin(GL_QUADS);
            glVertex3f(vertices[i + 1][j].x, vertices[i + 1][j].y, vertices[i + 1][j].z);
            glVertex3f(vertices[i + 1][j + 1].x, vertices[i + 1][j + 1].y, vertices[i + 1][j + 1].z);
            glVertex3f(vertices[i][j + 1].x, vertices[i][j + 1].y, vertices[i][j + 1].z);
            glVertex3f(vertices[i][j].x, vertices[i][j].y, vertices[i][j].z);
            glEnd();
        }
    }
}

void drawSphereSegment(double radius, double translate)
{
    glPushMatrix();
    glTranslated(translate, 0.0, 0.0);
    glScaled(radius, radius, radius);
    buildUnitPositiveX();
    glPopMatrix();
}

/* Draw axes: X in Red, Y in Green and Z in Blue */
void drawAxes()
{
    glLineWidth(3);
    glBegin(GL_LINES);
    glColor3f(1, 0, 0); // Red
    // X axis
    glVertex3f(0, 0, 0);
    glVertex3f(1, 0, 0);

    glColor3f(0, 1, 0); // Green
    // Y axis
    glVertex3f(0, 0, 0);
    glVertex3f(0, 1, 0);

    glColor3f(0, 0, 1); // Blue
    // Z axis
    glVertex3f(0, 0, 0);
    glVertex3f(0, 0, 1);
    glEnd();
}

void drawTriangle(GLdouble a, GLdouble b)
{
    glBegin(GL_TRIANGLES);
    glVertex3d(b, b, a);
    glVertex3d(a, b, b);
    glVertex3d(b, a, b);
    glEnd();
}

void drawCylinder(double translate, double radius, double zScale)
{
    double currx;
    double curry;
    double angle_per_side;
    double angle_per_segment;
    glPushMatrix();
    glTranslated(translate, 0.0, 0.0);
    glScaled(radius, radius, zScale);

    angle_per_segment = CYLINDER_CENTER_ANGLE / (double)cylinder_fragment;
    angle_per_side = -CYLINDER_CENTER_ANGLE / 2.0;
    glBegin(GL_QUAD_STRIP);
    for (int i = 0; i <= cylinder_fragment; i++)
    {
        currx = cos((double)i * angle_per_segment + angle_per_side);
        curry = sin((double)i * angle_per_segment + angle_per_side);
        glVertex3d(currx, curry, 1.0);
        glVertex3d(currx, curry, -1.0);
    }
    glEnd();
    glPopMatrix();
}

void drawOctahedron()
{
    double axis_change = TRIANGLE_LEN - f;
    double converge = f / 3.0; // Converge to center of gravity(COG)
    /** At per step, an axis changed by value f but again converge to COG. So total change for main axis = axis_change+converge*/
    /** Whereas, the other axis will just converge to COG. The variable converge helps to keep the plane same.*/
    // Draw Octahedron
    glPushMatrix();
    // Blue Triangles
    glColor3d(0.0, 1.0, 1.0);
    drawTriangle(axis_change + converge, converge);
    glRotated(180.0, 0.0, 0.0, 1.0);
    drawTriangle(axis_change + converge, converge);
    glRotated(180.0, 1.0, 0.0, 0.0);
    drawTriangle(axis_change + converge, converge);
    glRotated(180.0, 0.0, 0.0, 1.0);
    drawTriangle(axis_change + converge, converge);

    // Pink Triangles
    glColor3d(1.0, 0.0, 1.0);
    glRotated(90.0, 0.0, 0.0, 1.0);
    drawTriangle(axis_change + converge, converge);
    glRotated(180.0, 0.0, 0.0, 1.0);
    drawTriangle(axis_change + converge, converge);
    glRotated(180.0, 1.0, 0.0, 0.0);
    drawTriangle(axis_change + converge, converge);
    glRotated(180.0, 0.0, 0.0, 1.0);
    drawTriangle(axis_change + converge, converge);
    glPopMatrix();

    glColor3d(1.0, 1.0, 0.0);
    glPushMatrix();
    glRotated(45.0, 0.0, 0.0, 1.0);
    glRotated(90.0, 1.0, 0.0, 0.0);
    /** Translate: It replresents how much the the cylinder will be translated from center. As we don't want to
     * keep the plane same for cylinder, we just use axis_change. As equation of triangle for xy plane is x+y=1,
     * so distance from center to cylinder= 1/sqrt(2). So translate at each point = axis_change/sqrt(2).
     *
     * zScale: Thiscontrols the height of the cylinders. Height of cylinders are scaled based on the distance
     * of the cylinders from the center.
     *
     * radius: at inital radius = 0 and at end radius=radius of sphere = 1/sqrt(3).
     * at NO_OF_STEPS radius change = (1/sqrt(3)-0)
     * So, at each step radius change = 1/(NO_OF_STEPS * sqrt(3)) = f/sqrt(3)
     */
    drawCylinder(axis_change / sqrt(2), f / sqrt(3), axis_change / sqrt(2));
    glRotated(90.0, 0.0, 1.0, 0.0);
    drawCylinder(axis_change / sqrt(2), f / sqrt(3), axis_change / sqrt(2));
    glRotated(90.0, 0.0, 1.0, 0.0);
    drawCylinder(axis_change / sqrt(2), f / sqrt(3), axis_change / sqrt(2));
    glRotated(90.0, 0.0, 1.0, 0.0);
    drawCylinder(axis_change / sqrt(2), f / sqrt(3), axis_change / sqrt(2));
    glPopMatrix();
    glPushMatrix();
    glRotated(45.0, 1.0, 0.0, 0.0);
    glRotated(90.0, 0.0, 0.0, 1.0);
    drawCylinder(axis_change / sqrt(2), f / sqrt(3), axis_change / sqrt(2));
    glRotated(90.0, 0.0, 1.0, 0.0);
    drawCylinder(axis_change / sqrt(2), f / sqrt(3), axis_change / sqrt(2));
    glRotated(90.0, 0.0, 1.0, 0.0);
    drawCylinder(axis_change / sqrt(2), f / sqrt(3), axis_change / sqrt(2));
    glRotated(90.0, 0.0, 1.0, 0.0);
    drawCylinder(axis_change / sqrt(2), f / sqrt(3), axis_change / sqrt(2));
    glPopMatrix();
    glPushMatrix();
    glRotated(45.0, 0.0, 1.0, 0.0);
    drawCylinder(axis_change / sqrt(2), f / sqrt(3), axis_change / sqrt(2));
    glRotated(90.0, 0.0, 1.0, 0.0);
    drawCylinder(axis_change / sqrt(2), f / sqrt(3), axis_change / sqrt(2));
    glRotated(90.0, 0.0, 1.0, 0.0);
    drawCylinder(axis_change / sqrt(2), f / sqrt(3), axis_change / sqrt(2));
    glRotated(90.0, 0.0, 1.0, 0.0);
    drawCylinder(axis_change / sqrt(2), f / sqrt(3), axis_change / sqrt(2));
    glPopMatrix();

    glPushMatrix();
    glColor3d(1, 0, 0);
    glRotated(90.0, 0.0, 0.0, 1.0);
    drawSphereSegment(f / sqrt(3), axis_change);
    glRotated(180.0, 0.0, 0.0, 1.0);
    drawSphereSegment(f / sqrt(3), axis_change);
    glColor3d(0.0, 1.0, 0.0);
    glRotated(90.0, 0.0, 0.0, 1.0);
    drawSphereSegment(f / sqrt(3), axis_change);
    glRotated(180.0, 0.0, 0.0, 1.0);
    drawSphereSegment(f / sqrt(3), axis_change);
    glColor3d(0.0, 0.0, 1.0);
    glRotated(90.0, 0.0, 1.0, 0.0);
    drawSphereSegment(f / sqrt(3), axis_change);
    glRotated(180.0, 0.0, 1.0, 0.0);
    drawSphereSegment(f / sqrt(3), axis_change);
    glPopMatrix();
}

void keyboardListener(unsigned char key, int xx, int yy)
{
    double rate = 0.02;
    switch (key)
    {

    case '1':
        r.x = r.x * cos(-rate) + l.x * sin(-rate);
        r.y = r.y * cos(-rate) + l.y * sin(-rate);
        r.z = r.z * cos(-rate) + l.z * sin(-rate);

        l.x = l.x * cos(-rate) - r.x * sin(-rate);
        l.y = l.y * cos(-rate) - r.y * sin(-rate);
        l.z = l.z * cos(-rate) - r.z * sin(-rate);
        break;

    case '2':
        r.x = r.x * cos(rate) + l.x * sin(rate);
        r.y = r.y * cos(rate) + l.y * sin(rate);
        r.z = r.z * cos(rate) + l.z * sin(rate);

        l.x = l.x * cos(rate) - r.x * sin(rate);
        l.y = l.y * cos(rate) - r.y * sin(rate);
        l.z = l.z * cos(rate) - r.z * sin(rate);
        break;

    case '3':
        l.x = l.x * cos(rate) + u.x * sin(rate);
        l.y = l.y * cos(rate) + u.y * sin(rate);
        l.z = l.z * cos(rate) + u.z * sin(rate);

        u.x = u.x * cos(rate) - l.x * sin(rate);
        u.y = u.y * cos(rate) - l.y * sin(rate);
        u.z = u.z * cos(rate) - l.z * sin(rate);
        break;

    case '4':
        l.x = l.x * cos(-rate) + u.x * sin(-rate);
        l.y = l.y * cos(-rate) + u.y * sin(-rate);
        l.z = l.z * cos(-rate) + u.z * sin(-rate);

        u.x = u.x * cos(-rate) - l.x * sin(-rate);
        u.y = u.y * cos(-rate) - l.y * sin(-rate);
        u.z = u.z * cos(-rate) - l.z * sin(-rate);
        break;

    case '5':
        u.x = u.x * cos(-rate) + r.x * sin(-rate);
        u.y = u.y * cos(-rate) + r.y * sin(-rate);
        u.z = u.z * cos(-rate) + r.z * sin(-rate);

        r.x = r.x * cos(-rate) - u.x * sin(-rate);
        r.y = r.y * cos(-rate) - u.y * sin(-rate);
        r.z = r.z * cos(-rate) - u.z * sin(-rate);
        break;

    case '6':
        u.x = u.x * cos(rate) + r.x * sin(rate);
        u.y = u.y * cos(rate) + r.y * sin(rate);
        u.z = u.z * cos(rate) + r.z * sin(rate);

        r.x = r.x * cos(rate) - u.x * sin(rate);
        r.y = r.y * cos(rate) - u.y * sin(rate);
        r.z = r.z * cos(rate) - u.z * sin(rate);
        break;
        // Controlling Object
    case 'a':
        rot_angle -= 10.0; // Rotate the object in the clockwise direction about its own axis
        break;
    case 'd':
        rot_angle += 10.0; // Rotate the object in the counter-clockwise direction about its own axis
        break;
    case ',':
        if (f + CHANGE_PER_STEP <= TRIANGLE_LEN)
        {
            f += CHANGE_PER_STEP;
        }
        break;
    case '.':
        if (f - CHANGE_PER_STEP >= 0)
        {
            f -= CHANGE_PER_STEP;
        }
        break;
    default:
        break;
    }
    glutPostRedisplay();
}

void specialKeyListener(int key, int x, int y)
{
    switch (key)
    {
    case GLUT_KEY_UP: // down arrow key
        pos.x += (l.x / 3);
        pos.y += (l.y / 3);
        pos.z += (l.z / 3);
        break;
    case GLUT_KEY_DOWN: // up arrow key
        pos.x -= (l.x / 3);
        pos.y -= (l.y / 3);
        pos.z -= (l.z / 3);
        break;

    case GLUT_KEY_RIGHT:
        pos.x -= (r.x / 3);
        pos.y -= (r.y / 3);
        pos.z -= (r.z / 3);
        break;
    case GLUT_KEY_LEFT:
        pos.x += (r.x / 3);
        pos.y += (r.y / 3);
        pos.z += (r.z / 3);
        break;

    case GLUT_KEY_PAGE_UP:
        pos.x += (u.x / 3.5);
        pos.y += (u.y / 3.5);
        pos.z += (u.z / 3.5);
        break;
    case GLUT_KEY_PAGE_DOWN:
        pos.x -= (u.x / 3.5);
        pos.y -= (u.y / 3.5);
        pos.z -= (u.z / 3.5);
        break;

    case GLUT_KEY_INSERT:
        break;

    case GLUT_KEY_HOME:
        break;
    case GLUT_KEY_END:
        break;

    default:
        break;
    }
    glutPostRedisplay();
}

/*  Handler for window-repaint event. Call back when the window first appears and
    whenever the window needs to be re-painted. */
void display()
{
    // glClear(GL_COLOR_BUFFER_BIT);            // Clear the color buffer (background)
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW); // To operate on Model-View matrix
    glLoadIdentity();           // Reset the model-view matrix

    // control viewing (or camera)
    gluLookAt(pos.x, pos.y, pos.z,
              pos.x + l.x, pos.y + l.y, pos.z + l.z,
              u.x, u.y, u.z);

    drawAxes();

    glRotated(rot_angle, 0.0, 1.0, 0.0); // Rotate spehere or octahedron by rot_angle

    drawOctahedron();

    glutSwapBuffers(); // Render now
}

/* Handler for window re-size event. Called back when the window first appears and
   whenever the window is re-sized with its new width and height */
void reshapeListener(GLsizei width, GLsizei height)
{ // GLsizei for non-negative integer
    // Compute aspect ratio of the new window
    if (height == 0)
        height = 1; // To prevent divide by 0
    GLfloat aspect = (GLfloat)width / (GLfloat)height;

    // Set the viewport to cover the new window
    glViewport(0, 0, width, height);

    // Set the aspect ratio of the clipping area to match the viewport
    glMatrixMode(GL_PROJECTION); // To operate on the Projection matrix
    glLoadIdentity();            // Reset the projection matrix
    // Enable perspective projection with fovy, aspect, zNear and zFar
    gluPerspective(45.0f, aspect, 0.1f, 100.0f);
}

/* Main function: GLUT runs as a console application starting at main()  */
int main(int argc, char **argv)
{
    pos.x = 2.2;
    pos.y = 2.2;
    pos.z = 2.2;

    l.x = -1 / sqrt(3.0);
    l.y = -1 / sqrt(3.0);
    l.z = -1 / sqrt(3.0);
    u.x = -1 / sqrt(6.0);
    u.y = 2 / sqrt(6.0);
    u.z = -1 / sqrt(6.0);
    r.x = u.y * l.z - u.z * l.y;
    r.y = u.z * l.x - u.x * l.z;
    r.z = u.x * l.y - u.y * l.x;

    glutInit(&argc, argv);                                    // Initialize GLUT
    glutInitWindowSize(640, 640);                             // Set the window's initial width & height
    glutInitWindowPosition(50, 50);                           // Position the window's initial top-left corner
    glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGB); // Depth, Double buffer, RGB color
    glutCreateWindow("OpenGL 3D Drawing");                    // Create a window with the given title
    glutDisplayFunc(display);                                 // Register display callback handler for window re-paint
    glutReshapeFunc(reshapeListener);                         // Register callback handler for window re-shape
    glutKeyboardFunc(keyboardListener);                       // Register callback handler for normal-key event
    glutSpecialFunc(specialKeyListener);                      // Register callback handler for special-key event
    initGL();                                                 // Our own OpenGL initialization
    glutMainLoop();                                           // Enter the event-processing loop
    return 0;
}
