#ifndef STEREO_H_
#define STEREO_H_

class Stereo
{
    void Initialise(int, int);

    StereoBM sbm;
    int nDisparity;
    Rect objectBoundingBox;
public:
    Stereo(int, int);
    void changeParameters(int, FLAGS::NUMDISPARITY = FLAGS::ABSOLUTE, int=21);
    Mat disparityMap(StereoPair&);
    Rect* closestObject(Mat&);
    int distanceToObject(Mat&, Mat&);
};

#endif /* STEREO_H_ */
