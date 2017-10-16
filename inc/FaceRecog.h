#ifndef FACE_RECOG_CONFIG_H
#define FACE_RECOG_CONFIG_H

// FaceRecog Predefinition, Macros and Setups
// include forward declarations first to resolve dependencies
#include "Configs/ForwardDeclares.h"
#include "Configs/Platform.h"
#include "Configs/ConsoleOptions.h"

// Common
#include "CommonCpp.h"

// Eigen
#include <Eigen/Core>
using namespace Eigen;

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
#include <cassert>
#include <deque>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <iterator>
#include <memory>
#include <omp.h>
#include <stdexcept>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <sys/stat.h>
#include <sys/types.h>
#include <vector>
#if defined(FACE_RECOG_LINUX)
    #include <unistd.h>
#elif defined(FACE_RECOG_WINDOWS)
    #include <direct.h>
#endif/*FACE_RECOG OS-SPECIFIC FILES*/
using namespace std;

// FaceRecog Configs
#include "Configs/ConfigFile.h"

// FaceRecog Utilities
#include "Utilities/MatDefines.h"
#include "Utilities/MultiColorType.h"
#include "Utilities/utilities.h"

// FaceRecog Camera
#include "Camera/CameraDefines.h"
#include "Camera/CameraType.h"

// FaceRecog Tracking Containers
#include "Tracks/Association.h"
#include "Tracks/CircularBuffer.h"
#include "Tracks/HaarFeature.h"
#include "Tracks/HaarFeatures.h"
#include "Tracks/Hungarian.h"
#include "Tracks/ImageRep.h"
#include "Tracks/Kernels.h"
#include "Tracks/LaRank.h"
#include "Tracks/Rect.h"
#include "Tracks/Sample.h"
#include "Tracks/Sampler.h"
#include "Tracks/Track.h"
#include "Tracks/TrackROI.h"

// FaceRecog Face Detectors
#include "Detectors/DetectorType.h"
#include "Detectors/IDetector.h"
#ifdef FACE_RECOG_HAS_VJ
#include "Detectors/EyeDetector.h"
#include "Detectors/FaceDetectorVJ.h"
#endif/*FACE_RECOG_HAS_VJ*/
#ifdef FACE_RECOG_HAS_FRCNN
    #ifndef FACE_RECOG_HAS_PYTHON
    #error Python is required to employ FRCNN
    #endif/*FACE_RECOG_HAS_PYTHON*/
    #include "Detectors/FaceDetectorFRCNN.h"
#endif/*FACE_RECOG_HAS_FRCNN*/
#ifdef FACE_RECOG_HAS_SSD
#include "Detectors/FaceDetectorSSD.h"
#endif/*FACE_RECOG_HAS_SSD*/
#ifdef FACE_RECOG_HAS_YOLO
#include "Detectors/FaceDetectorYOLO.h"
#endif/*FACE_RECOG_HAS_YOLO*/

// FaceRecog Trackers
#include "Trackers/ITracker.h"
#ifdef FACE_RECOG_HAS_CAMSHIFT
#include "Trackers/TrackerCamshift.h"
#endif/*FACE_RECOG_HAS_CAMSHIFT*/
#ifdef FACE_RECOG_HAS_COMPRESSIVE
#include "Trackers/TrackerCompressive.h"
#endif/*FACE_RECOG_HAS_COMPRESSIVE*/
#ifdef FACE_RECOG_HAS_KCF
#include "Trackers/TrackerKCF.h"
#endif/*FACE_RECOG_HAS_KCF*/
#ifdef FACE_RECOG_HAS_STRUCK
#include "Trackers/TrackerSTRUCK.h"
#endif/*FACE_RECOG_HAS_STRUCK*/

// FaceRecog Classifiers
#include "Classifiers/ClassifierType.h"
#include "Classifiers/IClassifier.h"
#ifdef FACE_RECOG_HAS_ESVM
#include "Classifiers/ClassifierEnsembleESVM.h"
#endif/*FACE_RECOG_HAS_ESVM*/
#ifdef FACE_RECOG_HAS_FACE_NET
    #ifndef FACE_RECOG_HAS_PYTHON
    #error Python is required to employ FaceNet
    #endif/*FACE_RECOG_HAS_PYTHON*/
    #include "Classifiers/ClassifierFaceNet.h"
#endif/*FACE_RECOG_HAS_FACE_NET*/
#if FACE_RECOG_HAS_TM
#include "Classifiers/ClassifierEnsembleTM.h"
#endif/*FACE_RECOG_HAS_TM*/

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
