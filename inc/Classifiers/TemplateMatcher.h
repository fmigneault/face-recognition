#ifndef FACE_RECOG_TEMPLATE_MATCHER_H
#define FACE_RECOG_TEMPLATE_MATCHER_H

#include "FaceRecogConfig.h"
#include "feHOG.h"

class TemplateMatcher
{
public:
    TemplateMatcher() {};
    TemplateMatcher(const std::vector<std::vector<FACE_RECOG_MAT> >& positiveROIs, const std::string negativesDir,
                    const std::vector<std::string>& positiveIDs ={}, const std::vector<std::vector<FACE_RECOG_MAT> >& additionalNegativeROIs ={});
    std::vector<double> predict(const FACE_RECOG_MAT& roi);
    inline size_t getPositiveCount() { return enrolledPositiveIDs.size(); }
    inline size_t getPatchCount() { return patchCounts.area(); }
    inline std::string getPositiveID(int positiveIndex);
    virtual ~TemplateMatcher() {}

private:
    void setConstants();
    void updateNormFeatures(const xstd::mvector<3, FeatureVector>& positiveSamples,
                            const xstd::mvector<2, FeatureVector>& negativeSamples);

    // distances
    double similarityFromEuclideanDistance(const FeatureVector& probeSample, const FeatureVector& templateSample);

    // constants
    cv::Size imageSize;
    cv::Size patchCounts;
    cv::Size blockSize;
    cv::Size blockStride;
    cv::Size cellSize;
    int nBins;
    FeatureExtractorHOG hog;
    std::string sampleFileExt;
    FileFormat sampleFileFormat;

    std::vector<std::string> enrolledPositiveIDs;
    xstd::mvector<3, FeatureVector> patchTemplates;                 // [patch][positive][representation](FeatureVector)

    // found min/max values from negative samples files + trained positives
    std::vector<FeatureVector> hogPatchFeaturesMin;                 // [patch](FeatureVector) <min>
    std::vector<FeatureVector> hogPatchFeaturesMax;                 // [patch](FeatureVector) <max>
};

#endif /*FACE_RECOG_TEMPLATE_MATCHER_H*/
