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
    FLAGS::NUMDISPARITY visualHistory[3];
    int visualHistoryIndex;
    int maxDisp;
    int minDisp;
    int numObjects;
    int objArea;
    int totalArea;
    double normaliseThresh;

    vector<Rect> boundingBoxes; //store all blob bounding boxes

    FLAGS::STEREOINFO stereoInfo;

    bool visualChangeAllowed();
    void addFrameToHistory();

public:
    Stereo(int);
    bool changeParameters(int);
    bool parameterChangeRequired();
    Mat disparityMap(StereoPair&);
    bool detectObjects(Mat&);
    void evaluate();

    FLAGS::VISUALS getVisualInfo();
    int getClosestObjectVal();
    int getNumObjects();
    int getClosestObjectArea();
    int getTotalArea();

    bool shouldBrake();
};

#endif /* STEREO_H_ */
