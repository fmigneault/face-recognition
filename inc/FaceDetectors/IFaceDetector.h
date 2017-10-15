#ifndef FACE_RECOG_IFACE_DETECTOR_H
#define FACE_RECOG_IFACE_DETECTOR_H

#include "FaceRecog.h"

#if CV_VERSION_MAJOR == 3
/* Declaration of global variable */
// extern float globalScore;
#elif CV_VERSION_EPOCH == 2
/*nothing, but checked*/
#else
#error "OpenCV version not confirmed"
#endif

class IFaceDetector
{
public:
    IFaceDetector() {}
    virtual ~IFaceDetector() {}
    virtual int loadDetector(std::string name);
    virtual std::vector<cv::Rect> mergeDetections(std::vector<std::vector<cv::Rect> >& faces);
    virtual void flipFaces(size_t index, std::vector<std::vector<cv::Rect> >& faces);
    virtual void cleanImages() { frames.clear(); }
    virtual void assignImage(FACE_RECOG_MAT frame);
    // pure virtual methods (mandatory overrides by derived classes)
    virtual int findFaces(std::vector<std::vector<cv::Rect> >& faces) = 0;
    virtual double evaluateConfidence(Track& track, FACE_RECOG_MAT& image) = 0;
protected:
    std::vector<FACE_RECOG_MAT> frames;
    double overlapThreshold;
};

#endif/*FACE_RECOG_IFACE_DETECTOR_H*/
