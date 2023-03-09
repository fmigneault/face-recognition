#ifndef FACE_RECOG_CONFIG_FILE_H
#define FACE_RECOG_CONFIG_FILE_H

#include <vector>
#include <string>
#include <ostream>
#include <opencv/cv.h>

#include "Camera/CameraType.h"
#include "Classifiers/ClassifierType.h"
#include "Tracks/CircularBuffer.h"

namespace config {
class ConfigFile
{
public:

    /* ============
        configs
    ============ */

    enum FeatureType
    {
        kFeatureTypeHaar,
        kFeatureTypeRaw,
        kFeatureTypeHistogram
    };

    enum KernelType
    {
        kKernelTypeLinear,
        kKernelTypeGaussian,
        kKernelTypeIntersection,
        kKernelTypeChi2
    };

    class FeatureKernelPair
    {
    public:
        FeatureType feature;
        KernelType kernel;
        std::vector<double> params;
        bool isValid();
    };

    struct DetectorParameters
    {
        double overlapThreshold;
        double scaleFactor;
        int nmsThreshold;
        int minNeighbours;
        cv::Size minSize;
        cv::Size maxSize;
        cv::Size confidenceSize;
    };

    // tracking parameters
    int                             seed;
    int                             searchRadius;
    double                          svmC;
    int                             svmBudgetSize;
    std::vector<FeatureKernelPair>  features;

    // detector parameters
    DetectorParameters face;
    DetectorParameters eyes;
    bool useEyesDetection;
    bool useEyeLocalizedPosition;

    // device parameters
    int deviceIndex;

    // camera parameters
    int cameraIndex;
    CameraType cameraType;
    bool useCameraTrigger;

    // face detectors and trackers
    bool FastDT;
    bool FRCNN;
    bool VJ;
    bool LBP;
    bool ImprovedLBP;
    bool YOLO;
    bool camshift;
    bool compressive;
    bool ESVM;
    bool FaceNet;
    bool TM;

    // face recognition
    bool useFaceRecognition;
    bool useGeometricPositiveStills;
    bool useSyntheticPositiveStills;
    bool useReferenceNegativeStills;
    bool useGeometricNegativeStills;
    int geometricTranslatePixels;    
    int geometricScalingMinSize;
    double geometricScalingFactor;
    std::string NEGDirExtra;
    std::string NEGDir;
    std::string POIDir;
    double thresholdFaceConsidered;
    double thresholdFaceRecognized;
    int roiAccumulationSize;
    ScoreMode roiAccumulationMode;

    // tracker association parameters
    bool useHungarianMatching;
    int associationTrackThreshold;

    // tracker creation and removal parameters
    int removeThresholdInsideBounds;
    int removeThresholdOutsideBounds;
    double removeScoreInsideBounds;
    double removeScoreOutsideBounds;
    int createThreshold;
    double createScore;
    double trackerOverlapThreshold;
    int detectionAugmentationOffset;
    int detectionFrameInterval;

    // localized ROI search
    bool useLocalSearchROI;
    bool use3CascadesLocalSearch;
    double bboxSizeMultiplyer;

    // output debug parameters
    bool verboseDebug;
    bool outputDebug;
    bool outputFrames;
    bool outputROI;
    bool outputLocalROI;
    int roiOutputSize;
    bool outputDirsClearOnStart;
    bool flipFrames;
    bool displayFrames;
    bool displayFrameRate;
    bool displayFrameNumber;
    bool displaySequenceTrackID;    
    int displayWindowX;
    int displayWindowY;
    int displayWindowW;
    int displayWindowH;
    bool displayOldROI;
    int roiColorMode;
    int roiThickness;
    int roiThicknessOld;

    // plot display
    bool displayPlots;
    int plotAccumulationPoints;
    int plotFigureWidth;
    int plotFigureHeight;
    int plotTrackDirection;
    int plotMaxTracks;
    int plotMaxPOI;
    bool plotResetOnTrackLost;

    /* ============
        methods
    ============ */

    ConfigFile() { setDefaults(); }
    ConfigFile(const std::string& path);
    std::string display() const;
    ClassifierType getClassifierType() const;

    friend std::ostream& operator<< (std::ostream& out, const ConfigFile& conf);

private:
    void setDefaults();
    void validateValues();
    static std::string FeatureName(FeatureType f);
    static std::string KernelName(KernelType k);
};
} // end namespace config

#endif /*FACE_RECOG_CONFIG_FILE_H*/
