/*
    Original Code Reference

        FAST-DT: FAce STructured Detection and Tracking
        Francesco Comaschi (f.comaschi@tue.nl)

    Code to accompany the paper:

        Online Multi-Face Detection and Tracking using Detector Confidence and Structured SVMs
        F. Comaschi, S. Stuijk, T. Basten, H. Corporaal
        Advanced Video and Signal-Based Surveillance (AVSS), 2015

    Multiple updates and modifications applied to introduce improvements or combined use with projet
*/

#ifndef FACE_RECOG_UTILITIES_H
#define FACE_RECOG_UTILITIES_H

#include "FaceRecog.h"
#include "Tracks/Track.h"

namespace util {

double overlap(const Rect& r1, const Rect& r2);
Rect getBiggestSquare(const Rect& r1, const Size& imageSize);
Rect getConstSizedRect(const Rect& r1, int sideLength, const Size& imageSize);
inline int intersect(const Rect& r1, const Rect& r2, double thresh) { return overlap(r1, r2) >= thresh; }
vector<Rect> mergeDetections(vector<vector<Rect> >& combo, double overlapThreshold, bool frontalOnly = false);
void mergeOverlappingTracks(vector<Track>& trackList, double minOverlap, FACE_RECOG_MAT& image);
void checkBoundaries(vector<Track>& trackList, int windowWidth, int windowHeight, int xLim, int yLim);
string rectPointCoordinates(const Rect& r, const string& sep = ",");
double rectDist(const Rect& r1, const Rect& r2);
void saveTrackROIToDisk(const Rect& roi, const FACE_RECOG_MAT& image, const string& label, int trackNumber, int desiredSize, const string& dirPath);
bool createDirIfNotExist(const string& dirName);
bool prepareFrameFileNames(vector<string>& fileNames, const string& framesRegexPath);
bool prepareTestSequences(vector<vector<string> >& sequenceFileNames, vector<string>& sequenceRegexPaths, const string& testFilePath);
int prepareEnrollROIs(const ConfigFile& config, bfs::path opencvSourcesDataPath, std::vector<std::vector<FACE_RECOG_MAT> >& POI_ROIs,
                      std::vector<std::string>& POI_IDs, std::vector<std::vector<FACE_RECOG_MAT> >& NEG_ROIs, logstream& logger);
int loadDirectoryROIs(const ConfigFile& config, bfs::path opencvSourcesDataPath, std::vector<FACE_RECOG_MAT>& ROIs,
                      std::vector<std::string>& IDs, logstream& logger, std::string alternativeROIPath = "");

/* OpenCL/CUDA device printing available only under OpenCV 3 */
#if CV_VERSION_MAJOR == 3
string getDeviceTypeString(ocl::Device device);
string getCudaSupportString(cuda::DeviceInfo device);
void printDeviceInfo(ocl::Device device, string tab = "", ostream& out = cout);
void printCudaDeviceInfo(cuda::DeviceInfo device, string tab = "", ostream& out = cout);
void setAndDisplayDevices(int deviceIndex, ostream& out = cout);
#endif /* CV_VERSION_MAJOR == 3 */

} // end namespace util

#endif /*FACE_RECOG_UTILITIES_H*/
