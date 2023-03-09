#include "FaceRecogConfig.h"
#ifdef FACE_RECOG_HAS_FRCNN


FaceDetectorFRCNN::FaceDetectorFRCNN()
{
    //folderPath = ".";
    folderPath = "/home/livia/code/facerecog/python/py-faster-rcnn/tools/";
    //filePath = "getImg.py";
    //filePath = "face_detect_cpp";
    filePath = "/home/livia/code/facerecog/python/py-faster-rcnn/tools/face_detect_cpp.py";
    pyFunc = "face_detect";
}

void FaceDetectorFRCNN::pyObjList2VecRect(PyObject* list)
{
    auto size = PyList_Size(list);
    std::vector<float> aRect(4);
    for (auto i = 0; i < size; ++i)
    {
        aRect[(i + 1) % 4] = PyFloat_AsDouble(PyList_GetItem(list, i));
        if ((i + 1) % 4 == 0)
            m_faces.push_back(cv::Rect(cv::Point(aRect[0], aRect[1]), cv::Point(aRect[2], aRect[3])));
    }
    if (m_faces.size() != 0)
    {
        for (const auto& e : m_faces)
            std::cout << e << std::endl;
        std::cout << m_faces.size() << std::endl;
    }
}

int FaceDetectorFRCNN::findFaces(std::vector<std::vector<cv::Rect> >& faces)
{
   //setenv("PYTHONPATH", folderPath.c_str(), 1);
   auto lastFrame = frames.back();
   PyObject* ndata = NULL;
   ndata = pbcvt::fromMatToNDArray(lastFrame);
   
   bp::handle<> handle(ndata);
   bp::object ndmat = bp::object(handle);
   std::vector<bp::object> args;
   args.emplace_back(ndmat);
   bp::object res = callPyFuncFromFile(pyFunc, filePath, args);
   if (res.is_none())
       return 0;
   pyObjList2VecRect(res.ptr());

   /*
   std::cout << "1" << std::endl;
   PyObject* res = callPyUsingModule(pyFunc, filePath, ndata);
   Py_CLEAR(ndata);*/
   return 1;
}

void FaceDetectorFRCNN::assignImage(FACE_RECOG_MAT frame)
{
    frames.emplace_back(frame);
}

#endif/*FACE_RECOG_HAS_FRCNN*/