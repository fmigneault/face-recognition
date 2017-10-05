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

#include "FaceRecogConfig.h"

namespace config {

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
        << left << tab << tab << setw(padSize) << setfill(padChar) << "FastDT"                             << sep << FastDT                             << endl
        << left << tab << tab << setw(padSize) << setfill(padChar) << "FRCNN"                              << sep << FRCNN                              << endl
        << left << tab << tab << setw(padSize) << setfill(padChar) << "VJ"                                 << sep << VJ                                 << endl
        << left << tab << tab << setw(padSize) << setfill(padChar) << "LBP"                                << sep << LBP                                << endl
        << left << tab << tab << setw(padSize) << setfill(padChar) << "ImprovedLBP"                        << sep << ImprovedLBP                        << endl
        << left << tab << tab << setw(padSize) << setfill(padChar) << "YOLO"                               << sep << YOLO                               << endl
        << left << tab << tab << setw(padSize) << setfill(padChar) << "camshift"                           << sep << camshift                           << endl
        << left << tab << tab << setw(padSize) << setfill(padChar) << "compressive"                        << sep << compressive                        << endl
        << left << tab << tab << setw(padSize) << setfill(padChar) << "ESVM"                               << sep << ESVM                               << endl
        << left << tab << tab << setw(padSize) << setfill(padChar) << "FaceNet"                            << sep << FaceNet                            << endl
        << left << tab << tab << setw(padSize) << setfill(padChar) << "TM"                                 << sep << TM                                 << endl
        << left << tab << "face recognition" << sep << endl
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
        << left << tab << "detection and tracking" << sep << endl
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

    return out.str();
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
        else if (name == "deviceIndex")                     iss >> deviceIndex;
        // camera parameters
        else if (name == "cameraIndex")                     iss >> cameraIndex;
        else if (name == "cameraType")                      iss >> cameraType;
        else if (name == "useCameraTrigger")                iss >> useCameraTrigger;
        // algorithms for face detection, tracking and recognition
        else if (name == "FastDT")                          iss >> FastDT;
        else if (name == "FRCNN")                           iss >> FRCNN;
        else if (name == "VJ")                              iss >> VJ;
        else if (name == "LBP")                             iss >> LBP;
        else if (name == "ImprovedLBP")                     iss >> ImprovedLBP;
        else if (name == "YOLO")                            iss >> YOLO;
        else if (name == "camshift")                        iss >> camshift;
        else if (name == "compressive")                     iss >> compressive;
        else if (name == "ESVM")                            iss >> ESVM;
        else if (name == "FaceNet")                         iss >> FaceNet;
        else if (name == "TM")                              iss >> TM;
        // face recognition parameters
        else if (name == "useFaceRecognition")              iss >> useFaceRecognition;
        else if (name == "useGeometricPositiveStills")      iss >> useGeometricPositiveStills;
        else if (name == "useSyntheticPositiveStills")      iss >> useSyntheticPositiveStills;
        else if (name == "useReferenceNegativeStills")      iss >> useReferenceNegativeStills;
        else if (name == "useGeometricNegativeStills")      iss >> useGeometricNegativeStills;
        else if (name == "geometricTranslatePixels")        iss >> geometricTranslatePixels;
        else if (name == "geometricScalingMinSize")         iss >> geometricScalingMinSize;
        else if (name == "geometricScalingFactor")          iss >> geometricScalingFactor;
        else if (name == "NEGDirExtra")                     iss >> NEGDirExtra;
        else if (name == "NEGDir")                          iss >> NEGDir;
        else if (name == "POIDir")                          iss >> POIDir;
        else if (name == "thresholdFaceConsidered")         iss >> thresholdFaceConsidered;
        else if (name == "thresholdFaceRecognized")         iss >> thresholdFaceRecognized;
        else if (name == "roiAccumulationSize")             iss >> roiAccumulationSize;
        else if (name == "roiAccumulationMode") {
            int mode;
            iss >> mode;
            roiAccumulationMode = (mode == ScoreMode::CUMUL) ? ScoreMode::CUMUL
                                : (mode == ScoreMode::AVG)   ? ScoreMode::AVG
                                :                              ScoreMode::RAW;
        }
        // detection and tracking parameters
        else if (name == "searchRadius")                    iss >> searchRadius;
        else if (name == "useHungarianMatching")            iss >> useHungarianMatching;
        else if (name == "associationTrackThreshold")       iss >> associationTrackThreshold;
        else if (name == "removeThresholdInsideBounds")     iss >> removeThresholdInsideBounds;
        else if (name == "removeThresholdOutsideBounds")    iss >> removeThresholdOutsideBounds;
        else if (name == "removeScoreInsideBounds")         iss >> removeScoreInsideBounds;
        else if (name == "removeScoreOutsideBounds")        iss >> removeScoreOutsideBounds;
        else if (name == "createThreshold")                 iss >> createThreshold;
        else if (name == "createScore")                     iss >> createScore;
        else if (name == "trackerOverlapThreshold")         iss >> trackerOverlapThreshold;
        else if (name == "detectionAugmentationOffset")     iss >> detectionAugmentationOffset;
        else if (name == "detectionFrameInterval")          iss >> detectionFrameInterval;
        // face bounding boxes parameters
        else if (name == "faceOverlapThreshold")            iss >> face.overlapThreshold;
        else if (name == "faceMinNeighbours")               iss >> face.minNeighbours;
        else if (name == "faceScaleFactor")                 iss >> face.scaleFactor;
        else if (name == "faceNmsThreshold")                iss >> face.nmsThreshold;
        else if (name == "faceMinWidth")                    iss >> face.minSize.width;
        else if (name == "faceMinHeight")                   iss >> face.minSize.height;
        else if (name == "faceMaxWidth")                    iss >> face.maxSize.width;
        else if (name == "faceMaxHeight")                   iss >> face.maxSize.height;
        else if (name == "faceConfidenceSize") {
            int size;
            iss >> size;
            face.confidenceSize = cv::Size(size, size);
        }
        // eyes bounding boxes parameters
        else if (name == "useEyesDetection")                iss >> useEyesDetection;
        else if (name == "useEyeLocalizedPosition")         iss >> useEyeLocalizedPosition;
        else if (name == "eyesOverlapThreshold")            iss >> eyes.overlapThreshold;
        else if (name == "eyesMinNeighbours")               iss >> eyes.minNeighbours;
        else if (name == "eyesScaleFactor")                 iss >> eyes.scaleFactor;
        else if (name == "eyesNmsThreshold")                iss >> eyes.nmsThreshold;
        else if (name == "eyesMinWidth")                    iss >> eyes.minSize.width;
        else if (name == "eyesMinHeight")                   iss >> eyes.minSize.height;
        else if (name == "eyesMaxWidth")                    iss >> eyes.maxSize.width;
        else if (name == "eyesMaxHeight")                   iss >> eyes.maxSize.height;
        // localized ROI detection
        else if (name == "useLocalSearchROI")               iss >> useLocalSearchROI;
        else if (name == "use3CascadesLocalSearch")         iss >> use3CascadesLocalSearch;
        else if (name == "bboxSizeMultiplyer")              iss >> bboxSizeMultiplyer;
        // output/debug/display parameters
        else if (name == "verboseDebug")                    iss >> verboseDebug;
        else if (name == "outputDebug")                     iss >> outputDebug;
        else if (name == "outputFrames")                    iss >> outputFrames;
        else if (name == "outputROI")                       iss >> outputROI;
        else if (name == "outputLocalROI")                  iss >> outputLocalROI;
        else if (name == "roiOutputSize")                   iss >> roiOutputSize;
        else if (name == "outputDirsClearOnStart")          iss >> outputDirsClearOnStart;
        else if (name == "flipFrames")                      iss >> flipFrames;
        else if (name == "displayFrames")                   iss >> displayFrames;
        else if (name == "displayFrameRate")                iss >> displayFrameRate;
        else if (name == "displayFrameNumber")              iss >> displayFrameNumber;
        else if (name == "displaySequenceTrackID")          iss >> displaySequenceTrackID;
        else if (name == "displayWindowX")                  iss >> displayWindowX;
        else if (name == "displayWindowY")                  iss >> displayWindowY;
        else if (name == "displayWindowW")                  iss >> displayWindowW;
        else if (name == "displayWindowH")                  iss >> displayWindowH;
        else if (name == "displayOldROI")                   iss >> displayOldROI;
        else if (name == "roiColorMode")                    iss >> roiColorMode;
        else if (name == "roiThickness")                    iss >> roiThickness;
        else if (name == "roiThicknessOld")                 iss >> roiThicknessOld;
        // plot display
        else if (name == "displayPlots")                    iss >> displayPlots;
        else if (name == "plotAccumulationPoints")          iss >> plotAccumulationPoints;
        else if (name == "plotFigureWidth")                 iss >> plotFigureWidth;
        else if (name == "plotFigureHeight")                iss >> plotFigureHeight;
        else if (name == "plotTrackDirection")              iss >> plotTrackDirection;
        else if (name == "plotMaxTracks")                   iss >> plotMaxTracks;
        else if (name == "plotMaxPOI")                      iss >> plotMaxPOI;
        else if (name == "plotResetOnTrackLost")            iss >> plotResetOnTrackLost;
    }
    validateValues();
}

void ConfigFile::setDefaults()
{
    seed = 0;
    svmC = 1.0;
    svmBudgetSize = 0;

    deviceIndex = 0;

    cameraType              = CameraType::UNDEFINED;
    cameraIndex             = -1;
    useCameraTrigger        = false;
            
    FastDT                  = true;
    FRCNN                   = false;
    VJ                      = false;
    LBP                     = false;
    ImprovedLBP             = false;
    YOLO                    = false;
    camshift                = false;
    compressive             = false;        
    ESVM                    = true;
    FaceNet                 = false;
    TM                      = false;
    
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
    roiAccumulationMode         = ScoreMode::RAW;
    
    searchRadius                    = 30;
    useHungarianMatching            = false;
    associationTrackThreshold       = 90;
    removeThresholdInsideBounds     = 4;
    removeThresholdOutsideBounds    = 4;
    removeScoreInsideBounds         = 0.2;
    removeScoreOutsideBounds        = 0.2;
    createThreshold                 = 2;
    createScore                     = 0.25;
    trackerOverlapThreshold         = 0.6;
    detectionFrameInterval          = 3;
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
    ASSERT_LOG(svmC > 0, "'svmC' not greater than zero");
    ASSERT_LOG(svmBudgetSize >= 0, "'svmBudgetSize' not greater or equal to zero");
    ASSERT_LOG(deviceIndex >= 0, "'deviceIndex' not greater or equal to zero");

    if (!useFaceRecognition) {
        ASSERT_WARN(!useGeometricPositiveStills, "'useGeometricPositiveStills' will be ignored since 'useFaceRecognition' is disabled");
        ASSERT_WARN(!useSyntheticPositiveStills, "'useSyntheticPositiveStills' will be ignored since 'useFaceRecognition' is disabled");
        ASSERT_WARN(!useReferenceNegativeStills, "'useReferenceNegativeStills' will be ignored since 'useFaceRecognition' is disabled");
        ASSERT_WARN(!useGeometricNegativeStills, "'useGeometricNegativeStills' will be ignored since 'useFaceRecognition' is disabled");
        if (useGeometricPositiveStills || useGeometricNegativeStills) {
            ASSERT_LOG(geometricTranslatePixels > 0, "'geometricTranslatePixels' not greater than 0");
            ASSERT_LOG(geometricScalingMinSize > 0, "'geometricScalingMinSize' not greater than 0");
            ASSERT_LOG(geometricScalingFactor > 0.0 && geometricScalingFactor < 1.0, "'geometricScalingFactor' not in range ]0,1[");
        }
    }
    else if (!useReferenceNegativeStills)
        ASSERT_WARN(!useGeometricNegativeStills, "'useGeometricNegativeStills' will be ignored since 'useReferenceNegativeStills' is disabled");

    ASSERT_LOG(face.overlapThreshold >= 0.0 && face.overlapThreshold <= 1.0, "'faceOverlapThreshold' not in range [0,1]");
    ASSERT_LOG(face.scaleFactor > 1.0, "'faceScaleFactor' not greater than 1");
    ASSERT_LOG(face.nmsThreshold >= 0, "'faceNmsThreshold' not greater or equal to 0");
    ASSERT_LOG(face.minNeighbours >= 0, "'faceMinNeighbours' not greater or equal to 0");
    ASSERT_LOG(face.minSize.width > 0 && face.minSize.height > 0, "'faceMinSize' not greater than zero");
    ASSERT_LOG(face.confidenceSize.width > 0 && face.confidenceSize.height > 0, "'faceConfidenceSize' not greater than zero");
    ASSERT_LOG(face.maxSize.width > face.minSize.width && face.maxSize.height > face.minSize.height,
               "'faceMaxSize' not greater than 'faceMinSize'");

    if (!useEyesDetection)
        ASSERT_WARN(!useEyeLocalizedPosition, "'useEyeLocalizedPosition' will be ignored since 'useEyesDetection' is disabled");
    else {
        ASSERT_LOG(eyes.overlapThreshold >= 0.0 && eyes.overlapThreshold <= 1.0, "'eyesOverlapThreshold' not in range [0,1]");
        ASSERT_LOG(eyes.scaleFactor > 1.0, "'eyesScaleFactor' not greater than 1");
        ASSERT_LOG(eyes.nmsThreshold >= 0, "'eyesNmsThreshold' not greater or equal to 0");
        ASSERT_LOG(eyes.minNeighbours >= 0, "'eyesMinNeighbours' not greater or equal to 0");
        ASSERT_LOG(eyes.minSize.width > 0 && eyes.minSize.height > 0, "'eyesMinSize' not greater than zero");
        ASSERT_LOG(eyes.maxSize.width > eyes.minSize.width && eyes.maxSize.height > eyes.minSize.height,
                   "'eyesMaxSize' not greater than 'eyesMinSize'");
    }

    ASSERT_LOG(displayWindowW > 0, "'displayWindowW' not greater than 0");
    ASSERT_LOG(displayWindowH > 0, "'displayWindowH' not greater than 0");

    ASSERT_LOG(searchRadius > 0, "'searchRadius' not greater than 0");
    ASSERT_LOG(searchRadius <= displayWindowW && searchRadius <= displayWindowH, "'search' radius not within display window boundaries");
    if (useHungarianMatching)
        ASSERT_LOG(associationTrackThreshold >= 0, "'associationTrackThreshold' not greater or equal to 0");
    ASSERT_LOG(removeThresholdInsideBounds >= 0, "'removeThresholdInsideBounds' not greater or equal to 0");
    ASSERT_LOG(removeThresholdOutsideBounds >= 0, "'removeThresholdOutsideBounds' not greater or equal to 0");
    ASSERT_LOG(removeScoreInsideBounds >= 0, "'removeScoreInsideBounds' not greater or equal to 0");
    ASSERT_LOG(removeScoreOutsideBounds >= 0, "'removeScoreOutsideBounds' not greater or equal to 0");
    ASSERT_LOG(createThreshold >= 0, "'createThreshold' not greater or equal to 0");
    ASSERT_LOG(createScore >= 0, "'createScore' not greater or equal to 0");
    ASSERT_LOG(trackerOverlapThreshold >= 0.0 && trackerOverlapThreshold <= 1.0, "'trackerOverlapThreshold' not in range [0,1]");
    ASSERT_LOG(detectionFrameInterval > 0, "'detectionFrameInterval' not greater than 0");
    if (useLocalSearchROI)
        ASSERT_LOG(bboxSizeMultiplyer > 0.0, "'bboxSizeMultiplyer' not greater than 0");

    ASSERT_LOG(cameraType.isDefined(), "undefined 'cameraType': [" + cameraType.name() + "]");
    ASSERT_LOG(cameraIndex >= 0, "'cameraIndex' not greater or equal to 0");

    ASSERT_LOG(roiOutputSize > 0, "'roiOutputSize' not greater than 0");
    ASSERT_LOG(roiThickness > 0, "'roiThickness' not greater than 0");
    ASSERT_LOG(roiThicknessOld > 0, "'roiThicknessOld' not greater than 0");

    if (displayPlots) {
        ASSERT_LOG(plotAccumulationPoints > 0, "'plotAccumulationPoints' not greater than 0");
        ASSERT_LOG(plotFigureWidth > 0, "'plotFigureWidth' not greater than 0");
        ASSERT_LOG(plotFigureHeight > 0, "'plotFigureHeight' not greater than 0");
        ASSERT_LOG(plotTrackDirection == 0 || plotTrackDirection == 1, "'plotTrackDirection' unknown value");
        ASSERT_LOG(plotMaxTracks > 0, "'plotMaxTracks' not greater than 0");
        ASSERT_LOG(plotMaxPOI > 0, "'plotMaxPOI' not greater than 0");
    }

    ASSERT_LOG((FastDT ^ FRCNN ^ VJ ^ LBP ^ ImprovedLBP ^ YOLO) || !(FastDT & FRCNN & VJ & LBP & ImprovedLBP & YOLO),
               "Only one face detectors can be used at the same time!");
    ASSERT_LOG((FastDT ^ camshift ^ compressive) || !(FastDT & camshift & compressive),
               "Only one face trackers can be used at the same time!");
    ASSERT_LOG((ESVM ^ FaceNet ^ TM) || !(ESVM & FaceNet & TM),
               "Only one face classifiers can be used at the same time!");

    if (FastDT) {
        ASSERT_LOG(features.size() > 0, "'features' employed with 'FastDT' face detector not greater than 0");
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

    ASSERT_LOG(bfs::is_directory(NEGDir), "Negative samples file directory not found [" + NEGDir + "]");
    ASSERT_LOG(bfs::is_directory(POIDir), "Person of Interest ROI directory not found [" + POIDir + "]");
    ASSERT_LOG(roiAccumulationSize > 0, "ROI and score accumulation size not greater than 0");
    if (roiAccumulationMode != ScoreMode::CUMUL) {
        ASSERT_LOG(thresholdFaceConsidered >= 0 && thresholdFaceConsidered <= 1, "'thresholdFaceConsidered' not in range [0,1]");
        ASSERT_LOG(thresholdFaceRecognized >= 0 && thresholdFaceRecognized <= 1, "'thresholdFaceRecognized' not in range [0,1]");
    }
    else {
        ASSERT_LOG(thresholdFaceConsidered >= 0 && thresholdFaceConsidered <= roiAccumulationSize, "'thresholdFaceConsidered' not in range [0,size]");
        ASSERT_LOG(thresholdFaceRecognized >= 0 && thresholdFaceRecognized <= roiAccumulationSize, "'thresholdFaceRecognized' not in range [0,size]");
    }
    ASSERT_LOG(thresholdFaceRecognized > thresholdFaceConsidered, "'thresholdFaceRecognized' not greater than 'thresholdFaceConsidered'");
    ASSERT_LOG(getClassifierType().isDefined(), "undefined 'classifierType': [" + getClassifierType().name() + "]");
}

} // end namespace config
