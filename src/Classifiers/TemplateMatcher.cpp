#include "Classifiers/TemplateMatcher.h"

using namespace cv;
using namespace std;

TemplateMatcher::TemplateMatcher(const std::vector<std::vector<FACE_RECOG_MAT> >& positiveROIs, const std::string negativeFileDir,
                                 const std::vector<std::string>& positiveIDs, 
                                 const std::vector<std::vector<FACE_RECOG_MAT> >& additionalNegativeROIs)
{
    setConstants();

    // add ID as indexes if omitted or not matching dimensions
    size_t nPositives = positiveROIs.size();
    if (positiveIDs.size() == nPositives)
        enrolledPositiveIDs = positiveIDs;
    else
    {
        enrolledPositiveIDs = std::vector<std::string>(nPositives);
        for (size_t pos = 0; pos < nPositives; ++pos)
            enrolledPositiveIDs[pos] = std::to_string(pos);
    }

    // load negatives to find features min/max    
    size_t nPatches = getPatchCount();
    size_t dimsNegatives[2]{ nPatches, 0 };
    xstd::mvector<2, FeatureVector> negativeSamples(dimsNegatives);
    for (size_t p = 0; p < nPatches; ++p)
        DataFile::readSampleDataFile(negativeFileDir + "negatives-hog-patch" + std::to_string(p) + 
                                     sampleFileExt, negativeSamples[p], sampleFileFormat);

    // get positive sample representations
    size_t dimsPatchPositive[3]{ nPatches, nPositives, 0 };
    patchTemplates = xstd::mvector<3, FeatureVector>(dimsPatchPositive);
    for (size_t pos = 0; pos < nPositives; ++pos)
    {
        size_t nRepresentations = positiveROIs[pos].size();
        for (size_t p = 0; p < nPatches; ++p)
            patchTemplates[p][pos] = std::vector<FeatureVector>(nRepresentations);
        for (size_t r = 0; r < nRepresentations; ++r) {
            std::vector<FACE_RECOG_MAT> patches = imPreprocess(positiveROIs[pos][r], imageSize, patchCounts);
            for (size_t p = 0; p < nPatches; ++p) {
                cv::Mat patch = GET_MAT(patches[p], ACCESS_READ);
                patchTemplates[p][pos][r] = hog.compute(patch);
            }
        }
    }

    updateNormFeatures(patchTemplates, negativeSamples);
}

void TemplateMatcher::setConstants()
{
    patchCounts = cv::Size(3, 3);
    imageSize = cv::Size(48, 48);
    blockSize = cv::Size(2, 2);
    blockStride = cv::Size(2, 2);
    cellSize = cv::Size(2, 2);
    nBins = 3;
    hog.initialize(imageSize, blockSize, blockStride, cellSize, nBins);

    sampleFileExt = ".bin";
    sampleFileFormat = BINARY;
}

std::string TemplateMatcher::getPositiveID(int positiveIndex)
{
    size_t nPositives = getPositiveCount();
    return (nPositives != 0 && positiveIndex >= 0 && positiveIndex < nPositives) ? enrolledPositiveIDs[positiveIndex] : "";
}

void TemplateMatcher::updateNormFeatures(const xstd::mvector<3, FeatureVector>& positiveSamples,
                                         const xstd::mvector<2, FeatureVector>& negativeSamples)
{
    size_t nPatches = getPatchCount();
    if (hogPatchFeaturesMin.size() == 0 || hogPatchFeaturesMax.size() == 0)
    {
        hogPatchFeaturesMin = std::vector<FeatureVector>(nPatches);
        hogPatchFeaturesMax = std::vector<FeatureVector>(nPatches);
    }

    for (size_t p = 0; p < nPatches; ++p) {
        std::vector<FeatureVector> normFeatureVectors(negativeSamples[p]);
        for (size_t r = 0; r < positiveSamples[p].size(); ++r) {
            normFeatureVectors.insert(normFeatureVectors.begin(), positiveSamples[p][r].begin(), positiveSamples[p][r].end());
            findNormParamsPerFeature(MIN_MAX, normFeatureVectors, hogPatchFeaturesMin[p], hogPatchFeaturesMax[p]);
        }
    }
}

std::vector<double> TemplateMatcher::predict(const FACE_RECOG_MAT& roi)
{
    size_t nPatches = getPatchCount();
    size_t nPositives = getPositiveCount();

    std::vector<FeatureVector> probeSampleFeatures(nPatches);
    std::vector<FACE_RECOG_MAT> patches = imPreprocess(roi, imageSize, patchCounts);

    size_t dimsScores[2]{ nPositives, 0 };
    xstd::mvector<2, double> scores(dimsScores, 0.0);
    std::vector<double> templateScores(nPositives, 0.0);
    for (size_t p = 0; p < nPatches; ++p) {
        cv::Mat patch = GET_MAT(patches[p], ACCESS_READ);
        probeSampleFeatures[p] = normalizePerFeature(MIN_MAX, hog.compute(patch), hogPatchFeaturesMin[p], hogPatchFeaturesMax[p]);
        for (size_t pos = 0; pos < nPositives; ++pos)
            for (size_t r = 0; r < patchTemplates[p][pos].size(); ++r)
                scores[pos][r] += similarityFromEuclideanDistance(probeSampleFeatures[p], patchTemplates[p][pos][r]);
    }
    for (size_t pos = 0; pos < nPositives; ++pos) {
        size_t nRepresentations = scores[pos].size();
        for (size_t r = 0; r < nRepresentations; ++r) {
            scores[pos][r] /= (double)nPatches;
            templateScores[pos] += scores[pos][r];
        }
        templateScores[pos] /= (double)nRepresentations;
    }

    return templateScores;
}

double TemplateMatcher::similarityFromEuclideanDistance(const FeatureVector& probeSample, const FeatureVector& templateSample)
{
    size_t nFeatures = templateSample.size();
    assert(probeSample.size() == nFeatures);
    double dist = 0;
    for (size_t f = 0; f < nFeatures; ++f)
        dist += (probeSample[f] - templateSample[f]) * (probeSample[f] - templateSample[f]);
    return 1 - std::sqrt(dist) / std::sqrt((double)nFeatures);
}
