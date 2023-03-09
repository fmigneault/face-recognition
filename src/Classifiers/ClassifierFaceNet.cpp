#include "FaceRecogConfig.h"
#ifdef FACE_RECOG_USE_FACE_NET


ClassifierFaceNet::ClassifierFaceNet()
{
    std::string rootFaceNet = "c:/libs/facenet/";
    folderPath = rootFaceNet + "src";               //"/home/livia/code/facerecog/python/facenet/src";
    filePath = rootFaceNet + "src/compare_cpp.py";  //"/home/livia/code/facerecog/python/facenet/src/compare_cpp.py";
    pyFuncInit = "compare_init";
    pyFuncPredict = "main";
}

ClassifierFaceNet::~ClassifierFaceNet() { }

std::vector<double> ClassifierFaceNet::predict(cv::Mat roi)
{
    std::vector<bp::object> args;
    bp::object res = callPyFuncFromFile(pyFuncInit, filePath, args);

    THROW("FUNCTION INCOMPLETE - PARTIAL DONE");

    return std::vector<double> {1,2,3};
}

#endif/*FACE_RECOG_USE_FACE_NET*/