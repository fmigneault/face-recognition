#ifndef FACE_RECOG_DETECTOR_TYPE
#define FACE_RECOG_DETECTOR_TYPE

#include "Utilities/Common.h"

class DetectorType
{
public:
    enum Value
    {
        UNDEFINED               = -1,
        FACE_DETECTOR_GLOBAL    =  0,
        FACE_DETECTOR_LOCAL     =  1,
        EYE_DETECTOR            =  2,
    };
    inline DetectorType() { DetectorType(-1); }
    inline DetectorType(Value type) { _type = type; }
    DetectorType(int value);
    const std::string name() const;
    inline bool isDefined() const { return _type != UNDEFINED; }
    inline virtual bool operator==(const Value& DetectorType) const { return _type == DetectorType; }
    inline virtual bool operator!=(const Value& DetectorType) const { return _type != DetectorType; }
    friend std::ostream& operator<<(std::ostream& os, const DetectorType& ct);
    friend std::istream& operator>>(std::istream& is, DetectorType& ct);
    friend std::istream& operator>>(std::istream& is, Value& v);
private:
    Value _type;
};

#endif/*FACE_RECOG_DETECTOR_TYPE*/
