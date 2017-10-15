#include "FaceRecog.h"

ClassifierType::ClassifierType(int value)
{
    if      (value == ClassifierType::ENSEMBLE_ESVM)    _type = ClassifierType::ENSEMBLE_ESVM;
    else if (value == ClassifierType::ENSEMBLE_TM)      _type = ClassifierType::ENSEMBLE_TM;
    else if (value == ClassifierType::CNN_FACE_NET)     _type = ClassifierType::CNN_FACE_NET;
    else                                                _type = ClassifierType::UNDEFINED;
}

const std::string ClassifierType::name() const
{
    switch (_type) {
        case ClassifierType::ENSEMBLE_ESVM:     return "Ensemble of Exemplar-SVM";
        case ClassifierType::ENSEMBLE_TM:       return "Ensemble of Template Matchers";
        case ClassifierType::CNN_FACE_NET:      return "Convolutional Neural Network FaceNet";
        default:                                return "undefined";
    }
}

std::ostream& operator<<(std::ostream& os, const ClassifierType& ct)
{
    os << ct.name();
    return os;
}

std::istream& operator>>(std::istream& is, ClassifierType& ct)
{
    is >> ct._type;
    return is;
}

std::istream& operator>>(std::istream& is, ClassifierType::Value& v)
{
    int value;
    is >> value;
    v = (ClassifierType::Value)value;
    return is;
}
