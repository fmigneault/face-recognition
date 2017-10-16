#include "Detectors/DetectorType.h"
#include "FaceRecog.h"

DetectorType::DetectorType(int value)
{
    if      (value == DetectorType::FACE_DETECTOR_GLOBAL)   _type = DetectorType::FACE_DETECTOR_GLOBAL;
    else if (value == DetectorType::FACE_DETECTOR_LOCAL)    _type = DetectorType::FACE_DETECTOR_LOCAL;
    else if (value == DetectorType::EYE_DETECTOR)           _type = DetectorType::EYE_DETECTOR;
    else                                                    _type = DetectorType::UNDEFINED;
}

const std::string DetectorType::name() const
{
    switch (_type) {
        case DetectorType::FACE_DETECTOR_GLOBAL:    return "Global Face Detector";
        case DetectorType::FACE_DETECTOR_LOCAL:     return "Local Face Detector";
        case DetectorType::EYE_DETECTOR:            return "Eye Detector";
        default:                                    return "undefined";
    }
}

std::ostream& operator<<(std::ostream& os, const DetectorType& ct)
{
    os << ct.name();
    return os;
}

std::istream& operator>>(std::istream& is, DetectorType& ct)
{
    is >> ct._type;
    return is;
}

std::istream& operator>>(std::istream& is, DetectorType::Value& v)
{
    int value;
    is >> value;
    v = (DetectorType::Value)value;
    return is;
}
