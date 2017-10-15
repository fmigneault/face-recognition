#ifndef FACE_RECOG_CLASSIFIER_ENSEMBLE_TEMPLATE_MATCHER_H
#define FACE_RECOG_CLASSIFIER_ENSEMBLE_TEMPLATE_MATCHER_H

#include "FaceRecog.h"
#include "Classifiers/TemplateMatcher.h"

class ClassifierEnsembleTM final : public IClassifier
{
public:
    ClassifierEnsembleTM();
    ClassifierEnsembleTM(const std::vector<std::vector<FACE_RECOG_MAT> >& positiveROIs, const std::string negativeFileDir,
                         const std::vector<std::string>& positiveIDs = {},
                         const std::vector<std::vector<FACE_RECOG_MAT> >& additionalNegativeROIs = {});
    ~ClassifierEnsembleTM() {}
    void initialise() override;
    std::vector<double> predict(const FACE_RECOG_MAT& roi) override;
    std::string targetID;
private:
    std::shared_ptr<TemplateMatcher> TM;
};

#endif /*FACE_RECOG_CLASSIFIER_ENSEMBLE_TEMPLATE_MATCHER_H*/
