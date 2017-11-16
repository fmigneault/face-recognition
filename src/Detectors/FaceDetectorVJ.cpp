/*
    Original Code Reference

    FAST-DT: FAce STructured Detection and Tracking
    Francesco Comaschi (f.comaschi@tue.nl)

    Code to accompany the paper:

        Online Multi-Face Detection and Tracking using Detector Confidence and Structured SVMs
        F. Comaschi, S. Stuijk, T. Basten, H. Corporaal
        Advanced Video and Signal-Based Surveillance (AVSS), 2015

    * Multiple updates and modifications applied to introduce improvements or combined use with projet
*/

#ifdef FACE_RECOG_HAS_VJ

#include "Detectors/FaceDetectorVJ.h"
#include "FaceRecog.h"

FaceDetectorVJ::FaceDetectorVJ()
{
    setDefaults();
}

FaceDetectorVJ::FaceDetectorVJ(double scaleFactor, int nmsThreshold, cv::Size minSize, cv::Size maxSize,
                               cv::Size evalSize, int minNeighbours, double overlapThreshold)
{
    setDefaults();
    initializeParameters(scaleFactor, nmsThreshold, minSize, maxSize,
                         evalSize, minNeighbours, overlapThreshold);
}

void FaceDetectorVJ::setDefaults()
{
    initializeParameters(1.2, 4, Size(20, 20), Size(60, 60), Size(40, 40), 0, 0.1);
}

void FaceDetectorVJ::initializeParameters(double scaleFactor, int nmsThreshold, cv::Size minSize,
                                          cv::Size maxSize, cv::Size evalSize, int minNeighbours, double overlapThreshold)
{
    if (scaleFactor > 1)        this->scaleFactor = scaleFactor;
    if (nmsThreshold >= 0)      this->nmsThreshold = nmsThreshold;
    if (minSize != Size())      this->minSize = minSize;
    if (maxSize != Size())      this->maxSize = maxSize;
    if (evalSize != Size())     this->evalSize = evalSize;
    if (minNeighbours >= 0)     this->minNeighbours = minNeighbours;
    if (overlapThreshold >= 0)  this->overlapThreshold = overlapThreshold;
}

bool FaceDetectorVJ::loadDetector(string modelPath, FlipMode faceFlipMode)
{
    #if FACE_RECOG_USE_CUDA
    Ptr<cv::cuda::CascadeClassifier> cascade = cv::cuda::CascadeClassifier::create(modelPath);
    bool success_load_cascade = !cascade.empty();
    #else
    cv::CascadeClassifier cascade;
    bool success_load_cascade = cascade.load(modelPath);
    #endif

    if (success_load_cascade) {
        #if FACE_RECOG_USE_CUDA
        cascade->setScaleFactor(scaleFactor);
        cascade->setMinObjectSize(minSize);
        cascade->setMaxObjectSize(maxSize);
        cascade->setMinNeighbors(minNeighbours);
        #endif

        faceFinder.push_back(cascade);
        stageCount.push_back(1);
        modelPaths.push_back(modelPath);
        faceFlipModes.push_back(faceFlipMode);
    }
    return success_load_cascade;
}

void FaceDetectorVJ::assignImage(const FACE_RECOG_MAT& frame)
{
    cleanImages();
    size_t nClassifiers = faceFinder.size();
    for (size_t i = 0; i < nClassifiers; ++i) {
        FlipMode fm = (i < faceFlipModes.size()) ? faceFlipModes[i] : NONE;
        frames.push_back(imFlip(frame, fm));
    }
}

bool FaceDetectorVJ::detect(vector<vector<Rect>>& bboxes)
{
    size_t nClassifiers = faceFinder.size();
    size_t nImages = frames.size();
    ASSERT_LOG(nImages == nClassifiers, "Different number of images and classifiers in `FaceRecogVJ::detect`");

    if (bboxes.size() != nClassifiers)
        bboxes = std::vector<std::vector<Rect>>(nClassifiers);

    #pragma omp parallel for
    for (omp_size_t c = 0; c < nClassifiers; ++c)
    {
        #if FACE_RECOG_USE_CUDA
        auto cascade_gpu = faceFinder[c];
        cascade_gpu->detectMultiScale(frames[c], foundObjects_gpu);
        cascade_gpu->convert(foundObjects_gpu, bboxes[c]);
        #else
        faceFinder[c].detectMultiScale(frames[c], bboxes[c], scaleFactor, nmsThreshold, CASCADE_SCALE_IMAGE, minSize, maxSize);
        #endif
    }
    return true;
}

void FaceDetectorVJ::flipDetections(size_t index, vector<vector<Rect>>& bboxes)
{
    for (size_t i = 0; i < bboxes[index].size(); ++i)
    {
        Rect bbox = bboxes[index][i];
        bboxes[index][i].x = frames[index].cols - bbox.x - bbox.width;
    }
}

vector<Rect> FaceDetectorVJ::mergeDetections(vector<vector<Rect>>& bboxes)
{
    size_t nBBox = bboxes.size();
    bool frontalOnly = nBBox == 1;
    if (!frontalOnly)
        for (size_t b = 0; b < nBBox; ++b)
            if (faceFlipModes[b] == HORIZONTAL)
                flipDetections(b, bboxes);
    return util::mergeDetections(bboxes, overlapThreshold, frontalOnly);
}

double FaceDetectorVJ::evaluateConfidence(const Track& track, const FACE_RECOG_MAT& image)
{
    /*EVALUATE CONFIDENCE FOR UNMATCHED TARGETS*/
    size_t nFaces = faceFinder.size();
    ASSERT_LOG(nFaces == stageCount.size(), "Different number of classifiers and stage count in `FaceDetectorVJ::evaluateConfidence`");

    vector<double> confidences(nFaces);
    vector<Rect> trackersROI;

    // get current bbox for confidence evaluation, with resize to specified config size
    Rect face = track.bbox();
    std::vector<FACE_RECOG_MAT> croppedFaces(nFaces);

    double score = 0;
    #pragma omp parallel for
    for (omp_size_t f = 0; f < nFaces; ++f) {
        croppedFaces[f] = imResize(imFlip(image(face).clone(), faceFlipModes[f]), evalSize);
        double tempWeights = 0;
        confidences[f] = faceFinder[f].classify(croppedFaces[f], tempWeights);
    }
    double maxConfidence = *std::max_element(confidences.begin(), confidences.end());
    return maxConfidence;

    ////////////////////////////////////////// OLD CODE (GPU vs. CPU) ==> gpu no 'classify' available
    //#if FACE_RECOG_USE_CUDA
    //auto cascade_gpu = faceFinder[i];
    //cascade_gpu->detectMultiScale(croppedTarget, foundObjects_gpu);
    //cascade_gpu->convert(foundObjects_gpu, trackersROI);
    //#else
    //faceFinder[i].detectMultiScale(croppedTarget, trackersROI, scaleFactor, minNeighbours, CV_HAAR_SCALE_IMAGE);
    //#endif
}

#endif/*FACE_RECOG_HAS_VJ*/
