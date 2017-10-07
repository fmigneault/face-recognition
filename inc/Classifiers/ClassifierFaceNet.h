#ifndef FACE_RECOG_CLASSIFIER_FACE_NET_H
#define FACE_RECOG_CLASSIFIER_FACE_NET_H

#include "FaceRecog.h"


class ClassifierFaceNet final : public IClassifier 
{
public:
    ClassifierFaceNet();
    ClassifierFaceNet(std::vector<cv::Mat> positiveROIs, std::string negativeFileDir, std::vector<std::string> positiveIDs = {}) {}
    ~ClassifierFaceNet();
    inline virtual void initialise() override {}
    std::vector<double> predict(cv::Mat roi) override;
private:
    std::string folderPath = "../python";
    std::string filePath = "../python";
    std::string pyFuncInit  = "face_detect";
    std::string pyFuncPredict  = "face_detect";
    std::vector<cv::Rect> m_faces;
};

#endif /*FACE_RECOG_CLASSIFIER_FACE_NET_H*/
