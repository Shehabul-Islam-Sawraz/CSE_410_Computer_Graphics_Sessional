#include <bits/stdc++.h>
#include <GL/glut.h>
#include "PointVector.h"
#include "Color.h"

using namespace std;

#define PI acos(-1)
#define degToRad(angle) (PI * angle / 180.0)
#define radToDeg(angle) ((180 * angle) / PI)
#define N_SLICES 100
#define N_STACKS 100

void drawSphere(double radius)
{
    PointVector points[N_STACKS][N_SLICES];
    int i, j;
    double h, r;
    for (i = 0; i <= N_STACKS; i++)
    {
        h = radius * sin(((double)i / (double)N_STACKS) * (PI / 2));
        r = radius * cos(((double)i / (double)N_STACKS) * (PI / 2));
        for (j = 0; j <= N_SLICES; j++)
        {
            points[i][j] = PointVector(r * cos(((double)j / (double)N_SLICES) * 2 * PI),
                                       r * sin(((double)j / (double)N_SLICES) * 2 * PI), h);
        }
    }
    // draw quads using generated points
    for (i = 0; i < N_STACKS; i++)
    {
        for (j = 0; j < N_SLICES; j++)
        {
            glBegin(GL_QUADS);

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

            glEnd();
        }
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
        // glutSolidSphere(radius, N_SLICES, N_STACKS);
        drawSphere(radius);
        glPopMatrix();
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
};

class Cube : public Object
{
public:
    double side_length;

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
};