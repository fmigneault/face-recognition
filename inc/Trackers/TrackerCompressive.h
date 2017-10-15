#ifndef FACE_RECOG_TRACKER_COMPRESSIVE_H
#define FACE_RECOG_TRACKER_COMPRESSIVE_H

#include "FaceRecog.h"

class TrackerCompressive final : public ITracker
{
public:
    TrackerCompressive(ConfigFile *configFile);
    virtual ~TrackerCompressive();
    TrackerCompressive(const TrackerCompressive &obj);  // copy constructor
    TrackerCompressive & operator=(const TrackerCompressive &T); // assignment operator
    virtual void initialize(const ImageRep& frame, FloatRect bb);
    virtual void reset();
    virtual cv::Rect track(const ImageRep& frame);
private:
    // FloatRect m_bb;
    cv::Rect bbox;
    bool m_initialized;
    int featureMinNumRect;
    int featureMaxNumRect;
    int featureNum;
    std::vector<std::vector<cv::Rect>> features;
    std::vector<std::vector<float>> featuresWeight;
    int rOuterPositive;
    std::vector<cv::Rect> samplePositiveBox;
    std::vector<cv::Rect> sampleNegativeBox;
    int rSearchWindow;
    cv::Mat imageIntegral;
    cv::Mat samplePositiveFeatureValue;
    cv::Mat sampleNegativeFeatureValue;
    std::vector<float> muPositive;
    std::vector<float> sigmaPositive;
    std::vector<float> muNegative;
    std::vector<float> sigmaNegative;
    float learnRate;
    std::vector<cv::Rect> detectBox;
    cv::Mat detectFeatureValue;
    cv::RNG rng;

private:
    void computeHaarFeature(cv::Rect& _objectBox, int _numFeature);
    void sampleRect(const cv::Mat& _image, cv::Rect& _objectBox, float _rInner, float _rOuter, int _maxSampleNum, std::vector<cv::Rect>& _sampleBox);
    void sampleRect(const cv::Mat& _image, cv::Rect& _objectBox, float _srw, std::vector<cv::Rect>& _sampleBox);
    void getFeatureValue(cv::Mat& _imageIntegral, std::vector<cv::Rect>& _sampleBox, cv::Mat& _sampleFeatureValue);
    void classifierUpdate(cv::Mat& _sampleFeatureValue, std::vector<float>& _mu, std::vector<float>& _sigma, float _learnRate);
    void radioClassifier(std::vector<float>& _muPos, std::vector<float>& _sigmaPos, std::vector<float>& _muNeg, std::vector<float>& _sigmaNeg,
                         cv::Mat& _sampleFeatureValue, float& _radioMax, int& _radioMaxIndex);

};

#endif /*FACE_RECOG_TRACKER_COMPRESSIVE_H*/
