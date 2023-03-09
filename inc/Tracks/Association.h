#ifndef FACE_RECOG_ASSOCIATION_H
#define FACE_RECOG_ASSOCIATION_H

#include "Tracks/Hungarian.h"
#include "Tracks/Track.h"
#include <opencv/cv.h>

using namespace config;

class Association
{
public:

	Association(ConfigFile *config);
	~Association();

	void extendSet(std::vector<Track>& tracks, std::vector<cv::Rect>& detections);
	int computeCost(std::vector<Track>& tracks, std::vector<cv::Rect>& detections);
	int matchTracks(std::vector<Track>& tracks, std::vector<cv::Rect>& detections,
			         std::vector<cv::Rect>& unmatched, ImageRep& frame );
	void matchCandidates(std::vector<Track>& tracks, std::vector<cv::Rect>& detections,
				         std::vector<Track>& unmatched);
	void reduceSet(std::vector<Track>& tracks);
    ConfigFile *m_config;
private:
	int **costMatrix;
	int detTrackThresh;
	size_t m;
	size_t n;
	void clear();    
};

#endif /*FACE_RECOG_ASSOCIATION_H*/
