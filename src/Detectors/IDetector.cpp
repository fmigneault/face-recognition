#include "Detectors/IDetector.h"
#include "FaceRecog.h"

int IDetector::loadDetector(string name)
{
    return -1;
}

void IDetector::assignImage(const FACE_RECOG_MAT& frame)
{
    frames.push_back(frame);
}

std::string IDetector::getModelPath(size_t modelIndex)
{
    return modelPaths[modelIndex];
}

size_t IDetector::modelCount()
{
    return modelPaths.size();
}

void IDetector::flipDetections(size_t index, vector<vector<Rect> >& bboxes)
{
    for (size_t i = 0; i < bboxes[index].size(); ++i)
    {
        Rect bbox = bboxes[index][i];
        bboxes[index][i].x = frames[index].cols - bbox.x - bbox.width;
    }
}

vector<Rect> IDetector::mergeDetections(vector<vector<Rect> >& bboxes)
{
    bool frontalOnly = bboxes.size() == 1;
    return util::mergeDetections(bboxes, overlapThreshold, frontalOnly);
}

std::shared_ptr<IDetector> buildSpecializedDetector(const ConfigFile& config, const std::string modelBasePath, DetectorType detectorType)
{
    ASSERT_LOG(bfs::is_directory(modelBasePath), "Model base path not found [" + modelBasePath + "]");

    std::shared_ptr<IDetector> detector = nullptr;
    int error = 0;

    // global face detector settings
    if (detectorType == DetectorType::FACE_DETECTOR_GLOBAL) {
        if (config.requireAnyCascade()) {
            FaceDetectorVJ vj(config.face.scaleFactor, config.face.nmsThreshold, config.face.minSize, config.face.maxSize,
                              config.face.confidenceSize, config.face.minNeighbours, config.face.overlapThreshold);
            if (config.HaarCascadeFrontal) {
                std::string model = (modelBasePath / bfs::path("haarcascades/haarcascade_frontalface_alt.xml")).string();
                error = vj.loadDetector(modelBasePath, NONE);
                ASSERT_LOG(!error, "Failed loading frontal global face detector file: '" + model + "'");
            }
            if (config.HaarCascadeProfile) {
                std::string model = (modelBasePath / bfs::path("haarcascades/haarcascade_profileface.xml")).string();
                error = vj.loadDetector(modelBasePath, NONE);
                ASSERT_LOG(!error, "Failed loading left profile global face detector file: '" + model + "'");
                error = vj.loadDetector(modelBasePath, HORIZONTAL);
                ASSERT_LOG(!error, "Failed loading right profile global face detector file: '" + model + "'");
            }
            if (config.LBPCascadeFrontal) {
                std::string model = (modelBasePath / bfs::path("lbpcascades/lbpcascade_frontalface.xml")).string();
                error = vj.loadDetector(modelBasePath, NONE);
                ASSERT_LOG(!error, "Failed loading frontal global face detector file: '" + model + "'");
            }
            if (config.LBPCascadeProfile) {
                std::string model = (modelBasePath / bfs::path("lbpcascades/lbpcascade_profileface.xml")).string();
                error = vj.loadDetector(modelBasePath, NONE);
                ASSERT_LOG(!error, "Failed loading left profile global face detector file: '" + model + "'");
                error = vj.loadDetector(modelBasePath, HORIZONTAL);
                ASSERT_LOG(!error, "Failed loading right profile global face detector file: '" + model + "'");
            }
            if (config.LBPCascadeFrontalImproved) {
                std::string model = (modelBasePath / bfs::path("lbpcascades/lbpcascade_frontalface_improved.xml")).string();
                error = vj.loadDetector(modelBasePath, NONE);
                ASSERT_LOG(!error, "Failed loading frontal face detector file: '" + model + "'");
            }
            detector = std::make_shared<FaceDetectorVJ>(vj);
        }
        else if (config.FRCNN) {
            #ifdef FACE_RECOG_HAS_FRCNN
            detector = std::make_shared<FaceDetectorFRCNN>();
            #endif/*FACE_RECOG_HAS_FRCNN*/
        }
        else if (config.SSD) {
            #ifdef FACE_RECOG_HAS_SSD
            detector = std::make_shared<FaceDetectorSSD>();
            #endif/*FACE_RECOG_HAS_SSD*/
        }
        else if (config.YOLO) {
            #ifdef FACE_RECOG_HAS_YOLO
            std::string modelPathProto = (modelBasePath / bfs::path("yolo-face-deploy.prototxt")).string();
            std::string modelPathCaffe = (modelBasePath / bfs::path("yolo-face.caffemodel")).string();
            detector = std::make_shared<FaceDetectorYOLO>(modelPathProto, modelPathCaffe);
            #endif/*FACE_RECOG_HAS_YOLO*/
        }
        ASSERT_LOG(detector != nullptr, "Failed to initialize global face detector from specified 'config'");
        ASSERT_LOG(detector->modelCount() > 0, "Invalid count of global face detector models");
    }
    // local face detector settings
    else if (detectorType == DetectorType::FACE_DETECTOR_LOCAL && config.useLocalSearchROI) {
        FaceDetectorVJ vj(config.face.scaleFactor, config.face.nmsThreshold, config.face.minSize, config.face.maxSize,
                          config.face.confidenceSize, config.face.minNeighbours, config.face.overlapThreshold);
        // frontal model
        std::string model = (modelBasePath / bfs::path("haarcascades/haarcascade_frontalface_alt.xml")).string();
        error = vj.loadDetector(modelBasePath, NONE);
        ASSERT_LOG(!error, "Failed loading frontal local face detector file: '" + model + "'");
        // profile models
        if (config.use3CascadesLocalSearch) {
            std::string model = (modelBasePath / bfs::path("haarcascades/haarcascade_profileface.xml")).string();
            error = vj.loadDetector(modelBasePath, NONE);
            ASSERT_LOG(!error, "Failed loading left profile local face detector file: '" + model + "'");
            error = vj.loadDetector(modelBasePath, HORIZONTAL);
            ASSERT_LOG(!error, "Failed loading right profile local face detector file: '" + model + "'");
        }
        detector = std::make_shared<FaceDetectorVJ>(vj);
        ASSERT_LOG(detector != nullptr, "Failed to initialize local face detector from specified 'config'");
        ASSERT_LOG(detector->modelCount() > 0, "Invalid count of local face detector models");
    }
    // eye detector settings
    else if (detectorType == DetectorType::EYE_DETECTOR && config.useEyesDetection) {
        EyeDetector eyes(config.eyes.scaleFactor, config.eyes.nmsThreshold, config.eyes.minSize, config.eyes.maxSize);
        // left/right eyes
        std::string modelLeft  = (modelBasePath / bfs::path("haarcascades/haarcascade_lefteye_2splits.xml")).string();
        std::string modelRight = (modelBasePath / bfs::path("haarcascades/haarcascade_righteye_2splits.xml")).string();
        eyes.loadDetector(modelLeft);
        eyes.loadDetector(modelRight);
        eyes.leftEyeIndex  = 0;
        eyes.rightEyeIndex = 1;
        ASSERT_LOG(detector != nullptr, "Failed to initialize eye detector from specified 'config'");
        ASSERT_LOG(detector->modelCount() == 2, "Invalid count of eye detector models");
    }

    return detector;
}
