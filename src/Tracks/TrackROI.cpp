#include "Tracks/TrackROI.h"
#include <stdexcept>
#include <deque>
#include <iterator>

cv::Rect ROI::getSubRect(size_t pos) const
{
    if (pos < _subRoi.size())
        return _subRoi[pos];
    
    throw std::out_of_range("Index out of sub-ROI range");
}

void ROI::addSubRect(cv::Rect roi, size_t pos)
{
    _subRoi.insert(_subRoi.begin() + pos, roi);
}

size_t ROI::countSubROI() const
{
    return _subRoi.size();        
}

void ROI::setAllSubRect(std::vector<cv::Rect> vRect)
{
    _subRoi.clear();    
    _subRoi.assign(vRect.begin(), vRect.end());
}

std::vector<cv::Rect> ROI::getAllSubRect() const
{
    std::vector<cv::Rect> rects(_subRoi.begin(), _subRoi.end());
    return rects;
}

TrackROI::TrackROI(size_t trackSize)
{
    init(trackSize);
}

TrackROI::TrackROI(size_t trackSize, cv::Rect roi)
{
    init(trackSize);
    addROI(roi);
}

TrackROI::TrackROI(size_t trackSize, ROI roi)
{
    init(trackSize);
    addROI(roi);
}

void TrackROI::setTrackSize(size_t trackSize)
{
    if (trackSize < 1) return;
    if (trackSize < _trackSize)
    {
        while (trackSize < _roiCumul.size())
            _roiCumul.pop_back();
    }
    _trackSize = trackSize;
}

void TrackROI::addROI(cv::Rect roi, size_t pos)
{    
    addROI(ROI(roi), pos);
}

void TrackROI::addROI(ROI roi, size_t pos)
{
    if (pos >= _trackSize) return;

    _roiCumul.insert(_roiCumul.begin() + pos, roi);

    if (_roiCumul.size() > _trackSize)
        _roiCumul.pop_back();
}

void TrackROI::setROI(cv::Rect roi, size_t pos)
{
    setROI(ROI(roi), pos);
}

void TrackROI::setROI(ROI roi, size_t pos)
{
    if (pos >= _trackSize) return;
    if (pos < _roiCumul.size())
        _roiCumul[pos] = roi;
    else
        addROI(roi, pos);
}

ROI TrackROI::getROI(size_t pos) const
{
    if (pos < _roiCumul.size())
        return _roiCumul[pos];

    throw std::out_of_range("Index out of accumulated ROI range");
}

cv::Rect TrackROI::getRect(size_t pos) const
{
    return getROI(pos).getRect();
}

size_t TrackROI::countROI() const
{
    return _roiCumul.size();
}
