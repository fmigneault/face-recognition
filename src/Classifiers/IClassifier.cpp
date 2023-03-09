#include "FaceRecogConfig.h"

std::shared_ptr<IClassifier> buildSpecializedClassifier(const ConfigFile& config,
                                                        const std::vector<std::vector<FACE_RECOG_MAT> >& positiveROIs,
                                                        const std::vector<std::string>& positiveIDs,
                                                        const std::vector<std::vector<FACE_RECOG_MAT> >& additionalNegativeROIs)
{
    std::shared_ptr<IClassifier> classifier = nullptr;
    ClassifierType classifierType = config.getClassifierType();
    if (classifierType == ClassifierType::ENSEMBLE_TM)
    {
        if (positiveROIs.size() > 0)
            classifier.reset(new ClassifierEnsembleTM(positiveROIs, config.NEGDir, positiveIDs, additionalNegativeROIs));
        else
            classifier.reset(new ClassifierEnsembleTM());
    }
    #ifdef FACE_RECOG_HAS_ESVM
    else if (classifierType == ClassifierType::ENSEMBLE_ESVM)
    {
        if (positiveROIs.size() > 0)
            classifier.reset(new ClassifierEnsembleESVM(positiveROIs, config.NEGDir, positiveIDs, additionalNegativeROIs));
        else
            classifier.reset(new ClassifierEnsembleESVM());
    }
    #endif/*FACE_RECOG_HAS_ESVM*/
    #ifdef FACE_RECOG_HAS_FACE_NET
    else if (classifierType == ClassifierType::CNN_FACE_NET)
    {
        if (positiveIDs.size() > 0)
            classifier.reset(new ClassifierFaceNet(positiveROIs, config.NEGDir, positiveIDs));
        else
            classifier.reset(new ClassifierFaceNet());
    }
    #endif/*FACE_RECOG_HAS_FACE_NET*/
    return classifier;
}