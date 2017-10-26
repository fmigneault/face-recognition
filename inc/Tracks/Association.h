#ifndef FACE_RECOG_ASSOCIATION_H
#define FACE_RECOG_ASSOCIATION_H

#include "Utilities/Common.h"
#include "Configs/ConfigFile.h"
#include "Tracks/ImageRep.h"
#include "Tracks/Track.h"

class Association
{
public:

    Association(ConfigFile* config);
    ~Association();

    void extendSet(std::vector<Track>& tracks, std::vector<cv::Rect>& detections);
    int computeCost(std::vector<Track>& tracks, std::vector<cv::Rect>& detections);
    int matchTracks(std::vector<Track>& tracks, std::vector<cv::Rect>& detections,
                     std::vector<cv::Rect>& unmatched, ImageRep& frame);
    void matchCandidates(std::vector<Track>& tracks, std::vector<cv::Rect>& detections,
                         std::vector<Track>& unmatched);
    void reduceSet(std::vector<Track>& tracks);
private:
    ConfigFile* _config;
    int **costMatrix;
    int detTrackThresh;
    size_t m;
    size_t n;
    void clear();
};

#endif /*FACE_RECOG_ASSOCIATION_H*/
