/*
    Code modified for FaceRecod project

    Original code adaptation reference:
        FAST-DT: FAce STructured Detection and Tracking
        Francesco Comaschi (f.comaschi@tue.nl)

        * Code to accompany the paper:
            Online Multi-Face Detection and Tracking using Detector Confidence and Structured SVMs
            F. Comaschi, S. Stuijk, T. Basten, H. Corporaal
            Advanced Video and Signal-Based Surveillance (AVSS), 2015

    Original work:
        STRUCK: Structured Output Tracking with Kernels
        Sam Hare, Amir Saffari, Philip H. S. Torr
        International Conference on Computer Vision (ICCV), 2011
        Copyright (C) 2011 Sam Hare, Oxford Brookes University, Oxford, UK
 */

#ifdef FACE_RECOG_HAS_STRUCK

#include "Trackers/TrackerSTRUCK.h"
#include "FaceRecog.h"

TrackerSTRUCK::TrackerSTRUCK(ConfigFile* configFile) :
    m_pLearner(0),
    m_needsIntegralImage(false)
{
    m_initialized = false;
    updateConfig(configFile);
    TrackerSTRUCK::reset();
}

TrackerSTRUCK::TrackerSTRUCK(const TrackerSTRUCK &obj)
{
    this->m_config = obj.m_config;
    this->m_initialized = obj.m_initialized;
    this->m_bb = obj.m_bb;
    if (m_pLearner != NULL) delete m_pLearner;
    for (size_t i = 0; i < m_features.size(); ++i)
    {
        delete m_features[i];
        delete m_kernels[i];
    }
    m_features.clear();
    m_kernels.clear();
    this->m_needsIntegralHist = obj.m_needsIntegralHist;
    this->m_needsIntegralImage = obj.m_needsIntegralImage;
    m_features.resize(obj.m_features.size());
    for (size_t i = 0; i < obj.m_features.size(); i++)
    {
        m_features[i] = new HaarFeatures();
        *m_features[i] = *obj.m_features[i];
    }
    m_kernels.resize(obj.m_kernels.size());
    for (size_t i = 0; i < obj.m_kernels.size(); i++)
    {
        m_kernels[i] = new Kernel(0);
        *m_kernels[i] = *obj.m_kernels[i];
    }
    m_pLearner = new LaRank(m_config, *m_features.back(), *m_kernels.back());
    *m_pLearner = *obj.m_pLearner;
}

TrackerSTRUCK & TrackerSTRUCK::operator=(const TrackerSTRUCK &obj)
{
    // check for "self assignment" and do nothing in that case
    if (this == &obj) return *this;
    else
    {
        this->m_config = obj.m_config;
        this->m_initialized = obj.m_initialized;
        this->m_bb = obj.m_bb;
        this->m_needsIntegralHist = obj.m_needsIntegralHist;
        this->m_needsIntegralImage = obj.m_needsIntegralImage;
        for (size_t i = 0; i < m_features.size(); ++i)  // free the storage pointed to by m_features
        {
            if (m_features[i] != NULL)
                delete m_features[i];
            if (m_kernels[i] != NULL)
                delete m_kernels[i];
        }
        m_features.clear();
        m_kernels.clear();
        m_features.resize(obj.m_features.size());
        for (size_t i = 0; i < obj.m_features.size(); i++)
        {
            m_features[i] = new HaarFeatures();
            *m_features[i] = *obj.m_features[i];
        }
        m_kernels.resize(obj.m_kernels.size());
        for (size_t i = 0; i < obj.m_kernels.size(); i++)
        {
            m_kernels[i] = new Kernel(0);
            *m_kernels[i] = *obj.m_kernels[i];
        }

        if (m_pLearner != 0)
            delete m_pLearner;
        this->m_pLearner = new LaRank(m_config, *m_features.back(), *m_kernels.back());
        *m_pLearner = *obj.m_pLearner;  // TODO: define assignment operator for LaRank
        return *this;                       // return this IntList
    }
}

TrackerSTRUCK::~TrackerSTRUCK()
{
    if (m_pLearner != 0)
        delete m_pLearner;
    for (size_t i = 0; i < m_features.size(); ++i)
    {
        delete m_features[i];
        delete m_kernels[i];
    }
}

void TrackerSTRUCK::reset()
{
    m_initialized = false;

    if (m_pLearner != NULL) delete m_pLearner;
    for (size_t i = 0; i < m_features.size(); ++i)
    {
        delete m_features[i];
        delete m_kernels[i];
    }
    m_features.clear();
    m_kernels.clear();

    m_needsIntegralImage = false;
    m_needsIntegralHist = false;

    size_t numFeatures = m_config->features.size();

    vector<int> featureCounts;
    for (size_t i = 0; i < numFeatures; ++i)
    {
        if (m_config->features[i].feature != ConfigFile::kFeatureTypeHaar)
        {
            cout << "Error in tracking, only Haar features supported in current implementation" << endl;
            return;
        }
        m_features.push_back(new HaarFeatures());
        m_needsIntegralImage = true;
        featureCounts.push_back(m_features.back()->getCount());
        if (m_config->features[i].kernel != ConfigFile::kKernelTypeGaussian)
        {
            cout << "Error in tracking, only Gaussian kernel supported in current implementation" << endl;
            return;
        }
        m_kernels.push_back(new Kernel(m_config->features[i].params[0]));
    }
    m_pLearner = new LaRank(m_config, *m_features.back(), *m_kernels.back());
}


void TrackerSTRUCK::initialize(const ImageRep& image, FloatRect bb)
{
    if (m_initialized)
        reset();
    m_bb = IntRect(bb);
    for (int i = 0; i < 1; ++i)
        updateLearner(image);
    m_initialized = true;
}


cv::Rect TrackerSTRUCK::track(const ImageRep& image)
{
    assert(m_initialized);
    assert(m_config);
    vector<FloatRect> rects = Sampler::PixelSamples(m_bb, m_config->searchRadius);
    vector<FloatRect> keptRects;
    keptRects.reserve(rects.size());
    for (size_t i = 0; i < rects.size(); ++i)
    {
        if (!rects[i].isInside(image.getRect())) continue;
        keptRects.push_back(rects[i]);
    }
    MultiSample sample(image, keptRects);
    vector<double> scores;
    m_pLearner->eval(sample, scores);
    double bestScore = -DBL_MAX;
    size_t bestInd = -1;
    for (size_t i = 0; i < keptRects.size(); ++i)
    {
        if (scores[i] > bestScore)
        {
            bestScore = scores[i];
            bestInd = i;
        }
    }
    if (bestInd != -1)
    {
        m_bb = keptRects[bestInd];
        updateLearner(image);
    }
    cv::Rect bbox;
    bbox.x = (int)m_bb.xmin();
    bbox.y = (int)m_bb.ymin();
    bbox.width = (int)m_bb.width();
    bbox.height = (int)m_bb.height();
    return bbox;
}

void TrackerSTRUCK::updateLearner(const ImageRep& image)
{
    // note these returns the centre sample (m_bb) at index 0
    vector<FloatRect> rects = Sampler::RadialSamples(m_bb, 2 * m_config->searchRadius, 5, 16);
    vector<FloatRect> keptRects;
    keptRects.push_back(rects[0]); // the true sample
    for (size_t i = 1; i < rects.size(); ++i)
    {
        if (!rects[i].isInside(image.getRect())) continue;
        keptRects.push_back(rects[i]);
    }
    MultiSample sample(image, keptRects);
    m_pLearner->update(sample, 0);
}

#endif/*FACE_RECOG_HAS_STRUCK*/
