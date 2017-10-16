#ifndef FACE_RECOG_EYE_DETECTOR_H
#define FACE_RECOG_EYE_DETECTOR_H

#include "FaceRecog.h"

class EyeDetector final : public IDetector
{
public:
    EyeDetector();
    EyeDetector(double scale, int nms, cv::Size minSize, cv::Size maxSize);
    void initializeParameters(double scaleFactor, int nmsThreshold, cv::Size minSize, cv::Size maxSize);
    int loadDetector(std::string name);
    // specialized overrides
    void assignImage(const FACE_RECOG_MAT& frame) override;
    double evaluateConfidence(const Track& track, const FACE_RECOG_MAT& image) override;
    void flipDetections(size_t index, vector<vector<Rect> >& faces) override;
    int detect(vector<vector<Rect> >& bboxes) override;
    // utility members
    std::vector<cv::Rect> getFoundEyes(std::vector<cv::Rect> relRect);
    size_t rightEyeIndex;
    size_t leftEyeIndex;

private:
    #if FACE_RECOG_USE_CUDA
    std::vector<Ptr<cv::cuda::CascadeClassifier> > _eyeCascades;
    #else
    std::vector<FACE_RECOG_NAMESPACE::CascadeClassifier> _eyeCascades;
    #endif

    FACE_RECOG_MAT _searchImage;        // sub image ROI to search for eyes
    double _scaleFactor;
    int _nmsThreshold;
    cv::Size _minSize;
    cv::Size _maxSize;
    int _minNeighbours;
};

#endif // !FACE_RECOG_EYE_DETECTOR_H
