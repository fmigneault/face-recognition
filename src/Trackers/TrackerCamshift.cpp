#ifdef FACE_RECOG_HAS_CAMSHIFT

#include "Trackers/TrackerCamshift.h"
#include "FaceRecog.h"

TrackerCamshift::TrackerCamshift(ConfigFile* configFile)
{
    m_initialized = false;
    updateConfig(configFile);
    TrackerCamshift::reset();
}

TrackerCamshift::TrackerCamshift(const TrackerCamshift &obj)
{
    this->m_initialized = obj.m_initialized;
    this->m_bb = obj.m_bb;
    this->term_crit = obj.term_crit;
    this->roi_hist = obj.roi_hist;
}

TrackerCamshift & TrackerCamshift::operator=(const TrackerCamshift &obj)
{
  // check for "self assignment" and do nothing in that case
    if (this == &obj) return *this;
    else
    {
        this->m_initialized = obj.m_initialized;
        this->m_bb = obj.m_bb;
        this->term_crit = obj.term_crit;
        this->roi_hist = obj.roi_hist;
        return *this;                   // return this IntList
    }
}

TrackerCamshift::~TrackerCamshift() { }

void TrackerCamshift::reset() { }

void TrackerCamshift::initialize(const ImageRep& image, FloatRect bb)
{
    term_crit = cv::TermCriteria(cv::TermCriteria::COUNT | cv::TermCriteria::EPS, 10, 1.0);
    FACE_RECOG_MAT totalImage, roi, hsv_roi;

    // Create bounding box
    m_bb = bb;
    totalImage = GET_UMAT(image.getColourImage(), ACCESS_READ);
    cv::Rect bboxInsideFrame = rectInsideFrame(totalImage, m_bb.toCvRect());
    roi = totalImage(bboxInsideFrame);
    FACE_RECOG_NAMESPACE::cvtColor(roi, hsv_roi, CV_BGR2HSV);

    cv::calcHist(&GET_MAT(hsv_roi, ACCESS_READ), 1, channels, cv::Mat(), roi_hist, 2, histSize, ranges, true, false);
    cv::normalize(roi_hist, roi_hist, 0, 255, cv::NORM_MINMAX);

    m_initialized = true;
}

cv::Rect TrackerCamshift::track(const ImageRep& image)
{
    FACE_RECOG_MAT backProjFrame, hsvFrame;
    FACE_RECOG_NAMESPACE::cvtColor(image.getColourImage(), hsvFrame, CV_BGR2HSV);

    cv::calcBackProject(&GET_MAT(hsvFrame, ACCESS_READ), 1, channels, roi_hist, backProjFrame, ranges);
    cv::RotatedRect rotRec = cv::CamShift(backProjFrame, m_bb.toCvRect(), term_crit);

    return rectInsideFrame(image.getColourImage(), rotRec.boundingRect());
}

cv::Rect TrackerCamshift::rectInsideFrame(const FACE_RECOG_MAT& frame, cv::Rect rect)
{
    cv::Rect bboxInsideFrame = rect;
    if (rect.x + rect.width > frame.cols)
        bboxInsideFrame.x = frame.cols - rect.width;
    if (rect.y + rect.height > frame.rows)
        bboxInsideFrame.y = frame.rows - rect.height;
    if (rect.y < 0)
        bboxInsideFrame.y = 0;
    if (rect.x < 0)
        bboxInsideFrame.x = 0;
    return bboxInsideFrame;
}

#endif/*FACE_RECOG_HAS_CAMSHIFT*/
