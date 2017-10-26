#ifndef FACE_RECOG_FACE_DETECTOR_YOLO_H
#define FACE_RECOG_FACE_DETECTOR_YOLO_H
#ifdef  FACE_RECOG_HAS_YOLO

#include "Utilities/Common.h"
#include "Utilities/MatDefines.h"
#include "Detectors/IDetector.h"
#include "Tracks/Track.h"

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
    double evaluateConfidence(const Track& track, const FACE_RECOG_MAT& image) override;
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

#endif/*FACE_RECOG_HAS_YOLO*/
#endif/*FACE_RECOG_FACE_DETECTOR_YOLO_H*/
