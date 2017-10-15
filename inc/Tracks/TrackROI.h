#ifndef FACE_RECOG_TRACK_ROI_H
#define FACE_RECOG_TRACK_ROI_H

#include "FaceRecog.h"

/* Bounding box containers for accumulation of ROIs and sub-ROIs
 *   Default position (-1) will put the ROI/sub-ROI at the start of the container's accumulated ROIs (most recent first)
 *   Otherwise, the specified position is overwritten with the new ROI/sub-ROI
 *   Sub-ROIs are saved under their corresponding parent ROI */
class ROI
{
public:
    // constructors
    inline ROI() { }
    inline ROI(cv::Rect roi) { setRect(roi); }
    // setters
    inline void setRect(cv::Rect roi) { _roi = roi; }
    inline void updateROI(cv::Rect roi) { _roiOriginal = _roi; setRect(roi); }
    void addSubRect(cv::Rect roi, size_t pos = 0);
    void setAllSubRect(std::vector<cv::Rect> vRect);
    // getters
    inline cv::Rect getRect() const { return _roi; }
    inline cv::Rect getOriginalRect() const { return isUpdatedROI() ? _roiOriginal : _roi; }
    inline bool isUpdatedROI() const { return _roiOriginal.area() > 0; }
    cv::Rect getSubRect(size_t pos = 0) const;
    std::vector<cv::Rect> getAllSubRect() const;
    size_t countSubROI() const;
private:
    cv::Rect _roi;                  // main ROI
    cv::Rect _roiOriginal;          // retain the last version of main ROI
    std::deque<cv::Rect> _subRoi;   // to contain sub-region of main ROI
};

class TrackROI
{
public:
    // constructors
    inline TrackROI() { init(1); }
    TrackROI(size_t trackSize);
    TrackROI(size_t trackSize, cv::Rect roi);
    TrackROI(size_t trackSize, ROI roi);
    // setters
    void setTrackSize(size_t trackSize);
    void addROI(cv::Rect roi, size_t pos = 0);  // insert before position, front by default
    void addROI(ROI roi, size_t pos = 0);       // insert before position, front by default
    void setROI(cv::Rect, size_t pos = 0);      // update at position, latest (most recently added) by default
    void setROI(ROI roi, size_t pos = 0);       // update at position, latest (most recently added) by default
    // getters
    inline size_t getTrackSize() const { return _trackSize; }
    ROI getROI(size_t pos = 0) const;
    cv::Rect getRect(size_t pos = 0) const;
    size_t countROI() const;
private:
    inline void init(size_t trackSize) { _trackSize = trackSize; }
    std::deque<ROI> _roiCumul;
    size_t _trackSize;
};

#endif /*FACE_RECOG_TRACK_ROI_H*/
