#include <windows.h> // for MS Windows
#include <GL/glut.h> // GLUT, include glu.h and gl.h
#include <cmath>

/* Initialize OpenGL Graphics */
void initGL()
{
    // Set "clearing" or background color
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f); // Black and opaque
    glEnable(GL_DEPTH_TEST);              // Enable depth testing for z-culling
}

// Global variables
GLfloat eyex = 2, eyey = 2, eyez = 2;
GLfloat centerx = 0, centery = 0, centerz = 0;
GLfloat upx = 0, upy = 1, upz = 0;
bool isAxes = true, isCube = false, isPyramid = false;
GLdouble rot_angle = 0;
double d = 1.0 / sqrt(3);
double f = 0.0;

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

oid drawCylinder(double height, double radius, int segments)
{
    double tempx = radius, tempy = 0;
    double currx, curry;
    glBegin(GL_QUADS);
    for (int i = 1; i <= segments; i++)
    {
        double theta = i * 2.0 * M_PI / segments;
        currx = radius * cos(theta);
        curry = radius * sin(theta);

        GLfloat c = (2 + cos(theta)) / 3;
        glColor3f(c, c, c);
        glVertex3f(currx, curry, height / 2);
        glVertex3f(currx, curry, -height / 2);

        glVertex3f(tempx, tempy, -height / 2);
        glVertex3f(tempx, tempy, height / 2);

        tempx = currx;
        tempy = curry;
    }
    glEnd();
}

void drawOctahedron()
{
    // Draw Octahedron
    glPushMatrix();
    glColor3d(0.0, 1.0, 1.0);
    drawTriangle(1, 0);
    glRotated(180.0, 0.0, 0.0, 1.0);
    drawTriangle(1, 0);
    glRotated(180.0, 1.0, 0.0, 0.0);
    drawTriangle(1, 0);
    glRotated(180.0, 0.0, 0.0, 1.0);
    drawTriangle(1, 0);
    glColor3d(1.0, 0.0, 1.0);
    glRotated(90.0, 0.0, 0.0, 1.0);
    drawTriangle(1, 0);
    glRotated(180.0, 0.0, 0.0, 1.0);
    drawTriangle(1, 0);
    glRotated(180.0, 1.0, 0.0, 0.0);
    drawTriangle(1, 0);
    glRotated(180.0, 0.0, 0.0, 1.0);
    drawTriangle(1, 0);
    glPopMatrix();
}

/*  Handler for window-repaint event. Call back when the window first appears and
    whenever the window needs to be re-painted. */
void display()
{
    // glClear(GL_COLOR_BUFFER_BIT);            // Clear the color buffer (background)
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW); // To operate on Model-View matrix
    glLoadIdentity();           // Reset the model-view matrix

    // default arguments of gluLookAt
    // gluLookAt(0,0,0, 0,0,-100, 0,1,0);

    // control viewing (or camera)
    gluLookAt(eyex, eyey, eyez,
              centerx, centery, centerz,
              upx, upy, upz); ///////// Hex-rays theke eikhaner centerx,centery,centerz pore chng korbo

    drawAxes();

    glRotated(rot_angle, 0.0, 0.0, 1.0); // Rotate spehere or octahedron by rot_angle

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
    /*if (width >= height) {
        // aspect >= 1, set the height from -1 to 1, with larger width
        gluOrtho2D(-1.0 * aspect, 1.0 * aspect, -1.0, 1.0);
    } else {
        // aspect < 1, set the width to -1 to 1, with larger height
        gluOrtho2D(-1.0, 1.0, -1.0 / aspect, 1.0 / aspect);
    }*/
    // Enable perspective projection with fovy, aspect, zNear and zFar
    gluPerspective(45.0f, aspect, 0.1f, 100.0f);
}

//////// Hex-rays case 49-54,115,119 eigula handle kora baki
/* Callback handler for normal-key event */
void keyboardListener(unsigned char key, int x, int y)
{
    float v = 0.1;
    switch (key)
    {
    // Control eye (location of the eye)
    // control eyex
    case '1':
        eyex += v;
        break;
    case '2':
        eyex -= v;
        break;
    // control eyey
    case '3':
        eyey += v;
        break;
    case '4':
        eyey -= v;
        break;
    // control eyez
    case '5':
        eyez += v;
        break;
    case '6':
        eyez -= v;
        break;

    // Control center (location where the eye is looking at)
    // control centerx
    case 'q':
        centerx += v;
        break;
    case 'w':
        centerx -= v;
        break;
    // control centery
    case 'e':
        centery += v;
        break;
    case 'r':
        centery -= v;
        break;
    // control centerz
    case 't':
        centerz += v;
        break;
    case 'y':
        centerz -= v;
        break;

    // Controlling Object
    case 'a':
        rot_angle -= 10.0; // Rotate the object in the clockwise direction about its own axis
        break;
    case 'd':
        rot_angle += 10.0; // Rotate the object in the counter-clockwise direction about its own axis
        break;

    // Control exit
    case 27:     // ESC key
        exit(0); // Exit window
        break;
    default:
        return glutPostRedisplay();
    }
    glutPostRedisplay(); // Post a paint request to activate display()
}

/* Callback handler for special-key event */
void specialKeyListener(int key, int x, int y)
{
    double v = 0.25;
    double lx = centerx - eyex;
    double lz = centerz - eyez;
    double s;
    switch (key)
    {
    case GLUT_KEY_LEFT:
        eyex += v * (upy * lz);
        eyez += v * (-lx * upy);
        s = sqrt(eyex * eyex + eyez * eyez) / (4 * sqrt(2));
        eyex /= s;
        eyez /= s;
        break;
    case GLUT_KEY_RIGHT:
        eyex += v * (-upy * lz);
        eyez += v * (lx * upy);
        s = sqrt(eyex * eyex + eyez * eyez) / (4 * sqrt(2));
        eyex /= s;
        eyez /= s;
        break;
    case GLUT_KEY_UP:
        eyey += v;
        break;
    case GLUT_KEY_DOWN:
        eyey -= v;
        break;

    default:
        return;
    }
    glutPostRedisplay(); // Post a paint request to activate display()
}

/* Main function: GLUT runs as a console application starting at main()  */
int main(int argc, char **argv)
{
    glutInit(&argc, argv);                                    // Initialize GLUT
    glutInitWindowSize(640, 640);                             // Set the window's initial width & height
    glutInitWindowPosition(50, 50);                           // Position the window's initial top-left corner
    glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGB); // Depth, Double buffer, RGB color
    glutCreateWindow("OpenGL 3D Drawing");                    // Create a window with the given title
    glutDisplayFunc(display);                                 // Register display callback handler for window re-paint
    glutReshapeFunc(reshapeListener);                         // Register callback handler for window re-shape
    glutKeyboardFunc(keyboardListener);                       // Register callback handler for normal-key event
    // glutSpecialFunc(specialKeyListener);                      // Register callback handler for special-key event
    initGL();       // Our own OpenGL initialization
    glutMainLoop(); // Enter the event-processing loop
    return 0;
}
