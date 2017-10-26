/*
    Forward declarations of classes defined and employed by FaceRecog

    This header allows inclusion of all FaceRecog module headers in any order
    by pre-declaring not yet defined required dependencies of any given class.
*/
#ifndef FACE_RECOG_FORWARD_DECLARES_H
#define FACE_RECOG_FORWARD_DECLARES_H

// Configurations & Generic
class CameraType;
class ConfigFile;

// Tracks
class Association;
class CircularBuffer;
class HaarFeature;
class HaarFeatures;
class ImageRep;
class Kernels;
class LaRank;
class ROI;
class MultiSample;
class Sample;
class Sampler;
class Track;
class TrackROI;

// Face Detection
class DetectorType;
class IDetector;
class EyeDetector;
#if FACE_RECOG_HAS_FRCNN
class FaceDetectorFRCNN;
#endif/*FACE_RECOG_HAS_FRCNN*/
#if FACE_RECOG_HAS_SSD
class FaceDetectorSSD;
#endif/*FACE_RECOG_HAS_SSD*/
#if FACE_RECOG_HAS_VJ
class FaceDetectorVJ;
#endif/*FACE_RECOG_HAS_VJ*/
#if FACE_RECOG_HAS_YOLO
class FaceDetectorYOLO;
#endif/*FACE_RECOG_HAS_YOLO*/

// Face Tracking
class ITracker;
#if FACE_RECOG_HAS_CAMSHIFT
class TrackerCamshift;
#endif/*FACE_RECOG_HAS_CAMSHIFT*/
#if FACE_RECOG_HAS_COMPRESSIVE
class TrackerCompressive;
#endif/*FACE_RECOG_HAS_COMPRESSIVE*/
#if FACE_RECOG_HAS_KCF
class TrackerKCF;
#endif/*FACE_RECOG_HAS_KCF*/
#if FACE_RECOG_HAS_STRUCK
class TrackerSTRUCK;
#endif/*FACE_RECOG_HAS_STRUCK*/

// Face Recognition
class ClassifierType;
class IClassifier;
#if FACE_RECOG_HAS_ESVM
class ClassifierEnsembleESVM;
#endif/*FACE_RECOG_HAS_ESVM*/
#if FACE_RECOG_HAS_FACE_NET
class ClassifierEnsembleFaceNet;
#endif/*FACE_RECOG_HAS_FACE_NET*/
#if FACE_RECOG_HAS_TM
class TemplateMatcher;
class ClassifierEnsembleTM;
#endif/*FACE_RECOG_HAS_TM*/

#endif/*FACE_RECOG_FORWARD_DECLARES_H*/
