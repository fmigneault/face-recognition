#ifdef FACE_RECOG_HAS_FACE_NET

#include "Classifiers/ClassifierFaceNet.h"
#include "FaceRecog.h"

ClassifierFaceNet::ClassifierFaceNet()
{
    std::string rootFaceNet = "c:/libs/facenet/";
    folderPath = rootFaceNet + "src";               //"/home/livia/code/facerecog/python/facenet/src";
    filePath = rootFaceNet + "src/compare_cpp.py";  //"/home/livia/code/facerecog/python/facenet/src/compare_cpp.py";
    pyFuncInit = "compare_init";
    pyFuncPredict = "main";
}

ClassifierFaceNet::~ClassifierFaceNet() { }

std::vector<double> ClassifierFaceNet::predict(const FACE_RECOG_MAT& roi)
{
    std::vector<bp::object> args;
    bp::object res = callPyFuncFromFile(pyFuncInit, filePath, args);

    THROW("FUNCTION INCOMPLETE - PARTIAL DONE");

    return std::vector<double> {1, 2, 3};
}

#endif/*FACE_RECOG_HAS_FACE_NET*/
