#ifndef FACE_RECOG_CAMERA_TYPE
#define FACE_RECOG_CAMERA_TYPE

#include "FaceRecog.h"

class CameraType
{
public:
    enum Value
    {
        UNDEFINED           = -2,
        FILE_STREAM         = -1,
        CV_VIDEO_CAPTURE    =  0,
        PGR_FLYCAPTURE2     =  1,
    };
    inline CameraType() { CameraType(-2); }
    inline CameraType(Value type) { _type = type; }
    CameraType(int value);
    const std::string name() const;
    inline bool isDefined() const { return _type != UNDEFINED; }
    inline virtual bool operator==(const Value& cameraType) const { return _type == cameraType; }
    inline virtual bool operator!=(const Value& cameraType) const { return _type != cameraType; }
    friend std::ostream& operator<<(std::ostream& os, const CameraType& ct);
    friend std::istream& operator>>(std::istream& is, CameraType& ct);
private:
    Value _type;
};

#endif/*FACE_RECOG_CAMERA_TYPE*/
