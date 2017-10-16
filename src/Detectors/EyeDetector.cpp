#include "Detectors/EyeDetector.h"
#include "FaceRecog.h"

EyeDetector::EyeDetector()
{
    initializeParameters(1.5, 4, cv::Size(5, 5), cv::Size(50, 50));
}

EyeDetector::EyeDetector(double scale, int nms, cv::Size minSize, cv::Size maxSize)
{
    initializeParameters(scale, nms, minSize, maxSize);
}

void EyeDetector::initializeParameters(double scaleFactor, int nmsThreshold, cv::Size minSize, cv::Size maxSize)
{
    _scaleFactor = scaleFactor;
    _nmsThreshold = nmsThreshold;
    _minSize = minSize;
    _maxSize = maxSize;
}

int EyeDetector::loadDetector(std::string name)
{
    bool success_load_cascade = false;
    #if FACE_RECOG_USE_CUDA
    Ptr<cv::cuda::CascadeClassifier> _cascade = cv::cuda::CascadeClassifier::create(name);
    success_load_cascade = !_cascade.empty();
    #else
    FACE_RECOG_NAMESPACE::CascadeClassifier _cascade;
    success_load_cascade = _cascade.load(name);
    #endif

    if (!success_load_cascade)
    {
        std::cerr << "ERROR: Could not load classifier cascade" << name << std::endl;
        return -1;
    }
    else
        std::cout << "Cascade " << name << " successfully loaded" << std::endl;

    #if FACE_RECOG_USE_CUDA
    cascade->setScaleFactor(_scaleFactor);
    cascade->setMinObjectSize(_minSize);
    cascade->setMaxObjectSize(_maxSize);
    cascade->setMinNeighbors(_nmsThreshold);
    #endif

    _eyeCascades.push_back(_cascade);
    return 0;
}

void EyeDetector::assignImage(const FACE_RECOG_MAT& frame)
{
    _searchImage = frame;
}

double EyeDetector::evaluateConfidence(const Track& track, const FACE_RECOG_MAT& image)
{
    return (track.getROI().getSubRect().area()) ? 1.0 : 0.0;
}

void EyeDetector::flipDetections(size_t index, vector<vector<Rect> >& faces) {}

// Return detections from the specified ROI
int EyeDetector::detect(vector<vector<Rect> >& bboxes)
{
    #pragma omp parallel for
    for (long c = 0; c < _eyeCascades.size(); ++c)
    {
        std::vector<cv::Rect> _foundEyes;

        #if FACE_RECOG_USE_CUDA
        auto _cascade = _eyeCascades[c];
        _cascade->detectMultiScale(frames[c], foundObjects_gpu);
        _cascade->convert(foundObjects_gpu, bboxes[c]);
        #else
        _eyeCascades[c].detectMultiScale(_searchImage, _foundEyes, _scaleFactor, _nmsThreshold, CV_HAAR_SCALE_IMAGE, _minSize, _maxSize);
        #endif

        bboxes[c] = getFoundEyes(_foundEyes);
    }
};

// Return the absolute position of the ROIs of found eyes within the image specified using found relative positions
std::vector<cv::Rect> EyeDetector::getFoundEyes(std::vector<cv::Rect> relRect)
{
    std::vector<cv::Rect> absRect(relRect);
    for (size_t i = 0; i < absRect.size(); i++)
    {
        cv::Size whole;
        cv::Point offset;
        _searchImage.locateROI(whole, offset);
        absRect[i] += offset;
    }
    return absRect;
}
