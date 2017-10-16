#ifndef FACE_RECOG_FACE_DETECTOR_SSD_H
#define FACE_RECOG_FACE_DETECTOR_SSD_H

#include "FaceRecog.h"

class FaceDetectorSSD final : public IDetector
{
public:
    FaceDetectorSSD();
    inline ~FaceDetectorSSD() {}
    // specific to specialized class
    void initializeParameters(double scaleFactor, int nmsThreshold, cv::Size minSize, cv::Size maxSize,
                              cv::Size evalSize, int minNeighbours, double overlapThreshold);
    FaceDetectorSSD(double scaleFactor, int nmsThreshold, cv::Size minSize, cv::Size maxSize,
                    cv::Size evalSize, int minNeighbours, double overlapThreshold);
    void setDefaults();
    int loadDetector(std::string name, FlipMode faceFlipMode = NONE);
    // specialized overrides
    void assignImage(const FACE_RECOG_MAT& frame) override;
    int detect(std::vector<std::vector<cv::Rect> >& bboxes) override;
    double evaluateConfidence(const Track& track, const FACE_RECOG_MAT& image) override;
    void flipDetections(size_t index, vector<vector<Rect> >& faces) override;
    vector<Rect> mergeDetections(vector<vector<Rect> >& faces) override;

private:
    #if FACE_RECOG_USE_CUDA
    vector<Ptr<FACE_RECOG_NAMESPACE::CascadeClassifier>> faceFinder;
    FACE_RECOG_MAT foundObjects_gpu;        // Buffer to transfer gpu objects found with 'detectMultiScale' to 'vector<Rect>'
    #else
    std::vector<FACE_RECOG_NAMESPACE::CascadeClassifier> faceFinder;
    #endif

    std::vector<FlipMode> faceFlipModes;    // flip operation applied before processing faces for corresponding loaded CascadeClassifier / frames
    std::vector<int> stageCount;            // stage counts of loaded CascadeClassifiers

    cv::Size evalSize;  // classifier training window for detector confidence evaluation
    cv::Size minSize;
    cv::Size maxSize;
    double scaleFactor;
    int nmsThreshold;
    int minNeighbours;
};


#endif /* FACE_RECOG_FACE_DETECTOR_SSD_H */
