#include <bits/stdc++.h>

using namespace std;

#define SCENE_FILE "../Test-Cases/1/scene.txt"
#define STAGE1_FILE "../Test-Cases/1/mystage1.txt"
#define STAGE2_FILE "../Test-Cases/1/mystage2.txt"
#define STAGE3_FILE "../Test-Cases/1/mystage3.txt"

int main()
{
    ifstream scene, config;
    ofstream stage;

    double eyeX, eyeY, eyeZ;
    double lookX, lookY, lookZ;
    double upX, upY, upZ;
    double fovY, aspectRatio, near, far;

    // Stage 1 Start
    // -------------
    scene.open(SCENE_FILE);
    stage.open(STAGE1_FILE);

    scene >> eyeX >> eyeY >> eyeZ;
    scene >> lookX >> lookY >> lookZ;
    scene >> upX >> upY >> upZ;
    scene >> fovY >> aspectRatio >> near >> far;

    return 0;
}