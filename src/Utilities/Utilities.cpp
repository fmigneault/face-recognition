#include "Utilities/Utilities.h"
#include "FaceRecog.h"

namespace util {
bool createDirIfNotExist(const string& dirName)
{
    struct stat st = { 0 };
    if (stat(dirName.c_str(), &st) == -1)
    {
        #if defined(FACE_RECOG_LINUX)
        return mkdir(dirName.c_str(), 0700) == 0;
        #elif defined(FACE_RECOG_WINDOWS)
        return _mkdir(dirName.c_str()) == 0;
        #endif
    }
    return false;
}

bool prepareFrameFileNames(vector<string>& fileNames, const string& framesRegexPath)
{
    bfs::path p(framesRegexPath);
    if (is_directory(p))    // expecting a regex path (/a/regex/path/%%04d.jpg)
        return false;       // return if only the directory was specified
    p = p.parent_path();
    if (!is_directory(p))
        return false;
    else {
        for (auto &entry : boost::make_iterator_range(bfs::directory_iterator(p), {}))
            fileNames.push_back(entry.path().stem().string());
        sort(fileNames.begin(), fileNames.end());
    }
    return !fileNames.empty();
}

bool prepareTestSequences(vector<vector<string> >& sequenceFileNames, vector<string>& sequenceRegexPaths, const string& testFilePath)
{
    bfs::path p(testFilePath);
    ifstream testFile(testFilePath);
    if (!testFile.is_open())
        return false;
    else {
        string testFileSequenceRegexPath;
        while (getline(testFile, testFileSequenceRegexPath))
        {
            sequenceFileNames.push_back(vector<string>());
            size_t iTest = sequenceFileNames.size() - 1;
            if (!util::prepareFrameFileNames(sequenceFileNames[iTest], testFileSequenceRegexPath))
                return false;
            sequenceRegexPaths.push_back(testFileSequenceRegexPath);
        }
    }
    return !sequenceFileNames.empty();
}

int prepareEnrollROIs(const ConfigFile& config, bfs::path opencvSourcesDataPath, std::vector<std::vector<FACE_RECOG_MAT> >& POI_ROIs,
                      std::vector<std::string>& POI_IDs, std::vector<std::vector<FACE_RECOG_MAT> >& NEG_ROIs, logstream& logger)
{
    // load basic single still from directory, generate local ROI as required
    std::vector<FACE_RECOG_MAT> stillPOI;
    if (loadDirectoryROIs(config, opencvSourcesDataPath, stillPOI, POI_IDs, logger) != EXIT_SUCCESS)
        return EXIT_FAILURE;

    POI_ROIs = std::vector<std::vector<FACE_RECOG_MAT> >(stillPOI.size());
    std::vector<std::string> dummyIDs;
    if (config.useReferenceNegativeStills)
        NEG_ROIs = std::vector<std::vector<FACE_RECOG_MAT> >(stillPOI.size());

    for (size_t poi = 0; poi < POI_IDs.size(); ++poi) {
        // transfer original POI ROIs, and add additional images from geometric operations if requested
        if (!config.useGeometricPositiveStills)
            POI_ROIs[poi].push_back(stillPOI[poi]);
        else
            POI_ROIs[poi] = imSyntheticGeneration(stillPOI[poi],
                                                  config.geometricTranslatePixels,
                                                  config.geometricScalingFactor,
                                                  config.geometricScalingMinSize, true);  // output includes original

        // additional images from synthetic generation if available and requested
        bfs::path synthImagePOIPath = bfs::path(config.POIDir) / POI_IDs[poi];
        if (config.useSyntheticPositiveStills && bfs::exists(synthImagePOIPath)) {
            if (loadDirectoryROIs(config, opencvSourcesDataPath, POI_ROIs[poi], dummyIDs, logger, synthImagePOIPath.string()) != EXIT_SUCCESS)
                return EXIT_FAILURE;
        }
        logger << "Loaded " << POI_ROIs[poi].size() << " POI still(s) for '" << POI_IDs[poi] << "'" << std::endl;
    }

    // additional still images as negative counter-examples
    if (config.useReferenceNegativeStills)
    {
        // load at the very least other positive individual representations as counter-examples
        for (size_t poi = 0; poi < POI_IDs.size(); ++poi) {
            for (size_t neg = 0; neg < POI_IDs.size(); ++neg)
                if (poi != neg)
                    NEG_ROIs[poi].insert(NEG_ROIs[poi].end(), POI_ROIs[neg].begin(), POI_ROIs[neg].end());
        }
        // get additional negatives stills if the corresponding directory is available
        if (bfs::exists(config.NEGDirExtra)) {
            std::vector<FACE_RECOG_MAT> negROIs;
            if (loadDirectoryROIs(config, opencvSourcesDataPath, negROIs, dummyIDs, logger, config.NEGDirExtra) != EXIT_SUCCESS)
                return EXIT_FAILURE;
            // expand additional negative stills even more using geometric operations on them if requested
            if (config.useGeometricNegativeStills) {
                size_t nExtraNegativesRaw = negROIs.size();
                for (size_t neg = 0; neg < nExtraNegativesRaw; ++neg) {
                    std::vector<FACE_RECOG_MAT> geometricNegROIs = imSyntheticGeneration(negROIs[neg],
                                                                                         config.geometricTranslatePixels,
                                                                                         config.geometricScalingFactor,
                                                                                         config.geometricScalingMinSize, false);
                    negROIs.insert(negROIs.end(), geometricNegROIs.begin(), geometricNegROIs.end());
                }
            }
            for (size_t poi = 0; poi < POI_IDs.size(); ++poi)
                NEG_ROIs[poi].insert(NEG_ROIs[poi].end(), negROIs.begin(), negROIs.end());
        }

        for (size_t poi = 0; poi < POI_IDs.size(); ++poi)
            logger << "Loaded " << NEG_ROIs[poi].size() << " NEG still(s) for '" << POI_IDs[poi] << "'" << std::endl;
    }

    return EXIT_SUCCESS;
}

int loadDirectoryROIs(const ConfigFile& config, bfs::path opencvSourcesDataPath, std::vector<FACE_RECOG_MAT>& ROIs,
                      std::vector<std::string>& IDs, logstream& logger, std::string alternativeROIPath)
{
    if (alternativeROIPath == "")
        alternativeROIPath = config.POIDir;
    if (!bfs::is_directory(alternativeROIPath))
        return EXIT_FAILURE;

    // special image names (for auto-generation if missing)
    std::string roiSuffix = "_roi";
    std::string locSuffix = "_loc";

    // read POI image stills (ROI)
    std::string currentPathName;
    std::stringstream oss;
    bfs::directory_iterator end_itr;
    cv::CascadeClassifier cc;
    bfs::path basePathPOI(alternativeROIPath);
    for (bfs::directory_iterator itr(basePathPOI); itr != end_itr; ++itr)
    {
        // generate ROI image from whole still if not already available (same name appended with appropriate suffix)
        // prepare name/path verification values
        std::string currentFilePath = itr->path().string();
        if (!bfs::is_regular_file(currentFilePath)) continue;
        std::string imgLoadedPath;
        std::string imgName = itr->path().stem().string();
        std::string imgNameROI = imgName + roiSuffix;
        std::string imgNameLOC = imgName + locSuffix;
        bfs::path imgPathROI = basePathPOI / bfs::path(imgNameROI + itr->path().extension().string());
        bfs::path imgPathLOC = basePathPOI / bfs::path(imgNameLOC + itr->path().extension().string());
        std::string imgNameEndROI = imgName.substr(imgName.length() - roiSuffix.length());
        std::string imgNameEndLOC = imgName.substr(imgName.length() - locSuffix.length());
        cv::Mat poi;

        // LBP improved is trained to obtain focused localized face region directly (without much background)
        // Other cascades produce larger ROIs that usually require additional cropping to remove background
        bool isFocusLocalized = config.LBPCascadeFrontalImproved;

        // generate ROI/LOC if not found, then add it to POI
        // ROI/LOC image must not already exist, current image must not end with either ROI/LOC suffixes
        if (((!isFocusLocalized && !bfs::is_regular_file(imgPathROI)) || (isFocusLocalized && !bfs::is_regular_file(imgPathLOC)))
            && imgNameEndROI != roiSuffix && imgNameEndLOC != locSuffix)
        {
            if (cc.empty()) {
                bfs::path ccFile(isFocusLocalized
                                 ? "lbpcascades/lbpcascade_frontalface_improved.xml"
                                 : "haarcascades/haarcascade_frontalface_alt.xml");
                cc.load((opencvSourcesDataPath / ccFile).string());
            }
            cv::Mat img = cv::imread(currentFilePath, cv::IMREAD_GRAYSCALE);
            if (img.size() == cv::Size()) continue;
            std::vector<cv::Rect> facesROI;
            cc.detectMultiScale(img, facesROI, 1.05, 3, cv::CASCADE_SCALE_IMAGE, cv::Size(20, 20), img.size());
            if (facesROI.size() != 1) {
                for (size_t roi = 0; roi < facesROI.size(); ++roi) {
                    std::string imgPath = (basePathPOI / bfs::path(isFocusLocalized ? imgNameLOC : imgNameROI)).string();
                    imgPath += "_" + std::to_string(roi) + itr->path().extension().string();
                    cv::imwrite(imgPath, img(facesROI[roi]));
                }
                logger << "Expected only 1 face from stills POI generation, found and generated " << facesROI.size()
                       << " ROI from '" << currentFilePath << "'" << std::endl;
                return EXIT_FAILURE;
            }
            poi = img(facesROI[0]);
            imgLoadedPath = (isFocusLocalized ? imgPathLOC : imgPathROI).string();
            cv::imwrite(imgLoadedPath, poi);  // available for next time
        }
        // otherwise, load the found POI image and remove any suffix
        else {
            poi = cv::imread(currentFilePath, cv::IMREAD_GRAYSCALE);
            imgNameEndROI = imgName.substr(imgName.length() - roiSuffix.length());
            imgNameEndLOC = imgName.substr(imgName.length() - locSuffix.length());
            if (imgNameEndROI == roiSuffix) imgName = imgName.substr(0, imgName.length() - roiSuffix.length());
            if (imgNameEndLOC == locSuffix) imgName = imgName.substr(0, imgName.length() - locSuffix.length());
        }

        if (poi.size() == cv::Size()) continue;
        if (contains(IDs, imgName)) continue;   // avoid re-adding POI ROI if already added from previous generation
        IDs.push_back(imgName);
        ROIs.push_back(GET_UMAT(poi, cv::ACCESS_READ));
    }
    return EXIT_SUCCESS;
}

Rect getBiggestSquare(const Rect& r1, const Size& imageSize)
{
    Rect resizedRect;
    int difference;

    if (r1.width < r1.height) {
        difference = r1.height - r1.width;
        resizedRect.width = r1.height;
        resizedRect.height = r1.height;
        resizedRect.x = (int)(r1.x - difference / 2);
        resizedRect.y = r1.y;
    }
    if (r1.width > r1.height) {
        difference = r1.width - r1.height;
        resizedRect.height = r1.width;
        resizedRect.height = r1.height;
        resizedRect.x = r1.x;
        resizedRect.y = (int)(r1.y - difference / 2);
    }

    if (resizedRect.x < 0)
        resizedRect.x = 0;
    if (resizedRect.y < 0)
        resizedRect.y = 0;
    if (resizedRect.x + resizedRect.width > imageSize.width)
        resizedRect.width = imageSize.width - resizedRect.x;
    if (resizedRect.y + resizedRect.height > imageSize.height)
        resizedRect.height = imageSize.height - resizedRect.y;

    return resizedRect;
}

void saveTrackROIToDisk(const Rect& roi, const FACE_RECOG_MAT& image, const string& label,
                        int trackNumber, int desiredSize, const string& dirPath)
{
    stringstream fileLocation, folderName;

    // roi = getBiggestSquare(roi, image.size());

    folderName << dirPath;
    createDirIfNotExist(folderName.str());
    folderName << "/person_" << trackNumber;
    createDirIfNotExist(folderName.str());

    FACE_RECOG_MAT resizedMat = image(roi);
    Size size(desiredSize, desiredSize);
    FACE_RECOG_NAMESPACE::resize(resizedMat, resizedMat, size, 0, 0, cv::INTER_AREA);

    fileLocation << folderName.str() << "/" << label << ".png";
    imwrite(fileLocation.str().c_str(), resizedMat);
}

Rect getConstSizedRect(const Rect& r1, int sideLength, const Size& imageSize)
{
    Rect resizedRect;
    int difference = r1.height - sideLength;
    resizedRect.x = (int)(r1.x + difference / 2);
    resizedRect.y = (int)(r1.y + difference / 2);
    resizedRect.height = sideLength;
    resizedRect.width = sideLength;

    if (resizedRect.x < 0)
        resizedRect.x = 0;
    if (resizedRect.y < 0)
        resizedRect.y = 0;
    if (resizedRect.x + resizedRect.width > imageSize.width)
        resizedRect.width = imageSize.width - resizedRect.x;
    if (resizedRect.y + resizedRect.height > imageSize.height)
        resizedRect.height = imageSize.height - resizedRect.y;

    return resizedRect;
}

double overlap(const Rect& r1, const Rect& r2)
{
    double J = 0;
    int ax1 = r1.x;
    int ax2 = r1.x + r1.width - 1;
    int ay1 = r1.y;
    int ay2 = r1.y + r1.height - 1;
    int bx1 = r2.x;
    int bx2 = r2.x + r2.width - 1;
    int by1 = r2.y;
    int by2 = r2.y + r2.height - 1;
    if ((ax1 < bx2) && (ax2 > bx1) && (ay1 < by2) && (ay2 > by1))
    {
        int aInter = (min(ax2, bx2) - max(ax1, bx1) + 1) * (min(ay2, by2) - max(ay1, by1) + 1);
        int aUnion = r1.area() + r2.area() - aInter;
        J = (double)aInter / (double)aUnion;
    }
    return J;
}

double rectDist(const Rect& r1, const Rect& r2)
{
    double J = overlap(r1, r2);
    return J > 0 ? 1.f - J : 1.f;
}

#if FACE_RECOG_EXPERIMENTAL_MERGEDET
vector<Rect> mergeDetections(vector<vector<Rect> >& combo, double overlapThreshold, bool frontalOnly)
{
    vector<Rect> detections;

    // assume only the first detector as frontal if specified
    size_t nDetectors = frontalOnly ? 1 : combo.size();

    // loop over face detector models
    for (size_t d = 0; d < nDetectors; ++d) {
        for (size_t i = 0; i < combo[d].size(); ++i) {
            // first detector as reference for initial detections
            if (d == 0)
                detections.push_back(combo[0][i]);
            else {
                // test other detector detections against first one
                for (size_t j = 0; j < detections.size(); ++j) {
                    Rect oldDet = detections[j];
                    Rect newDet = combo[d][i];
                    if (util::intersect(oldDet, newDet, overlapThreshold))
                        break; // do not insert new detection already seen
                }
                // add new detections not detected by previous detectors
                if (j >= detections.size())
                    detections.push_back(combo[d][i]);
            }
        }
    }
    return detections;
}
#else/*!FACE_RECOG_EXPERIMENTAL_MERGEDET*/
/*TODO: this has to be done for a generic number of faces
(this implementation is for compatibility with Duffner&Obodez experiments)*/
vector<Rect> mergeDetections(vector<vector<Rect> >& combo, double overlapThreshold, bool frontalOnly)
{
    vector<Rect> result;
    vector<Rect> frontLeft;

    // copy all frontal faces to temporary vector
    for (size_t i = 0; i < combo[0].size(); ++i)
        frontLeft.push_back(combo[0][i]);
    if (frontalOnly)
        return frontLeft;

    // loop over left profile faces
    for (size_t i = 0; i < combo[1].size(); ++i)
    {
        size_t j = 0;
        // loop over frontal faces
        for (j = 0; j < combo[0].size(); ++j)
        {
            Rect frontal = combo[0][j];
            Rect left = combo[1][i];
            if (util::intersect(left, frontal, overlapThreshold))
                break; // do not insert left face into final result
        }
        if (j >= combo[0].size()) // there was no overlap
            frontLeft.push_back(combo[1][i]); // insert left face into temporry result
    }

    // loop over right profile faces
    for (size_t i = 0; i < combo[2].size(); ++i)
    {
        size_t j = 0;
        // loop over left and frontal faces
        for (j = 0; j < frontLeft.size(); ++j)
        {
            Rect alreadySeen = frontLeft[j];
            Rect right = combo[2][i];
            if (util::intersect(alreadySeen, right, overlapThreshold))
                break;
        }
        if (j >= frontLeft.size())
            result.push_back(combo[2][i]);
    }

    // add back front and left
    for (size_t i = 0; i < frontLeft.size(); ++i)
        result.push_back(frontLeft[i]);
    return result;
}
#endif/*FACE_RECOG_EXPERIMENTAL_MERGEDET*/

string rectPointCoordinates(const Rect& r, const string& sep)
{
    return to_string(r.x) + sep + to_string(r.y) + sep + to_string(r.br().x) + sep + to_string(r.br().y);
}

void checkBoundaries(vector<Track>& trackList, int windowWidth, int windowHeight, int xLim, int yLim)
{
    for (unsigned int i = 0; i < trackList.size(); ++i)
    {
        Rect rect = trackList[i].bbox();
        int x1 = rect.x;
        int x2 = rect.x + rect.width - 1;
        int y1 = rect.y;
        int y2 = rect.y + rect.height - 1;
        if ((x1 - xLim <= 0) || (y1 - yLim <= 0) ||
            (x2 + xLim >= windowWidth) || (y2 + yLim >= windowHeight))
        {
            trackList.erase(trackList.begin() + i);
            i--;
        }
    }
}

void mergeOverlappingTracks(vector<Track>& trackList, double minOverlap, FACE_RECOG_MAT& image)
{
    /*The trackers are removed if they overlap and the overlapping area is more than half of the rectangle*/
    for (unsigned int i = 0; i < trackList.size(); ++i)
    {
        for (unsigned int j = i + 1; j < trackList.size(); ++j)    // Compare with the Rects to the right
        {
            Rect r1 = trackList[i].bbox();
            Rect r2 = trackList[j].bbox();
            if (util::intersect(r1, r2, minOverlap))
            {
                Rect result;
                result.x = (r1.x + r2.x) / 2; // take the average
                result.y = (r1.y + r2.y) / 2; // take the average
                result.width = (r1.width + r2.width) / 2; // take the average
                result.height = (r1.height + r2.height) / 2; // take the average
                // if they overlap and the j-th rectangle is smaller remove j-th rectangle, otherwise remove the i-th
                if (r1.area() >= r2.area())
                {
                    // eliminate j-th rectangle, keep on looking for other overlapping rectangles
                    // before eliminating, add to the non-genuine list
                    trackList.erase(trackList.begin() + j);
                    j--;
                }
                else // if the i-th rectangle is smaller, just eliminate it and pass to the next element
                {
                    trackList.erase(trackList.begin() + i);
                    i--;
                    break;
                }
            }
        } // j loop
    } // i  loop
}

/* OpenCL device printing available only under OpenCV 3 */
#if CV_VERSION_MAJOR == 3
string getDeviceTypeString(cv::ocl::Device device)
{
    switch (device.type())
    {
        case cv::ocl::Device::TYPE_DEFAULT:     return "Default";
        case cv::ocl::Device::TYPE_ALL:         return "All";
        case cv::ocl::Device::TYPE_ACCELERATOR: return "Accelerator";
        case cv::ocl::Device::TYPE_CPU:         return "CPU";
        case cv::ocl::Device::TYPE_DGPU:        return "DGPU";
        case cv::ocl::Device::TYPE_IGPU:        return "IGPU";
        case cv::ocl::Device::TYPE_GPU:         return "GPU";
        default:                                return "Unknown";
    }
}

string getCudaSupportString(cv::cuda::DeviceInfo device)
{
    string CC =
        device.supports(cuda::FEATURE_SET_COMPUTE_10) ? "1.0 " : "" +
        device.supports(cuda::FEATURE_SET_COMPUTE_11) ? "1.1 " : "" +
        device.supports(cuda::FEATURE_SET_COMPUTE_12) ? "1.2 " : "" +
        device.supports(cuda::FEATURE_SET_COMPUTE_13) ? "1.3 " : "" +
        device.supports(cuda::FEATURE_SET_COMPUTE_20) ? "2.0 " : "" +
        device.supports(cuda::FEATURE_SET_COMPUTE_21) ? "2.1 " : "" +
        device.supports(cuda::FEATURE_SET_COMPUTE_30) ? "3.0 " : "" +
        device.supports(cuda::FEATURE_SET_COMPUTE_32) ? "3.2 " : "" +
        device.supports(cuda::FEATURE_SET_COMPUTE_35) ? "3.5 " : "" +
        device.supports(cuda::FEATURE_SET_COMPUTE_50) ? "5.0 " : "";
    if (CC != "")
        CC += "(above 5.0 not available)";
    return CC;
}

void printDeviceInfo(cv::ocl::Device device, string tab, ostream& out)
{
    string typeName = device.isAMD()    ? "AMD" :
                      device.isIntel()  ? "Intel" :
                      device.isNVidia() ? "NVidia" : "Unknown";
    out << tab << "name:                  " << device.name() << endl;
    out << tab << "type name:             " << typeName << endl;
    out << tab << "type:                  " << getDeviceTypeString(device) << endl;
    out << tab << "available:             " << device.available() << endl;
    out << tab << "imageSupport:          " << device.imageSupport() << endl;
    out << tab << "version:               " << device.version() << endl;
    out << tab << "driverVersion:         " << device.driverVersion() << endl;
    out << tab << "executionCapabilities: " << device.executionCapabilities() << endl;
    out << tab << "compilerAvailable:     " << device.compilerAvailable() << endl;
    out << tab << "linkerAvailable:       " << device.linkerAvailable() << endl;
    out << tab << "OpenCL_C_Version:      " << device.OpenCL_C_Version() << endl;
}

void printCudaDeviceInfo(cv::cuda::DeviceInfo device, string tab, ostream& out)
{
    out << tab << "name:                        " << device.name() << endl;
    out << tab << "type name:                   " << "NVidia CUDA" << endl;
    out << tab << "deviceID:                    " << device.deviceID() << endl;
    out << tab << "supports:                    " << getCudaSupportString(device) << endl;
    out << tab << "isCompatible:                " << device.isCompatible() << endl;
    out << tab << "version:                     " << device.majorVersion() << "." << device.minorVersion() << endl;
    out << tab << "multiProcessorCount:         " << device.multiProcessorCount() << endl;
    out << tab << "maxThreadsDim:               " << device.maxThreadsDim() << endl;
    out << tab << "maxThreadsPerBlock:          " << device.maxThreadsPerBlock() << endl;
    out << tab << "maxThreadsPerMultiProcessor: " << device.maxThreadsPerMultiProcessor() << endl;
}

void setAndDisplayDevices(int deviceIndex, ostream& out)
{
    const int right = 60;
    const string tab1 = "  ";
    const string tab2 = tab1 + tab1;
    const string tab3 = tab1 + tab2;
    const string tab4 = tab2 + tab2;
    const string sep0 = string(right, '*');
    const string sep1 = tab1 + string(right - tab1.length(), '=');
    const string sep2 = tab2 + string(right - tab2.length(), '-');
    const string sep3 = tab3 + string(right - tab3.length(), '-');

    out << sep0 << endl << "Devide Information" << endl << sep1 << endl;
    out << tab1 << "Number of CPU:     " << cv::getNumberOfCPUs() << endl;
    out << tab1 << "Number of Threads: " << cv::getNumThreads() << endl;

    // Try with any OpenCL recognized devices
    out << sep1 << endl << tab1 << "Checking OpenCL availability..." << endl;
    if (!cv::ocl::haveOpenCL())
        out << tab2 << "OpenCL is not available." << endl;
    else
    {
        // OpenCL basic status
        out << tab2 << "OpenCL available." << endl;
        bool useOpenCL = cv::ocl::useOpenCL();
        out << tab2 << "OpenCL previous setting: " << useOpenCL << endl;
        if (useOpenCL)
            out << tab2 << "OpenCL already activated, resuming..." << endl;
        else
        {
            out << tab2 << "Activating OpenCL..." << endl;
            cv::ocl::setUseOpenCL(true);
            out << tab2 << "Verify OpenCL activated: " << cv::ocl::useOpenCL() << endl;
        }

        // OpenCL info with 'Context'
        out << sep2 << endl << tab2 << "Printing info using 'Context'..." << endl;
        cv::ocl::Context context;
        if (!context.create(cv::ocl::Device::TYPE_ALL))
            out << tab2 << "Failed creating the context." << endl;
        else
        {
            size_t ctxNumDevices = context.ndevices();
            if (ctxNumDevices <= 0)
                out << tab2 << "Failed detecting any device." << endl;
            else
            {
                out << tab2 << "Found OpenCL enabled device(s): " << ctxNumDevices << endl;
                for (size_t i = 0; i < ctxNumDevices; ++i)
                {
                    out << tab2 << "Device " << i << endl;
                    printDeviceInfo(context.device(i), tab3, out);
                }
            }

            // OpenCL setting GPU with input index
            out << sep2 << endl << tab2 << "Trying to set device with OpenCL..." << endl;
            context.create(cv::ocl::Device::TYPE_GPU | ocl::Device::TYPE_DGPU | ocl::Device::TYPE_IGPU);
            size_t ctxNumDevicesGPU = context.ndevices();
            if (ctxNumDevicesGPU > 0 && deviceIndex >= 0 && deviceIndex < ctxNumDevicesGPU)
            {
                out << tab2 << "Setting device to use with index " << deviceIndex << " (" << context.device(deviceIndex).name() << ")" << endl;
                // NOTE:
                //      Environment variable 'OPENCV_OPENCL_DEVICE' must be set to desired device type before running the code.
                //      The variable takes the following form:
                //
                //          OPENCV_OPENCL_DEVICE = <vendor>:<type>:<number>
                //
                //          <vendor> can be in the form 'NVIDIA', 'Intel', etc.
                //          <type>   can be specified as 'CPU', 'GPU', etc.
                //          <number> can be either the index (eg: 0, 1, etc.) or the card's number (eg: '650' for GTX650, etc.)
                //
                //      If only a partial device type is specified (ex: OPENCV_OPENCL_DEVICE = 'NVIDIA'), all matching devices will be returned
                //      If no device type is specified, OpenCL tends to only select the default primary device, and others are not available.
                //
                cv::ocl::Device(context.device(deviceIndex)); // Change GPU to use here
            }
            else
            {
                out << tab2 << "Specified GPU index (" << deviceIndex << ") is out of device range..." << endl;
                out << tab2 << "Will employ the GPU set by default (if any available)." << endl;
            }
        }

        // OpenCL info with 'PlatformInfo'
        out << sep2 << endl << tab2 << "Printing info using 'PlatformInfo'..." << endl;
        vector<cv::ocl::PlatformInfo> platforms;
        cv::ocl::getPlatfomsInfo(platforms);
        size_t nPlatforms = platforms.size();
        if (nPlatforms <= 0)
            out << tab2 << "Failed detecting any platform." << endl;
        else
        {
            out << tab2 << "Found " << nPlatforms << " platforms..." << endl;
            out << tab2 << "Getting devices for each platform..." << endl;
            for (size_t i = 0; i < nPlatforms; ++i)
            {
                cv::ocl::PlatformInfo plt = platforms[i];
                out << sep2 << endl;
                out << tab2 << "Platform " << i << ":" << endl;
                out << tab3 << "name:    " << plt.name() << endl;
                out << tab3 << "vendor:  " << plt.vendor() << endl;
                out << tab3 << "version: " << plt.version() << endl;
                size_t pltNumDevices = plt.deviceNumber();
                if (pltNumDevices <= 0)
                    out << tab3 << "Failed detecting any device for platform '" << plt.name() << "'." << endl;
                else
                {
                    out << tab3 << "Found OpenCL enabled device(s): " << pltNumDevices << endl;
                    for (size_t i = 0; i < pltNumDevices; ++i)
                    {
                        cv::ocl::Device device;
                        plt.getDevice(device, (int)i);
                        out << sep3 << endl;
                        out << tab3 << "Device " << i << endl;
                        printDeviceInfo(device, tab4, out);
                    }
                }
            }
        }
    }

    // Try specifically for CUDA devices
    out << sep1 << endl << tab1 << "Checking CUDA availability (using CUDA namespace)..." << endl;
    int cudaNumDevices = cuda::getCudaEnabledDeviceCount();
    if (cudaNumDevices <= 0)
        out << tab2 << "No CUDA enabled devices found." << endl;
    else
    {
        out << tab2 << "Found CUDA enabled device(s): " << cudaNumDevices << endl;
        out << tab2 << "Current CUDA device: " << cuda::getDevice() << endl;
        out << tab2 << "Printing information of CUDA device(s)..." << endl << sep2 ;
        for (int i = 0; i < cudaNumDevices; ++i)
        {
            out << tab2 << "Device " << i << endl;
            cuda::DeviceInfo device(i);
            printCudaDeviceInfo(device, tab3, out);
        }
        // Try setting the CUDA device
        out << sep2 << endl << tab2 << "Trying to set GPU device with CUDA..." << endl;
        if (cudaNumDevices > 0 && deviceIndex >= 0 && deviceIndex < cudaNumDevices)
        {
            out << "Setting CUDA device: " << deviceIndex << endl;
            cuda::setDevice(deviceIndex);
        }
        else
        {
            out << tab2 << "Specified GPU index (" << deviceIndex << ") is out of device range..." << endl;
            out << tab2 << "Will employ the GPU set by default (if any available)." << endl;
        }
    }

    // Display resulting device selection
    out << sep1 << endl;
    out << tab1 << "Displaying set OpenCL device for processing:" << endl;
    printDeviceInfo(cv::ocl::Device::getDefault(), tab2, out);

    out << sep0 << endl;
}
#endif /* CV_VERSION_MAJOR == 3 */
} // namespace util
