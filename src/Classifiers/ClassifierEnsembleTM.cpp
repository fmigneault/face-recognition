// Ensemble of Template Matcher classifiers
#include "FaceRecogConfig.h"

ClassifierEnsembleTM::ClassifierEnsembleTM()
{
    initialise();
}

void ClassifierEnsembleTM::initialise()
{
    TM.reset(new TemplateMatcher());
}

ClassifierEnsembleTM::ClassifierEnsembleTM(const std::vector<std::vector<FACE_RECOG_MAT> >& positiveROIs, const std::string negativeFileDir,
                                           const std::vector<std::string>& positiveIDs, 
                                           const std::vector<std::vector<FACE_RECOG_MAT> >& additionalNegativeROIs)
{
    TM.reset(new TemplateMatcher(positiveROIs, negativeFileDir, positiveIDs, additionalNegativeROIs));
}

std::vector<double> ClassifierEnsembleTM::predict(const FACE_RECOG_MAT& roi)
{
    return TM->predict(roi);
}