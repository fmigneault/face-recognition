#ifndef FACE_RECOG_H
#define FACE_RECOG_H

// FaceRecog Dependencies and Utilities
#include "Utilities/Common.h"
#include "Utilities/Macros.h"
#include "Utilities/ForwardDeclares.h"
#include "Utilities/MatDefines.h"
#include "Utilities/MultiColorType.h"
#include "Utilities/Utilities.h"

// FaceRecog Configs
#include "Configs/Platform.h"
#include "Configs/ConfigFile.h"
#include "Configs/ConsoleOptions.h"
#include "Configs/Version.h"

// FaceRecog Camera
#include "Camera/CameraDefines.h"
#include "Camera/CameraType.h"

// FaceRecog Tracking Containers
#include "Tracks/Association.h"
#include "Tracks/CircularBuffer.h"
#include "Tracks/HaarFeature.h"
#include "Tracks/HaarFeatures.h"
#include "Tracks/Hungarian.h"
#include "Tracks/ImageRep.h"
#include "Tracks/Kernels.h"
#include "Tracks/LaRank.h"
#include "Tracks/Rect.h"
#include "Tracks/Sample.h"
#include "Tracks/Sampler.h"
#include "Tracks/TrackROI.h"
#include "Tracks/Track.h"

// FaceRecog Face Detectors
#include "Detectors/DetectorType.h"
#include "Detectors/IDetector.h"
#ifdef FACE_RECOG_HAS_VJ
#include "Detectors/EyeDetector.h"
#include "Detectors/FaceDetectorVJ.h"
#endif/*FACE_RECOG_HAS_VJ*/
#ifdef FACE_RECOG_HAS_FRCNN
    #ifndef FACE_RECOG_HAS_PYTHON
    #error Python is required to employ FRCNN
    #endif/*FACE_RECOG_HAS_PYTHON*/
    #include "Detectors/FaceDetectorFRCNN.h"
#endif/*FACE_RECOG_HAS_FRCNN*/
#ifdef FACE_RECOG_HAS_SSD
#include "Detectors/FaceDetectorSSD.h"
#endif/*FACE_RECOG_HAS_SSD*/
#ifdef FACE_RECOG_HAS_YOLO
#include "Detectors/FaceDetectorYOLO.h"
#endif/*FACE_RECOG_HAS_YOLO*/

// FaceRecog Trackers
#include "Trackers/ITracker.h"
#ifdef FACE_RECOG_HAS_CAMSHIFT
#include "Trackers/TrackerCamshift.h"
#endif/*FACE_RECOG_HAS_CAMSHIFT*/
#ifdef FACE_RECOG_HAS_COMPRESSIVE
#include "Trackers/TrackerCompressive.h"
#endif/*FACE_RECOG_HAS_COMPRESSIVE*/
#ifdef FACE_RECOG_HAS_KCF
#include "Trackers/TrackerKCF.h"
#endif/*FACE_RECOG_HAS_KCF*/
#ifdef FACE_RECOG_HAS_STRUCK
#include "Trackers/TrackerSTRUCK.h"
#endif/*FACE_RECOG_HAS_STRUCK*/

// FaceRecog Classifiers
#include "Classifiers/ClassifierType.h"
#include "Classifiers/IClassifier.h"
#ifdef FACE_RECOG_HAS_ESVM
#include "Classifiers/ClassifierEnsembleESVM.h"
#endif/*FACE_RECOG_HAS_ESVM*/
#ifdef FACE_RECOG_HAS_FACE_NET
    #ifndef FACE_RECOG_HAS_PYTHON
    #error Python is required to employ FaceNet
    #endif/*FACE_RECOG_HAS_PYTHON*/
    #include "Classifiers/ClassifierFaceNet.h"
#endif/*FACE_RECOG_HAS_FACE_NET*/
#if FACE_RECOG_HAS_TM
#include "Classifiers/ClassifierEnsembleTM.h"
#endif/*FACE_RECOG_HAS_TM*/

#endif/*FACE_RECOG_H*/
