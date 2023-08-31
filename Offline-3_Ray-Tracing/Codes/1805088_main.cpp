#include "1805088_utils.h"

using namespace std;

double near_distance;
double far_distance;
double fov_x;
double fov_y;
double aspect_ratio;
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

#define INPUT_FILE "../Assignment-RayTracer/description.txt"
#define IMG_PATH_TEXTURE_BLACK "../Assignment-RayTracer/texture_b.bmp"
#define IMG_PATH_TEXTURE_WHITE "../Assignment-RayTracer/texture_w.bmp"

#define CAMERA_ROTATE_STEP 5
#define CAMERA_MOVE_STEP 10

/* Draw axes: X in Red, Y in Green and Z in Blue */
void drawAxes()
{
    if (draw_axes)
    {
        glLineWidth(1.5);
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
            if (t_min < far_distance)
            {
                image.set_pixel(col, row, color.r * 255, color.g * 255, color.b * 255);
            }

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

    cout << "Load Data Start" << endl;

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

    objects.push_back(checkBoard);

    Object *object;
    int noOfObjects;
    string objectType;

    input >> noOfObjects;

    for (int i = 0; i < noOfObjects; i++)
    {
        input >> objectType;
        // cout << objectType << endl;

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

            object = new Pyramid(lowest_point, width, height);
            object->setColor(pyramidColor);
            object->setCoEfficients(ambient, diffuse, specular, reflection);
            object->setShine(shininess);
            objects.push_back(object);
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
            objects.push_back(object);
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
        pointLights.push_back(pointLight);
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
        spotLights.push_back(spotLight);
    }

    cout << "Input Read Done" << endl;
}

void animate()
{
    // codes for any changes in Models, Camera
    glutPostRedisplay();
}

void keyboardListener(unsigned char key, int x, int y)
{
    double rate = degToRad(CAMERA_ROTATE_STEP);

    switch (key)
    {
    case ' ':
        texture_mode = !texture_mode;
        break;
    case '0':
        if (texture_mode)
        {
            if (!texture_loaded)
            {
                checkBoard->loadTexture(IMG_PATH_TEXTURE_BLACK, IMG_PATH_TEXTURE_WHITE);
                texture_loaded = true;
            }
        }
        traceRays();
        break;
    case '1':
        // Rotate left
        r.x = r.x * cos(-rate) + l.x * sin(-rate);
        r.y = r.y * cos(-rate) + l.y * sin(-rate);
        r.z = r.z * cos(-rate) + l.z * sin(-rate);

        l.x = l.x * cos(-rate) - r.x * sin(-rate);
        l.y = l.y * cos(-rate) - r.y * sin(-rate);
        l.z = l.z * cos(-rate) - r.z * sin(-rate);
        break;

    case '2':
        // Rotate right
        r.x = r.x * cos(rate) + l.x * sin(rate);
        r.y = r.y * cos(rate) + l.y * sin(rate);
        r.z = r.z * cos(rate) + l.z * sin(rate);

        l.x = l.x * cos(rate) - r.x * sin(rate);
        l.y = l.y * cos(rate) - r.y * sin(rate);
        l.z = l.z * cos(rate) - r.z * sin(rate);
        break;

    case '3':
        // Rotate up
        l.x = l.x * cos(rate) + u.x * sin(rate);
        l.y = l.y * cos(rate) + u.y * sin(rate);
        l.z = l.z * cos(rate) + u.z * sin(rate);

        u.x = u.x * cos(rate) - l.x * sin(rate);
        u.y = u.y * cos(rate) - l.y * sin(rate);
        u.z = u.z * cos(rate) - l.z * sin(rate);
        break;

    case '4':
        // Rotate down
        l.x = l.x * cos(-rate) + u.x * sin(-rate);
        l.y = l.y * cos(-rate) + u.y * sin(-rate);
        l.z = l.z * cos(-rate) + u.z * sin(-rate);

        u.x = u.x * cos(-rate) - l.x * sin(-rate);
        u.y = u.y * cos(-rate) - l.y * sin(-rate);
        u.z = u.z * cos(-rate) - l.z * sin(-rate);
        break;

    case '5':
        // Rotate clockwise
        u.x = u.x * cos(-rate) + r.x * sin(-rate);
        u.y = u.y * cos(-rate) + r.y * sin(-rate);
        u.z = u.z * cos(-rate) + r.z * sin(-rate);

        r.x = r.x * cos(-rate) - u.x * sin(-rate);
        r.y = r.y * cos(-rate) - u.y * sin(-rate);
        r.z = r.z * cos(-rate) - u.z * sin(-rate);
        break;

    case '6':
        // Rotate counter-clockwise
        u.x = u.x * cos(rate) + r.x * sin(rate);
        u.y = u.y * cos(rate) + r.y * sin(rate);
        u.z = u.z * cos(rate) + r.z * sin(rate);

        r.x = r.x * cos(rate) - u.x * sin(rate);
        r.y = r.y * cos(rate) - u.y * sin(rate);
        r.z = r.z * cos(rate) - u.z * sin(rate);
        break;

    default:
        break;
    }
}

void specialKeyListener(int key, int x, int y)
{
    switch (key)
    {
    case GLUT_KEY_DOWN:
        //	Move Back
        pos.x -= l.x * CAMERA_MOVE_STEP;
        pos.y -= l.y * CAMERA_MOVE_STEP;
        pos.z -= l.z * CAMERA_MOVE_STEP;
        break;

    case GLUT_KEY_UP:
        //	Move Front
        pos.x += l.x * CAMERA_MOVE_STEP;
        pos.y += l.y * CAMERA_MOVE_STEP;
        pos.z += l.z * CAMERA_MOVE_STEP;
        break;

    case GLUT_KEY_RIGHT:
        //	Move Right
        pos.x += r.x * CAMERA_MOVE_STEP;
        pos.y += r.y * CAMERA_MOVE_STEP;
        pos.z += r.z * CAMERA_MOVE_STEP;
        break;

    case GLUT_KEY_LEFT:
        //	Move Left
        pos.x -= r.x * CAMERA_MOVE_STEP;
        pos.y -= r.y * CAMERA_MOVE_STEP;
        pos.z -= r.z * CAMERA_MOVE_STEP;
        break;

    case GLUT_KEY_PAGE_UP:
        //	Move Up
        pos.x += u.x * CAMERA_MOVE_STEP;
        pos.y += u.y * CAMERA_MOVE_STEP;
        pos.z += u.z * CAMERA_MOVE_STEP;
        break;

    case GLUT_KEY_PAGE_DOWN:
        //	Move Down
        pos.x -= u.x * CAMERA_MOVE_STEP;
        pos.y -= u.y * CAMERA_MOVE_STEP;
        pos.z -= u.z * CAMERA_MOVE_STEP;
        break;

    default:
        break;
    }
}

void mouseListener(int button, int state, int x, int y)
{
    switch (button)
    {
    case GLUT_LEFT_BUTTON:
        if (state == GLUT_DOWN)
        {
            draw_axes = !draw_axes;
        }
        break;

    case GLUT_RIGHT_BUTTON:
        break;

    default:
        break;
    }
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

    // checkBoard->draw();

    for (Object *each : objects)
    {
        each->draw();
    }

    for (PointLight *light : pointLights)
    {
        light->draw();
    }

    for (SpotLight *light : spotLights)
    {
        light->draw();
    }

    glutSwapBuffers();
}

int main(int argc, char **argv)
{
    pos.x = 200;
    pos.y = 200;
    pos.z = 50;

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

    glutKeyboardFunc(keyboardListener);
    glutSpecialFunc(specialKeyListener);
    glutMouseFunc(mouseListener);

    glutMainLoop(); // The main loop of OpenGL

    return 0;
}