#ifdef FACE_RECOG_HAS_KCF

#include "Trackers/TrackerKCF.h"
#include "FaceRecog.h"

TrackerKCF::TrackerKCF(ConfigFile* configFile) :
    isInitialized(false)
{
    updateConfig(configFile);
    TrackerKCF::reset();
}

TrackerKCF::TrackerKCF(const TrackerKCF &obj) :
    isInitialized(false)
{

}

TrackerKCF & TrackerKCF::operator=(const TrackerKCF &obj)
{
    // check for "self assignment" and do nothing in that case
    if (this == &obj)
        return *this;
    else
        return *this;   // return this IntList
}

TrackerKCF::~TrackerKCF()
{

}

void TrackerKCF::reset()
{

}

void TrackerKCF::initialize(const ImageRep& image, FloatRect bb)
{
    if (isInitialized)
        reset();

    isInitialized = true;
}


cv::Rect TrackerKCF::track(const ImageRep& image)
{
    assert(isInitialized);


    cv::Rect bbox;
    return bbox;
}

void TrackerKCF::updateLearner(const ImageRep& image)
{

}

#endif/*FACE_RECOG_HAS_KCF*/
