#include "FaceDetectors/EyeDetector.h"
#include <iostream>
#include <string>

EyeDetector::EyeDetector()
{
    initializeParameters(1.5, 4, cv::Size(5, 5), cv::Size(50, 50));
}

EyeDetector::EyeDetector(double scale, int nms, cv::Size minSize, cv::Size maxSize)
{
    initializeParameters(scale, nms, minSize, maxSize);
}

void EyeDetector::initializeParameters(double scaleFactor, int nmsThreshold, cv::Size minSize, cv::Size maxSize)
{
    _scaleFactor = scaleFactor;
    _nmsThreshold = nmsThreshold;
    _minSize = minSize;
    _maxSize = maxSize;
}

int EyeDetector::loadDetector(std::string name)
{
    bool success_load_cascade = false;
    #if FACE_RECOG_USE_CUDA
    Ptr<cv::cuda::CascadeClassifier> _cascade = cv::cuda::CascadeClassifier::create(name);
    success_load_cascade = !_cascade.empty();
    #else
    FACE_RECOG_NAMESPACE::CascadeClassifier cascade;
    success_load_cascade = _cascade.load(name);
    #endif

    if (!success_load_cascade)
    {
        std::cerr << "ERROR: Could not load classifier cascade" << name << std::endl;
        return -1;
    }
    else
        std::cout << "Cascade " << name << " successfully loaded" << std::endl;

    #if FACE_RECOG_USE_CUDA
    cascade->setScaleFactor(_scaleFactor);
    cascade->setMinObjectSize(_minSize);
    cascade->setMaxObjectSize(_maxSize);
    cascade->setMinNeighbors(_nmsThreshold);
    #endif

    return 0;
}

// Search only locally within the specified ROI, or globally if no ROI was specified
void EyeDetector::findEyes(FACE_RECOG_MAT image, cv::Rect faceROI)
{
    _searchImage = image(faceROI);
    _foundEyes.clear();
    _cascade.detectMultiScale(_searchImage, _foundEyes, _scaleFactor, _nmsThreshold, CV_HAAR_SCALE_IMAGE, _minSize, _maxSize);
};

// Return the absolute position of the ROIs of found eyes within the image specified
std::vector<cv::Rect> EyeDetector::getFoundEyes()
{
    std::vector<cv::Rect> absRect(_foundEyes);
    for (size_t i = 0; i < absRect.size(); i++)
    {
        cv::Size whole;
        cv::Point offset;
        _searchImage.locateROI(whole, offset);
        absRect[i] += offset;
    }
    return absRect;
}
