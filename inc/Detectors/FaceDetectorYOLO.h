﻿#ifndef FACE_RECOG_FACE_DETECTOR_YOLO_H
#define FACE_RECOG_FACE_DETECTOR_YOLO_H

#ifdef _MSC_VER
#define C_MSVC
#endif/*_MSC_VER*/

// FaceRecog
#include "FaceRecog.h"

// OpenCV
#include <opencv2/core/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/opencv.hpp>

// STD
#include <chrono>
#include <fstream>
#include <limits>
#include <malloc.h>
#include <stdio.h>
#include <string>
#include <vector>

// Boost
#include "boost/algorithm/string.hpp"
#include <boost/math/special_functions/next.hpp>
#include <boost/progress.hpp>
#include <boost/random.hpp>

// Google
#include "google/protobuf/text_format.h"

// Caffe
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
using std::string;
using namespace std;
namespace db = caffe::db;

class FaceDetectorYOLO final : public IDetector
{
public:
    /*
    Example model paths:
        modelPathProto = "../yolo-face-deploy.prototxt";
        modelPathCaffe = "../yolo-face.caffemodel";
    */
    FaceDetectorYOLO(std::string modelPathProto, std::string modelPathCaffe);
    ~FaceDetectorYOLO() {}
    // specialized overrides
    int detect(std::vector<std::vector<cv::Rect>>& faces) override;
    double evaluateConfidence(const Target& target, const FACE_RECOG_MAT& image) override;
    std::vector<cv::Rect> mergeDetections(std::vector<std::vector<cv::Rect>>& faces) override { return m_faces; }
    void assignImage(const FACE_RECOG_MAT& frame) override;
    // unused overrides
    int loadDetector(std::string name) override { return 1; }
    void flipDetections(size_t index, std::vector<std::vector<cv::Rect>>& faces) override { return; }
private:
    void loadData(cv::Mat& image);
    void getBox(std::vector<float> result, float* pro_obj, int* idx_class, std::vector<std::vector<int> >& bboxs, float thresh, cv::Mat image);
    std::string folderPath = "../python";
    std::string filePath = "../python";
    std::string pyFunc = "face_detect";
    std::vector<cv::Rect> m_faces;
    boost::shared_ptr<Net<float> > net;
};

#endif/*FACE_RECOG_FACE_DETECTOR_YOLO_H*/