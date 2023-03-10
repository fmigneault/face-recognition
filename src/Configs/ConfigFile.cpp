/*
    Code modified for FaceRecod project

    Original code adaptation reference:

        FAST-DT: FAce STructured Detection and Tracking
        Francesco Comaschi (f.comaschi@tue.nl)

        * Code to accompany the paper:
            Online Multi-Face Detection and Tracking using Detector Confidence and Structured SVMs
            F. Comaschi, S. Stuijk, T. Basten, H. Corporaal
            Advanced Video and Signal-Based Surveillance (AVSS), 2015

    Original work:

        STRUCK: Structured Output Tracking with Kernels
        Sam Hare, Amir Saffari, Philip H. S. Torr
        International Conference on Computer Vision (ICCV), 2011
        Copyright (C) 2011 Sam Hare, Oxford Brookes University, Oxford, UK
 */

#include "Configs/ConfigFile.h"
#include "FaceRecog.h"

std::string ConfigFile::FeatureName(FeatureType f)
{
    switch (f) {
        case kFeatureTypeRaw:           return "raw";
        case kFeatureTypeHaar:          return "haar";
        case kFeatureTypeHistogram:     return "histogram";
        default:                        return "undefined";
    }
}

std::string ConfigFile::KernelName(KernelType k)
{
    switch (k) {
        case kKernelTypeLinear:         return "linear";
        case kKernelTypeGaussian:       return "gaussian";
        case kKernelTypeIntersection:   return "intersection";
        case kKernelTypeChi2:           return "chi2";
        default:                        return "undefined";
    }
}

std::string ConfigFile::display() const
{
    string tab = "  ", sep = ": ";
    char padChar = '.';
    size_t padSize = 40;
    size_t padLine = 60;

    // save flags before altering them to restore after
    std::ios coutInitState(nullptr);
    coutInitState.copyfmt(std::cout);

    ostringstream featParams;
    for (size_t f = 0; f < features.size(); ++f) {
        featParams << left << tab << tab << setw(padSize) << setfill(padChar) << "feature" << sep << f << endl
            << tab << left << tab << tab << setw(padSize) << setfill(padChar) << "feature"
            << sep << ConfigFile::FeatureName(features[f].feature) << endl
            << tab << left << tab << tab << setw(padSize) << setfill(padChar) << "kernel"
            << sep << ConfigFile::KernelName(features[f].kernel) << endl;
        if (features[f].params.size() > 0) {
            featParams << tab << left << tab << tab << setw(padSize) << setfill(padChar) << "params" << sep;
            for (size_t p = 0; p < features[f].params.size(); ++p)
                featParams << setw(padSize) << setfill(' ') << features[f].params[p] << endl;
        }
        else featParams << endl;
    }
    ostringstream out;
    out << string(padLine, '=') << endl
        << "config" << sep << endl
        << left << tab << "device" << sep << endl
        << left << tab << tab << setw(padSize) << setfill(padChar) << "deviceindex"                        << sep << deviceIndex                        << endl
        << left << tab << "camera" << sep << endl
        << left << tab << tab << setw(padSize) << setfill(padChar) << "cameraIndex"                        << sep << cameraIndex                        << endl
        << left << tab << tab << setw(padSize) << setfill(padChar) << "cameraType"                         << sep << cameraType                         << endl
        << left << tab << tab << setw(padSize) << setfill(padChar) << "useCameraTrigger"                   << sep << useCameraTrigger                   << endl
        << left << tab << "algorithms" << sep << endl
        << left << tab << tab << "face detection" << sep << endl
        << left << tab << tab << tab << setw(padSize) << setfill(padChar) << "FRCNN"                       << sep << FRCNN                              << endl
        << left << tab << tab << tab << setw(padSize) << setfill(padChar) << "HaarCascadeFrontal"          << sep << HaarCascadeFrontal                 << endl
        << left << tab << tab << tab << setw(padSize) << setfill(padChar) << "HaarCascadeProfile"          << sep << HaarCascadeProfile                 << endl
        << left << tab << tab << tab << setw(padSize) << setfill(padChar) << "LBPCascadeFrontal"           << sep << LBPCascadeFrontal                  << endl
        << left << tab << tab << tab << setw(padSize) << setfill(padChar) << "LBPCascadeProfile"           << sep << LBPCascadeProfile                  << endl
        << left << tab << tab << tab << setw(padSize) << setfill(padChar) << "LBPCascadeFrontalImproved"   << sep << LBPCascadeFrontalImproved          << endl
        << left << tab << tab << tab << setw(padSize) << setfill(padChar) << "SSD"                         << sep << SSD                                << endl
        << left << tab << tab << tab << setw(padSize) << setfill(padChar) << "YOLO"                        << sep << YOLO                               << endl
        << left << tab << tab << "face tracking" << sep << endl
        << left << tab << tab << tab << setw(padSize) << setfill(padChar) << "Camshift"                    << sep << Camshift                           << endl
        << left << tab << tab << tab << setw(padSize) << setfill(padChar) << "Compressive"                 << sep << Compressive                        << endl
        << left << tab << tab << tab << setw(padSize) << setfill(padChar) << "STRUCK"                      << sep << STRUCK                             << endl
        << left << tab << tab << tab << setw(padSize) << setfill(padChar) << "KCF"                         << sep << KCF                                << endl
        << left << tab << tab << "face recognition" << sep << endl
        << left << tab << tab << tab << setw(padSize) << setfill(padChar) << "ESVM"                        << sep << ESVM                               << endl
        << left << tab << tab << tab << setw(padSize) << setfill(padChar) << "FaceNet"                     << sep << FaceNet                            << endl
        << left << tab << tab << tab << setw(padSize) << setfill(padChar) << "TM"                          << sep << TM                                 << endl
        << left << tab << "face recognition parameters" << sep << endl
        << left << tab << tab << setw(padSize) << setfill(padChar) << "useFaceRecognition"                 << sep << useFaceRecognition                 << endl
        << left << tab << tab << setw(padSize) << setfill(padChar) << "useGeometricPositiveStills"         << sep << useGeometricPositiveStills         << endl
        << left << tab << tab << setw(padSize) << setfill(padChar) << "useSyntheticPositiveStills"         << sep << useSyntheticPositiveStills         << endl
        << left << tab << tab << setw(padSize) << setfill(padChar) << "useReferenceNegativeStills"         << sep << useReferenceNegativeStills         << endl
        << left << tab << tab << setw(padSize) << setfill(padChar) << "useGeometricNegativeStills"         << sep << useGeometricNegativeStills         << endl
        << left << tab << tab << setw(padSize) << setfill(padChar) << "geometricTranslatePixels"           << sep << geometricTranslatePixels           << endl
        << left << tab << tab << setw(padSize) << setfill(padChar) << "geometricScalingMinSize"            << sep << geometricScalingMinSize            << endl
        << left << tab << tab << setw(padSize) << setfill(padChar) << "geometricScalingFactor"             << sep << geometricScalingFactor             << endl
        << left << tab << tab << setw(padSize) << setfill(padChar) << "NEGDirExtra"                        << sep << NEGDirExtra                        << endl
        << left << tab << tab << setw(padSize) << setfill(padChar) << "NEGDir"                             << sep << NEGDir                             << endl
        << left << tab << tab << setw(padSize) << setfill(padChar) << "POIDir"                             << sep << POIDir                             << endl
        << left << tab << tab << setw(padSize) << setfill(padChar) << "thresholdFaceConsidered"            << sep << thresholdFaceConsidered            << endl
        << left << tab << tab << setw(padSize) << setfill(padChar) << "thresholdFaceRecognized"            << sep << thresholdFaceRecognized            << endl
        << left << tab << tab << setw(padSize) << setfill(padChar) << "roiAccumulationSize"                << sep << roiAccumulationSize                << endl
        << left << tab << tab << setw(padSize) << setfill(padChar) << "roiAccumulationMode"                << sep << roiAccumulationMode                << endl
        << left << tab << tab << setw(padSize) << setfill(padChar) << "modelsFileSave"                     << sep << modelsFileSave                     << endl
        << left << tab << tab << setw(padSize) << setfill(padChar) << "modelsFileLoad"                     << sep << modelsFileLoad                     << endl
        << left << tab << tab << setw(padSize) << setfill(padChar) << "modelsFileDir"                      << sep << modelsFileDir                      << endl
        << left << tab << "detection/tracking parameters" << sep << endl
        << left << tab << tab << setw(padSize) << setfill(padChar) << "searchRadius"                       << sep << searchRadius                       << endl
        << left << tab << tab << setw(padSize) << setfill(padChar) << "useHungarianMatching"               << sep << useHungarianMatching               << endl
        << left << tab << tab << setw(padSize) << setfill(padChar) << "associationTrackThreshold"          << sep << associationTrackThreshold          << endl
        << left << tab << tab << setw(padSize) << setfill(padChar) << "removeTrackCountThresholdInBounds"  << sep << removeTrackCountThresholdInBounds  << endl
        << left << tab << tab << setw(padSize) << setfill(padChar) << "removeTrackCountThresholdOutBounds" << sep << removeTrackCountThresholdOutBounds << endl
        << left << tab << tab << setw(padSize) << setfill(padChar) << "removeTrackConfidenceInBounds"      << sep << removeTrackConfidenceInBounds      << endl
        << left << tab << tab << setw(padSize) << setfill(padChar) << "removeTrackConfidenceOutBounds"     << sep << removeTrackConfidenceOutBounds     << endl
        << left << tab << tab << setw(padSize) << setfill(padChar) << "createTrackCountThreshold"          << sep << createTrackCountThreshold          << endl
        << left << tab << tab << setw(padSize) << setfill(padChar) << "createTrackConfidenceThreshold"     << sep << createTrackConfidenceThreshold     << endl
        << left << tab << tab << setw(padSize) << setfill(padChar) << "trackerOverlapThresh"               << sep << trackerOverlapThreshold            << endl
        << left << tab << tab << setw(padSize) << setfill(padChar) << "detectionAugmentationOffset"        << sep << detectionAugmentationOffset        << endl
        << left << tab << tab << setw(padSize) << setfill(padChar) << "detectionFrameInterval"             << sep << detectionFrameInterval             << endl
        << left << tab << "training (Fast-DT)" << endl
        << left << tab << tab << setw(padSize) << setfill(padChar) << "seed"                               << sep << seed                               << endl
        << left << tab << tab << setw(padSize) << setfill(padChar) << "svmC"                               << sep << svmC                               << endl
        << left << tab << tab << setw(padSize) << setfill(padChar) << "svmBudgetSize"                      << sep << svmBudgetSize                      << endl
        << left << tab << tab << setw(padSize) << setfill(padChar) << "kernel/features"                    << sep << endl << featParams.str()
        << left << tab << "face bounding boxes" << sep << endl
        << left << tab << tab << setw(padSize) << setfill(padChar) << "faceOverlapThreshold"               << sep << face.overlapThreshold              << endl
        << left << tab << tab << setw(padSize) << setfill(padChar) << "faceMinNeighbours"                  << sep << face.minNeighbours                 << endl
        << left << tab << tab << setw(padSize) << setfill(padChar) << "faceScaleFactor"                    << sep << face.scaleFactor                   << endl
        << left << tab << tab << setw(padSize) << setfill(padChar) << "faceNmsThreshold"                   << sep << face.nmsThreshold                  << endl
        << left << tab << tab << setw(padSize) << setfill(padChar) << "faceMinSize"                        << sep << face.minSize                       << endl
        << left << tab << tab << setw(padSize) << setfill(padChar) << "faceMaxSize"                        << sep << face.maxSize                       << endl
        << left << tab << tab << setw(padSize) << setfill(padChar) << "faceConfidenceSize"                 << sep << face.confidenceSize                << endl
        << left << tab << "eyes bounding boxes" << sep << endl
        << left << tab << tab << setw(padSize) << setfill(padChar) << "useEyeDetection"                    << sep << useEyesDetection                   << endl
        << left << tab << tab << setw(padSize) << setfill(padChar) << "useEyeLocalizedPosition"            << sep << useEyeLocalizedPosition            << endl
        << left << tab << tab << setw(padSize) << setfill(padChar) << "eyesOverlapThreshold"               << sep << eyes.overlapThreshold              << endl
        << left << tab << tab << setw(padSize) << setfill(padChar) << "eyesMinNeighbours"                  << sep << eyes.minNeighbours                 << endl
        << left << tab << tab << setw(padSize) << setfill(padChar) << "eyesScaleFactor"                    << sep << eyes.scaleFactor                   << endl
        << left << tab << tab << setw(padSize) << setfill(padChar) << "eyesNmsThreshold"                   << sep << eyes.nmsThreshold                  << endl
        << left << tab << tab << setw(padSize) << setfill(padChar) << "eyesMinSize"                        << sep << eyes.minSize                       << endl
        << left << tab << tab << setw(padSize) << setfill(padChar) << "eyesMaxSize"                        << sep << eyes.maxSize                       << endl
        << left << tab << "localized ROI detection" << sep << endl
        << left << tab << tab << setw(padSize) << setfill(padChar) << "useLocalSearchROI"                  << sep << useLocalSearchROI                  << endl
        << left << tab << tab << setw(padSize) << setfill(padChar) << "use3CascadesLocalSearch"            << sep << use3CascadesLocalSearch            << endl
        << left << tab << tab << setw(padSize) << setfill(padChar) << "bboxSizeMultiplyer"                 << sep << bboxSizeMultiplyer                 << endl
        << left << tab << "output/debug/display" << sep << endl
        << left << tab << tab << setw(padSize) << setfill(padChar) << "verboseConfig"                      << sep << verboseConfig                      << endl
        << left << tab << tab << setw(padSize) << setfill(padChar) << "verboseDevices"                     << sep << verboseDevices                     << endl
        << left << tab << tab << setw(padSize) << setfill(padChar) << "verboseDebug"                       << sep << verboseDebug                       << endl
        << left << tab << tab << setw(padSize) << setfill(padChar) << "outputDebug"                        << sep << outputDebug                        << endl
        << left << tab << tab << setw(padSize) << setfill(padChar) << "outputFrames"                       << sep << outputFrames                       << endl
        << left << tab << tab << setw(padSize) << setfill(padChar) << "outputROI"                          << sep << outputROI                          << endl
        << left << tab << tab << setw(padSize) << setfill(padChar) << "outputLocalROI"                     << sep << outputLocalROI                     << endl
        << left << tab << tab << setw(padSize) << setfill(padChar) << "roiOutputSize"                      << sep << roiOutputSize                      << endl
        << left << tab << tab << setw(padSize) << setfill(padChar) << "outputDirsClearOnStart"             << sep << outputDirsClearOnStart             << endl
        << left << tab << tab << setw(padSize) << setfill(padChar) << "flipFrames"                         << sep << flipFrames                         << endl
        << left << tab << tab << setw(padSize) << setfill(padChar) << "displayFrames"                      << sep << displayFrames                      << endl
        << left << tab << tab << setw(padSize) << setfill(padChar) << "displayFrameRate"                   << sep << displayFrameRate                   << endl
        << left << tab << tab << setw(padSize) << setfill(padChar) << "displayFrameNumber"                 << sep << displayFrameNumber                 << endl
        << left << tab << tab << setw(padSize) << setfill(padChar) << "displaySequenceTrackID"             << sep << displaySequenceTrackID             << endl
        << left << tab << tab << setw(padSize) << setfill(padChar) << "displayWindowX"                     << sep << displayWindowX                     << endl
        << left << tab << tab << setw(padSize) << setfill(padChar) << "displayWindowY"                     << sep << displayWindowY                     << endl
        << left << tab << tab << setw(padSize) << setfill(padChar) << "displayWindowW"                     << sep << displayWindowW                     << endl
        << left << tab << tab << setw(padSize) << setfill(padChar) << "displayWindowH"                     << sep << displayWindowH                     << endl
        << left << tab << tab << setw(padSize) << setfill(padChar) << "displayUpdateLocalROI"              << sep << displayOldROI                      << endl
        << left << tab << tab << setw(padSize) << setfill(padChar) << "roiColorMode"                       << sep << roiColorMode                       << endl
        << left << tab << tab << setw(padSize) << setfill(padChar) << "roiThickness"                       << sep << roiThickness                       << endl
        << left << tab << tab << setw(padSize) << setfill(padChar) << "roiThicknessOld"                    << sep << roiThicknessOld                    << endl
        << left << tab << "plot display" << sep << endl
        << left << tab << tab << setw(padSize) << setfill(padChar) << "displayPlots"                       << sep << displayPlots                       << endl
        << left << tab << tab << setw(padSize) << setfill(padChar) << "plotAccumulationPoints"             << sep << plotAccumulationPoints             << endl
        << left << tab << tab << setw(padSize) << setfill(padChar) << "plotFigureWidth"                    << sep << plotFigureWidth                    << endl
        << left << tab << tab << setw(padSize) << setfill(padChar) << "plotFigureHeight"                   << sep << plotFigureHeight                   << endl
        << left << tab << tab << setw(padSize) << setfill(padChar) << "plotTrackDirection"                 << sep << plotTrackDirection                 << endl
        << left << tab << tab << setw(padSize) << setfill(padChar) << "plotMaxTracks"                      << sep << plotMaxTracks                      << endl
        << left << tab << tab << setw(padSize) << setfill(padChar) << "plotMaxPOI"                         << sep << plotMaxPOI                         << endl
        << left << tab << tab << setw(padSize) << setfill(padChar) << "plotResetOnTrackLost"               << sep << plotResetOnTrackLost               << endl
        << string(padLine, '=') << endl;

    std::string out_str(out.str());
    std::cout.copyfmt(coutInitState);   // restore output initial state
    return out_str;
}

ostream& operator<< (ostream& out, const ConfigFile& conf)
{
    out << conf.display();
    return out;
}

ClassifierType ConfigFile::getClassifierType() const
{
    if      (FaceNet)   return ClassifierType::CNN_FACE_NET;
    else if (ESVM)      return ClassifierType::ENSEMBLE_ESVM;
    else if (TM)        return ClassifierType::ENSEMBLE_TM;
    return ClassifierType::UNDEFINED;
}

bool ConfigFile::FeatureKernelPair::isValid()
{
    return FeatureName(feature) != "" && KernelName(kernel) != "";
}

bool ConfigFile::requireAnyCascade() const
{
    return HaarCascadeFrontal | HaarCascadeProfile | LBPCascadeFrontal | LBPCascadeProfile | LBPCascadeFrontalImproved;
}

ConfigFile::ConfigFile(const std::string& path)
{
    setDefaults();

    ifstream f(path.c_str());
    ASSERT_LOG(f, "error: could not load config file: '" + path + "'\n");

    string line, name, tmp;
    while (getline(f, line))
    {
        istringstream iss(line);
        iss >> name >> tmp;

        // skip invalid lines and comments
        if (iss.fail() || tmp != "=" || name[0] == '#') continue;

        // training (Fast-DT)
        if      (name == "seed") iss >> seed;
        else if (name == "svmC") iss >> svmC;
        else if (name == "svmBudgetSize") iss >> svmBudgetSize;
        else if (name == "feature")
        {
            string featureName, kernelName;
            double param;
            iss >> featureName >> kernelName >> param;
            FeatureKernelPair fkp;

            if      (featureName == FeatureName(kFeatureTypeHaar))      fkp.feature = kFeatureTypeHaar;
            else if (featureName == FeatureName(kFeatureTypeRaw))       fkp.feature = kFeatureTypeRaw;
            else if (featureName == FeatureName(kFeatureTypeHistogram)) fkp.feature = kFeatureTypeHistogram;
            else THROW("unrecognised feature: " + featureName);

            if      (kernelName == KernelName(kKernelTypeLinear))       fkp.kernel = kKernelTypeLinear;
            else if (kernelName == KernelName(kKernelTypeIntersection)) fkp.kernel = kKernelTypeIntersection;
            else if (kernelName == KernelName(kKernelTypeChi2))         fkp.kernel = kKernelTypeChi2;
            else if (kernelName == KernelName(kKernelTypeGaussian)) {
                ASSERT_LOG(iss, "gaussian kernel requires a parameter (sigma)");
                ASSERT_LOG(param > 0, "gaussian kernel sigma must be greater than zero");
                fkp.kernel = kKernelTypeGaussian;
                fkp.params.push_back(param);
            }
            else THROW("unrecognised kernel: " + kernelName);
            features.push_back(fkp);
        }

        // device parameters
        else if (name == "deviceIndex")                             iss >> deviceIndex;
        // camera parameters
        else if (name == "cameraIndex")                             iss >> cameraIndex;
        else if (name == "cameraType")                              iss >> cameraType;
        else if (name == "useCameraTrigger")                        iss >> useCameraTrigger;
        // algorithms for face detection
        else if (name == "FRCNN")                                   iss >> FRCNN;
        else if (name == "HaarCascadeFrontal")                      iss >> HaarCascadeFrontal;
        else if (name == "HaarCascadeProfile")                      iss >> HaarCascadeProfile;
        else if (name == "LBPCascadeFrontal")                       iss >> LBPCascadeFrontal;
        else if (name == "LBPCascadeProfile")                       iss >> LBPCascadeProfile;
        else if (name == "LBPCascadeFrontalImproved")               iss >> LBPCascadeFrontalImproved;
        else if (name == "SSD")                                     iss >> SSD;
        else if (name == "YOLO")                                    iss >> YOLO;
        // algorithms for face tracking
        else if (name == "Camshift")                                iss >> Camshift;
        else if (name == "Compressive")                             iss >> Compressive;
        else if (name == "KCF")                                     iss >> KCF;
        else if (name == "STRUCK")                                  iss >> STRUCK;
        // algorithms for face recognition
        else if (name == "ESVM")                                    iss >> ESVM;
        else if (name == "FaceNet")                                 iss >> FaceNet;
        else if (name == "TM")                                      iss >> TM;
        // face recognition parameters
        else if (name == "useFaceRecognition")                      iss >> useFaceRecognition;
        else if (name == "useGeometricPositiveStills")              iss >> useGeometricPositiveStills;
        else if (name == "useSyntheticPositiveStills")              iss >> useSyntheticPositiveStills;
        else if (name == "useReferenceNegativeStills")              iss >> useReferenceNegativeStills;
        else if (name == "useGeometricNegativeStills")              iss >> useGeometricNegativeStills;
        else if (name == "geometricTranslatePixels")                iss >> geometricTranslatePixels;
        else if (name == "geometricScalingMinSize")                 iss >> geometricScalingMinSize;
        else if (name == "geometricScalingFactor")                  iss >> geometricScalingFactor;
        else if (name == "NEGDirExtra")                             iss >> NEGDirExtra;
        else if (name == "NEGDir")                                  iss >> NEGDir;
        else if (name == "POIDir")                                  iss >> POIDir;
        else if (name == "thresholdFaceConsidered")                 iss >> thresholdFaceConsidered;
        else if (name == "thresholdFaceRecognized")                 iss >> thresholdFaceRecognized;
        else if (name == "roiAccumulationSize")                     iss >> roiAccumulationSize;
        else if (name == "roiAccumulationMode") {
            int mode;
            iss >> mode;
            roiAccumulationMode = (mode == CircularBuffer::ScoreMode::CUMUL) ? CircularBuffer::ScoreMode::CUMUL
                                : (mode == CircularBuffer::ScoreMode::AVG)   ? CircularBuffer::ScoreMode::AVG
                                :                                              CircularBuffer::ScoreMode::RAW;
        }
        else if (name == "modelsFileSave")                          iss >> modelsFileSave;
        else if (name == "modelsFileLoad")                          iss >> modelsFileLoad;
        else if (name == "modelsFileDir")                           iss >> modelsFileDir;
        // detection and tracking parameters
        else if (name == "searchRadius")                            iss >> searchRadius;
        else if (name == "useHungarianMatching")                    iss >> useHungarianMatching;
        else if (name == "associationTrackThreshold")               iss >> associationTrackThreshold;
        else if (name == "removeTrackCountThresholdInBounds")       iss >> removeTrackCountThresholdInBounds;
        else if (name == "removeTrackCountThresholdOutBounds")      iss >> removeTrackCountThresholdOutBounds;
        else if (name == "removeTrackConfidenceInBounds")           iss >> removeTrackConfidenceInBounds;
        else if (name == "removeTrackConfidenceOutBounds")          iss >> removeTrackConfidenceOutBounds;
        else if (name == "createTrackCountThreshold")               iss >> createTrackCountThreshold;
        else if (name == "createTrackConfidenceThreshold")          iss >> createTrackConfidenceThreshold;
        else if (name == "trackerOverlapThreshold")                 iss >> trackerOverlapThreshold;
        else if (name == "detectionAugmentationOffset")             iss >> detectionAugmentationOffset;
        else if (name == "detectionFrameInterval")                  iss >> detectionFrameInterval;
        // face bounding boxes parameters
        else if (name == "faceOverlapThreshold")                    iss >> face.overlapThreshold;
        else if (name == "faceMinNeighbours")                       iss >> face.minNeighbours;
        else if (name == "faceScaleFactor")                         iss >> face.scaleFactor;
        else if (name == "faceNmsThreshold")                        iss >> face.nmsThreshold;
        else if (name == "faceMinWidth")                            iss >> face.minSize.width;
        else if (name == "faceMinHeight")                           iss >> face.minSize.height;
        else if (name == "faceMaxWidth")                            iss >> face.maxSize.width;
        else if (name == "faceMaxHeight")                           iss >> face.maxSize.height;
        else if (name == "faceConfidenceSize") {
            int size;
            iss >> size;
            face.confidenceSize = cv::Size(size, size);
        }
        // eyes bounding boxes parameters
        else if (name == "useEyesDetection")                        iss >> useEyesDetection;
        else if (name == "useEyeLocalizedPosition")                 iss >> useEyeLocalizedPosition;
        else if (name == "eyesOverlapThreshold")                    iss >> eyes.overlapThreshold;
        else if (name == "eyesMinNeighbours")                       iss >> eyes.minNeighbours;
        else if (name == "eyesScaleFactor")                         iss >> eyes.scaleFactor;
        else if (name == "eyesNmsThreshold")                        iss >> eyes.nmsThreshold;
        else if (name == "eyesMinWidth")                            iss >> eyes.minSize.width;
        else if (name == "eyesMinHeight")                           iss >> eyes.minSize.height;
        else if (name == "eyesMaxWidth")                            iss >> eyes.maxSize.width;
        else if (name == "eyesMaxHeight")                           iss >> eyes.maxSize.height;
        // localized ROI detection
        else if (name == "useLocalSearchROI")                       iss >> useLocalSearchROI;
        else if (name == "use3CascadesLocalSearch")                 iss >> use3CascadesLocalSearch;
        else if (name == "bboxSizeMultiplyer")                      iss >> bboxSizeMultiplyer;
        // output/debug/display parameters
        else if (name == "verboseConfig")                           iss >> verboseConfig;
        else if (name == "verboseDevices")                          iss >> verboseDevices;
        else if (name == "verboseDebug")                            iss >> verboseDebug;
        else if (name == "outputDebug")                             iss >> outputDebug;
        else if (name == "outputFrames")                            iss >> outputFrames;
        else if (name == "outputROI")                               iss >> outputROI;
        else if (name == "outputLocalROI")                          iss >> outputLocalROI;
        else if (name == "roiOutputSize")                           iss >> roiOutputSize;
        else if (name == "outputDirsClearOnStart")                  iss >> outputDirsClearOnStart;
        else if (name == "flipFrames")                              iss >> flipFrames;
        else if (name == "displayFrames")                           iss >> displayFrames;
        else if (name == "displayFrameRate")                        iss >> displayFrameRate;
        else if (name == "displayFrameNumber")                      iss >> displayFrameNumber;
        else if (name == "displaySequenceTrackID")                  iss >> displaySequenceTrackID;
        else if (name == "displayWindowX")                          iss >> displayWindowX;
        else if (name == "displayWindowY")                          iss >> displayWindowY;
        else if (name == "displayWindowW")                          iss >> displayWindowW;
        else if (name == "displayWindowH")                          iss >> displayWindowH;
        else if (name == "displayOldROI")                           iss >> displayOldROI;
        else if (name == "roiColorMode")                            iss >> roiColorMode;
        else if (name == "roiThickness")                            iss >> roiThickness;
        else if (name == "roiThicknessOld")                         iss >> roiThicknessOld;
        // plot display
        else if (name == "displayPlots")                            iss >> displayPlots;
        else if (name == "plotAccumulationPoints")                  iss >> plotAccumulationPoints;
        else if (name == "plotFigureWidth")                         iss >> plotFigureWidth;
        else if (name == "plotFigureHeight")                        iss >> plotFigureHeight;
        else if (name == "plotTrackDirection")                      iss >> plotTrackDirection;
        else if (name == "plotMaxTracks")                           iss >> plotMaxTracks;
        else if (name == "plotMaxPOI")                              iss >> plotMaxPOI;
        else if (name == "plotResetOnTrackLost")                    iss >> plotResetOnTrackLost;
    }
    validateValues();
}

void ConfigFile::setDefaults()
{
    seed                        = 0;
    svmC                        = 1.0;
    svmBudgetSize               = 0;

    deviceIndex                 = 0;

    cameraType                  = CameraType::UNDEFINED;
    cameraIndex                 = -1;
    useCameraTrigger            = false;

    FRCNN                       = false;
    HaarCascadeFrontal          = true;
    HaarCascadeProfile          = true;
    LBPCascadeFrontal           = false;
    LBPCascadeProfile           = false;
    LBPCascadeFrontalImproved   = false;
    SSD                         = false;
    YOLO                        = false;

    Camshift                    = false;
    Compressive                 = false;
    KCF                         = false;
    STRUCK                      = true;

    ESVM                        = true;
    FaceNet                     = false;
    TM                          = false;

    useFaceRecognition          = true;
    useGeometricPositiveStills  = false;
    useSyntheticPositiveStills  = false;
    useReferenceNegativeStills  = false;
    useGeometricNegativeStills  = false;
    geometricTranslatePixels    = 10;
    geometricScalingMinSize     = 24;
    geometricScalingFactor      = 0.9;
    NEGDirExtra                 = "../POI/NEG/";
    NEGDir                      = "../NEG/";
    POIDir                      = "../POI/";
    thresholdFaceConsidered     = 0.80;
    thresholdFaceRecognized     = 0.95;
    roiAccumulationSize         = 30;
    roiAccumulationMode         = CircularBuffer::ScoreMode::RAW;
    modelsFileSave              = false;
    modelsFileLoad              = false;
    modelsFileDir               = "./models/";

    searchRadius                            = 30;
    useHungarianMatching                    = false;
    associationTrackThreshold               = 90;
    removeTrackCountThresholdInBounds       = 4;
    removeTrackCountThresholdOutBounds      = 4;
    removeTrackConfidenceInBounds           = 0.2;
    removeTrackConfidenceOutBounds          = 0.2;
    createTrackCountThreshold               = 2;
    createTrackConfidenceThreshold          = 0.25;
    trackerOverlapThreshold                 = 0.6;
    detectionFrameInterval                  = 3;
    features.clear();

    face.overlapThreshold   = 0.1;
    face.scaleFactor        = 1.2;
    face.nmsThreshold       = 4;
    face.minNeighbours      = 0;
    face.minSize            = cv::Size(25, 25);
    face.maxSize            = cv::Size(120, 120);
    face.confidenceSize     = cv::Size(40, 40);

    useEyesDetection        = false;
    useEyeLocalizedPosition = false;
    eyes.overlapThreshold   = 0.1;
    eyes.scaleFactor        = 1.2;
    eyes.nmsThreshold       = 4;
    eyes.minNeighbours      = 0;
    eyes.minSize            = cv::Size(5, 5);
    eyes.maxSize            = cv::Size(40, 40);

    useLocalSearchROI       = true;
    use3CascadesLocalSearch = false;
    bboxSizeMultiplyer      = 1.5;

    verboseConfig           = true;
    verboseDevices          = true;
    verboseDebug            = true;
    outputDebug             = true;
    outputFrames            = true;
    outputROI               = true;
    outputLocalROI          = true;
    roiOutputSize           = 96;
    outputDirsClearOnStart  = false;
    flipFrames              = false;
    displayFrames           = true;
    displayFrameRate        = false;
    displayFrameNumber      = false;
    displaySequenceTrackID  = false;
    displayWindowX          = 0;
    displayWindowY          = 0;
    displayWindowW          = 800;
    displayWindowH          = 600;
    displayOldROI           = true;
    roiColorMode            = MultiColorType::GREEN_ORANGE_RED;
    roiThickness            = 2;
    roiThicknessOld         = 1;

    displayPlots            = false;
    plotAccumulationPoints  = 30;
    plotFigureWidth         = 800;
    plotFigureHeight        = 800;
    plotTrackDirection      = 0;
    plotMaxTracks           = 10;
    plotMaxPOI              = 10;
    plotResetOnTrackLost    = true;
}

void ConfigFile::validateValues()
{
    ASSERT_LOG(svmC > 0, "Config 'svmC' not greater than zero");
    ASSERT_LOG(svmBudgetSize >= 0, "Config 'svmBudgetSize' not greater or equal to zero");
    ASSERT_LOG(deviceIndex >= 0, "Config 'deviceIndex' not greater or equal to zero");

    if (!useFaceRecognition) {
        ASSERT_WARN(!useGeometricPositiveStills, "Config 'useGeometricPositiveStills' will be ignored since 'useFaceRecognition' is disabled");
        ASSERT_WARN(!useSyntheticPositiveStills, "Config 'useSyntheticPositiveStills' will be ignored since 'useFaceRecognition' is disabled");
        ASSERT_WARN(!useReferenceNegativeStills, "Config 'useReferenceNegativeStills' will be ignored since 'useFaceRecognition' is disabled");
        ASSERT_WARN(!useGeometricNegativeStills, "Config 'useGeometricNegativeStills' will be ignored since 'useFaceRecognition' is disabled");
        if (useGeometricPositiveStills || useGeometricNegativeStills) {
            ASSERT_LOG(geometricTranslatePixels > 0, "Config 'geometricTranslatePixels' not greater than 0");
            ASSERT_LOG(geometricScalingMinSize > 0, "Config 'geometricScalingMinSize' not greater than 0");
            ASSERT_LOG(geometricScalingFactor > 0.0 && geometricScalingFactor < 1.0, "Config 'geometricScalingFactor' not in range ]0,1[");
        }
    }
    else if (!useReferenceNegativeStills)
        ASSERT_WARN(!useGeometricNegativeStills, "Config 'useGeometricNegativeStills' ignored since 'useReferenceNegativeStills' is disabled");

    ASSERT_LOG(face.overlapThreshold >= 0.0 && face.overlapThreshold <= 1.0, "'faceOverlapThreshold' not in range [0,1]");
    ASSERT_LOG(face.scaleFactor > 1.0, "Config 'faceScaleFactor' not greater than 1");
    ASSERT_LOG(face.nmsThreshold >= 0, "Config 'faceNmsThreshold' not greater or equal to 0");
    ASSERT_LOG(face.minNeighbours >= 0, "Config 'faceMinNeighbours' not greater or equal to 0");
    ASSERT_LOG(face.minSize.width > 0 && face.minSize.height > 0, "Config 'faceMinSize' not greater than zero");
    ASSERT_LOG(face.confidenceSize.width > 0 && face.confidenceSize.height > 0, "Config 'faceConfidenceSize' not greater than zero");
    ASSERT_LOG(face.maxSize.width > face.minSize.width && face.maxSize.height > face.minSize.height,
               "Config 'faceMaxSize' not greater than 'faceMinSize'");

    if (!useEyesDetection)
        ASSERT_WARN(!useEyeLocalizedPosition, "Config 'useEyeLocalizedPosition' will be ignored since 'useEyesDetection' is disabled");
    else {
        ASSERT_LOG(eyes.overlapThreshold >= 0.0 && eyes.overlapThreshold <= 1.0, "Config 'eyesOverlapThreshold' not in range [0,1]");
        ASSERT_LOG(eyes.scaleFactor > 1.0, "Config 'eyesScaleFactor' not greater than 1");
        ASSERT_LOG(eyes.nmsThreshold >= 0, "Config 'eyesNmsThreshold' not greater or equal to 0");
        ASSERT_LOG(eyes.minNeighbours >= 0, "Config 'eyesMinNeighbours' not greater or equal to 0");
        ASSERT_LOG(eyes.minSize.width > 0 && eyes.minSize.height > 0, "Config 'eyesMinSize' not greater than zero");
        ASSERT_LOG(eyes.maxSize.width > eyes.minSize.width && eyes.maxSize.height > eyes.minSize.height,
                   "Config 'eyesMaxSize' not greater than 'eyesMinSize'");
    }

    ASSERT_LOG(displayWindowW > 0, "Config 'displayWindowW' not greater than 0");
    ASSERT_LOG(displayWindowH > 0, "Config 'displayWindowH' not greater than 0");

    ASSERT_LOG(searchRadius > 0, "Config 'searchRadius' not greater than 0");
    ASSERT_LOG(searchRadius <= displayWindowW && searchRadius <= displayWindowH, "Config 'search' radius not within display window boundaries");
    if (useHungarianMatching)
        ASSERT_LOG(associationTrackThreshold >= 0, "Config 'associationTrackThreshold' not greater or equal to 0");
    ASSERT_LOG(removeTrackCountThresholdInBounds >= 0, "Config 'removeTrackCountThresholdInBounds' not greater or equal to 0");
    ASSERT_LOG(removeTrackCountThresholdOutBounds >= 0, "Config 'removeTrackCountThresholdOutBounds' not greater or equal to 0");
    ASSERT_LOG(removeTrackConfidenceInBounds >= 0, "Config 'removeTrackConfidenceInBounds' not greater or equal to 0");
    ASSERT_LOG(removeTrackConfidenceOutBounds >= 0, "Config 'removeTrackConfidenceOutBounds' not greater or equal to 0");
    ASSERT_LOG(createTrackCountThreshold >= 0, "Config 'createTrackCountThreshold' not greater or equal to 0");
    ASSERT_LOG(createTrackConfidenceThreshold >= 0, "Config 'createTrackConfidenceThreshold' not greater or equal to 0");
    ASSERT_LOG(trackerOverlapThreshold >= 0.0 && trackerOverlapThreshold <= 1.0, "Config 'trackerOverlapThreshold' not in range [0,1]");
    ASSERT_LOG(detectionFrameInterval > 0, "Config 'detectionFrameInterval' not greater than 0");
    if (useLocalSearchROI)
        ASSERT_LOG(bboxSizeMultiplyer > 0.0, "Config 'bboxSizeMultiplyer' not greater than 0");

    ASSERT_LOG(cameraType.isDefined(), "Config 'cameraType' undefined: [" + cameraType.name() + "]");
    ASSERT_LOG(cameraIndex >= 0, "Config 'cameraIndex' not greater or equal to 0");

    ASSERT_LOG(roiOutputSize > 0, "Config 'roiOutputSize' not greater than 0");
    ASSERT_LOG(roiThickness > 0, "Config 'roiThickness' not greater than 0");
    ASSERT_LOG(roiThicknessOld > 0, "Config 'roiThicknessOld' not greater than 0");

    if (displayPlots) {
        ASSERT_LOG(plotAccumulationPoints > 0, "Config 'plotAccumulationPoints' not greater than 0");
        ASSERT_LOG(plotFigureWidth > 0, "Config 'plotFigureWidth' not greater than 0");
        ASSERT_LOG(plotFigureHeight > 0, "Config 'plotFigureHeight' not greater than 0");
        ASSERT_LOG(plotTrackDirection == 0 || plotTrackDirection == 1, "Config 'plotTrackDirection' unknown value");
        ASSERT_LOG(plotMaxTracks > 0, "Config 'plotMaxTracks' not greater than 0");
        ASSERT_LOG(plotMaxPOI > 0, "Config 'plotMaxPOI' not greater than 0");
    }

    bool anyCascade = requireAnyCascade();
    ASSERT_LOG((anyCascade ^ SSD ^ FRCNN ^ YOLO) ^ (anyCascade & SSD & FRCNN & YOLO),
               "At least one and only one face detector type can be used at the same time!");
    ASSERT_LOG((STRUCK ^ KCF ^ Camshift ^ Compressive) ^ (STRUCK & KCF & Camshift & Compressive),
               "At least one and only one face tracker type can be used at the same time!");
    ASSERT_LOG((ESVM ^ FaceNet ^ TM) ^ (ESVM & FaceNet & TM),
               "At least one and only one face classifier type can be used at the same time!");

    if (STRUCK) {
        ASSERT_LOG(features.size() > 0, "Config 'features' employed with 'STRUCK' face tracker not greater than 0");
    }
    if (FRCNN) {
        #ifndef FACE_RECOG_HAS_FRCNN
        THROW("Face detector FRCNN specified in config cannot be used without 'FACE_RECOG_HAS_FRCNN' definition");
        #endif/*FACE_RECOG_HAS_FRCNN*/
    }
    if (YOLO) {
        #ifndef FACE_RECOG_HAS_YOLO
        THROW("Face detector YOLO specified in config cannot be used without 'FACE_RECOG_HAS_YOLO' definition");
        #endif/*FACE_RECOG_HAS_YOLO*/
    }

    ASSERT_LOG(roiAccumulationSize > 0, "Config 'roiAccumulationSize' not greater than 0");
    ASSERT_WARN(useFaceRecognition, "Config 'useFaceRecognition' detected will disable all face recognition operations and checks");
    if (useFaceRecognition) {
        if (roiAccumulationMode != CircularBuffer::ScoreMode::CUMUL) {
            ASSERT_LOG(thresholdFaceConsidered >= 0 && thresholdFaceConsidered <= 1, "Config 'thresholdFaceConsidered' not in range [0,1]");
            ASSERT_LOG(thresholdFaceRecognized >= 0 && thresholdFaceRecognized <= 1, "Config 'thresholdFaceRecognized' not in range [0,1]");
        }
        else {
            ASSERT_LOG(thresholdFaceConsidered >= 0 && thresholdFaceConsidered <= roiAccumulationSize,
                       "Config 'thresholdFaceConsidered' not in range [0,roiAccumulationSize]");
            ASSERT_LOG(thresholdFaceRecognized >= 0 && thresholdFaceRecognized <= roiAccumulationSize,
                       "Config 'thresholdFaceRecognized' not in range [0,roiAccumulationSize]");
        }
        ASSERT_LOG(thresholdFaceRecognized > thresholdFaceConsidered, "Config 'thresholdFaceRecognized' not greater than 'thresholdFaceConsidered'");
        ASSERT_LOG(bfs::is_directory(NEGDir), "Config 'NEGDir' for negative samples files directory not found [" + NEGDir + "]");
        ASSERT_LOG(bfs::is_directory(POIDir), "Config 'POIDir' for Person of Interest ROI directory not found [" + POIDir + "]");
        ASSERT_LOG(getClassifierType().isDefined(), "Config 'classifierType' undefined: [" + getClassifierType().name() + "]");
        if (modelsFileSave) {
            ASSERT_LOG(bfs::is_directory(modelsFileDir),
                    "Config 'modelsFileDir' saving directory requested by 'modelsFileSave' not found [" + modelsFileDir + "]");
        }
    }
}
