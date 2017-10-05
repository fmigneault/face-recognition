#ifndef FACE_RECOG_CONFIG_H
#define FACE_RECOG_CONFIG_H

// Common
#include "CommonCpp.h"

// FaceRecog Configs Files
#include "Configs/ConfigFile.h"
#include "Configs/ConsoleOptions.h"
#include "Configs/Platform.h"
using namespace config;

// Eigen
#include <Eigen/Core>

// OpenCV
#include <opencv2/opencv.hpp>
#include <opencv2/plot.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include "opencv2/objdetect/objdetect.hpp"
#include <opencv2/videoio/videoio.hpp>
#if CV_VERSION_MAJOR == 3
    #include "opencv2/core/ocl.hpp"
    #include "opencv2/core/cuda.hpp"
#endif
using namespace cv;

// FaceRecog Utilities
#include "Utilities/MatDefines.h"
#include "Utilities/MultiColorType.h"
#include "Utilities/utilities.h"

// Python
#ifdef FACE_RECOG_HAS_PYTHON
    #include <Python.h>
    #include "Python/PythonInterop.h"
    #include "Python/PyCvBoostConverter.h"
    #define PY_INITIALIZE() Py_Initialize()
    #define PY_FINALIZE()   Py_Finalize()
#else/*!FACE_RECOG_HAS_PYTHON*/
    #define PY_INITIALIZE()
    #define PY_FINALIZE()
#endif/*FACE_RECOG_HAS_PYTHON*/

// Boost
#include <boost/filesystem.hpp>
#include <boost/range/iterator_range.hpp>
#include <boost/circular_buffer.hpp>
#ifdef FACE_RECOG_HAS_PYTHON
#include <boost/python.hpp>
namespace bp = boost::python;
#endif/*FACE_RECOG_HAS_PYTHON*/
namespace bfs = boost::filesystem;

// mvector
#include "mvector.hpp"
using namespace xstd;

// STD and other common libraries
#include <stdio.h>
#include <iostream>
#include <iomanip>
#include <string>
#include <memory>
#include <omp.h>
#include <fstream>
#include <iostream>
#include <sys/types.h>
#include <sys/stat.h>
#include <string>
#include <vector>
#if defined(FACE_RECOG_LINUX)
    #include <unistd.h>
#elif defined(FACE_RECOG_WINDOWS)
    #include <direct.h>
#endif/*FACE_RECOG OS-SPECIFIC FILES*/
using namespace std;

// FaceRecog Camera
#include "Camera/CameraDefines.h"

// FaceRecog Face Detectors/Trackers
#include "FaceDetectors/IFaceDetector.h"
#include "FaceDetectors/EyeDetector.h"
#ifdef FACE_RECOG_HAS_FRCNN
    #ifndef FACE_RECOG_HAS_PYTHON
    #error Python is required to employ FRCNN
    #endif/*FACE_RECOG_HAS_PYTHON*/
    #include "FaceDetectors/FaceDetectorFRCNN.h"
#endif/*FACE_RECOG_HAS_FRCNN*/
#ifdef FACE_RECOG_HAS_SSD
#include "FaceDetectors/FaceDetectorSSD.h"
#endif/*FACE_RECOG_HAS_SSD*/
#ifdef FACE_RECOG_HAS_YOLO
    #include "FaceDetectors/FaceDetectorYolo.h"
#endif/*FACE_RECOG_HAS_YOLO*/
#ifdef FACE_RECOG_HAS_KCF
#include "Trackers/FaceDetectorVJ.h"
#endif/*FACE_RECOG_HAS_KCF*/
#ifdef FACE_RECOG_HAS_KCF
#include "Trackers/FaceDetectorVJ.h"
#endif/*FACE_RECOG_HAS_KCF*/
#ifdef FACE_RECOG_HAS_KCF
#include "Trackers/FaceDetectorVJ.h"
#endif/*FACE_RECOG_HAS_KCF*/

// FaceRecog Tracking Containers
#include "Tracks/Association.h"
#include "Tracks/CircularBuffer.h"
#include "Tracks/Hungarian.h"
#include "Tracks/Track.h"

// FaceRecog Classifiers
#include "Classifiers/TemplateMatcher.h"
#include "Classifiers/IClassifier.h"
#include "Classifiers/ClassifierEnsembleTM.h"
#ifdef FACE_RECOG_HAS_ESVM
    #include "Classifiers/ClassifierEnsembleESVM.h"
#endif/*FACE_RECOG_HAS_ESVM*/
#ifdef FACE_RECOG_HAS_FACE_NET
    #ifndef FACE_RECOG_HAS_PYTHON
    #error Python is required to employ FaceNet
    #endif/*FACE_RECOG_HAS_PYTHON*/
    #include "Classifiers/ClassifierFaceNet.h"
#endif/*FACE_RECOG_HAS_FACE_NET*/

// Macros
#define INITIALIZE()                            PY_INITIALIZE()
#define FINALIZE(out)                           PY_FINALIZE(); return out
#define ASSERT_LOG_FINALIZE(test,msg,log,out)   if (!(test)) { log << msg; FINALIZE(out); }

#if defined(NDEBUG) || (!defined(NDEBUG) && !defined(DEBUG) && !defined(_DEBUG))
    #define FACE_RECOG_DEBUG(x)
#else
    #define FACE_RECOG_DEBUG(x) x
#endif

// Additional defines
#define TRACK_RESERVE_SIZE 50   // value employed to reserve vector of tracks (only to reduce required reallocation on expanding size)

#endif/*FACE_RECOG_CONFIG_H*/
