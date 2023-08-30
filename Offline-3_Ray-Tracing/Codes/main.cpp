#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <GL/glut.h>
#include "1805088_utils.h"

using namespace std;

#define pi (2 * acos(0.0))
GLdouble near_distance;
GLdouble far_distance;
GLdouble fov_x;
GLdouble fov_y;
GLdouble aspect_ratio;
int level_of_recursion = 0;
int screen_size = 0;
bool draw_axes, texture_mode, texture_loaded;
int imageCount = 0;

PointVector pos, l, r, u;
CheckerBoard *checkBoard;

vector<Object *> objects;
vector<PointLight *> pointLights;
vector<SpotLight *> spotLights;

#define GL_WINDOW_HEIGHT 700
#define GL_WINDOW_WIDTH 700
#define CHECKERBOARD_WIDTH 800

#define INPUT_FILE "../Assignment-RayTracer/scene.txt"
#define IMG_PATH_TEXTURE_BLACK "../Assignment-RayTracer/texture_b.bmp"
#define IMG_PATH_TEXTURE_WHITE "../Assignment-RayTracer/texture_w.bmp"

#define CAMERA_ROTATE_STEP 5
#define CAMERA_MOVE_STEP 10

double cameraHeight;
double cameraAngle;
int drawgrid;
int drawaxes;
double angle;

struct point
{
    double x, y, z;
};

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

void traceRays()
{
    cout << "Rendering Start" << endl;

    if (texture_loaded && texture_mode)
    {
        cout << "Rendering with texture" << endl;
    }

    //	In Bitmap, column comes first
    bitmap_image image(screen_size, screen_size);
    for (int col = 0; col < screen_size; col++)
    {
        for (int row = 0; row < screen_size; row++)
        {
            image.set_pixel(col, row, 0, 0, 0);
        }
    }

    double planeDistance;
    planeDistance = (GL_WINDOW_HEIGHT / 2.0) / tan(degToRad(fov_y / 2.0));

    PointVector topLeft;
    topLeft = pos + (l * planeDistance) - (r * (GL_WINDOW_WIDTH / 2.0)) + (u * (GL_WINDOW_HEIGHT / 2.0));

    double du = (double)GL_WINDOW_WIDTH / (screen_size * 1.0);
    double dv = (double)GL_WINDOW_HEIGHT / (screen_size * 1.0);

    //	Choosing middle of the grid
    topLeft = topLeft + (r * (0.5 * du)) - (u * (0.5 * dv));

    int nearest;
    double t, t_min;
    PointVector currentPixel{};

    int percent = 0;
    int targetValue = 0;

    for (int col = 0; col < screen_size; col++)
    {
        for (int row = 0; row < screen_size; row++)
        {
            nearest = -1;
            t_min = INFINITY;
            currentPixel = topLeft + (r * (col * du)) - (u * (row * dv));

            Ray ray(pos, (currentPixel - pos));
            Color color;

            for (int i = 0; i < objects.size(); i++)
            {
                t = objects[i]->intersect(ray, color, 0);
                if (t > 0 && t < t_min)
                {
                    t_min = t;
                    nearest = i;
                }
            }

            if (nearest != -1)
            {
                t_min = objects[nearest]->intersect(ray, color, 1);
            }
            color.normalize();
            image.set_pixel(col, row, color.r * 255, color.g * 255, color.b * 255);

            if (col == targetValue)
            {
                cout << "Rendering " << percent << "%"
                     << " complete" << endl;
                percent += 10;
                targetValue += (screen_size / 10);
            }
        }
    }
    string imageName = "Output_" + to_string(imageCount) + ".bmp";
    imageCount++;
    image.save_image(imageName);
    image.clear();
    cout << "Image '" + imageName + "' saved!" << endl;

    cout << "Rendering Complete" << endl;
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

    double checkboard_tile_width, cb_ambient, cb_diffuse, cb_reflection;
    input >> checkboard_tile_width >> cb_ambient >> cb_diffuse >> cb_reflection;

    if (cb_ambient + cb_diffuse + cb_reflection != 1.0)
    {
        cout << "'cb_ambient + cb_diffuse + cb_reflection' must be equal to 1.0" << endl;
        exit(0);
    }

    checkBoard = new CheckerBoard(1.25 * far_distance, checkboard_tile_width);
    checkBoard->setCoEfficients(cb_ambient, cb_diffuse, 0.0, cb_reflection);
    checkBoard->setShine(10);

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
            // objects.push_back(object);
        }
        else if (objectType == "pyramid")
        {
            PointVector lowest_point;
            double width, height;
            Color pyramidColor;
            double ambient, diffuse, specular, reflection;
            int shininess;

            input >> lowest_point.x >> lowest_point.y >> lowest_point.z;
            input >> width >> height;
            input >> pyramidColor.r >> pyramidColor.g >> pyramidColor.b;
            input >> ambient >> diffuse >> specular >> reflection;
            input >> shininess;

            // object = new Pyramid(lowest_point, width, height);
            // object->setColor(pyramidColor);
            // object->setCoEfficients(ambient, diffuse, specular, reflection);
            // object->setShine(shininess);
            // objects.push_back(object);
        }
        else if (objectType == "cube")
        {
            PointVector bottom_lower_left_point;
            double side_length;
            Color cubeColor;
            double ambient, diffuse, specular, reflection;
            int shininess;

            input >> bottom_lower_left_point.x >> bottom_lower_left_point.y >> bottom_lower_left_point.z;
            input >> side_length;
            input >> cubeColor.r >> cubeColor.g >> cubeColor.b;
            input >> ambient >> diffuse >> specular >> reflection;
            input >> shininess;

            object = new Cube(bottom_lower_left_point, side_length);
            object->setColor(cubeColor);
            object->setCoEfficients(ambient, diffuse, specular, reflection);
            object->setShine(shininess);
            // objects.push_back(object);
        }
        else
        {
            cout << "ERROR: Unknown Object Type '" << objectType << "'" << endl;
            break;
        }
    }

    int noOfPointLights;
    input >> noOfPointLights;

    for (int i = 0; i < noOfPointLights; i++)
    {
        PointVector position_source;
        double fallOffParameter;

        input >> position_source.x >> position_source.y >> position_source.z;
        input >> fallOffParameter;

        PointLight *pointLight = new PointLight(position_source, fallOffParameter);
        // pointLights.push_back(pointLight);
    }

    int nSpotLights;
    input >> nSpotLights;

    for (int i = 0; i < nSpotLights; i++)
    {
        PointVector position_source;
        double fallOffParameter;
        PointVector direction;
        double cutoff_angle;

        input >> position_source.x >> position_source.y >> position_source.z;
        input >> fallOffParameter;
        input >> direction.x >> direction.y >> direction.z;
        input >> cutoff_angle;

        SpotLight *spotLight = new SpotLight(position_source, fallOffParameter, direction, cutoff_angle);
        // spotLights.push_back(spotLight);
    }

    cout << "Input Read Done" << endl;
}

void drawSquare(double a)
{
    // glColor3f(1.0,0.0,0.0);
    glBegin(GL_QUADS);
    {
        glVertex3f(a, a, 2);
        glVertex3f(a, -a, 2);
        glVertex3f(-a, -a, 2);
        glVertex3f(-a, a, 2);
    }
    glEnd();
}

void drawCircle(double radius, int segments)
{
    int i;
    struct point points[100];
    glColor3f(0.7, 0.7, 0.7);
    // generate points
    for (i = 0; i <= segments; i++)
    {
        points[i].x = radius * cos(((double)i / (double)segments) * 2 * pi);
        points[i].y = radius * sin(((double)i / (double)segments) * 2 * pi);
    }
    // draw segments using generated points
    for (i = 0; i < segments; i++)
    {
        glBegin(GL_LINES);
        {
            glVertex3f(points[i].x, points[i].y, 0);
            glVertex3f(points[i + 1].x, points[i + 1].y, 0);
        }
        glEnd();
    }
}

void drawCone(double radius, double height, int segments)
{
    int i;
    double shade;
    struct point points[100];
    // generate points
    for (i = 0; i <= segments; i++)
    {
        points[i].x = radius * cos(((double)i / (double)segments) * 2 * pi);
        points[i].y = radius * sin(((double)i / (double)segments) * 2 * pi);
    }
    // draw triangles using generated points
    for (i = 0; i < segments; i++)
    {
        // create shading effect
        if (i < segments / 2)
            shade = 2 * (double)i / (double)segments;
        else
            shade = 2 * (1.0 - (double)i / (double)segments);
        glColor3f(shade, shade, shade);

        glBegin(GL_TRIANGLES);
        {
            glVertex3f(0, 0, height);
            glVertex3f(points[i].x, points[i].y, 0);
            glVertex3f(points[i + 1].x, points[i + 1].y, 0);
        }
        glEnd();
    }
}

void drawSphere(double radius, int slices, int stacks)
{
    struct point points[100][100];
    int i, j;
    double h, r;
    // generate points
    for (i = 0; i <= stacks; i++)
    {
        h = radius * sin(((double)i / (double)stacks) * (pi / 2));
        r = radius * cos(((double)i / (double)stacks) * (pi / 2));
        for (j = 0; j <= slices; j++)
        {
            points[i][j].x = r * cos(((double)j / (double)slices) * 2 * pi);
            points[i][j].y = r * sin(((double)j / (double)slices) * 2 * pi);
            points[i][j].z = h;
        }
    }
    // draw quads using generated points
    for (i = 0; i < stacks; i++)
    {
        glColor3f((double)i / (double)stacks, (double)i / (double)stacks, (double)i / (double)stacks);
        for (j = 0; j < slices; j++)
        {
            glBegin(GL_QUADS);
            {
                // upper hemisphere
                glVertex3f(points[i][j].x, points[i][j].y, points[i][j].z);
                glVertex3f(points[i][j + 1].x, points[i][j + 1].y, points[i][j + 1].z);
                glVertex3f(points[i + 1][j + 1].x, points[i + 1][j + 1].y, points[i + 1][j + 1].z);
                glVertex3f(points[i + 1][j].x, points[i + 1][j].y, points[i + 1][j].z);
                // lower hemisphere
                glVertex3f(points[i][j].x, points[i][j].y, -points[i][j].z);
                glVertex3f(points[i][j + 1].x, points[i][j + 1].y, -points[i][j + 1].z);
                glVertex3f(points[i + 1][j + 1].x, points[i + 1][j + 1].y, -points[i + 1][j + 1].z);
                glVertex3f(points[i + 1][j].x, points[i + 1][j].y, -points[i + 1][j].z);
            }
            glEnd();
        }
    }
}

void drawSS()
{
    glColor3f(1, 0, 0);
    drawSquare(20);

    glRotatef(angle, 0, 0, 1);
    glTranslatef(110, 0, 0);
    glRotatef(2 * angle, 0, 0, 1);
    glColor3f(0, 1, 0);
    drawSquare(15);

    glPushMatrix();
    {
        glRotatef(angle, 0, 0, 1);
        glTranslatef(60, 0, 0);
        glRotatef(2 * angle, 0, 0, 1);
        glColor3f(0, 0, 1);
        drawSquare(10);
    }
    glPopMatrix();

    glRotatef(3 * angle, 0, 0, 1);
    glTranslatef(40, 0, 0);
    glRotatef(4 * angle, 0, 0, 1);
    glColor3f(1, 1, 0);
    drawSquare(5);
}

void keyboardListener(unsigned char key, int x, int y)
{
    switch (key)
    {

    case '1':
        drawgrid = 1 - drawgrid;
        break;

    default:
        break;
    }
}

void specialKeyListener(int key, int x, int y)
{
    switch (key)
    {
    case GLUT_KEY_DOWN: // down arrow key
        cameraHeight -= 3.0;
        break;
    case GLUT_KEY_UP: // up arrow key
        cameraHeight += 3.0;
        break;

    case GLUT_KEY_RIGHT:
        cameraAngle += 0.03;
        break;
    case GLUT_KEY_LEFT:
        cameraAngle -= 0.03;
        break;

    case GLUT_KEY_PAGE_UP:
        break;
    case GLUT_KEY_PAGE_DOWN:
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
}

void mouseListener(int button, int state, int x, int y)
{ // x, y is the x-y of the screen (2D)
    switch (button)
    {
    case GLUT_LEFT_BUTTON:
        if (state == GLUT_DOWN)
        { // 2 times?? in ONE click? -- solution is checking DOWN or UP
            drawaxes = 1 - drawaxes;
        }
        break;

    case GLUT_RIGHT_BUTTON:
        //........
        break;

    case GLUT_MIDDLE_BUTTON:
        //........
        break;

    default:
        break;
    }
}

void display()
{

    // clear the display
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(0, 0, 0, 0); // color black
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    /********************
    / set-up camera here
    ********************/
    // load the correct matrix -- MODEL-VIEW matrix
    glMatrixMode(GL_MODELVIEW);

    // initialize the matrix
    glLoadIdentity();

    // now give three info
    // 1. where is the camera (viewer)?
    // 2. where is the camera looking?
    // 3. Which direction is the camera's UP direction?

    // gluLookAt(100,100,100,	0,0,0,	0,0,1);
    // gluLookAt(200*cos(cameraAngle), 200*sin(cameraAngle), cameraHeight,		0,0,0,		0,0,1);
    gluLookAt(0, 0, 200, 0, 0, 0, 0, 1, 0);

    // again select MODEL-VIEW
    glMatrixMode(GL_MODELVIEW);

    /****************************
    / Add your objects from here
    ****************************/
    // add objects

    drawAxes();
    // drawGrid();

    // glColor3f(1,0,0);
    // drawSquare(10);

    drawSS();

    // drawCircle(30,24);

    // drawCone(20,50,24);

    // drawSphere(30,24,20);

    // ADD this line in the end --- if you use double buffer (i.e. GL_DOUBLE)
    glutSwapBuffers();
}

void animate()
{
    angle += 0.05;
    // codes for any changes in Models, Camera
    glutPostRedisplay();
}

void init()
{
    // codes for initialization
    drawgrid = 0;
    drawaxes = 1;
    cameraHeight = 150.0;
    cameraAngle = 1.0;
    angle = 0;

    // clear the screen
    glClearColor(0, 0, 0, 0);

    /************************
    / set-up projection here
    ************************/
    // load the PROJECTION matrix
    glMatrixMode(GL_PROJECTION);

    // initialize the matrix
    glLoadIdentity();

    // give PERSPECTIVE parameters
    gluPerspective(80, 1, 1, 1000.0);
    // field of view in the Y (vertically)
    // aspect ratio that determines the field of view in the X direction (horizontally)
    // near distance
    // far distance
}

int main(int argc, char **argv)
{
    glutInit(&argc, argv);
    glutInitWindowSize(500, 500);
    glutInitWindowPosition(0, 0);
    glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGB); // Depth, Double buffer, RGB color

    glutCreateWindow("My OpenGL Program");

    init();

    glEnable(GL_DEPTH_TEST); // enable Depth Testing

    glutDisplayFunc(display); // display callback function
    glutIdleFunc(animate);    // what you want to do in the idle time (when no drawing is occuring)

    glutKeyboardFunc(keyboardListener);
    glutSpecialFunc(specialKeyListener);
    glutMouseFunc(mouseListener);

    glutMainLoop(); // The main loop of OpenGL

    return 0;
}
