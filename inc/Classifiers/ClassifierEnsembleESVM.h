#ifndef CLASSIFIER_ENSEMBLE_ESVM_H
#define CLASSIFIER_ENSEMBLE_ESVM_H

#include "FaceRecog.h"
#include "esvmEnsemble.h"

class ClassifierEnsembleESVM final : public IClassifier
{
public:
    ClassifierEnsembleESVM();
    ClassifierEnsembleESVM(const std::vector<std::vector<FACE_RECOG_MAT> >& positiveROIs, std::string negativeFileDir,
                           const std::vector<std::string>& positiveIDs ={}, 
                           const std::vector<std::vector<FACE_RECOG_MAT> >& additionalNegativeROIs ={});
    inline virtual void initialise() override {}
    std::vector<double> predict(const FACE_RECOG_MAT& roi) override;
private:
    std::shared_ptr<esvmEnsemble> EoESVM;
};

#endif /*CLASSIFIER_ENSEMBLE_ESVM_H*/
