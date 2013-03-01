#ifndef STEREO_H_
#define STEREO_H_

class Stereo
{
    void Initialise(int, int);

    StereoBM sbm;
public:
    Stereo(int, int);
    Mat disparityMap(StereoPair&);
    Rect objectPresent(Mat&);
    int distanceToObject(Mat&, Rect);
};

#endif /* STEREO_H_ */
