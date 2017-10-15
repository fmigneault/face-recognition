#ifndef FACE_RECOG_FLYCAPTURE2_UTILITIES_H
#define FACE_RECOG_FLYCAPTURE2_UTILITIES_H

/* Try to find if the FlyCapture2 SDK is available */
#include "CameraDefines.h"
#include "FlyCapture2Define.h"

/* Include functions if the SDK is found */
#if FACE_RECOG_HAS_FLYCAPTURE2
#include <string>
#include "opencv\cv.h"

bool EvaluateAndPrintCameraError(FlyCapture2::Error cameraError, std::string errorMessage);
FlyCapture2::ErrorType ConnectCameraPGR(FlyCapture2::Camera* camera, int cameraIndex = 0,
                                        bool useTrigger = false, FlyCapture2::Format7ImageSettings* frameConfig = nullptr);
FlyCapture2::ErrorType SetupCameraTrigger(FlyCapture2::Camera* camera, bool useTrigger);
bool PollForTriggerReady(FlyCapture2::Camera* camera, bool verbose = false);
bool FireSoftwareTrigger(FlyCapture2::Camera* camera, bool verbose = false);
bool FireTriggerWhenReady(FlyCapture2::Camera* camera, bool verbose = false);
void ConvertRGBImagePGR2CV(FlyCapture2::Image src, cv::OutputArray dst);

#endif /*FACE_RECOG_HAS_FLYCAPTURE2*/

#endif /*FACE_RECOG_FLYCAPTURE2_UTILITIES_H*/
