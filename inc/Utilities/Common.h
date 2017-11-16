#ifndef FACE_RECOG_COMMON_H
#define FACE_RECOG_COMMON_H

/* Headers of common library dependencies external of FaceRecog */

// Define platform to remove ambiguous `ACCESS_MASK` between Windows and OpenCV
// Windows header must be included before OpenCV's headers
#include "Configs/Platform.h"

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
// Since 3.3.1 - 'cv::plot::Plot2d::create' instead of 'cv::plot::createPlot2d'
#if CV_VERSION_MAJOR >= 3 && CV_VERSION_MINOR >= 3 && CV_VERSION_REVISION > 0
    #define CV_NEW_PLOT2D_CREATE
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
#include <boost/algorithm/string.hpp>
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

// Google
#ifdef FACE_RECOG_HAS_PROTOBUF
#include "google/protobuf/text_format.h"
#endif/*FACE_RECOG_HAS_PROTOBUF*/

// Caffe
#ifdef FACE_RECOG_HAS_CAFFE
#include "caffe/blob.hpp"
#include "caffe/common.hpp"
#include "caffe/common.hpp"
#include "caffe/layer.hpp"
#include "caffe/net.hpp"
#include "caffe/proto/caffe.pb.h"
#include "caffe/util/db.hpp"
#include "caffe/util/format.hpp"
#include "caffe/util/io.hpp"
#include "caffe/util/math_functions.hpp"
#include "caffe/util/rng.hpp"
using caffe::Blob;
using caffe::Caffe;
using caffe::Datum;
using caffe::Net;
using caffe::Layer;
namespace db = caffe::db;
#endif/*FACE_RECOG_HAS_CAFFE*/

#endif/*FACE_RECOG_COMMON_H*/
