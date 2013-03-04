#ifndef STEREO_H_
#define STEREO_H_

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
public:
    Stereo(int, int);
    bool changeParameters(int=21);
    bool parameterChangeRequired();
    Mat disparityMap(StereoPair&);
    bool detectObjects(Mat&);
    FLAGS::VISUALS getVisualInfo();
    int getClosestObjectVal();

    int distanceToObject(Mat&, Mat&);
};

#endif /* STEREO_H_ */
