#ifdef FACE_RECOG_HAS_ESVM

#include "Classifiers/ClassifierEnsembleESVM.h"
#include "FaceRecog.h"

ClassifierEnsembleESVM::ClassifierEnsembleESVM()
{
    EoESVM.reset(new esvmEnsemble());
}

ClassifierEnsembleESVM::ClassifierEnsembleESVM(const std::vector<std::vector<FACE_RECOG_MAT>>& positiveROIs,
                                               const std::string& negativeFileDir,
                                               const std::vector<std::string>& positiveIDs,
                                               const std::vector<std::vector<FACE_RECOG_MAT>>& additionalNegativeROIs,
                                               const std::string& modelsFileDir)
{
    size_t nPositives = positiveROIs.size();
    size_t nAdditionalNegatives = additionalNegativeROIs.size();
    std::vector<std::vector<cv::Mat>> posROI(nPositives);
    std::vector<std::vector<cv::Mat>> negROI;
    if (nPositives == nAdditionalNegatives)
        negROI = std::vector<std::vector<cv::Mat>>(nAdditionalNegatives);

    for (size_t pos = 0; pos < nPositives; ++pos) {
        size_t nRepresentations = positiveROIs[pos].size();
        posROI[pos] = std::vector<cv::Mat>(nRepresentations);
        for (size_t r = 0; r < nRepresentations; ++r)
            posROI[pos][r] = GET_MAT(positiveROIs[pos][r], cv::ACCESS_READ);
        if (nPositives == nAdditionalNegatives) {
            size_t nNegatives = additionalNegativeROIs[pos].size();
            negROI[pos] = std::vector<cv::Mat>(nNegatives);
            for (size_t neg = 0; neg < nNegatives; ++neg)
                negROI[pos][neg] = GET_MAT(additionalNegativeROIs[pos][neg], cv::ACCESS_READ);
        }
    }
    EoESVM.reset(new esvmEnsemble(posROI, negativeFileDir, positiveIDs, negROI));
    if (!modelsFileDir.empty())
        EoESVM->saveModels(modelsFileDir);
}

std::vector<double> ClassifierEnsembleESVM::predict(const FACE_RECOG_MAT& roi)
{
    return EoESVM->predict(GET_MAT(roi, ACCESS_READ));
}

#endif/*FACE_RECOG_USE_ESVM*/
