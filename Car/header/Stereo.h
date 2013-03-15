#ifndef STEREO_H_
#define STEREO_H_

#include "../header/Car.h"

class Stereo
{
    void Initialise(int, int);

    StereoBM sbm;

    int nDisparity;
    Rect objectBoundingBox;
    int closestObjectVal;
    FLAGS::NUMDISPARITY dispChange;
    FLAGS::VISUALS visual;
    int maxDisp;
    int minDisp;
    int numObjects;
    double objArea;
public:
    Stereo(int);
    bool changeParameters(int, Car&);
    bool parameterChangeRequired();
    Mat disparityMap(StereoPair&);
    bool detectObjects(Mat&);
    FLAGS::VISUALS getVisualInfo();
    int getClosestObjectVal();
    int getNumObjects();
    double getClosestObjectArea();
    bool texturelessObjectPresent();

    bool shouldBrake();

};

#endif /* STEREO_H_ */
