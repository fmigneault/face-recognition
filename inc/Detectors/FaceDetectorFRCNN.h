#ifndef FACE_RECOG_FACE_DETECTOR_FRCNN_H
#define FACE_RECOG_FACE_DETECTOR_FRCNN_H
#ifdef  FACE_RECOG_HAS_FRCNN

#include "Utilities/Common.h"
#include "Utilities/MatDefines.h"
#include "Python/PyCvBoostConverter.h"
#include "Python/PythonInterop.h"
#include "Detectors/IDetector.h"
#include "Tracks/Track.h"

class FaceDetectorFRCNN final : public IDetector
{
public:
    FaceDetectorFRCNN(std::string basePath);
    ~FaceDetectorFRCNN() {}
    // specific to specialized class
    void pyObjList2VecRect(PyObject* list);
    // specialized overrides
    int detect(std::vector<std::vector<cv::Rect> >& bboxes) override;
    std::vector<cv::Rect> mergeDetections(std::vector<std::vector<cv::Rect> >& faces) override { return m_faces; }
    void assignImage(const FACE_RECOG_MAT& frame) override;
    // unused overrides
    int loadDetector(std::string name) override { return 1; }
    void flipDetections(size_t index, std::vector<std::vector<cv::Rect> >& faces) override { return; }
    double evaluateConfidence(const Track& track, const FACE_RECOG_MAT& image)  override { return 1; }
private:
    std::string folderPath = "../python";
    std::string filePath = "../python";
    std::string pyFunc = "face_detect";
    std::vector<cv::Rect> m_faces;
};

#endif/*FACE_RECOG_HAS_FRCNN*/
#endif/*FACE_RECOG_FACE_DETECTOR_FRCNN_H*/
