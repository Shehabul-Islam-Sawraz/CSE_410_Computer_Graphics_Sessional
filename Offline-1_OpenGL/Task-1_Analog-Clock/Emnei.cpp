#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <iostream>
#include <string> // for string

#include <GL/glut.h>

#define PI acos(-1)
#define HOUR_LEN 48
#define F first
#define S second
#define CLOCK_RADIUS 95         // Main clock circle radius
#define QUARTER_TIME_RADIUS 4.1 // Quarter time indicator circles radius
#define MIN_NOTE_START (CLOCK_RADIUS - 13)
#define MIN_NOTE_END (CLOCK_RADIUS - 6)
#define HOUR_NOTE_START (CLOCK_RADIUS - 20)
#define HOUR_NOTE_END (CLOCK_RADIUS - 5)
#define MIN_LEN (CLOCK_RADIUS - 13)
#define SEC_LEN (CLOCK_RADIUS - 13)
#define CENTER_X 0
#define CENTER_Y 120
#define BOB_RADIUS 20
#define BOB_AMPLITUDE 14
#define ROPE_LENGTH 150
#define TIME_START_X 30
#define TIME_START_Y 50

using namespace std;

float theta_bob = BOB_AMPLITUDE;
float angle = 0.0;
int flag = 1;
float period = 2.0;
int hour, minute, second;

/* Convert angle from Degree to Radian */
double toRad(double ang)
{
    return PI * ang / 180;
}

void DrawCircle(float cx, float cy, float r)
{
    glBegin(GL_POLYGON);
    for (float theta = 0; theta < 360; theta += 0.5)
    {
        float x = cx + r * cos(toRad(theta));
        float y = cy + r * sin(toRad(theta));
        glVertex2f(x, y);
    }
    glEnd();
}

void drawBorder()
{
    glVertex2d(-CLOCK_RADIUS, 38);
    glVertex2d(-CLOCK_RADIUS, -150);
    glVertex2d(0, -230);
    glVertex2d(CLOCK_RADIUS, -150);
    glVertex2d(CLOCK_RADIUS, 38);
}

void drawPendulumBorder()
{
    glLineWidth(7);
    // Bottom part
    glBegin(GL_TRIANGLE_FAN);
    glColor3f(.80, .67, .49);
    drawBorder();
    glEnd();
    glBegin(GL_LINE_STRIP);
    glColor3f(0.55, 0.39, 0.26);
    drawBorder();
    glEnd();
}

string format(int val)
{
    return (((val < 9) ? "0" : "") + to_string(val));
}

void DrawClockTime()
{
    glColor3f(.80, .67, .49);
    glBegin(GL_POLYGON);
    glVertex2d(CENTER_X - TIME_START_X - 3, CENTER_Y + TIME_START_Y - 3);
    glVertex2d(CENTER_X - TIME_START_X - 3, CENTER_Y + TIME_START_Y + 15);
    glVertex2d(CENTER_X - TIME_START_X + 65, CENTER_Y + TIME_START_Y + 15);
    glVertex2d(CENTER_X - TIME_START_X + 65, CENTER_Y + TIME_START_Y - 3);
    glEnd();
    glColor3f(0, 0, 0);
    glRasterPos2f(CENTER_X - TIME_START_X, CENTER_Y + TIME_START_Y);
    // string exten = (hour >= 12) ? " pm" : " am";
    // hour %= 12; // Make hour in 12 format
    string time = format(hour) + ":" + format(minute) + ":" + format(second);
    for (int i = 0; i < time.size(); i++)
        glutBitmapCharacter(GLUT_BITMAP_8_BY_13, time[i]);
}

void BobTimer(int value)
{
    // Update the angle based on the time and period
    float t = glutGet(GLUT_ELAPSED_TIME) / 1000.0f; // Convert milliseconds to seconds
    float angularFrequency = 2.0f * PI / period;
    angle = BOB_AMPLITUDE * sin(angularFrequency * t); // Simple harmonic motion formula
    // printf("Angle %lf\n", angle);
    glutPostRedisplay();
    glutTimerFunc(16, BobTimer, value); // Update every 16 milliseconds (60 FPS)
}

void BobMotion()
{
    glColor3f(.65, .39, .15);
    glPushMatrix();
    glRotatef(angle, 0, 0, 1);
    DrawCircle(0, -9, 12);
    glLineWidth(7);
    glBegin(GL_LINES);
    glVertex2d(0, -10);
    glVertex2d(0, -10 - ROPE_LENGTH);
    glEnd();
    glBegin(GL_TRIANGLE_FAN);
    for (int i = 0; i <= 360; i++)
    {
        double angle = 2 * PI * i / 360;
        double x = BOB_RADIUS * cos(angle);
        double y = BOB_RADIUS * sin(angle);
        glVertex2d(x, y - 170);
    }
    glEnd();
    glPopMatrix();
}

void line(double x1, double y1, double x2, double y2)
{
    glBegin(GL_LINES);
    glVertex2f(x1, y1);
    glVertex2f(x2, y2);
    glEnd();
}

void initGL(void)
{
    glClearColor(.35, .4, .38, 1.0); // Set background color to Dark Ash
}

/* Handler for window re-size event. Called back when the window first appears and
   whenever the window is re-sized with its new width and height */
void reshape(GLsizei width, GLsizei height)
{
    // Set the viewport to cover the new window
    glViewport(0, 0, width, height);
    // Set the aspect ratio of the clipping area to match the viewport
    glMatrixMode(GL_PROJECTION); // To operate on the Projection matrix
    glLoadIdentity();            // Reset the projection matrix
    gluOrtho2D(-width / 2.0, width / 2.0 + 1, -height / 2.0, height / 2.0 + 1);
}

double x1, yy1, x2, y2, x3, y3;
int n;

double angRotate(double x)
{
    x = x - 90;
    x = 360 - x; // Getting angle from the direction of axis(9pm-----3 pm)
    return x;
}

void callLine(int LEN, double ang)
{
    line(CENTER_X, CENTER_Y, CENTER_X + LEN * cos(toRad(ang)), CENTER_Y + LEN * sin(toRad(ang)));           // Clock hand line
    line(CENTER_X, CENTER_Y, CENTER_X + 20 * cos(toRad(180 + ang)), CENTER_Y + 20 * sin(toRad(180 + ang))); // The extra part of clock hand
}

void hourFunc(double ang)
{
    ang = angRotate(ang);
    glLineWidth(5);
    glColor3f(0, 0, 0);
    callLine(HOUR_LEN, ang);
}
void minFunc(double ang)
{
    ang = angRotate(ang);
    glLineWidth(3);
    glColor3f(0, 0, 0);
    callLine(MIN_LEN, ang);
}
void secFunc(double ang)
{
    ang = angRotate(ang);
    glLineWidth(2);
    glColor3f(0, 0, 0);
    callLine(SEC_LEN, ang);
}

void perSecond()
{
    // Clock Time
    DrawClockTime();
    time_t _sec = time(NULL); // Getting current time in GMT-0
    int sec = (int)_sec;
    sec += 6 * 3600;  // Convert time to GMT+6(as we are currently in this GMT)
    sec %= 24 * 3600; // Getting hours(in range of seconds from 0 to 24*3600) from total seconds
    // printf("Second: %d\n", sec);

    hour = (int)sec / 3600; // Getting value of hours

    sec %= 12 * 3600;                  // Getting hours(in range of seconds from 0 to 12*3600) from total seconds
    hourFunc(sec * 360 / (12 * 3600)); // Getting degree of hours from 0 to 360 degree

    sec %= 3600; // Getting value of minutes(in range of seconds from 0-3600)

    minute = sec / 60; // Getting value of minutes

    minFunc(sec * 360 / 3600);

    sec %= 60;
    second = sec; // Getting value of minutes
    secFunc(sec * 360 / 60);
}

void display()
{
    glClear(GL_COLOR_BUFFER_BIT); // Clear the color buffer
    // Draw Pendulum Box
    drawPendulumBorder();

    glColor3f(.38, .22, .09);
    DrawCircle(CENTER_X, CENTER_Y, CLOCK_RADIUS + 30);
    glColor3f(.47, .28, .11);
    DrawCircle(CENTER_X, CENTER_Y, CLOCK_RADIUS + 20);
    glColor3f(.65, .39, .15);
    DrawCircle(CENTER_X, CENTER_Y, CLOCK_RADIUS + 10);

    // clock background
    glColor3f(.91, .89, .76);
    DrawCircle(CENTER_X, CENTER_Y, CLOCK_RADIUS);

    glColor3f(0.75, 0.62, 0.3);
    DrawCircle(CENTER_X + CLOCK_RADIUS - 10, CENTER_Y, QUARTER_TIME_RADIUS);
    DrawCircle(CENTER_X, CENTER_Y + CLOCK_RADIUS - 10, QUARTER_TIME_RADIUS);
    DrawCircle(CENTER_X - (CLOCK_RADIUS - 10), CENTER_Y, QUARTER_TIME_RADIUS);
    DrawCircle(CENTER_X, CENTER_Y - (CLOCK_RADIUS - 10), QUARTER_TIME_RADIUS);

    // minute indicator
    // glColor3f(0.86, 0.71, 0.35);
    glColor3f(0, 0, 0);
    glLineWidth(2.5);
    for (int i = 0; i < 60; i++)
    {
        if (i % 15 == 0) // For quarter time we have drawn circles
            continue;
        line(CENTER_X + MIN_NOTE_START * cos(toRad(i * 6)), CENTER_Y + MIN_NOTE_START * sin(toRad(i * 6)), CENTER_X + MIN_NOTE_END * cos(toRad(i * 6)), CENTER_Y + MIN_NOTE_END * sin(toRad(i * 6))); // For each second, 6 degree is rotated
    }

    // hour indicator
    glLineWidth(3.5);
    for (int i = 0; i < 12; i++)
    {
        if (i % 3 == 0) // For quarter time we have drawn circles
            continue;
        line(CENTER_X + HOUR_NOTE_START * cos(toRad(i * 30)), CENTER_Y + HOUR_NOTE_START * sin(toRad(i * 30)), CENTER_X + HOUR_NOTE_END * cos(toRad(i * 30)), CENTER_Y + HOUR_NOTE_END * sin(toRad(i * 30)));
    }

    perSecond();

    // center
    glColor3f(.55, .56, .59);
    DrawCircle(CENTER_X, CENTER_Y, 5);

    // Draw pendulum
    BobMotion();
    glFlush();
    glutSwapBuffers();
}

void Timer(int value)
{
    glutPostRedisplay();
    glutTimerFunc(1000, Timer, value); // Changing time after every 1s
}

int main(int argc, char **argv)
{
    glutInit(&argc, argv);
    glutInitWindowSize(500, 600);
    glutCreateWindow("Analog Clock With Pendulum");
    glutDisplayFunc(display);
    glutReshapeFunc(reshape); // Register callback handler for window re-size event
    initGL();
    Timer(0);
    BobTimer(0);
    glutMainLoop();
    return 0;
}