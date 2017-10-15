#include "Camera/FlyCapture2Utilities.h"
/* Include functions if the SDK is found
 *
 * References:
 *   Sample PGR camera capture for OpenCV
 *      <https://gist.github.com/kevinhughes27/5543668>
 *   Available trigger modes:
 *      <https://www.ptgrey.com/KB/10250>
 *   Code sample for trigger mdoe:
 *      <http://www.technical-recipes.com/2011/integrating-the-flycapture-sdk-for-use-with-opencv/2/> */
#if FACE_RECOG_HAS_FLYCAPTURE2

#include <string>
#include <iostream>
using namespace std;

#include <opencv2/opencv.hpp>
using namespace cv;

const unsigned int SOFTWARE_TRIGGER_REGISTER_ADDRESS = 0x62C;
const unsigned int TRIGGER_FIRE_VALUE = 0x80000000;

bool EvaluateAndPrintCameraError(FlyCapture2::Error cameraError, string errorMessage)
{
    if (cameraError != FlyCapture2::PGRERROR_OK)
    {
        cout << (errorMessage == "" ? "Unspecified PGR camera error" : errorMessage) << endl
             << "  Description: " << cameraError.GetDescription() << endl << "  ";
        cameraError.PrintErrorTrace();
        return true;
    }
    return false;
}

FlyCapture2::ErrorType ConnectCameraPGR(FlyCapture2::Camera* camera, int cameraIndex,
                                        bool useTrigger, FlyCapture2::Format7ImageSettings* frameConfig)
{
    if (camera == nullptr || cameraIndex < 0)
        return FlyCapture2::PGRERROR_INVALID_PARAMETER;

    FlyCapture2::BusManager busMgr;
    FlyCapture2::PGRGuid pgrGuid;
    FlyCapture2::CameraInfo cameraInfo;
    FlyCapture2::Error cameraError;
    unsigned int numCameras = 0;

    cameraError = busMgr.GetNumOfCameras(&numCameras);
    if (EvaluateAndPrintCameraError(cameraError, "Failed to retrieve camera quantity"))
        return cameraError.GetType();
    if (numCameras <= 0)
    {
        cout << "No PGR camera detected, cannot establish connection." << endl;
        return FlyCapture2::PGRERROR_NOT_FOUND;
    }

    cameraError = busMgr.GetCameraFromIndex(cameraIndex, &pgrGuid);
    if (EvaluateAndPrintCameraError(cameraError, "Failed to retrieve camera with index"))
        return cameraError.GetType();

    cameraError = camera->Connect(&pgrGuid);
    if (EvaluateAndPrintCameraError(cameraError, "Failed to connect PGR camera"))
        return cameraError.GetType();

    cameraError = camera->Connect(&pgrGuid);
    if (EvaluateAndPrintCameraError(cameraError, "Failed to connect PGR camera"))
        return cameraError.GetType();
    cout << "PGR Camera connect:         Success" << endl;

    cameraError = camera->GetCameraInfo(&cameraInfo);
    if (EvaluateAndPrintCameraError(cameraError, "Failed to get PGR camera info"))
        return cameraError.GetType();
    cout << "PGR Camera get info:        Success" << endl;

    FlyCapture2::ErrorType triggerError = SetupCameraTrigger(camera, useTrigger);
    if (triggerError != FlyCapture2::PGRERROR_OK)
        return triggerError;

    if (frameConfig != nullptr)
    {
        FlyCapture2::Format7PacketInfo pckInfo;
        bool valid;
        cameraError = camera->ValidateFormat7Settings(frameConfig, &valid, &pckInfo);
        if (EvaluateAndPrintCameraError(cameraError, "Failed to validate PGR camera frame format settings"))
            return cameraError.GetType();
        else if (!valid)
        {
            cout << "Invalid frame format settings specified for PGR camera" << endl;
            return FlyCapture2::PGRERROR_INVALID_SETTINGS;
        }

        unsigned int packetByteSize = pckInfo.recommendedBytesPerPacket;
        cameraError = camera->SetFormat7Configuration(frameConfig, packetByteSize);
        if (EvaluateAndPrintCameraError(cameraError, "Failed to apply PGR camera frame format settings"))
            return cameraError.GetType();
    }

    cameraError = camera->StartCapture();
    if (EvaluateAndPrintCameraError(cameraError, "Failed to start PGR camera capture"))
        return cameraError.GetType();
    cout << "PGR camera start capture:   Success" << endl;

    // Return PGR camera ready to capture
    return FlyCapture2::PGRERROR_OK;
}

FlyCapture2::ErrorType SetupCameraTrigger(FlyCapture2::Camera* camera, bool useTrigger)
{
    FlyCapture2::TriggerModeInfo triggerModeInfo;
    FlyCapture2::Error cameraError = camera->GetTriggerModeInfo(&triggerModeInfo);
    if (EvaluateAndPrintCameraError(cameraError, "Failed to retrive PGR camera trigger info"))
        return cameraError.GetType();
    else if ((!triggerModeInfo.present || !triggerModeInfo.softwareTriggerSupported) && useTrigger)
    {
        cout << "External software trigger mode not supported by PGR camera" << endl;
        return FlyCapture2::PGRERROR_INVALID_SETTINGS;
    }
    else
    {
        FlyCapture2::TriggerMode triggerMode;
        triggerMode.onOff = useTrigger;
        triggerMode.mode = 0;       // Standard External Trigger
        triggerMode.source = 7;     // Software Trigger
        triggerMode.parameter = 0;

        // Set trigger mode and verify that the camera is ready
        cameraError = camera->SetTriggerMode(&triggerMode);
        if (EvaluateAndPrintCameraError(cameraError, "Failed to set PGR camera trigger mode"))
            return cameraError.GetType();
        if (!useTrigger)
            cout << "PGR camera without trigger: Success" << endl;
        else if (PollForTriggerReady(camera, false))                // Validation read register
            cout << "PGR camera trigger ready:   Success" << endl;
        else
            return FlyCapture2::PGRERROR_READ_REGISTER_FAILED;
    }
    return FlyCapture2::PGRERROR_OK;
}

bool PollForTriggerReady(FlyCapture2::Camera* camera, bool verbose)
{
    FlyCapture2::Error cameraError;
    unsigned int registerValue = 0;
    unsigned int pollCount = 0;

    // Wait for trigger ready register
    do
    {
        cameraError = camera->ReadRegister(SOFTWARE_TRIGGER_REGISTER_ADDRESS, &registerValue);
        if (EvaluateAndPrintCameraError(cameraError, "Problem occurred while reading PGR camera register"))
            return false;
        pollCount++;
    } while ((registerValue >> 31) != 0);

    if (verbose)
        cout << "Trigger ready poll count: " << pollCount << endl;
    return true;
}

bool FireSoftwareTrigger(FlyCapture2::Camera* camera, bool verbose)
{
    if (verbose)
        cout << "Fire software trigger" << endl;

    FlyCapture2::Error cameraError = camera->WriteRegister(SOFTWARE_TRIGGER_REGISTER_ADDRESS, TRIGGER_FIRE_VALUE);
    return !EvaluateAndPrintCameraError(cameraError, "Failed to write register for PGR camera trigger");
}

bool FireTriggerWhenReady(FlyCapture2::Camera* camera, bool verbose)
{
    bool ready = PollForTriggerReady(camera, verbose);
    if (verbose)
        cout << (ready ? "Trigger check ready" : "Trigger check failure") << endl;
    if (!ready)
        return false;

    ready = FireSoftwareTrigger(camera, verbose);
    if (verbose)
        cout << (ready ? "Trigger fire success" : "Trigger fire failure") << endl;
    if (!ready)
        return false;

    return true;
}

void ConvertRGBImagePGR2CV(FlyCapture2::Image src, cv::OutputArray dst)
{
    FlyCapture2::Image rgb; // Buffer for converting the PGR Image

    // convert to OpenCV Mat
    src.Convert(FlyCapture2::PIXEL_FORMAT_BGR, &rgb);
    unsigned int pgrRows = rgb.GetRows();
    unsigned int pgrCols = rgb.GetCols();
    unsigned int rowBytes = (int)((double)rgb.GetReceivedDataSize() / (double)pgrRows);
    dst.assign(cv::Mat(pgrRows, pgrCols, CV_8UC3, rgb.GetData(), rowBytes));
}

#endif /*FACE_RECOG_HAS_FLYCAPTURE2*/
