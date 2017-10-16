#ifndef FACE_RECOG_IFACE_DETECTOR_H
#define FACE_RECOG_IFACE_DETECTOR_H

#include "FaceRecog.h"

class IDetector
{
public:
    IDetector() {}
    virtual ~IDetector() {}
    virtual int loadDetector(std::string name);
    virtual std::vector<cv::Rect> mergeDetections(std::vector<std::vector<cv::Rect> >& bboxes);
    virtual void flipDetections(size_t index, std::vector<std::vector<cv::Rect> >& bboxes);
    virtual void cleanImages() { frames.clear(); }
    // pure virtual methods (mandatory overrides by derived classes)
    virtual void assignImage(const FACE_RECOG_MAT& frame) = 0;
    virtual int detect(std::vector<std::vector<cv::Rect> >& bboxes) = 0;
    virtual double evaluateConfidence(const Track& track, const FACE_RECOG_MAT& image) = 0;
    // utility methods
    size_t modelCount();
    std::string getModelPath(size_t modelIndex);
protected:
    std::vector<std::string> modelPaths;
    std::vector<FACE_RECOG_MAT> frames;
    double overlapThreshold;
};

std::shared_ptr<IDetector> buildSpecializedDetector(const ConfigFile& config, const std::string modelBasePath, DetectorType detectorType);

#endif/*FACE_RECOG_IFACE_DETECTOR_H*/
