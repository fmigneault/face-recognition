#include "Camera/CameraType.h"

#include <string>

CameraType::CameraType(int value)
{
    if      (value == (int)CameraType::FILE_STREAM)         _type = CameraType::FILE_STREAM;
    else if (value == (int)CameraType::CV_VIDEO_CAPTURE)    _type = CameraType::CV_VIDEO_CAPTURE;
    else if (value == (int)CameraType::PGR_FLYCAPTURE2)     _type = CameraType::PGR_FLYCAPTURE2;
    else                                                    _type = CameraType::UNDEFINED;
}

const std::string CameraType::name() const
{    
    switch (_type) {                   
        case CameraType::FILE_STREAM:       return "file stream";
        case CameraType::CV_VIDEO_CAPTURE:  return "OpenCV VideoCapture";
        case CameraType::PGR_FLYCAPTURE2:   return "PGR FlyCapture2";
        default:                            return "undefined";
    }
}

std::ostream& operator<< (std::ostream& os, const CameraType& ct) 
{ 
    os << ct.name();
    return os; 
}

std::istream& operator>> (std::istream& is, CameraType& ct)
{
    int v;
    is >> v;
    ct = CameraType(v);
    return is;
}
