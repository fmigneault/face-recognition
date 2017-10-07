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

#include "FaceRecog.h"


FaceDetectorVJ::FaceDetectorVJ()
{
	SetDefaults();
}

FaceDetectorVJ::FaceDetectorVJ(double scaleFactor, int nmsThreshold, cv::Size minSize, cv::Size maxSize,
                               cv::Size evalSize, int minNeighbours, double overlapThreshold)
{
    SetDefaults();
    initializeParameters(scaleFactor, nmsThreshold, minSize, maxSize,
                         evalSize, minNeighbours, overlapThreshold);
}

void FaceDetectorVJ::SetDefaults()
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

int FaceDetectorVJ::loadDetector(string name, FlipMode faceFlipMode)
{
    bool success_load_cascade = false;
    #if FACE_RECOG_USE_CUDA
	Ptr<cv::cuda::CascadeClassifier> cascade = cv::cuda::CascadeClassifier::create(name);
    success_load_cascade = !cascade.empty();
    #else
    FACE_RECOG_NAMESPACE::CascadeClassifier cascade;
    success_load_cascade = cascade.load(name);
    #endif

	if (!success_load_cascade)
	{
		cerr << "ERROR: Could not load classifier cascade " << name << endl;
		return -1;
	}

    #if FACE_RECOG_USE_CUDA
    cascade->setScaleFactor(scaleFactor);
    cascade->setMinObjectSize(minSize);
    cascade->setMaxObjectSize(maxSize);
    cascade->setMinNeighbors(minNeighbours);
    #endif

	faceFinder.push_back(cascade);
	stageCount.push_back(1);
    faceFlipModes.push_back(faceFlipMode);
	return 0;
}

void FaceDetectorVJ::assignImage(FACE_RECOG_MAT frame)
{
    size_t iFrame = frames.size();
    FlipMode fm = (iFrame < faceFlipModes.size()) ? faceFlipModes[iFrame] : NONE;
    frames.push_back(imFlip(frame, fm));
}

int FaceDetectorVJ::findFaces(vector<vector<Rect> >& faces)
{
	size_t nClassifiers = faceFinder.size();
    size_t nImages = frames.size();
	if (nImages != nClassifiers)
	{
		cerr << "ERROR in findFaces, different number of images and classifiers" << endl;
		return -1;
	}

    #pragma omp parallel for
	for (long c = 0; c < nClassifiers; ++c)
	{
        #if FACE_RECOG_USE_CUDA
        auto cascade_gpu = faceFinder[c];
        cascade_gpu->detectMultiScale(frames[c], foundObjects_gpu);
        cascade_gpu->convert(foundObjects_gpu, faces[c]);
        #else
		faceFinder[c].detectMultiScale(frames[c], faces[c], scaleFactor, nmsThreshold, CASCADE_SCALE_IMAGE, minSize, maxSize);
        #endif
	}
	return 0;
}

double FaceDetectorVJ::evaluateConfidence(Track& track, FACE_RECOG_MAT& image)
{
	/*EVALUATE CONFIDENCE FOR UNMATCHED TARGETS*/
    size_t nFaces = faceFinder.size();
	if (nFaces != stageCount.size()) {
		cerr << "ERROR in evaluateConfidence, different number of classifiers and stage count" << endl;
		return -1;
	}

	vector<double> confidences(nFaces);
	vector<Rect> trackersROI;

    // get current bbox for confidence evaluation, with resize to specified config size
	Rect face = track.bbox();
    std::vector<FACE_RECOG_MAT> croppedFaces(nFaces);

    double score = 0;
    #pragma omp parallel for
    for (long f = 0; f < nFaces; ++f) {
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

void FaceDetectorVJ::flipFaces(size_t index, vector<vector<Rect> >& faces)
{
    for (size_t i = 0; i < faces[index].size(); ++i)
    {
        Rect face_i = faces[index][i];
        faces[index][i].x = frames[index].cols - face_i.x - face_i.width;
    }
}

vector<Rect> FaceDetectorVJ::mergeDetections(vector<vector<Rect> >& faces)
{
    size_t nFaces = faces.size();
    bool frontalOnly = nFaces == 1;
    if (!frontalOnly)
        for (size_t f = 0; f < nFaces; ++f)
            if (faceFlipModes[f] == HORIZONTAL)
                flipFaces(f, faces);
    return util::mergeDetections(faces, overlapThreshold, frontalOnly);
}
