#include "1805088_utils.h"

using namespace std;

#define INPUT_FILE "../Assignment-RayTracer/description.txt"

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
    cout << near_distance << " " << far_distance << " " << fov_y << " " << aspect_ratio << endl;
    input >> level_of_recursion >> screen_size;
    cout << level_of_recursion << " " << screen_size << endl;
    fov_x = aspect_ratio * fov_y;
    cout << fov_x << endl;

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
    cout << noOfObjects << endl;

    for (int i = 0; i < noOfObjects; i++)
    {
        input >> objectType;
        cout << objectType << endl;

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
            cout << bottom_lower_left_point.x << endl;
            input >> side_length;
            cout << side_length << endl;
            input >> cubeColor.r >> cubeColor.g >> cubeColor.b;
            cout << cubeColor.r << " " << cubeColor.g << " " << cubeColor.b << endl;
            input >> ambient >> diffuse >> specular >> reflection;
            cout << ambient << " " << diffuse << " " << specular << " " << reflection << endl;
            input >> shininess;
            cout << shininess << endl;

            object = new Cube(bottom_lower_left_point, side_length);
            object->setColor(cubeColor);
            object->setCoEfficients(ambient, diffuse, specular, reflection);
            object->setShine(shininess);
            objects.push_back(object);
            cout << "Cube Inout Shesh" << endl;
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

int main()
{
    load_data();
    return 0;
}