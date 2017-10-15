#ifndef FACE_RECOG_EYE_DETECTOR_H
#define FACE_RECOG_EYE_DETECTOR_H

#include "FaceRecog.h"

class EyeDetector
{
public:
    EyeDetector();
    EyeDetector(double scale, int nms, cv::Size minSize, cv::Size maxSize);
    void initializeParameters(double scaleFactor, int nmsThreshold, cv::Size minSize, cv::Size maxSize);
    int loadDetector(std::string name);
    void findEyes(FACE_RECOG_MAT image, cv::Rect faceRoi = cv::Rect());
    std::vector<cv::Rect> getFoundEyes();

private:
    #if FACE_RECOG_USE_CUDA
    Ptr<cv::cuda::CascadeClassifier> _cascade
    #else
    FACE_RECOG_NAMESPACE::CascadeClassifier _cascade;
    #endif

    FACE_RECOG_MAT _searchImage;           // sub image ROI to search for eyes
    std::vector<cv::Rect> _foundEyes;   // relative position in ROI
    double _scaleFactor;
    int _nmsThreshold;
    cv::Size _minSize;
    cv::Size _maxSize;
    int _minNeighbours;
};

#endif // !FACE_RECOG_EYE_DETECTOR_H
