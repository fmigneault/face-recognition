#ifndef FACE_RECOG_ICLASSIFIER_H
#define FACE_RECOG_ICLASSIFIER_H

#include "FaceRecogConfig.h"

class IClassifier
{
public:
    virtual ~IClassifier() {}
    virtual void initialise() = 0;
    virtual std::vector<double> predict(const FACE_RECOG_MAT& roi) = 0;
};

std::shared_ptr<IClassifier> buildSpecializedClassifier(const ConfigFile& config,
                                                        const std::vector<std::vector<FACE_RECOG_MAT> >& positiveROIs,
                                                        const std::vector<std::string>& positiveIDs,
                                                        const std::vector<std::vector<FACE_RECOG_MAT> >& additionalNegativeROIs);

#endif /*FACE_RECOG_ICLASSIFIER_H*/