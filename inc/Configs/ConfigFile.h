#ifndef FACE_RECOG_CONFIG_FILE_H
#define FACE_RECOG_CONFIG_FILE_H

#include "Utilities/Common.h"
#include "Camera/CameraType.h"
#include "Classifiers/ClassifierType.h"
#include "Tracks/CircularBuffer.h"

class ConfigFile
{
public:

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

    // face detection
    bool FRCNN;
    bool HaarCascadeFrontal;
    bool HaarCascadeProfile;
    bool LBPCascadeFrontal;
    bool LBPCascadeProfile;
    bool LBPCascadeFrontalImproved;
    bool SSD;
    bool YOLO;

    // face tracking
    bool Camshift;
    bool Compressive;
    bool KCF;
    bool STRUCK;

    // face recognition
    bool ESVM;
    bool FaceNet;
    bool TM;

    // face recognition parameters
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
    CircularBuffer::ScoreMode roiAccumulationMode;
    bool modelsFileSave;
    std::string modelsFileDir;

    // tracker association parameters
    bool useHungarianMatching;
    int associationTrackThreshold;

    // tracker creation and removal parameters
    int removeTrackCountThresholdInBounds;
    int removeTrackCountThresholdOutBounds;
    double removeTrackConfidenceInBounds;
    double removeTrackConfidenceOutBounds;
    int createTrackCountThreshold;
    double createTrackConfidenceThreshold;
    double trackerOverlapThreshold;
    int detectionAugmentationOffset;
    int detectionFrameInterval;

    // localized ROI search
    bool useLocalSearchROI;
    bool use3CascadesLocalSearch;
    double bboxSizeMultiplyer;

    // output debug parameters
    bool verboseConfig;
    bool verboseDevices;
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
    bool requireAnyCascade() const;
    friend std::ostream& operator<<(std::ostream& out, const ConfigFile& conf);

private:
    void setDefaults();
    void validateValues();
    static std::string FeatureName(FeatureType f);
    static std::string KernelName(KernelType k);
};

#endif /*FACE_RECOG_CONFIG_FILE_H*/
