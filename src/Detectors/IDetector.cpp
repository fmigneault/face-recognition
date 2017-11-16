#include "Detectors/IDetector.h"
#include "FaceRecog.h"

bool IDetector::loadDetector(string name)
{
    return false;
}

void IDetector::assignImage(const FACE_RECOG_MAT& frame)
{
    frames.push_back(frame);
}

std::string IDetector::getModelPath(size_t modelIndex)
{
    return modelIndex < modelPaths.size() ? modelPaths[modelIndex] : "";
}

std::string IDetector::getModelName(size_t modelIndex)
{
    return modelIndex < modelPaths.size() ? bfs::path(getModelPath(modelIndex)).stem().string() : "";
}

size_t IDetector::modelCount()
{
    return modelPaths.size();
}

void IDetector::flipDetections(size_t index, vector<vector<Rect>>& bboxes)
{
    for (size_t i = 0; i < bboxes[index].size(); ++i)
    {
        Rect bbox = bboxes[index][i];
        bboxes[index][i].x = frames[index].cols - bbox.x - bbox.width;
    }
}

vector<Rect> IDetector::mergeDetections(vector<vector<Rect>>& bboxes)
{
    bool frontalOnly = bboxes.size() == 1;
    return util::mergeDetections(bboxes, overlapThreshold, frontalOnly);
}

int IDetector::detectMerge(std::vector<cv::Rect>& bboxes)
{
    vector<vector<Rect>> multiBBoxes;
    if (!detect(multiBBoxes))
        return -1;
    bboxes = mergeDetections(multiBBoxes);
    return (int)(bboxes.size());
}

std::shared_ptr<IDetector> buildSpecializedDetector(const ConfigFile& config, const std::string modelBasePath, DetectorType detectorType)
{
    ASSERT_LOG(bfs::is_directory(modelBasePath), "Model base path not found [" + modelBasePath + "]");
    std::shared_ptr<IDetector> detector = nullptr;

    // global face detector settings
    if (detectorType == DetectorType::FACE_DETECTOR_GLOBAL) {
        if (config.requireAnyCascade()) {
            FaceDetectorVJ vj(config.face.scaleFactor, config.face.nmsThreshold, config.face.minSize, config.face.maxSize,
                              config.face.confidenceSize, config.face.minNeighbours, config.face.overlapThreshold);
            if (config.HaarCascadeFrontal) {
                std::string modelFilePath = (modelBasePath / bfs::path("haarcascades/haarcascade_frontalface_alt.xml")).generic_string();
                ASSERT_LOG(vj.loadDetector(modelFilePath, NONE), "Load frontal global face detector model [" + modelFilePath + "] failed");
            }
            if (config.HaarCascadeProfile) {
                std::string modelFilePath = (modelBasePath / bfs::path("haarcascades/haarcascade_profileface.xml")).generic_string();
                ASSERT_LOG(vj.loadDetector(modelFilePath, NONE),       "Load profile(L) global face detector model [" + modelFilePath + "] failed");
                ASSERT_LOG(vj.loadDetector(modelFilePath, HORIZONTAL), "Load profile(R) global face detector model [" + modelFilePath + "] failed");
            }
            if (config.LBPCascadeFrontal) {
                std::string modelFilePath = (modelBasePath / bfs::path("lbpcascades/lbpcascade_frontalface.xml")).generic_string();
                ASSERT_LOG(vj.loadDetector(modelFilePath, NONE), "Load frontal global face detector model [" + modelFilePath + "] failed");
            }
            if (config.LBPCascadeProfile) {
                std::string modelFilePath = (modelBasePath / bfs::path("lbpcascades/lbpcascade_profileface.xml")).generic_string();
                ASSERT_LOG(vj.loadDetector(modelFilePath, NONE),       "Load profile(L) global face detector model [" + modelFilePath + "] failed");
                ASSERT_LOG(vj.loadDetector(modelFilePath, HORIZONTAL), "Load profile(R) global face detector model [" + modelFilePath + "] failed");
            }
            if (config.LBPCascadeFrontalImproved) {
                std::string modelFilePath = (modelBasePath / bfs::path("lbpcascades/lbpcascade_frontalface_improved.xml")).generic_string();
                ASSERT_LOG(vj.loadDetector(modelBasePath, NONE), "Load frontal global face detector model [" + modelFilePath + "] failed");
            }
            detector = std::static_pointer_cast<IDetector>(std::make_shared<FaceDetectorVJ>(vj));
        }
        else if (config.FRCNN) {
            #ifdef FACE_RECOG_HAS_FRCNN
            detector = std::static_pointer_cast<IDetector>(std::make_shared<FaceDetectorFRCNN>());
            #endif/*FACE_RECOG_HAS_FRCNN*/
        }
        else if (config.SSD) {
            #ifdef FACE_RECOG_HAS_SSD
            detector = std::static_pointer_cast<IDetector>(std::make_shared<FaceDetectorSSD>());
            #endif/*FACE_RECOG_HAS_SSD*/
        }
        else if (config.YOLO) {
            #ifdef FACE_RECOG_HAS_YOLO
            std::string modelPathProto = (modelBasePath / bfs::path("yolo-face-deploy.prototxt")).generic_string();
            std::string modelPathCaffe = (modelBasePath / bfs::path("yolo-face.caffemodel")).generic_string();
            detector = std::static_pointer_cast<IDetector>(std::make_shared<FaceDetectorYOLO>(modelPathProto, modelPathCaffe));
            #endif/*FACE_RECOG_HAS_YOLO*/
        }
        ASSERT_LOG(detector != nullptr, "Initialize global face detector from specified 'config' failed");
        ASSERT_LOG(detector->modelCount() > 0, "Invalid count of global face detector models");
    }
    // local face detector settings
    else if (detectorType == DetectorType::FACE_DETECTOR_LOCAL && config.useLocalSearchROI) {
        FaceDetectorVJ vj(config.face.scaleFactor, config.face.nmsThreshold, config.face.minSize, config.face.maxSize,
                          config.face.confidenceSize, config.face.minNeighbours, config.face.overlapThreshold);
        // frontal model
        std::string modelFilePath = (modelBasePath / bfs::path("haarcascades/haarcascade_frontalface_alt.xml")).generic_string();
        ASSERT_LOG(vj.loadDetector(modelFilePath, NONE), "Load frontal local face detector model [" + modelFilePath + "] failed");
        // profile models
        if (config.use3CascadesLocalSearch) {
            std::string modelFilePath = (modelBasePath / bfs::path("haarcascades/haarcascade_profileface.xml")).generic_string();
            ASSERT_LOG(vj.loadDetector(modelFilePath, NONE),       "Load profile(L) local face detector model [" + modelFilePath + "] failed");
            ASSERT_LOG(vj.loadDetector(modelFilePath, HORIZONTAL), "Load profile(R) local face detector model [" + modelFilePath + "] failed");
        }
        detector = std::static_pointer_cast<IDetector>(std::make_shared<FaceDetectorVJ>(vj));
        ASSERT_LOG(detector != nullptr, "Initialize local face detector from specified 'config' failed");
        ASSERT_LOG(detector->modelCount() > 0, "Invalid count of local face detector models");
    }
    // eye detector settings
    else if (detectorType == DetectorType::EYE_DETECTOR && config.useEyesDetection) {
        EyeDetector eyes(config.eyes.scaleFactor, config.eyes.nmsThreshold, config.eyes.minSize, config.eyes.maxSize);
        // left/right eyes
        std::string modelFilePathL = (modelBasePath / bfs::path("haarcascades/haarcascade_lefteye_2splits.xml")).generic_string();
        std::string modelFilePathR = (modelBasePath / bfs::path("haarcascades/haarcascade_righteye_2splits.xml")).generic_string();
        ASSERT_LOG(eyes.loadDetector(modelFilePathL), "Load eye(L) detector model [" + modelFilePathL + "] failed");
        ASSERT_LOG(eyes.loadDetector(modelFilePathR), "Load eye(R) detector model [" + modelFilePathR + "] failed");
        eyes.leftEyeIndex  = 0;
        eyes.rightEyeIndex = 1;
        detector = std::static_pointer_cast<IDetector>(std::make_shared<EyeDetector>(eyes));
        ASSERT_LOG(detector != nullptr, "Initialize eye detector from specified 'config' failed");
        ASSERT_LOG(detector->modelCount() == 2, "Invalid count of eye detector models");
    }

    return detector;
}
