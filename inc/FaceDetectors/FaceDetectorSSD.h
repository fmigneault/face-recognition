#ifndef FACE_RECOG_FACE_DETECTOR_SSD_H
#define FACE_RECOG_FACE_DETECTOR_SSD_H

#include "FaceRecogConfig.h"


class FaceDetectorSSD final : public IFaceDetector
{
public:
    FaceDetectorSSD();
    inline ~FaceDetectorSSD() {}
    // specific to specialized class
    void initializeParameters(double scaleFactor, int nmsThreshold, cv::Size minSize, cv::Size maxSize,
                              cv::Size evalSize, int minNeighbours, double overlapThreshold);
    FaceDetectorSSD(double scaleFactor, int nmsThreshold, cv::Size minSize, cv::Size maxSize,
                   cv::Size evalSize, int minNeighbours, double overlapThreshold);
    void SetDefaults();
    int loadDetector(std::string name, FlipMode faceFlipMode = NONE);
    // specialized overrides
    void assignImage(FACE_RECOG_MAT frame) override;
    int findFaces(std::vector<std::vector<cv::Rect> >& faces) override;
    double evaluateConfidence(Track& track, FACE_RECOG_MAT& image) override;
    void flipFaces(size_t index, vector<vector<Rect> >& faces) override;
    vector<Rect> mergeDetections(vector<vector<Rect> >& faces) override;

private:
    #if FACE_RECOG_USE_CUDA
    vector<Ptr<FACE_RECOG_NAMESPACE::CascadeClassifier>> faceFinder;
    FACE_RECOG_MAT foundObjects_gpu;   // Buffer to transfer gpu objects found with 'detectMultiScale' to 'vector<Rect>'
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
