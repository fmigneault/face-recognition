/*
    Original Code Reference

    FAST-DT: FAce STructured Detection and Tracking
    Francesco Comaschi (f.comaschi@tue.nl)

    Code to accompany the paper:

        Online Multi-Face Detection and Tracking using Detector Confidence and Structured SVMs
        F. Comaschi, S. Stuijk, T. Basten, H. Corporaal
        Advanced Video and Signal-Based Surveillance (AVSS), 2015

    Multiple updates and modifications applied to introduce improvements or combined use with projet
*/

#ifndef FACE_RECOG_FACE_DETECTOR_VJ_H
#define FACE_RECOG_FACE_DETECTOR_VJ_H

#include "FaceRecog.h"

class FaceDetectorVJ final : public IDetector
{
public:
    FaceDetectorVJ();
    inline ~FaceDetectorVJ() {}
    // specific to specialized class
    void initializeParameters(double scaleFactor, int nmsThreshold, cv::Size minSize, cv::Size maxSize,
                              cv::Size evalSize, int minNeighbours, double overlapThreshold);
    FaceDetectorVJ(double scaleFactor, int nmsThreshold, cv::Size minSize, cv::Size maxSize,
                   cv::Size evalSize, int minNeighbours, double overlapThreshold);
    void setDefaults();
    int loadDetector(std::string modelPath, FlipMode faceFlipMode = NONE);
    // specialized overrides
    void assignImage(const FACE_RECOG_MAT& frame) override;
    int detect(std::vector<std::vector<cv::Rect> >& bboxes) override;
    double evaluateConfidence(const Track& track, const FACE_RECOG_MAT& image) override;
    void flipDetections(size_t index, vector<vector<Rect> >& bboxes) override;
    vector<Rect> mergeDetections(vector<vector<Rect> >& bboxes) override;

private:
    #if FACE_RECOG_USE_CUDA
    vector<Ptr<FACE_RECOG_NAMESPACE::CascadeClassifier>> faceFinder;
    FACE_RECOG_MAT foundObjects_gpu;        // Buffer to transfer gpu objects found with 'detectMultiScale' to 'vector<Rect>'
    #else
    std::vector<FACE_RECOG_NAMESPACE::CascadeClassifier> faceFinder;
    #endif

    std::vector<FlipMode> faceFlipModes;    // flip operation applied before processing bboxes for corresponding loaded CascadeClassifier / frames
    std::vector<int> stageCount;            // stage counts of loaded CascadeClassifiers

    cv::Size evalSize;  // classifier training window for detector confidence evaluation
    cv::Size minSize;
    cv::Size maxSize;
    double scaleFactor;
    int nmsThreshold;
    int minNeighbours;
};

#endif /* FACE_RECOG_FACE_DETECTOR_VJ_H */
