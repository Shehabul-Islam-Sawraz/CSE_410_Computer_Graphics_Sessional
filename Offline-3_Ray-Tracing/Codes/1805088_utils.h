#include <bits/stdc++.h>
#include <GL/glut.h>
#include "PointVector.h"
#include "Color.h"
#include "bitmap_image.hpp"
#include <vector>

using namespace std;

#define PI acos(-1)
#define degToRad(angle) (PI * angle / 180.0)
#define radToDeg(angle) ((180 * angle) / PI)
#define N_SLICES 100
#define N_STACKS 100
#define INF 1e9
#define EPSILON 0.0000001

class Ray;
class Color;
class PointVector;
class Object;
class Pyramid;
class Cube;
class PointLight;
class SpotLight;
class Sphere;

extern int level_of_recursion;
extern bool texture_mode, texture_loaded;

extern vector<Object *> objects;
extern vector<PointLight *> pointLights;
extern vector<SpotLight *> spotLights;

template <typename type>
int minIndex(const vector<type> &list, type minval, type maxval)
{
    type cur_min = maxval;
    int minIdx = -1;
    for (int i = 0; i < list.size(); ++i)
    {
        if (list[i] > minval && list[i] < cur_min)
        {
            cur_min = list[i];
            minIdx = i;
        }
    }
    return minIdx;
}

void drawSphere(double radius, int noOfStacks, int noOfSlices, bool isLightSource,
                double tx, double ty, double tz, double color_r, double color_g, double color_b)
{
    PointVector points[noOfStacks][noOfSlices];
    int i, j;
    double h, r;
    for (i = 0; i < noOfStacks; i++)
    {
        h = radius * sin(((double)i / (double)noOfStacks) * (PI / 2));
        r = radius * cos(((double)i / (double)noOfStacks) * (PI / 2));
        for (j = 0; j < noOfSlices; j++)
        {
            points[i][j] = PointVector(r * cos(((double)j / (double)noOfSlices) * 2 * PI),
                                       r * sin(((double)j / (double)noOfSlices) * 2 * PI), h);
        }
    }
    // draw quads using generated points
    for (i = 0; i < noOfStacks; i++)
    {
        glPushMatrix();
        glColor3d(color_r, color_g, color_b);
        glTranslatef(tx, ty, tz);
        if (isLightSource)
        {
            glColor3d((double)i / (double)noOfStacks, (double)i / (double)noOfStacks, (double)i / (double)noOfStacks);
        }
        for (j = 0; j < noOfSlices; j++)
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
        glPopMatrix();
    }
}

void drawPyramid(double width, double height)
{
    glBegin(GL_TRIANGLES);

    glVertex3d(0.0, 0.0, 0.0);
    glVertex3d(0.0, width, 0.0);
    glVertex3d(width / 2.0, width / 2.0, height);
    glVertex3d(0.0, width, 0.0);
    glVertex3d(width, width, 0.0);
    glVertex3d(width / 2.0, width / 2.0, height);
    glVertex3d(width, width, 0.0);
    glVertex3d(width, 0.0, 0.0);
    glVertex3d(width / 2.0, width / 2.0, height);
    glVertex3d(width, 0.0, 0.0);
    glVertex3d(0.0, 0.0, 0.0);
    glVertex3d(width / 2.0, width / 2.0, height);
    glVertex3d(0.0, 0.0, 0.0);
    glVertex3d(0.0, width, 0.0);
    glVertex3d(width, width, 0.0);
    glVertex3d(0.0, 0.0, 0.0);
    glVertex3d(width, width, 0.0);
    glVertex3d(width, 0.0, 0.0);

    glEnd();
}

void drawCube(double side)
{
    glBegin(GL_QUADS);

    glVertex3d(0.0, 0.0, 0.0);
    glVertex3d(0.0, side, 0.0);
    glVertex3d(side, side, 0.0);
    glVertex3d(side, 0.0, 0.0);
    glVertex3d(0.0, 0.0, side);
    glVertex3d(0.0, side, side);
    glVertex3d(side, side, side);
    glVertex3d(side, 0.0, side);
    glVertex3d(0.0, 0.0, 0.0);
    glVertex3d(0.0, side, 0.0);
    glVertex3d(0.0, side, side);
    glVertex3d(0.0, 0.0, side);
    glVertex3d(side, 0.0, 0.0);
    glVertex3d(side, side, 0.0);
    glVertex3d(side, side, side);
    glVertex3d(side, 0.0, side);
    glVertex3d(0.0, 0.0, 0.0);
    glVertex3d(side, 0.0, 0.0);
    glVertex3d(side, 0.0, side);
    glVertex3d(0.0, 0.0, side);
    glVertex3d(0.0, side, 0.0);
    glVertex3d(side, side, 0.0);
    glVertex3d(side, side, side);
    glVertex3d(0.0, side, side);

    glEnd();
}

Color **loadBitmapImage(string filename)
{
    bitmap_image img(filename);

    int width = img.width();
    int height = img.height();

    // vector<vector<Color>> result(width, vector<Color>(height));
    Color **result = new Color *[width];
    for (int i = 0; i < width; i++)
    {
        result[i] = new Color[height];
    }

    for (int i = 0; i < width; ++i)
    {
        for (int j = 0; j < height; ++j)
        {
            unsigned char red, green, blue;
            img.get_pixel(i, j, red, green, blue);

            Color color;
            color.r = static_cast<float>(red) / 255.0f;
            color.g = static_cast<float>(green) / 255.0f;
            color.b = static_cast<float>(blue) / 255.0f;

            color.normalize();

            result[i][j] = color;
        }
    }

    return result;
}

void drawPointLightSource(PointVector pos, double fallOffParameter)
{
    glPushMatrix();
    glColor3f(1.0f, 1.0f, 1.0f);
    glTranslatef(pos.x, pos.y, pos.z);
    glutSolidSphere(3, 16, 16);
    /**
     * Eikhane is Light Source er false k true banaite hbe naki check kora lagbe
     */
    // drawSphere(5, 16, 16, false,
    //            pos.x, pos.y, pos.z,
    //            1.0f, 1.0f, 1.0f);
    glPopMatrix();
}

void drawSpotLightSource(PointVector pos, double fallOffParameter)
{
    drawPointLightSource(pos, fallOffParameter);
    glBegin(GL_LINES);
    {
        for (int i = -1; i <= 1; i++)
        {
            for (int j = -1; j <= 1; j++)
            {
                for (int k = -1; k <= 1; k++)
                {
                    glVertex3f(pos.x, pos.y, pos.z);
                    glVertex3f(pos.x + i * 2.0, pos.y + j * 2.0, pos.z + k * 2.0);
                }
            }
        }
    }
    glEnd();
}

class PointLight
{
public:
    PointVector position_source;
    double fallOffParameter;

    PointLight() {}

    PointLight(PointVector position, double fallOffParameter)
    {
        this->position_source = position;
        this->fallOffParameter = fallOffParameter;
    }

    void draw()
    {
        drawPointLightSource(position_source, fallOffParameter);
    }
};

class SpotLight : public PointLight
{
public:
    PointVector light_direction;
    double cutoff_angle;

    SpotLight() {}

    SpotLight(PointVector position, double fallOffParameter, PointVector direction, double cutoff_angle)
    {
        this->position_source = position;
        this->fallOffParameter = fallOffParameter;
        this->light_direction = direction;
        this->cutoff_angle = cutoff_angle;
    }

    void draw()
    {
        drawSpotLightSource(position_source, fallOffParameter);
    }
};

class Ray
{
public:
    PointVector start;
    PointVector direction;

    Ray() {}

    Ray(const Ray &ray)
    {
        this->start = ray.start;
        this->direction = ray.direction;
    }

    Ray(PointVector start, PointVector direction)
    {
        this->start = start;
        this->direction = direction;
        this->direction.normalize();
    }
};

class Object
{
public:
    PointVector reference_point;
    Color color;

    double object_height, object_width, object_length;
    double coEfficients[4]; // ambient, diffuse, specular, reflection coefficients
    int shine;

    Object() {}

    virtual void draw() {}

    void setCoEfficients(double ambient, double diffuse, double specular, double reflection)
    {
        coEfficients[0] = ambient;
        coEfficients[1] = diffuse;
        coEfficients[2] = specular;
        coEfficients[3] = reflection;
    }

    void setColor(Color color)
    {
        this->color = color;
    }

    void setShine(int shine)
    {
        this->shine = shine;
    }

    double intersect(Ray &ray, Color &color, int level)
    {
        double t_min = this->getTmin(ray);

        if (level == 0)
        {
            return t_min;
        }

        PointVector intPoint;
        intPoint = ray.start + ray.direction * t_min;

        color = this->getColorAt(intPoint) * this->coEfficients[0];
        color.normalize();

        PointVector normal = this->getNormalAt(intPoint);
        normal.normalize();

        //	----------------------------------
        //	Basic Light Illumination & Casting
        //	----------------------------------
        //	Point Lights

        double lambertValue = 0, phongValue = 0;

        for (PointLight *eachLight : pointLights)
        {
            PointVector lightDirection = eachLight->position_source - intPoint;
            double distance = sqrt(lightDirection.x * lightDirection.x +
                                   lightDirection.y * lightDirection.y +
                                   lightDirection.z * lightDirection.z);
            lightDirection.normalize();

            PointVector lightPosition;
            lightPosition = intPoint + lightDirection * EPSILON;

            Ray lightRay(lightPosition, lightDirection);

            Color tempColor;
            bool inShadow = false;
            double t, t_min_original = INFINITY;

            for (Object *eachObject : objects)
            {
                t = eachObject->intersect(lightRay, tempColor, 0);
                if (t > 0 && t < t_min_original)
                {
                    t_min_original = t;
                }
            }

            //  If we aren't in the shadow region, we compute
            //  DIFFUSE and SPECULAR componenets ---> R = 2.(L.N)N - L
            //
            //	lambert val is cos theta. theta is the ang betn L and N
            //	phong val is cos(ang betn R & V) to the power shine

            if (t_min < t_min_original)
            {
                PointVector R = normal * 2.0 * (normal.dotProduct(lightDirection)) - lightDirection;
                R.normalize();

                double scaling_factor = exp(-distance * distance * eachLight->fallOffParameter);

                lambertValue += max(normal.dotProduct(lightDirection), 0.0) * scaling_factor;
                phongValue += max(pow(R.dotProduct(ray.direction), this->shine), 0.0) * scaling_factor;

                color = color + this->getColorAt(intPoint) * lambertValue * this->coEfficients[1];
                color.normalize();

                color = color + this->getColorAt(intPoint) * phongValue * this->coEfficients[2];
                color.normalize();
            }
        }

        //	Spot Lights
        //  https://ogldev.org/www/tutorial21/tutorial21.html

        for (SpotLight *eachSpotLight : spotLights)
        {
            PointVector lightDirection = eachSpotLight->light_direction;
            double distance = sqrt(lightDirection.x * lightDirection.x +
                                   lightDirection.y * lightDirection.y +
                                   lightDirection.z * lightDirection.z);
            lightDirection.normalize();

            PointVector lightToIntersect = intPoint - eachSpotLight->position_source;
            lightToIntersect.normalize();

            double cos_theta = lightDirection.dotProduct(lightToIntersect);
            double theta = radToDeg(acos(cos_theta));

            if (theta > eachSpotLight->cutoff_angle)
            {
                continue;
            }

            //	^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
            //	Except the beginning part of the spotLight,
            //	remaining will be the same as PointLight

            lightDirection = eachSpotLight->position_source - intPoint;
            lightDirection.normalize();

            PointVector lightPosition = intPoint + lightDirection * EPSILON;
            Ray lightRay(lightPosition, lightDirection);

            Color tempColor;
            double t, t_min_original = INFINITY;

            for (Object *eachObject : objects)
            {
                t = eachObject->intersect(lightRay, tempColor, 0);
                if (t > 0 && t < t_min_original)
                {
                    t_min_original = t;
                }
            }

            if (t_min < t_min_original)
            {
                PointVector R = normal * 2.0 * (normal.dotProduct(lightDirection)) - lightDirection;
                R.normalize();

                double scaling_factor = exp(-distance * distance * eachSpotLight->fallOffParameter);

                lambertValue += max(normal.dotProduct(lightDirection), 0.0) * scaling_factor;
                phongValue += max(pow(R.dotProduct(ray.direction), this->shine), 0.0) * scaling_factor;

                color = color + this->getColorAt(intPoint) * lambertValue * this->coEfficients[1];
                color.normalize();

                color = color + this->getColorAt(intPoint) * phongValue * this->coEfficients[2];
                color.normalize();
            }
        }

        //	----------------------------
        //  Perform recursive reflection
        //	----------------------------
        if (level >= level_of_recursion)
            return t_min;

        PointVector refRayDirection;
        refRayDirection = ray.direction - normal * 2.0 * (normal.dotProduct(ray.direction));
        refRayDirection.normalize();

        PointVector refRayStart = refRayStart = intPoint + refRayDirection * EPSILON;
        Ray refRay(refRayStart, refRayDirection);

        //  Find out Nearest intersected Object
        //	-----------------------------------
        Color refColor;
        int nearest = -1;
        double t_ref, t_ref_min = INFINITY;

        for (int i = 0; i < objects.size(); i++)
        {
            t_ref = objects[i]->intersect(refRay, refColor, 0);
            if (t_ref > 0 && t_ref < t_ref_min)
            {
                t_ref_min = t_ref;
                nearest = i;
            }
        }

        if (nearest != -1)
        {
            t_ref_min = objects[nearest]->intersect(refRay, refColor, level + 1);
            color = color + refColor * this->coEfficients[3];
            color.normalize();
        }

        return t_min;
    }

    virtual double getTmin(Ray &ray)
    {
        return -1.0;
    }

    virtual PointVector getNormalAt(PointVector &intPoint)
    {
        return PointVector();
    }

    virtual Color getColorAt(PointVector &intPoint)
    {
        return this->color;
    }
};

class CheckerBoard : public Object
{
public:
    double cb_width;
    double tile_width;
    Color **textureBlack;
    Color **textureWhite;
    bitmap_image whiteTextureImg, blackTextureImg;

    CheckerBoard(double cb_width, double tile_width)
    {
        this->reference_point = PointVector(-cb_width / 2, -cb_width / 2, 0);
        this->cb_width = cb_width;
        this->tile_width = tile_width;
        this->object_length = tile_width;
    }

    void draw() override
    {
        int noOfGrid = (int)(cb_width / tile_width);

        for (int row = 0; row < noOfGrid; row++)
        {
            for (int col = 0; col < noOfGrid; col++)
            {
                if ((row + col) % 2 == 0)
                {
                    glColor3d(0, 0, 0);
                }
                else
                {
                    glColor3d(255, 255, 255);
                }

                glBegin(GL_QUADS);
                {
                    glVertex3f(reference_point.x + col * tile_width, reference_point.y + tile_width * row, reference_point.z);
                    glVertex3f(reference_point.x + (col + 1) * tile_width, reference_point.y + tile_width * row, reference_point.z);
                    glVertex3f(reference_point.x + (col + 1) * tile_width, reference_point.y + tile_width * (row + 1), reference_point.z);
                    glVertex3f(reference_point.x + col * tile_width, reference_point.y + tile_width * (row + 1), reference_point.z);
                }
                glEnd();
            }
        }
    }

    void loadTexture(string black_img_path, string white_img_path)
    {
        this->blackTextureImg = bitmap_image(black_img_path);
        this->whiteTextureImg = bitmap_image(white_img_path);

        this->textureBlack = loadBitmapImage(black_img_path);
        this->textureWhite = loadBitmapImage(white_img_path);
        cout << "Texture load complete" << endl;
    }

    double getTmin(Ray &ray)
    {
        PointVector normal(0, 0, 1);

        /**
            P(t) = Ro + t * Rd
            H(P) = n·P + D = 0 [as xy plane so D=0]
            n·(Ro + t * Rd) = 0
            t = -(n·Ro) / n·Rd
         */

        double t = (-1.0) * ((normal.dotProduct(ray.start)) / (normal.dotProduct(ray.direction)));
        PointVector intersectingRay = ray.start + ray.direction * t;

        if (intersectingRay.x < reference_point.x || intersectingRay.x > -reference_point.x)
        {
            return -1;
        }

        if (intersectingRay.y < reference_point.y || intersectingRay.y > -reference_point.y)
        {
            return -1;
        }

        return t;
    }

    PointVector getNormalAt(PointVector &intPoint) override
    {
        return PointVector(0, 0, 1);
    }

    Color getColorAt(PointVector &intPoint) override
    {
        int row = (int)((intPoint.x - reference_point.x) / tile_width);
        int col = (int)((intPoint.y - reference_point.y) / tile_width);

        double tile = 0.5;
        double tex = 1 - tile;
        int c = (row + col) % 2;

        if (!texture_mode)
        {
            return Color(c, c, c);
        }

        double x = (intPoint.x - reference_point.x) - tile_width * row;
        double y = (intPoint.y - reference_point.y) - tile_width * col;

        int tex_x;
        int tex_y;

        if (c == 0) // Black
        {
            tex_x = (int)(blackTextureImg.height() * x) / tile_width;
            tex_y = (int)(blackTextureImg.width() * y) / tile_width;

            return textureBlack[tex_x][tex_y];
        }
        else
        {
            tex_x = (int)(whiteTextureImg.height() * x) / tile_width;
            tex_y = (int)(whiteTextureImg.width() * y) / tile_width;

            return textureWhite[tex_x][tex_y];
        }

        // unsigned char r, g, b;
        // texture.get_pixel(tex_x, tex_y, r, g, b);
        // // cout << __LINE__ << endl;

        // double color_r = c * tile + (r / 255.0) * tex;
        // double color_g = c * tile + (g / 255.0) * tex;
        // double color_b = c * tile + (b / 255.0) * tex;

        // return Color(color_r, color_g, color_b);
    }
};

class Sphere : public Object
{
public:
    double radius;

    Sphere(PointVector center, double radius)
    {
        this->reference_point = center;
        this->radius = radius;
    }

    void draw() override
    {
        glPushMatrix();
        glColor3d(this->color.r, this->color.g, this->color.b);
        glTranslated(this->reference_point.x, this->reference_point.y, this->reference_point.z);
        glutSolidSphere(radius, N_SLICES, N_STACKS);
        // drawSphere(radius, N_STACKS, N_SLICES, false,
        //            this->reference_point.x, this->reference_point.y, this->reference_point.z,
        //            this->color.r, this->color.g, this->color.b);
        glPopMatrix();
    }

    double getTmin(Ray &ray)
    {
        PointVector sphereOrigin = ray.start - reference_point; // Ro = Ro - ref

        /**
            Quadratic: at2 + bt + c = 0
            a = 1  (remember, ||Rd|| = 1)
            b = 2Rd·Ro
            c = Ro·Ro – r2

         */

        double a = 1;
        double b = (ray.direction.dotProduct(sphereOrigin)) * 2.0;
        double c = (sphereOrigin.dotProduct(sphereOrigin)) - radius * radius;

        double det = b * b - 4 * a * c;
        if (det < 0)
        {
            return -1;
        }

        double d = sqrt(det);
        double t1 = (-b - d) / (2 * a);
        double t2 = (-b + d) / (2 * a);

        if (t1 < 0 && t2 < 0)
        {
            return -1;
        }

        if (t1 > 0)
        {
            return t1;
        }

        if (t2 > 0)
        {
            return t2;
        }

        return -1;
    }

    PointVector getNormalAt(PointVector &intPoint) override
    {
        PointVector normal = intPoint - reference_point;
        normal.normalize();
        return normal;
    }
};

class Pyramid : public Object
{
public:
    Pyramid(PointVector ref_point, double width, double height)
    {
        this->reference_point = ref_point;
        this->object_width = width;
        this->object_height = height;
    }

    void draw() override
    {
        glPushMatrix();
        glColor3d(this->color.r, this->color.g, this->color.b);
        glTranslated(this->reference_point.x, this->reference_point.y, this->reference_point.z);
        drawPyramid(this->object_width, object_height);
        glPopMatrix();
    }

    double getTmin(Ray &ray)
    {
        vector<double> tS(5, -1);
        PointVector normal(0, 0, 1);
        PointVector pyramidOrigin = ray.start - reference_point; // Ro = Ro - ref

        // Determining the Point of intersection at the bottom
        if (ray.direction.z != 0)
        {
            double tt = (-1.0) * ((normal.dotProduct(pyramidOrigin)) / (normal.dotProduct(ray.direction)));

            double X = ray.start.x + tt * ray.direction.x;
            double Y = ray.start.y + tt * ray.direction.y;

            if (reference_point.x < X && X < (reference_point.x + this->object_width) && reference_point.y < Y && Y < (reference_point.y + this->object_width))
            {
                tS[0] = tt;
            }
        }

        // five corner points of pyramid
        PointVector V0 = PointVector(reference_point.x, reference_point.y, reference_point.z);
        PointVector V1 = PointVector(reference_point.x + this->object_width, reference_point.y, reference_point.z);
        PointVector V2 = PointVector(reference_point.x + this->object_width, reference_point.y + this->object_width, reference_point.z);
        PointVector V3 = PointVector(reference_point.x, reference_point.y + this->object_width, reference_point.z);
        PointVector V4 = V0;
        vector<PointVector> arr = {V0, V1, V2, V3, V4};
        PointVector C = PointVector(reference_point.x + this->object_width / 2.0,
                                    reference_point.y + this->object_width / 2.0,
                                    reference_point.z + this->object_height); // top point

        // for each triangles
        for (int a = 1; a < 5; a++)
        {
            PointVector A = arr[a - 1];
            PointVector B = arr[a];
            PointVector v1 = B - A;
            PointVector v2 = C - A;

            PointVector normal = v1.crossProduct(v2);
            normal.normalize();

            PointVector dir = ray.direction;
            PointVector v = (C - ray.start);
            double const_a = dir.dotProduct(normal);
            if (const_a > -EPSILON && const_a < EPSILON)
            {
                continue;
            }
            double t = v.dotProduct(normal) / dir.dotProduct(normal);

            PointVector intersectionPoint = ray.start + dir * t;

            PointVector v0 = C - A;
            v1 = B - A;
            v2 = intersectionPoint - A;

            double dot00 = v0.dotProduct(v0);
            double dot01 = v0.dotProduct(v1);
            double dot02 = v0.dotProduct(v2);
            double dot11 = v1.dotProduct(v1);
            double dot12 = v1.dotProduct(v2);

            // Compute barycentric coordinates
            double invDenom = 1.0 / (dot00 * dot11 - dot01 * dot01);
            double U = (dot11 * dot02 - dot01 * dot12) * invDenom;
            double V = (dot00 * dot12 - dot01 * dot02) * invDenom;

            if ((U >= 0) && (V >= 0) && (U + V <= 1.0))
            {
                tS[a] = t;
            }
        }

        int index = minIndex(tS, 0.0, INF);
        return index == -1 ? -1 : tS[index];
    }

    PointVector getNormalAt(PointVector &intersectionPoint) override
    {
        // five corner points of pyramid
        PointVector V0 = PointVector(reference_point.x, reference_point.y, reference_point.z);
        PointVector V1 = PointVector(reference_point.x + this->object_width, reference_point.y, reference_point.z);
        PointVector V2 = PointVector(reference_point.x + this->object_width, reference_point.y + this->object_width, reference_point.z);
        PointVector V3 = PointVector(reference_point.x, reference_point.y + this->object_width, reference_point.z);
        PointVector V4 = V0;
        vector<PointVector> arr = {V0, V1, V2, V3, V4};
        PointVector C = PointVector(reference_point.x + this->object_width / 2.0,
                                    reference_point.y + this->object_width / 2.0,
                                    reference_point.z + this->object_height); // top point

        PointVector base_normal = (V3 - V0).crossProduct(V1 - V0);
        base_normal.normalize();
        if (abs((V0 - intersectionPoint).dotProduct(base_normal) - 0.0) < EPSILON)
        {
            return base_normal;
        }

        // upper four triangles
        for (int a = 1; a < 5; a++)
        {
            PointVector A = arr[a - 1], B = arr[a];
            PointVector normal = (C - A).crossProduct(B - A);
            normal.normalize();

            PointVector v = (C - intersectionPoint);
            v.normalize();
            if (abs(v.dotProduct(normal) - 0) < EPSILON)
            {
                normal.normalize();
                return normal;
            }
        }
        cout << intersectionPoint << " Point is not intersecting any face of pyramid ";
        return this->getNormalAt(intersectionPoint);
    }
};

class Cube : public Object
{
public:
    double side_length;
    int intPointSide = INF;

    Cube(PointVector center, double side)
    {
        this->reference_point = center;
        this->side_length = side;
    }

    void draw() override
    {
        glPushMatrix();
        glColor3d(this->color.r, this->color.g, this->color.b);
        glTranslated(this->reference_point.x, this->reference_point.y, this->reference_point.z);
        drawCube(side_length);
        glPopMatrix();
    }

    double getTmin(Ray &ray)
    {
        double t_min;
        // Calculate the minimum and maximum t values for each axis
        double tMinX = (reference_point.x - ray.start.x) / ray.direction.x;
        double tMaxX = (reference_point.x + side_length - ray.start.x) / ray.direction.x;
        double tMinY = (reference_point.y - ray.start.y) / ray.direction.y;
        double tMaxY = (reference_point.y + side_length - ray.start.y) / ray.direction.y;
        double tMinZ = (reference_point.z - ray.start.z) / ray.direction.z;
        double tMaxZ = (reference_point.z + side_length - ray.start.z) / ray.direction.z;

        // Find the minimum and maximum t values among all axes
        double tMin = max(max(min(tMinX, tMaxX), min(tMinY, tMaxY)), min(tMinZ, tMaxZ));
        double tMax = min(min(max(tMinX, tMaxX), max(tMinY, tMaxY)), max(tMinZ, tMaxZ));

        // Check if there is a valid intersection
        if (tMin <= tMax && tMax >= 0)
        {
            t_min = tMin >= 0 ? tMin : tMax; // Return the minimum positive t if it exists, otherwise return tMax
        }
        else
        {
            t_min = -1; // No intersection
        }

        // Determine which face of the cube was hit and calculate the normal
        if (tMin == tMinX)
        {
            intPointSide = -1;
        }
        else if (tMin == tMinY)
        {
            intPointSide = -2;
        }
        else if (tMin == tMinZ)
        {
            intPointSide = -3;
        }
        else if (tMin == tMaxX)
        {
            intPointSide = 1;
        }
        else if (tMin == tMaxY)
        {
            intPointSide = 2;
        }
        else if (tMin == tMaxZ)
        {
            intPointSide = 3;
        }

        return t_min;
    }

    PointVector getNormalAt(PointVector &intersectionPoint) override
    {
        if (intPointSide == -1)
        {
            return PointVector(-1, 0, 0);
        }
        else if (intPointSide == -2)
        {
            return PointVector(0, -1, 0);
        }
        else if (intPointSide == -3)
        {
            return PointVector(0, 0, -1);
        }
        else if (intPointSide == 1)
        {
            return PointVector(1, 0, 0);
        }
        else if (intPointSide == 2)
        {
            return PointVector(0, 1, 0);
        }
        else if (intPointSide == 3)
        {
            return PointVector(0, 0, 1);
        }
        else
        {
            return PointVector(0, 0, 0);
        }
    }
};