/*
    On-line Face Detection, Tracking and Recognition

    Includes:
        - Viola-Jones: face/eye detectors (Haar/LBP)
        - FAST-DT: face detector+tracker (FAce STructured Detection and Tracking)
        - Camshift: tracker
        - Compressive Tracking: tracker
        - FRCNN: Faster R-CNN face detection (Faster Region-based Convolutional Neural Networks)
        - KCF: Kernelized Correlation Filters tracker
        - SSD: Single Shot MultiBox Detector (CNN)
        - YOLO: Real-Time Object Detection using CNN
        - EoESVM: Ensemble of Exemplar-SVM face recognition
        - FaceNet: face recognition using deep convolutional neural network
*/

#include "FaceRecog.h"

/* DEBUG */
#define FACE_RECOG_USE_EXCEPTION_LOGGING 1
#define FACE_RECOG_DISABLE_COLOR_CONSOLE 1
#define FACE_RECOG_USE_PSEUDO_INPUT_ARGS 0
#define FACE_RECOG_EXPERIMENTAL_MERGEDET 0

int main(int argc, char *argv[])
{
    #if FACE_RECOG_USE_EXCEPTION_LOGGING
    try {
    #endif/*FACE_RECOG_USE_EXCEPTION_LOGGING*/

    INITIALIZE();

    /********************************************************************************************************************************************/
    /* INPUT ARGUMENTS                                                                                                                          */
    /********************************************************************************************************************************************/

    std::string configPath = "./config.txt";
    std::string outDir = "./output";
    std::string imgDir = "./images";
    std::string resultFilePath = "./results.txt";
    std::string framesPath, testFilePath;
    bool optArgI = false, optArgO = false, optArgP = false, optArgR = false, optArgT = false, optArgV = false;
    int argmin = 2, argmax = 12;

    #if FACE_RECOG_USE_PSEUDO_INPUT_ARGS
    argc = argmax;
    argv = new char*[argc];
    argv[0] = "FaceRecog";
    argv[1] = std::getenv("OpenCV_DIR");    // path to openCV root (try generic env var that should be set if OpenCV was installed)
    argv[2] = "-i";
    argv[3] = "./images";
    argv[4] = "-o";
    argv[5] = "./output";
    argv[6] = "-c";
    argv[7] = "../config.txt";
    argv[8] = "-t";
    argv[9] = "./sequences-path.txt";
    argv[10] = "-r";
    argv[11] = "./results.txt";
    #endif/*FACE_RECOG_SKIP_INPUT_ARGS_CHECK*/

    std::string app = argv[0];
    std::string opencv_root = argv[1];      // path to openCV root (git sources)
    std::string tab = "    ";
    std::string align = tab + string(app.length(), ' ');
    std::string usageMsg = "usage:\n"
        + tab + app + " <opencv_root>\n"
        + align + " [-o <output_frames_directory>] [-i <images_directory>] [-c <config_file_path>]\n"
        + align + " [-p <frames_path_regex>|-t <test_file_path>|-v <video_path>] [-r <result_file_path]\n";
    if (argc < argmin || argc > argmax)
    {
        std::cout << usageMsg;
        FINALIZE(EXIT_FAILURE);
    }
    if (argc > argmin)
    {
        for (int argi = argmin; argi < argc; argi += 2)
        {
            std::string opt = string(argv[argi]);
            if (opt == "-c")
            {
                configPath = std::string(argv[argi + 1]);
                if (!bfs::is_regular_file(configPath)) {
                    std::cout << usageMsg;
                    FINALIZE(EXIT_FAILURE);
                }
            }
            else if (opt == "-i")
            {
                imgDir = std::string(argv[argi + 1]);
                optArgI = true;
            }
            else if (opt == "-o")
            {
                outDir = std::string(argv[argi + 1]);
                optArgO = true;
            }
            else if (opt == "-p" && !optArgT)   // ignore '-p' if specified and '-t' already processed
            {
                optArgR = true;                 // default enable if not specified
                optArgP = true;
                optArgV = false;                // override '-v' if '-p' specified and '-v' already processed
                framesPath = std::string(argv[argi + 1]);
            }
            else if (opt == "-r")
            {
                optArgR = true;
                resultFilePath = std::string(argv[argi + 1]);
            }
            else if (opt == "-t")
            {
                optArgR = true;                 // default enable if not specified
                optArgT = true;
                optArgP = false;                // override '-p' if '-t' specified and '-p' already processed
                optArgV = false;                // override '-v' if '-t' specified and '-v' already processed
                testFilePath = std::string(argv[argi + 1]);
            }
            else if (opt == "-v" && !optArgP && !optArgT)   // ignore '-v' if already specified '-p' or '-t'
            {
                optArgV = true;
                framesPath = std::string(argv[argi + 1]);
            }
            else
            {
                std::cout << "Unknown option argument encountered: '" << opt << "'" << std::endl << usageMsg;
                FINALIZE(EXIT_FAILURE);
            }
        }
    }
    if (optArgR && !(optArgP || optArgT || optArgV)) {
        ASSERT_WARN(false, "Ignoring '-r' option since (-p|-t|-v) option was detected");
        optArgR = false;                        // disable '-r' if not in a possible testing case (not live-feed)
    }

    // provide here configuration file
    static ConfigFile *conf = new ConfigFile(configPath);
    ASSERT_LOG(conf, "ConfigFile reference invalid");

    if (conf->outputDirsClearOnStart && bfs::is_directory(outDir))
        bfs::remove_all(outDir);
    if (optArgO)
        ASSERT_LOG(bfs::create_directory(outDir), "Failed to generate required output directory");
    if (conf->outputDirsClearOnStart && bfs::is_directory(imgDir))
        bfs::remove_all(imgDir);
    if (optArgI)
        ASSERT_LOG(bfs::create_directory(imgDir), "Failed to generate required image directory");

    std::string imgDirLocal = imgDir + "_local";
    if (conf->outputDirsClearOnStart && bfs::is_directory(imgDirLocal))
        bfs::remove_all(imgDirLocal);
    if (conf->outputLocalROI)
        bfs::create_directory(imgDirLocal);

    /********************************************************************************************************************************************/
    /* OUTPUT FILES AND LOGGING                                                                                                                 */
    /********************************************************************************************************************************************/

    // Loggers initialization
    std::string logOutputFilePath = "./output.txt";
    std::string logTimingFilePath = "./timing.txt";
    std::string logOutBBoxFilePath = "./outputBboxes.txt";
    std::string logDebugFilePath = "./debug.txt";
    bfs::remove(logOutputFilePath);
    bfs::remove(logTimingFilePath);
    bfs::remove(logOutBBoxFilePath);
    bfs::remove(logDebugFilePath);
    logstream logOutput(logOutputFilePath, true, true);
    ASSERT_LOG_FINALIZE(!resultFilePath.empty(), "Option '-r' result file path cannot be empty", logOutput, EXIT_FAILURE);
    logstream logResult(resultFilePath, false, optArgR);
    FACE_RECOG_DEBUG(
        logstream logOutBBox(logOutBBoxFilePath, false, true);
        logstream logTiming(logTimingFilePath, conf->verboseDebug, conf->outputDebug);
        logstream logDebug(logDebugFilePath, conf->verboseDebug, conf->outputDebug);
    );

    if (conf->verboseConfig) {
        logOutput << *conf;     // output/display configs read from file
        logOutput.ofss.flush(); // wait for output completion before moving on (many configs, sometime not enough time to complete before next calls)
    }

    /********************************************************************************************************************************************/
    /* COMMON PATHS                                                                                                                             */
    /********************************************************************************************************************************************/

    bfs::path opencvRootPath(opencv_root);
    bfs::path opencvSourceDataPath = opencvRootPath / bfs::path("data/");
    if (!bfs::is_directory(opencvSourceDataPath))   // if not found under the default 'git source' dir structure, try the 'installed' one
        opencvSourceDataPath = opencvRootPath / bfs::path("etc/");
    std::string opencvSourceDataPathStr = opencvSourceDataPath.generic_string();
    #ifdef FACE_RECOG_HAS_VJ
    bfs::path lbpCascadesDir = opencvSourceDataPath / bfs::path("lbpcascades/");
    bfs::path haarCascadesDir = opencvSourceDataPath / bfs::path("haarcascades/");
    ASSERT_LOG_FINALIZE(bfs::is_directory(opencvSourceDataPath) && bfs::is_directory(lbpCascadesDir) && bfs::is_directory(haarCascadesDir),
                        "OpenCV source data directory not found to access 'CascadeClassifier' models", logOutput, EXIT_FAILURE);
    #endif/*FACE_RECOG_HAS_VJ*/

    /********************************************************************************************************************************************/
    /* CLASSIFIER                                                                                                                               */
    /********************************************************************************************************************************************/

    bfs::path POI_rootDir(conf->POIDir);
    std::vector<std::string> POI_IDs;
    std::vector<std::vector<FACE_RECOG_MAT> > POI_ROIs, NEG_ROIs;
    if (conf->useFaceRecognition) {
        ASSERT_LOG_FINALIZE(util::prepareEnrollROIs(*conf, opencvSourceDataPath, POI_ROIs, POI_IDs, NEG_ROIs, logOutput) == EXIT_SUCCESS,
                            "Error on POI loading", logOutput, EXIT_FAILURE);
        ASSERT_LOG_FINALIZE(POI_ROIs.size() > 0, "Can't execute face recognition without any Person of Interest (POI)", logOutput, EXIT_FAILURE);
    }
    const size_t targetCount = POI_IDs.size();

    FACE_RECOG_DEBUG(
        std::vector<double> minScores(targetCount,  DBL_MAX);
        std::vector<double> maxScores(targetCount, -DBL_MAX);
    );

    // apply classifier according to config
    std::shared_ptr<IClassifier> classifier;
    CircularBuffer accScores(conf->roiAccumulationSize);
    if (conf->useFaceRecognition) {
        logOutput << "Training face recognition classifiers..." << std::endl;
        classifier = buildSpecializedClassifier(*conf, POI_ROIs, POI_IDs, NEG_ROIs);
        if (classifier == nullptr) {
            logOutput << "Classifier not properly initialized!" << std::endl;
            logOutput << "    Verify that required sources and defines are provided for requested classifier type in 'config'" << std::endl;
            logOutput << "    specified type:    [" << conf->getClassifierType().name() << "]" << std::endl;
            logOutput << "    evaluated address: [" << classifier << "]" << std::endl;
            FINALIZE(EXIT_FAILURE);
        }
        logOutput << "Face recognition classifiers training complete" << std::endl;
    }
    POI_ROIs.clear();
    NEG_ROIs.clear();

    /********************************************************************************************************************************************/
    /* OUTPUT AND TIMERS                                                                                                                        */
    /********************************************************************************************************************************************/

    // Display all available devices, activate OpenCL and set device index to use
    #if CV_VERSION_MAJOR == 3
    if (conf->verboseDevices)
        util::setAndDisplayDevices(conf->deviceIndex, logOutput.ofss);
    #endif

    // Handle for colored console text in Windows
    #if defined(CONSOLE_COLOR_ENABLED) && !FACE_RECOG_DISABLE_COLOR_CONSOLE
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    #endif

    // processing time and counters
    TP frameTime = getTimeNowPrecise(), frameTimePrev = frameTime;
    double deltaTime = 0, sumTimeDetect = 0, sumTimeTrack = 0, sumTimeDetectLocal = 0;
    int totalFramesDetect = 0, totalFramesDetectLocal = 0;
    int frameCounter = 0;
    int sequenceCounter = 0;
    std::string currentFrameLabel;

    // create a window for display
    std::string windowName = "FaceRecog - Live Video";
    cv::namedWindow(windowName, cv::WINDOW_NORMAL);
    cv::moveWindow(windowName, conf->displayWindowX, conf->displayWindowY);

    // output bbox colors
    MultiColorType bboxColors = MultiColorType(conf->roiColorMode);
    ColorCode bboxColorNotMatched = bboxColors.getColorCode(0);
    ColorCode bboxColorConsidered = bboxColors.getColorCode(1);
    ColorCode bboxColorRecognized = bboxColors.getColorCode(2);

    // create a window for plotting scores
    std::string plotFigureName = "FaceRecog - Track Average Scores";
    std::string plotSingleName = "Track 1 - POI 1 - Plot";      //////////////////////////////////// DEBUG
    std::string plotScoresName = "Track 1 - POI 1 - Scores";    //////////////////////////////////// DEBUG
    xstd::mvector<2, cv::Mat> plotData;                         // [track][poi] accumulated scores
    xstd::mvector<2, cv::Ptr<cv::plot::Plot2d> > plotsPtr;      // [track][poi] display plots
    FACE_RECOG_MAT plotFigure, subPlot;                         // render all 'subplots' display in a common 'figure' window
    std::vector<int> plotTrackID;                               // memory of currently displayed track id, employed for resets
    size_t nPlotPOI = MIN(targetCount, conf->plotMaxPOI);
    if (conf->displayPlots && conf->useFaceRecognition)
    {
        size_t plotDims[2]{ (size_t)conf->plotMaxTracks, nPlotPOI };
        plotFigure = FACE_RECOG_MAT(cv::Size(conf->plotFigureWidth, conf->plotFigureHeight), CV_8UC3);
        plotsPtr = xstd::mvector<2, cv::Ptr<cv::plot::Plot2d> >(plotDims);
        plotData = xstd::mvector<2, cv::Mat>(plotDims);
        plotTrackID = std::vector<int>(conf->plotMaxTracks, -1);
        // min/max 'y-axis' inverted to match 'y' pixels increasing downward
        double plotMinY = conf->roiAccumulationMode == CircularBuffer::ScoreMode::CUMUL ? conf->roiAccumulationSize : 1.0;
        double plotMaxY = 0;
        for (size_t trk = 0; trk < conf->plotMaxTracks; ++trk) {
            for (size_t poi = 0; poi < nPlotPOI; ++poi) {
                plotData[trk][poi] = cv::Mat(conf->plotAccumulationPoints, 1, CV_64F);
                #ifdef CV_NEW_PLOT2D_CREATE
                plotsPtr[trk][poi] = cv::plot::Plot2d::create(plotData[trk][poi]);
                #else
                plotsPtr[trk][poi] = cv::plot::createPlot2d(plotData[trk][poi]);
                #endif
                plotsPtr[trk][poi]->setPlotBackgroundColor(rgbColorCode(BLACK));
                plotsPtr[trk][poi]->setPlotGridColor(rgbColorCode(DARK_GRAY));
                plotsPtr[trk][poi]->setMinY(plotMinY);
                plotsPtr[trk][poi]->setMaxY(plotMaxY);
                plotsPtr[trk][poi]->setPlotLineWidth(2);
                ///plotsPtr[t][poi]->setPlotLineColor(CV_RGB(128, 128, 128));
                for (int p = 0; p < conf->plotAccumulationPoints; ++p)
                    plotData[trk][poi].at<double>(p) = 0.0;
            }
        }
        cv::namedWindow(plotFigureName, cv::WINDOW_NORMAL);
        cv::namedWindow(plotSingleName, cv::WINDOW_NORMAL);     //////////////////////////////////// DEBUG
        cv::namedWindow(plotScoresName, cv::WINDOW_NORMAL);     //////////////////////////////////// DEBUG
    }

    /********************************************************************************************************************************************/
    /* FRAME AND TEST SEQUENCE INFORMATION                                                                                                      */
    /********************************************************************************************************************************************/

    std::vector<std::vector<std::string> > testSequenceFileNames;   // frame image names for every sequence
    std::vector<std::string> testSequenceRegexPaths;                // path + regex to frame image names
    std::string sequenceTrackID = "";                               // current running sequence

    // prepare sequence file names
    if (optArgP)
    {
        logOutput << "Preparing sequence test files. This might take some time..." << std::endl;
        testSequenceFileNames.push_back(std::vector<std::string>());
        testSequenceRegexPaths.push_back(framesPath);
        ASSERT_LOG_FINALIZE(util::prepareFrameFileNames(testSequenceFileNames[0], framesPath),
                            "Failed to prepare file names!", logOutput, EXIT_FAILURE);
        logOutput << "Sequence test files preparation complete" << std::endl;
        FACE_RECOG_DEBUG(
            logDebug << "[optArgP] Test sequence [0] example frame names:" << std::endl;
            for (size_t i = 0; i < testSequenceFileNames[0].size(); ++i)
                logDebug << "  " << testSequenceFileNames[0][i] << std::endl;
        );
    }

    // prepare multiple test sequences from single test specification file
    // each line of test specification file should correspond to an individual path as if calling 'FaceRecog' with a single sequence
    if (optArgT)
    {
        logOutput << "Preparing test sequence files. This might take some time..." << std::endl;
        ASSERT_LOG_FINALIZE(util::prepareTestSequences(testSequenceFileNames, testSequenceRegexPaths, testFilePath),
                            "Failed to prepare test sequence file names!", logOutput, EXIT_FAILURE);
        framesPath = testSequenceRegexPaths[0]; // first test sequence for initial VideoCapture open
        sequenceTrackID = bfs::path(framesPath).remove_filename().filename().string();
        FACE_RECOG_DEBUG(
            size_t nTestSequences = testSequenceRegexPaths.size();
            logDebug << "[optArgT] Test sequences file path: " << testFilePath << std::endl;
            logDebug << "[optArgT] Test sequences [0] path: " << framesPath << std::endl;
            logDebug << "[optArgT] Test sequences regex loaded: " << nTestSequences << std::endl;
            logDebug << "[optArgT] Test sequences paths loaded: " << testSequenceFileNames.size() << std::endl;
            for (size_t i = 0; i < nTestSequences; ++i)
                logDebug << "[optArgT] Test sequence frame count [" << i << "]: " << testSequenceFileNames[i].size() << std::endl;
            logDebug << "[optArgT] Test sequence [0] example frame names:" << std::endl;
            for (size_t i = 0; i < testSequenceFileNames[0].size(); ++i)
                logDebug << "  " << testSequenceFileNames[0][i] << std::endl;
        );
    }

    // Header of results file
    logResult << "SEQUENCE_TRACK_ID,SEQUENCE_NUMBER,FRAME_NUMBER,TRACK_COUNT,TARGET_COUNT,TRACK_NUMBER,"
              << "BEST_LABEL,BEST_SCORE_RAW,BEST_SCORE_ACC,ROI_TL_X,ROI_TL_Y,ROI_BR_X,ROI_BR_Y";
    for (size_t poi = 0; poi < targetCount; ++poi) {
        std::string spoi = std::to_string(poi);
        logResult << ",TARGET_LABEL_" + spoi + ",TARGET_SCORE_RAW_" + spoi + ",TARGET_SCORE_ACC_" + spoi;
    }
    logResult << std::endl;

    /********************************************************************************************************************************************/
    /* LOAD FACE DETECTORS                                                                                                                      */
    /********************************************************************************************************************************************/
    Association association(conf);

    // main face detector
    std::shared_ptr<IDetector> faceDetector = buildSpecializedDetector(*conf, opencvSourceDataPathStr, DetectorType::FACE_DETECTOR_GLOBAL);
    ASSERT_LOG_FINALIZE(faceDetector, "Global face detector not properly initialized", logOutput, EXIT_FAILURE);
    size_t nFaceModels = faceDetector->modelCount();

    // localized search face detector
    std::shared_ptr<IDetector> localFaceDetector = buildSpecializedDetector(*conf, opencvSourceDataPathStr, DetectorType::FACE_DETECTOR_LOCAL);
    size_t nLocalFaceModels = localFaceDetector ? localFaceDetector->modelCount() : 0;

    // left-right eye detectors
    std::shared_ptr<IDetector> eyesDetector = buildSpecializedDetector(*conf, opencvSourceDataPathStr, DetectorType::EYE_DETECTOR);
    size_t nEyeModels = eyesDetector ? eyesDetector->modelCount() : 0;

    FACE_RECOG_DEBUG(
        for (size_t d = 0; d < nFaceModels; ++d)
            logOutput << "Loaded global face detector model " << d << ": '" << faceDetector->getModelName(d) << "'" << std::endl;
        for (size_t d = 0; d < nLocalFaceModels; ++d)
            logOutput << "Loaded local face detector model " << d << ": '" << localFaceDetector->getModelName(d) << "'" << std::endl;
        for (size_t d = 0; d < nEyeModels; ++d)
            logOutput << "Loaded eye detector model " << d << ": '" << eyesDetector->getModelName(d) << "'" << std::endl;
    );

    /*TRAKING VECTORS*/
    int trackNumber = 0;
    vector<Track> currentTracks, initCandidates, newCandidates;
    currentTracks.reserve(25);
    /*DETECTION VECTORS*/
    vector<cv::Rect> mergedDet, NotMatchedDets, newROIs;
    vector<size_t> usedDetectorIndexes;

    /********************************************************************************************************************************************/
    /* IMAGE BUFFERS                                                                                                                            */
    /********************************************************************************************************************************************/
    // each classifier needs a copy of the input frame
    FACE_RECOG_MAT frame, frameGray, frameROI;

    // images for writing output (rectangle must be added to drawImg)
    cv::Mat drawImg(conf->displayWindowH, conf->displayWindowW, CV_8UC3);              // Buffer for display
    FACE_RECOG_MAT frameVideo(conf->displayWindowH, conf->displayWindowW, CV_8UC3);    // Buffer for reading

    #if FACE_RECOG_HAS_FLYCAPTURE2
    FlyCapture2::Image pgrRawImage;     // Buffer for reading the raw PGR Image
    FACE_RECOG_MAT frameRaw;            // Buffer for the non-resized converted image
    #endif

    /********************************************************************************************************************************************/
    /* CAPTURE DEVICE (CAMERA/FILES)                                                                                                            */
    /********************************************************************************************************************************************/
    /*Default VideoCapture object used, otherwise try with Point Grey Research FlyCapture2 SDK*/
    void* videoStream;
    bool isVideoOpen = false;
    FACE_RECOG_DEBUG(logDebug << "Camera Type: " << conf->cameraType << std::endl);

    /* Index of the camera to use, otherwise the frame sequence is used */
    if (conf->cameraType == CameraType::FILE_STREAM || conf->cameraIndex < 0)               // image files sequence or video file
    {
        videoStream = new VideoCapture;
        isVideoOpen = ((VideoCapture*)videoStream)->open(framesPath);
        if (isVideoOpen)
            logOutput << "CV file sequence ready for input..." << std::endl;
    }
    else if (conf->cameraType == CameraType::CV_VIDEO_CAPTURE && conf->cameraIndex >= 0)    // camera live-feed
    {
        videoStream = new VideoCapture;
        // ignore config camera index parameters if '-v' enforced via command line
        if (optArgV)
            isVideoOpen = ((VideoCapture*)videoStream)->open(framesPath);
        else
            isVideoOpen = ((VideoCapture*)videoStream)->open(conf->cameraIndex);
        if (isVideoOpen) {
            logOutput << "CV camera ready for capture..." << std::endl;
            ((VideoCapture*)videoStream)->set(CV_CAP_PROP_FRAME_HEIGHT, conf->displayWindowH);
            ((VideoCapture*)videoStream)->set(CV_CAP_PROP_FRAME_WIDTH, conf->displayWindowW);
        }
    }
    /*Frame files or auto detect with CV VideoCapture failed, try PG*/
    else if (conf->cameraType == CameraType::PGR_FLYCAPTURE2 && conf->cameraIndex >= 0)     // PGR camera live-feed
    {
        #if !FACE_RECOG_HAS_FLYCAPTURE2
        logOutput << "Point Grey Research FlyCaputre2 SDK not found, cannot employ camera type specified in 'config.txt'";
        #else
        videoStream = new FlyCapture2::Camera;
        isVideoOpen = (ConnectCameraPGR((FlyCapture2::Camera*)videoStream, conf->cameraIndex, conf->useCameraTrigger) == FlyCapture2::PGRERROR_OK);
        if (isVideoOpen)
            logOutput << "PGR camera ready for capture..." << std::endl;
        #endif /*FACE_RECOG_HAS_FLYCAPTURE2*/
    }

    ASSERT_LOG_FINALIZE(conf->cameraType.isDefined(), "Could not identify which camera/files to use as input", logOutput, EXIT_FAILURE);
    ASSERT_LOG_FINALIZE(isVideoOpen, "Failed to open specified video type in config [" + conf->cameraType.name() + "]", logOutput, EXIT_FAILURE);

    /********************************************************************************************************************************************/
    /* MAIN LOOP OVER FRAMES                                                                                                                    */
    /********************************************************************************************************************************************/
    for (;;)
    {
        // update test video stream as required
        if (optArgT && frameCounter >= testSequenceFileNames[sequenceCounter].size()) {
            ++sequenceCounter;
            frameCounter = 0;
            // end of all test sequences
            if (sequenceCounter >= testSequenceFileNames.size()) {
                logOutput << "All test sequences processed." << std::endl;
                break;
            }
            sequenceTrackID = bfs::path(testSequenceRegexPaths[sequenceCounter]).remove_filename().filename().string();
            ((VideoCapture*)videoStream)->open(testSequenceRegexPaths[sequenceCounter]);
            currentTracks.clear();  // reset tracks for starting new sequence
        }

        // update frame label or end loop when end reached with input files
        if (optArgP && frameCounter >= testSequenceFileNames[sequenceCounter].size()) {
            logOutput << "All input frames processed from path." << std::endl;
            break;
        }
        currentFrameLabel = (optArgP || optArgT) ? testSequenceFileNames[sequenceCounter][frameCounter] : std::to_string(frameCounter);

        FACE_RECOG_DEBUG(logDebug << "Delta: " << getDeltaTimePrecise(frameTimePrev, MILLISECONDS) << "ms" << std::endl);
        frameTimePrev = getTimeNowPrecise();

        if (conf->cameraType == CameraType::CV_VIDEO_CAPTURE || conf->cameraType == CameraType::FILE_STREAM)
        {
            // grab next VideoCapture frame
            if (!((VideoCapture*)videoStream)->read(frameVideo)) {
                logOutput << "CV camera frame grabbing error: 'VideoCapture::read'" << std::endl;
                break;
            }
        }
        else if (conf->cameraType == CameraType::PGR_FLYCAPTURE2)
        {
            #if FACE_RECOG_HAS_FLYCAPTURE2
            // Trigger next frame if trigger is employed
            if (conf->useCameraTrigger)
                FireTriggerWhenReady((FlyCapture2::Camera*)videoStream, conf->verboseDebug);

            // grab the next FlyCapture2 frame and check for error
            FlyCapture2::Error cameraError = ((FlyCapture2::Camera*)videoStream)->RetrieveBuffer(&pgrRawImage);
            if (EvaluateAndPrintCameraError(cameraError, "PGR camera frame grabbing error"))
            {
                // if image consistency error, drop the frame and retry, otherwise quit main loop (general error)
                if (cameraError == FlyCapture2::PGRERROR_IMAGE_CONSISTENCY_ERROR)
                    continue;
                else
                    break;
            }

            // convert to OpenCV image type
            ConvertRGBImagePGR2CV(pgrRawImage, frameRaw);

            // preprocessing resize as required
            if (frameRaw.size() != frameVideo.size())
                FACE_RECOG_NAMESPACE::resize(frameRaw, frameVideo, frameVideo.size(), 0, 0, cv::INTER_AREA);
            else
                frameVideo = frameRaw;
            #endif /*FACE_RECOG_HAS_FLYCAPTURE2*/
        }

        // Mirror image for display if using a camera video stream and if the option was set
        if (conf->displayFrames && conf->flipFrames && conf->cameraType != CameraType::FILE_STREAM)
            frameVideo = imFlip(frameVideo, FlipMode::HORIZONTAL);

        // Upload to GPU for processing
        #if FACE_RECOG_USE_CUDA
        frame.upload(frameVideo);
        #else
        frame = frameVideo; // pass directly
        #endif

        // grayscale
        FACE_RECOG_NAMESPACE::cvtColor(frame, frameGray, CV_BGR2GRAY);

        bool isNewDetection = frameCounter == 0 || frameCounter % conf->detectionFrameInterval == 0;

        //========================================================================================================================================
        // FACE DETECTION
        //========================================================================================================================================
        if (isNewDetection)
        {
            FACE_RECOG_DEBUG(frameTime = getTimeNowPrecise());

            faceDetector->assignImage(frameGray);
            faceDetector->detectMerge(mergedDet);

            FACE_RECOG_DEBUG(
                deltaTime = getDeltaTimePrecise(frameTime, MILLISECONDS);
                sumTimeDetect += deltaTime;
                logDebug << setprecision(3) << "detection time: " << deltaTime << " ms" << std::endl;
            );

            // reinit candidates
            for (size_t i = 0; i < initCandidates.size(); ++i)
                initCandidates[i].markNotMatched();

            for (size_t i = 0; i < mergedDet.size(); ++i)
            {
                int initX = mergedDet[i].x;
                int initY = mergedDet[i].y;

                mergedDet[i].x = (mergedDet[i].x - conf->detectionAugmentationOffset >= 0)
                    ? mergedDet[i].x - conf->detectionAugmentationOffset
                    : 0;
                mergedDet[i].y = (mergedDet[i].y - conf->detectionAugmentationOffset >= 0)
                    ? mergedDet[i].y - conf->detectionAugmentationOffset
                    : 0;
                mergedDet[i].width = (mergedDet[i].x + mergedDet[i].width + conf->detectionAugmentationOffset * 2 <= frame.cols)
                    ? mergedDet[i].width + conf->detectionAugmentationOffset * 2
                    : frame.cols - initX;
                mergedDet[i].height = (mergedDet[i].y + mergedDet[i].height + conf->detectionAugmentationOffset * 2 <= frame.rows)
                    ? mergedDet[i].height + conf->detectionAugmentationOffset * 2
                    : frame.rows - initY;

                FACE_RECOG_DEBUG(logDebug << "END -- mergedDet " << i << ": " << mergedDet[i] << std::endl);
            }

            // for performance evaluation
            ++totalFramesDetect;
        }

        //========================================================================================================================================
        // TRACKING
        //========================================================================================================================================
        // internal image representation for trackers
        // ImageRep image(frameGray, 1, 0);
        ImageRep image(GET_MAT(frame, ACCESS_READ), 1, 0);

        if (frameCounter == 0)
        {
            for (size_t i = 0; i < mergedDet.size(); ++i) {
                Track track(conf, mergedDet[i], trackNumber++);
                track.reInitTracking(image);
                currentTracks.push_back(track);
            }
        }
        else
        {
            //------------------------------------------------------------------------------------------------------------------------------------
            // TRACKING CURRENT TRACKS
            //------------------------------------------------------------------------------------------------------------------------------------
            FACE_RECOG_DEBUG(frameTime = getTimeNowPrecise());
            #pragma omp parallel for
            for (long i = 0; i < currentTracks.size(); ++i) {
                currentTracks[i].track(image);
                currentTracks[i].markNotMatched();   // no match with detection
            }
            FACE_RECOG_DEBUG(sumTimeTrack += getDeltaTimePrecise(frameTime, MILLISECONDS));
        }

        //----------------------------------------------------------------------------------------------------------------------------------------
        // RECENTER TRACKERS
        //----------------------------------------------------------------------------------------------------------------------------------------
        usedDetectorIndexes.clear();
        if (isNewDetection)
        {
            // MATCH DETECTOR WITH TRACKER IF IoU > thresh
            FACE_RECOG_DEBUG(logDebug << "Number of detections = " << mergedDet.size() << std::endl);
            cv::Rect currentResizedTrackBbox;
            cv::Rect currentResizedMergedDetBbox;
            //////////////////////////////////////////////// #pragma omp parallel for // Problem caused by concurent removal of tracks
            for (size_t i = 0; i < currentTracks.size(); ++i)
            {
                for (size_t j = 0; j < mergedDet.size(); ++j)
                {
                    int maxSize = std::max(currentTracks[i].bbox().width, mergedDet[j].width);
                    currentResizedTrackBbox = util::getConstSizedRect(currentTracks[i].bbox(), maxSize, frame.size());
                    currentResizedMergedDetBbox = util::getConstSizedRect(mergedDet[j], maxSize, frame.size());
                    FACE_RECOG_DEBUG(
                        logDebug << "Track #" << i << std::endl;
                        logDebug << "Overlap Resized: " << util::overlap(currentResizedTrackBbox, currentResizedMergedDetBbox) << std::endl;
                        logDebug << "Overlap Original: " << util::overlap(currentTracks[i].bbox(), mergedDet[j]) << std::endl;
                        logDebug << "InitTrack: " << currentTracks[i].bbox() << std::endl;
                        logDebug << "Init Merged Det: " << mergedDet[j] << std::endl;
                        logDebug << "Resized Track Det: " << currentResizedTrackBbox << std::endl;
                        logDebug << "Resized Merged Det: " << currentResizedMergedDetBbox << std::endl;
                    );
                    if (util::intersect(currentResizedTrackBbox, currentResizedMergedDetBbox, conf->face.overlapThreshold)) {
                        currentTracks[i].insertROI(mergedDet[j]);
                        currentTracks[i].reInitTracking(image);
                        currentTracks[i].markMatched();
                        usedDetectorIndexes.push_back(j);
                        FACE_RECOG_DEBUG(
                            logDebug << "Over " << conf->face.overlapThreshold << " overlap" << std::endl;
                            logDebug << "using Detector: " << j << std::endl;
                        );
                    }
                }
            }

            for (size_t i = 0; i < currentTracks.size(); ++i)
            {
                FACE_RECOG_DEBUG(
                    logDebug << "Number of tracks: " << i << "/" << currentTracks.size() << std::endl;
                    logDebug << "Current track " << i << " is matched: " << currentTracks[i].isMatched() << std::endl;
                );
                if (!currentTracks[i].isMatched())
                {
                    double maxConfidence = faceDetector->evaluateConfidence(currentTracks[i], frameGray);
                    bool onImageEdge = (currentTracks[i].bbox().x == 0 || currentTracks[i].bbox().y == 0 ||
                                        currentTracks[i].bbox().x + currentTracks[i].bbox().width == frame.cols ||
                                        currentTracks[i].bbox().y + currentTracks[i].bbox().height == frame.rows);
                    FACE_RECOG_DEBUG(
                        logDebug << "Max confidence for track " << currentTracks[i].getTrackNumber() << " = " << maxConfidence << std::endl;
                        if (onImageEdge)
                            logDebug << "BBOX " << i << " is on edge of image" << std::endl;
                    );
                    if ((maxConfidence <= conf->removeTrackConfidenceOutBounds && onImageEdge) ||
                        (maxConfidence <= conf->removeTrackConfidenceInBounds)) {
                        currentTracks[i].increaseRemoveCount();
                        FACE_RECOG_DEBUG(logDebug << "Remove count = " << currentTracks[i].getRemoveCount() << std::endl);
                    }
                    else {
                        FACE_RECOG_DEBUG(logDebug << "Remove count to zero" << std::endl);
                        currentTracks[i].setRemoveCount(0);
                    }
                }
            }

            for (size_t i = 0; i < currentTracks.size(); ++i)
            {
                //--------------------------------------------------------------------------------------------------------------------------------
                // CHECK FOR TRACK REMOVAL
                //--------------------------------------------------------------------------------------------------------------------------------
                for (std::vector<Track>::iterator iter = currentTracks.begin(); iter != currentTracks.end();)
                {
                    FACE_RECOG_DEBUG(
                        logDebug << "RemoveCount: " << (*iter).getRemoveCount() << std::endl;
                        logDebug << "RemoveThresh: " << conf->removeTrackCountThresholdOutBounds << std::endl;
                    );
                    if ((*iter).getRemoveCount() >= conf->removeTrackCountThresholdOutBounds) {
                        if (conf->useFaceRecognition)
                            accScores.removeTrackScores(iter->getTrackNumber());
                        iter = currentTracks.erase(iter);
                        FACE_RECOG_DEBUG(logDebug << "Removing track" << std::endl);
                    }
                    else
                        ++iter;
                }
            }
        }

        //----------------------------------------------------------------------------------------------------------------------------------------
        // ADD NEW CANDIDATES
        //----------------------------------------------------------------------------------------------------------------------------------------
        if (isNewDetection)
        {
            // Create unmatched detections
            for (size_t i = 0; i < mergedDet.size(); ++i)
            {
                if (find(usedDetectorIndexes.begin(), usedDetectorIndexes.end(), i) == usedDetectorIndexes.end()) {
                    NotMatchedDets.push_back(mergedDet[i]);
                    FACE_RECOG_DEBUG(logDebug << "Not Matched with Detector: " << i << std::endl);
                }
            }
            for (size_t i = 0; i < initCandidates.size(); ++i)
                initCandidates[i].markNotMatched();

            if ((initCandidates.size() >= 1) || (NotMatchedDets.size() >= 1))
            {
                //--------------------------------------------------------------------------------------------------------------------------------
                // HUNGARIAN MATCHING
                //--------------------------------------------------------------------------------------------------------------------------------
                frameTime = getTimeNowPrecise();
                if (conf->useHungarianMatching)
                {
                    association.extendSet(initCandidates, NotMatchedDets);
                    association.computeCost(initCandidates, NotMatchedDets);
                    association.matchCandidates(initCandidates, NotMatchedDets, newCandidates);
                    /*REMOVE FAKE TRACKS*/
                    association.reduceSet(initCandidates);
                }
                else
                {
                    for (size_t i = 0; i < NotMatchedDets.size(); ++i)
                    {
                        Track newTrack(conf, NotMatchedDets[i]);
                        newCandidates.push_back(newTrack);
                    }
                }
                FACE_RECOG_DEBUG(
                    deltaTime = getDeltaTimePrecise(frameTime, MICROSECONDS);
                    logDebug << setprecision(3) << "matching time: " << deltaTime << " ms" << std::endl;
                    logDebug << "Number of new candidates: " << newCandidates.size() << std::endl;
                );

                //--------------------------------------------------------------------------------------------------------------------------------
                // ADD NEW CANDIDATES (from this frame)
                //--------------------------------------------------------------------------------------------------------------------------------
                for (size_t i = 0; i < newCandidates.size(); ++i)
                    initCandidates.push_back(newCandidates[i]);
                newCandidates.clear();  // clear the set of candidates from current frame
                NotMatchedDets.clear(); // clear unmatched detections from current frame

                //--------------------------------------------------------------------------------------------------------------------------------
                // EVALUATE CONFIDENCE OF INIT CANDIDATES
                //--------------------------------------------------------------------------------------------------------------------------------
                for (size_t i = 0; i < initCandidates.size(); ++i)
                {
                    double maxConfidence = faceDetector->evaluateConfidence(initCandidates[i], frameGray);
                    FACE_RECOG_DEBUG(
                        logDebug << "Max confidence for candidate " << i << " = " << maxConfidence << std::endl;
                        logDebug << "initCandidates: " << initCandidates[i].bbox() << std::endl;
                    );
                    if (maxConfidence > conf->createTrackConfidenceThreshold) {
                        initCandidates[i].increaseCreateCount();
                        FACE_RECOG_DEBUG(logDebug << "Create count = " << initCandidates[i].getCreateCount() << std::endl);
                    }
                    else
                        initCandidates[i].setCreateCount(-1);
                }
            }

            // loop over candidates for track creation
            for (std::vector<Track>::iterator iter = initCandidates.begin(); iter != initCandidates.end();)
            {
                if ((*iter).getCreateCount() >= conf->createTrackCountThreshold)
                {
                    // reset creation counter
                    (*iter).setCreateCount(0);
                    (*iter).reInitTracking(image);
                    // add to current tracks
                    (*iter).setTrackNumber(trackNumber++);
                    (*iter).setTrackSize(conf->roiAccumulationSize);
                    currentTracks.push_back((*iter));
                    // remove from candidates
                    iter = initCandidates.erase(iter);
                    FACE_RECOG_DEBUG(logDebug << "Created track" << std::endl);
                }
                else if ((*iter).getCreateCount() == -1)
                    iter = initCandidates.erase(iter);
                else
                    ++iter; // otherwise, just keep among the candidates
            }
        }
        util::mergeOverlappingTracks(currentTracks, conf->trackerOverlapThreshold, frameGray);

        //========================================================================================================================================
        // LOCAL FACE DETECTION
        //========================================================================================================================================

        // update track matched with detection if using local search and validated
        if (conf->useLocalSearchROI)
        {
            FACE_RECOG_DEBUG(frameTime = getTimeNowPrecise());
            for (size_t i = 0; i < currentTracks.size(); ++i)
            {
                //--------------------------------------------------------------------------------------------------------------------------------
                // LOCALIZED ROI SEARCH
                //--------------------------------------------------------------------------------------------------------------------------------

                FACE_RECOG_DEBUG(
                    logDebug << "NEW LOCAL SEARCH ROI ==================================================" << std::endl;
                    TP frameTimeLocal = getTimeNowPrecise();
                );

                // expand ROI by a config factor to give more slack for local search detection
                // access contained VJ face detector to update parameters for local search (mostly for maxSize)
                int expandedMaxSize = (int)(currentTracks[i].bbox().width * conf->bboxSizeMultiplyer);
                std::shared_ptr<FaceDetectorVJ> vj(std::static_pointer_cast<FaceDetectorVJ>(localFaceDetector));
                cv::Size expandedMaxSizeROI = cv::Size(expandedMaxSize, expandedMaxSize);
                vj->initializeParameters(conf->face.scaleFactor, conf->face.nmsThreshold, conf->face.minSize, expandedMaxSizeROI,
                                         conf->face.confidenceSize, conf->face.minNeighbours, conf->face.overlapThreshold);

                // update max size with enlarged bbox for localized search
                cv::Rect localSearchBBox = util::getConstSizedRect(currentTracks[i].bbox(), expandedMaxSize, frame.size());
                frameROI = FACE_RECOG_MAT(frameGray, localSearchBBox);

                // execute localize search to find faces ROI
                std::vector<std::vector<cv::Rect> > localComboFaces(nLocalFaceModels);
                localFaceDetector->assignImage(frameROI);
                localFaceDetector->detect(localComboFaces);
                newROIs = localFaceDetector->mergeDetections(localComboFaces);

                FACE_RECOG_DEBUG(
                    logDebug << "current bbox: " << currentTracks[i].bbox() << std::endl;
                    logDebug << "local bbox: " << localSearchBBox << std::endl;
                    logDebug << "NB CC Local: " << nLocalFaceModels << std::endl;
                    logDebug << "combo size: " << localComboFaces.size() << std::endl;
                    logDebug << "localComboFaces: " << std::endl;
                    for (size_t ci = 0; ci < localComboFaces.size(); ++ci)
                        for (size_t cj = 0; cj < localComboFaces[ci].size(); ++cj)
                            logDebug << "(ci=" << ci << ",cj=" << cj << "): " << localComboFaces[ci][cj] << std::endl;
                    logDebug << "newROIs: " << std::endl;
                    for (size_t ci = 0; ci < newROIs.size(); ++ci)
                        logDebug << "(ci=" << ci << "): " << newROIs[ci] << std::endl;
                    if (newROIs.size() > 0) {
                        for (size_t c = 0; c < nLocalFaceModels; ++c) {
                            if (newROIs[c].area() > 0) {
                                logDebug << "combo bbox[" << c << "]: " << localComboFaces[c] << std::endl;
                                logDebug << "newROI[" << c << "]: " << newROIs[c] << std::endl;
                                std::string roiName = currentFrameLabel + "_c=" + std::to_string(c);
                                cv::Rect roiAbsPos = newROIs[c];
                                roiAbsPos.x += localSearchBBox.x;
                                roiAbsPos.y += localSearchBBox.y;
                            }
                            else
                                logDebug << "combo bbox[" << c << "]: [empty (area !> 0)]" << std::endl;
                        }
                    }
                    logDebug << "old roi: " << currentTracks[i].bbox() << std::endl;
                    logDebug << "expandedMaxSize: " << expandedMaxSize << std::endl;
                    logDebug << "new roi: " << localSearchBBox << std::endl;
                    logDebug << newROIs.size() << " detections in bbox region" << std::endl;
                );

                //--------------------------------------------------------------------------------------------------------------------------------
                // MERGE ORIGINAL AND LOCALIZED ROI
                //--------------------------------------------------------------------------------------------------------------------------------

                // Go through detection and find the closest one from the original detection (best match IoU)
                cv::Rect currentResizedTrackBbox, currentResizedROI;
                int bestJ = -1;
                double bestIoU = -1, tempIoU;
                for (size_t j = 0; j < newROIs.size(); ++j)
                {
                    // get absolute position on frame
                    newROIs[j].x += localSearchBBox.x;
                    newROIs[j].y += localSearchBBox.y;

                    int maxSize = std::max(currentTracks[i].bbox().width, newROIs[j].width);
                    currentResizedTrackBbox = util::getConstSizedRect(currentTracks[i].bbox(), maxSize, frame.size());
                    currentResizedROI = util::getConstSizedRect(newROIs[j], maxSize, frame.size());

                    tempIoU = util::overlap(currentResizedROI, currentResizedTrackBbox);
                    if (tempIoU > bestIoU) {
                        bestIoU = tempIoU;
                        bestJ = (int)j;
                    }
                    FACE_RECOG_DEBUG(
                        logDebug << "Match j=" << j << ": " << newROIs[j] << std::endl;
                        logDebug << "Current Track: " << currentTracks[i].bbox() << std::endl;
                        logDebug << "RESIZED - Match: " << currentResizedTrackBbox << std::endl;
                        logDebug << "RESIZED - Current Track: " << currentResizedROI << std::endl;
                        logDebug << "TEMP IOU: " << tempIoU << std::endl;
                    );
                }
                FACE_RECOG_DEBUG(logDebug << "Best IOU: " << bestIoU << std::endl);

                if (bestIoU > 0) {
                    // update latest ROI with adjusted local search bbox
                    ROI roi = currentTracks[i].getROI();
                    roi.updateROI(newROIs[bestJ]);
                    currentTracks[i].updateROI(roi);
                    currentTracks[i].reInitTracking(image);
                    currentTracks[i].markMatched();
                    FACE_RECOG_DEBUG(
                        logDebug << "Match: " << newROIs[bestJ] << std::endl;
                        logDebug << "CHANGED BBOX LOCALIZED SEARCH" << std::endl;
                    );
                }
                FACE_RECOG_DEBUG(
                    deltaTime = getDeltaTimePrecise(frameTimeLocal, MICROSECONDS);
                    logDebug << "Delta-Time localized ROI search: " << deltaTime << " ms" << std::endl;
                );
                ++totalFramesDetectLocal;
            }
            FACE_RECOG_DEBUG(
                deltaTime = getDeltaTimePrecise(frameTime, MILLISECONDS);
                sumTimeDetectLocal += deltaTime;
            );
        }

        //========================================================================================================================================
        // EYE DETECTION
        //========================================================================================================================================
        if (conf->useEyesDetection && currentTracks.size() > 0)
        {
            FACE_RECOG_DEBUG(logDebug << "Eye detection: " << currentTracks.size() << " tracks" << std::endl);

            for (size_t i = 0; i < currentTracks.size(); ++i)
            {
                ROI roi = currentTracks[i].getROI(); // Get most recent ROI without eyes
                #pragma omp parallel for
                for (long iDet = 0; iDet < nEyeModels; ++iDet)
                {
                    // Get the search area, either the full face ROI or localized position if specified
                    cv::Rect searchArea = roi.getRect();
                    if (conf->useEyeLocalizedPosition) {
                        std::shared_ptr<EyeDetector> eyesDetSpec = std::static_pointer_cast<EyeDetector>(eyesDetector);
                        // Top-Left 1/4 of face ROI (or Right if frame is flipped)
                        if ((iDet == eyesDetSpec->leftEyeIndex && !conf->flipFrames) || (iDet == eyesDetSpec->rightEyeIndex && conf->flipFrames))
                            searchArea = cv::Rect(searchArea.x, searchArea.y, searchArea.width / 2, searchArea.height / 2);
                        // Top-Right 1/4 of face ROI (or Left if frame is flipped)
                        if ((iDet == eyesDetSpec->rightEyeIndex && !conf->flipFrames) || (iDet == eyesDetSpec->leftEyeIndex && conf->flipFrames))
                            searchArea = cv::Rect(searchArea.x + searchArea.width / 2, searchArea.y, searchArea.width / 2, searchArea.height / 2);
                    }
                    eyesDetector->assignImage(FACE_RECOG_MAT(frameGray, searchArea));
                }
                // Find eyes with each eye detector and add them to the current ROI
                vector<vector<cv::Rect> > eyes;
                eyesDetector->detect(eyes);
                for (size_t iEye = 0; iEye < eyes.size(); ++iEye) {
                    for (size_t jEye = 0; jEye < eyes[iEye].size(); ++jEye) {
                        roi.addSubRect(eyes[iEye][jEye]);
                        currentTracks[i].setValidateEyeDetection();
                    }
                }
                currentTracks[i].updateROI(roi); // update current ROI with eyes added
            }
        }

        //========================================================================================================================================
        // FACE RECOGNITION
        //========================================================================================================================================
        if (conf->useFaceRecognition)
        {
            for (size_t i = 0; i < currentTracks.size(); ++i)
            {
                // skip probe if not validated with requested methods
                if (conf->useLocalSearchROI && !currentTracks[i].getROI().isUpdatedROI() && !currentTracks[i].isUnknown()) {
                    currentTracks[i].markUnknown();
                    continue;
                }
                if (conf->useEyesDetection && !currentTracks[i].isValidatedEyeDetection() && !currentTracks[i].isUnknown()) {
                    currentTracks[i].markUnknown();
                    continue;
                }

                //--------------------------------------------------------------------------------------------------------------------------------
                // PREDICT RECOGNITION SCORES
                //--------------------------------------------------------------------------------------------------------------------------------

                int currentTrackNum = currentTracks[i].getTrackNumber();
                FACE_RECOG_MAT probeROI = frame(currentTracks[i].bbox());
                std::vector<double> predictions(classifier->predict(probeROI));
                predictions = classifier->predict(probeROI);
                accScores.addPredictions(currentTrackNum, predictions);

                FACE_RECOG_DEBUG(
                    for (size_t pos = 0; pos < predictions.size(); ++pos) {
                        double acc = accScores.getScore(conf->roiAccumulationMode, currentTrackNum, pos);
                        if (minScores[pos] > predictions[pos])
                            minScores[pos] = predictions[pos];
                        if (maxScores[pos] < predictions[pos])
                            maxScores[pos] = predictions[pos];

                        logDebug << "TRACK #" << currentTrackNum << " i: " << pos << " fileName: " << POI_IDs[pos];
                        logDebug << setprecision(6) << " accPred: " << acc << " lastPred: " << predictions[pos];
                        logDebug << setprecision(6) << setprecision(6) << " min: " << minScores[pos] << " max: " << maxScores[pos] << std::endl;
                    }
                );

                //--------------------------------------------------------------------------------------------------------------------------------
                // UPDATE TARGET RECOGNITIONS
                //--------------------------------------------------------------------------------------------------------------------------------

                double bestGuestTargetScore; int bestGuestTargetIndex;
                accScores.getMaxPositiveInfo(conf->roiAccumulationMode, currentTrackNum, bestGuestTargetIndex, bestGuestTargetScore);
                if (bestGuestTargetIndex >= 0) {
                    if (bestGuestTargetScore >= conf->thresholdFaceRecognized) {
                        FACE_RECOG_DEBUG(logDebug << "bestGuestPos (recognized): " << bestGuestTargetIndex << std::endl);
                        currentTracks[i].markRecognized();
                        currentTracks[i].setName(POI_IDs[bestGuestTargetIndex]);
                    }
                    else if (bestGuestTargetScore >= conf->thresholdFaceConsidered) {
                        FACE_RECOG_DEBUG(logDebug << "bestGuestPos (considered): " << bestGuestTargetIndex << std::endl);
                        currentTracks[i].markConsidered();
                        currentTracks[i].setName(POI_IDs[bestGuestTargetIndex]);
                    }
                    else {
                        currentTracks[i].markUnknown();
                        currentTracks[i].setName("");
                    }
                }
            }

            FACE_RECOG_DEBUG(
                if (currentTracks.size() == 0)
                    logOutBBox << currentFrameLabel << getDeltaTimePrecise(frameTimePrev, MILLISECONDS) << "-1 0 0 0 0" << std::endl;
            );
        }

        //========================================================================================================================================
        // DISPLAY & OUTPUT
        //========================================================================================================================================

        // Must transfer back from GPU to draw on image
        if (conf->displayFrames || conf->outputFrames) {
            #if FACE_RECOG_USE_CUDA
            frame.download(drawImg);
            #else
            frame.copyTo(drawImg);
            #endif
        }

        // output recognition results 'start of line' to CSV file (generic information)
        //      SEQUENCE_TRACK_ID,SEQUENCE_NUMBER,FRAME_NUMBER,TRACK_COUNT,TARGET_COUNT
        size_t trackCount = currentTracks.size();
        logResult << sequenceTrackID << "," << sequenceCounter << "," << currentFrameLabel << "," << trackCount << "," << targetCount;

        for (size_t i = 0; i < trackCount; ++i)
        {
            //------------------------------------------------------------------------------------------------------------------------------------
            // UPDATE DISPLAY OF TARGET ROI & RECOGNITION INFO
            //------------------------------------------------------------------------------------------------------------------------------------

            if (conf->displayFrames || conf->outputFrames)
            {
                // either unused eye detection or required eyes are validated
                bool eyeOK = !conf->useEyesDetection || currentTracks[i].isValidatedEyeDetection();
                // display original ROI before update if available and requested
                bool showUpdate = conf->useLocalSearchROI && conf->displayOldROI && currentTracks[i].getROI().isUpdatedROI();

                ColorCode color = (eyeOK && currentTracks[i].isRecognized()) ? bboxColorRecognized  // Recognized
                                : (eyeOK && currentTracks[i].isConsidered()) ? bboxColorConsidered  // Considered
                                : bboxColorNotMatched;                                              // NotMatched | no eyes

                // draw old face ROI (original detection)
                if (showUpdate)
                    cv::rectangle(drawImg, currentTracks[i].getROI().getOriginalRect(), color, conf->roiThicknessOld);
                // draw updated face ROI and text index
                cv::rectangle(drawImg, currentTracks[i].bbox(), color, conf->roiThickness);

                // display recognition score and target ID
                int bestPosIndex = -1; double bestPosScore = 0;
                if (conf->useFaceRecognition) {
                    accScores.getMaxPositiveInfo(conf->roiAccumulationMode, currentTracks[i].getTrackNumber(), bestPosIndex, bestPosScore);
                    if ((currentTracks[i].isRecognized() || currentTracks[i].isConsidered()) && bestPosIndex >= 0) {
                        std::string strTargetTagAndScore = POI_IDs[bestPosIndex] + " | " + std::to_string(bestPosScore);
                        Point point = Point(currentTracks[i].bbox().x, currentTracks[i].bbox().y + currentTracks[i].bbox().height + 15);
                        cv::putText(drawImg, strTargetTagAndScore, point, FONT_HERSHEY_PLAIN, 1.0, color, 2);
                    }
                }

                /*  output rest of recognition results as CSV:
                        {<results>(i)}     for i=TRACK_COUNT
                    where each <results>(i):
                        ,TRACK_NUMBER,BEST_LABEL,BEST_SCORE_RAW,BEST_SCORE_ACC,
                        ROI_TL_X,ROI_TL_Y,ROI_BR_X,ROI_BR_Y{<result_target>(j)}     for j=TARGET_COUNT
                    where each <result_target>(j):
                        ,TARGET_LABEL,TARGET_SCORE_RAW,TARGET_SCORE_ACC
                */
                if (optArgR) {
                    cv::Point tl = currentTracks[i].bbox().tl();
                    cv::Point br = currentTracks[i].bbox().br();
                    size_t trackNumer = currentTracks[i].getTrackNumber();
                    std::string targetsLabelScores;
                    std::string bestPosID;
                    double bestRawScore = -1;
                    if (conf->useFaceRecognition) {
                        bestPosID = POI_IDs[bestPosIndex];
                        bestRawScore = accScores.getRawScore(trackNumer, bestPosIndex);
                        for (size_t j = 0; j < targetCount; ++j) {
                            targetsLabelScores += ("," + POI_IDs[j] + "," + std::to_string(accScores.getRawScore(trackNumer, j)) +
                                                   "," + std::to_string(accScores.getScore(conf->roiAccumulationMode, trackNumer, j)));
                        }
                    }
                    logResult << "," << trackNumer << "," << bestPosID << "," << bestRawScore << "," << bestPosScore;
                    logResult << "," << tl.x << "," << tl.y << "," << br.x << "," << br.y << targetsLabelScores;
                }

                // display sequence track ID, frame number and FPS where applicable and as requested
                int offset = 16;
                if (optArgT && conf->displaySequenceTrackID) {
                    cv::putText(drawImg, sequenceTrackID, Point(8, offset), FONT_HERSHEY_PLAIN, 1.0, rgbColorCode(ColorType::LIGHT_GREEN), 2);
                    offset += 16;
                }
                if (conf->displayFrameNumber) {
                    cv::putText(drawImg, currentFrameLabel, Point(8, offset), FONT_HERSHEY_PLAIN, 1.0, rgbColorCode(ColorType::LIGHT_GREEN), 2);
                    offset += 16;
                }
                if (conf->displayFrameRate) {
                    deltaTime = getDeltaTimePrecise(frameTimePrev, MICROSECONDS);
                    if (deltaTime > 0.0) {
                        std::ostringstream fps;
                        fps << "FPS " << std::fixed << std::showpoint << std::setprecision(2) << (1000000.0 / deltaTime);
                        cv::putText(drawImg, fps.str(), Point(8, offset), FONT_HERSHEY_PLAIN, 1.0, rgbColorCode(ColorType::LIGHT_GREEN), 2);
                        offset += 16;
                    }
                }

                // display track number
                FACE_RECOG_DEBUG(logDebug << "TRACK NUMBER test " << currentTracks[i].getTrackNumber() << std::endl);
                std::string strTrackerNumber = format("#%u", currentTracks[i].getTrackNumber());
                Point point = Point(currentTracks[i].bbox().x, currentTracks[i].bbox().y - 10);
                cv::putText(drawImg, strTrackerNumber, point, FONT_HERSHEY_PLAIN, 1.0, color, 2);

                // draw eyes ROI
                if (conf->useEyesDetection) {
                    ROI roi = currentTracks[i].getROI();
                    ColorCode darkColor = color / 2;
                    size_t nEyes = roi.countSubROI();
                    FACE_RECOG_DEBUG(logDebug << "Number of eyes found: " << nEyes << std::endl);
                    for (size_t iEye = 0; iEye < nEyes; ++iEye)
                        cv::rectangle(drawImg, roi.getSubRect(iEye), darkColor, 2);
                }
            }

            //------------------------------------------------------------------------------------------------------------------------------------
            // OUTPUT REQUESTED TARGET ROI
            //------------------------------------------------------------------------------------------------------------------------------------

            if (conf->outputROI)
                util::saveTrackROIToDisk(currentTracks[i].getROI().getOriginalRect(), frameGray, currentFrameLabel,
                                         currentTracks[i].getTrackNumber(), conf->roiOutputSize, imgDir);

            if (conf->outputLocalROI)
                util::saveTrackROIToDisk(currentTracks[i].bbox(), frameGray, currentFrameLabel,
                                         currentTracks[i].getTrackNumber(), conf->roiOutputSize, imgDirLocal);

            FACE_RECOG_DEBUG(
                if (currentTracks[i].isValidatedEyeDetection() || !conf->useEyesDetection)
                    logOutBBox << currentFrameLabel << " " << util::rectPointCoordinates(currentTracks[i].bbox(), " ") << std::endl;
            );
        }
        logResult << std::endl; // move to next line for future results to output (next frame)

        //----------------------------------------------------------------------------------------------------------------------------------------
        // PLOT DISPLAY
        //----------------------------------------------------------------------------------------------------------------------------------------
        if (conf->displayPlots && conf->useFaceRecognition)
        {
            size_t nTracks = MIN(currentTracks.size(), (size_t)conf->plotMaxTracks);
            for (size_t idx = 0; idx < nTracks; ++idx) {
                int track = currentTracks[idx].getTrackNumber();
                for (size_t poi = 0; poi < nPlotPOI; ++poi) {
                    // shift values 'right' or reset as required, then add most recent values at the end
                    for (int p = 0; p < conf->plotAccumulationPoints - 1; ++p) {
                        if (conf->plotResetOnTrackLost && plotTrackID[idx] != track)
                            plotData[idx][poi].at<double>(p) = 0.0;
                        else
                            plotData[idx][poi].at<double>(p) = plotData[idx][poi].at<double>(p + 1);
                    }
                    double latestPlotPoint = accScores.getScore(conf->roiAccumulationMode, track, poi);
                    plotData[idx][poi].at<double>(conf->plotAccumulationPoints - 1) = latestPlotPoint;
                    plotsPtr[idx][poi]->render(subPlot);

                    cv::imshow(plotSingleName, subPlot);
                    cv::imshow(plotScoresName, plotData[idx][poi]);

                    // resize 'subplot' to sub-region of 'figure'
                    int subPlotW = conf->plotTrackDirection ? conf->plotFigureWidth / (int)nTracks : conf->plotFigureWidth / (int)nPlotPOI;
                    int subPlotH = conf->plotTrackDirection ? conf->plotFigureHeight / (int)nPlotPOI : conf->plotFigureHeight / (int)nTracks;
                    int subPlotX = conf->plotTrackDirection ? subPlotW * (int)idx : subPlotW * (int)poi;
                    int subPlotY = conf->plotTrackDirection ? subPlotH * (int)poi : subPlotH * (int)idx;
                    cv::Rect subPlotRegion(subPlotX, subPlotY, subPlotW, subPlotH);
                    cv::resize(subPlot, plotFigure(subPlotRegion), cv::Size(subPlotW, subPlotH), 0.0, 0.0, cv::INTER_CUBIC);
                }
                plotTrackID[idx] = track;   // update memorized track number for next reset
            }
            cv::imshow(plotFigureName, plotFigure);
        }

        //----------------------------------------------------------------------------------------------------------------------------------------
        // WRITE OUTPUT FRAMES
        //----------------------------------------------------------------------------------------------------------------------------------------
        if (optArgI && conf->outputFrames) {
            string imagePath = imgDir + "/" + currentFrameLabel + ".png";
            cv::imwrite(imagePath, drawImg);
        }

        //----------------------------------------------------------------------------------------------------------------------------------------
        // DISPLAY PROCESSED FRAME
        //----------------------------------------------------------------------------------------------------------------------------------------
        if (conf->displayFrames) {
            cv::imshow(windowName, drawImg);
            cv::waitKey(1);     // Delay for frame rendering in window
        }

        ++frameCounter;
    }   // end of main loop over frames

    /********************************************************************************************************************************************/
    /* STATISTICS                                                                                                                               */
    /********************************************************************************************************************************************/

    FACE_RECOG_DEBUG(
        double dblTotalFrames = (double)frameCounter;
        double avgTimeDetect = sumTimeDetect / (double)totalFramesDetect;
        double avgTimeTrack = sumTimeTrack / dblTotalFrames;
        double avgTimeDetectLocal = sumTimeDetectLocal / (double)totalFramesDetectLocal;
        double avgTimeDetectPerFrame = sumTimeDetect / dblTotalFrames;
        double avgTimeTrackPerFrame = sumTimeTrack / dblTotalFrames;

        logTiming << "Number of frames in video: " << frameCounter << std::endl << setprecision(6);
        logTiming << "Average time per detection: " << avgTimeDetect << "ms" << std::endl;
        logTiming << "Average time per tracking: " << avgTimeTrack << "ms" << std::endl;
        logTiming << "Average time per local detection: " << avgTimeDetectLocal << "ms" << std::endl;
        logTiming << "Average time per detection with respect to original video: " << avgTimeDetectPerFrame << "ms" << std::endl;
        logTiming << "Average time per tracking with respect to original video: " << avgTimeTrackPerFrame << "ms" << std::endl;
    );

    FINALIZE(EXIT_SUCCESS);

    #if FACE_RECOG_USE_EXCEPTION_LOGGING
    } // try
    catch (std::exception& ex) {
        std::cout << "Unhandled exception occurred in 'FaceRecog'" << std::endl
                  << "  Exception: [" << ex.what() << "]" << std::endl;
        FINALIZE(EXIT_FAILURE);
    }
    #endif/*FACE_RECOG_USE_EXCEPTION_LOGGING*/
}
