#ifndef FACE_RECOG_TRACKER_CAMSHIFT_H
#define FACE_RECOG_TRACKER_CAMSHIFT_H

#include "FaceRecog.h"


class ImageRep;

class TrackerCamshift final : public ITracker
{
public:    
	TrackerCamshift(ConfigFile *configFile);
	virtual ~TrackerCamshift();
	TrackerCamshift( const TrackerCamshift &obj);  // copy constructor
	TrackerCamshift & operator=(const TrackerCamshift &T); // assignment operator
	virtual void initialize(const ImageRep& frame, FloatRect bb);
	virtual void reset();
	virtual cv::Rect track(const ImageRep& frame);
private:
	cv::Rect rectInsideFrame(cv::Mat frame, cv::Rect rect);
	cv::Rect rectInsideFrame(cv::UMat frame, cv::Rect rect);
	cv::TermCriteria term_crit;
	cv::MatND roi_hist;
    // we compute the histogram for all 3 channels channels
    const int channels[3] = {0, 1, 2};
    // Quantize the hue to 30 levels
    // and the saturation to 32 levels
    const int hbins = 30, sbins = 32;
    const int histSize[2] = {hbins, sbins};
    // hue varies from 0 to 179, see cvtColor
    const float hranges[2] = { 0, 180 };
    // saturation varies from 0 (black-gray-white) to
    // 255 (pure spectrum color)
    const float sranges[2] = { 0, 256 };
    const float* ranges[2] = { hranges, sranges };
};

#endif /*FACE_RECOG_TRACKER_CAMSHIFT_H*/
