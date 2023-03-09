#include "opencv2/core/version.hpp"

#include <opencv/highgui.h>
#include <iostream>
#include <omp.h>

#include "Tracks/CircularBuffer.h"

using namespace cv;
using namespace std;

CircularBuffer::CircularBuffer(size_t scoreAccumulationWindowSize) { scoreWindowSize = scoreAccumulationWindowSize;  }

void CircularBuffer::addPredictions(size_t trackNumber, std::vector<double> scores)
{
    // Add Circular buffer if new
    if (latestWindowScores.count(trackNumber) < 1) {
        latestWindowScores[trackNumber] = boost::circular_buffer<std::vector<double> >(scoreWindowSize);
        latestCumulScores[trackNumber] = std::vector<double>(scores.size());
    }

    updateMax(trackNumber, scores);
    latestWindowScores[trackNumber].push_back(scores);
    updateCumul(trackNumber);
}

void CircularBuffer::removeTrackScores(size_t trackNumber)
{
    latestWindowScores.erase(trackNumber);
    latestCumulScores.erase(trackNumber);
    maxScores.erase(trackNumber);    
}

void CircularBuffer::updateMax(size_t trackNumber, std::vector<double> scores)
{
    if (maxScores.count(trackNumber) < 1)
        maxScores[trackNumber] = std::vector<double>(scores.size(), -DBL_MAX);

    for (size_t i = 0; i < scores.size(); ++i)
        if (scores[i] > maxScores[trackNumber][i])
            maxScores[trackNumber][i] = scores[i];
}

void CircularBuffer::updateCumul(size_t trackNumber)
{
    for (size_t poi = 0; poi < latestCumulScores[trackNumber].size(); ++poi) {
        double acc = 0;
        for (size_t cirIdx = 0; cirIdx < latestWindowScores[trackNumber].size(); ++cirIdx)
            acc += latestWindowScores[trackNumber][cirIdx][poi];
        latestCumulScores[trackNumber][poi] = acc;
    }
}

double CircularBuffer::getMaxScore(size_t trackNumber, size_t POINumber)
{
    return maxScores[trackNumber][POINumber];
}

double CircularBuffer::getRawScore(size_t trackNumber, size_t POINumber)
{
    return latestWindowScores[trackNumber][0][POINumber];
}

double CircularBuffer::getCumulScore(size_t trackNumber, size_t POINumber)
{
    return latestCumulScores[trackNumber][POINumber];
}

double CircularBuffer::getAverageScore(size_t trackNumber, size_t POINumber)
{
    return latestCumulScores[trackNumber][POINumber] / (double)scoreWindowSize;
}

double CircularBuffer::getScore(ScoreMode sm, size_t trackNumber, size_t POINumber)
{
    switch (sm) {
        case RAW:   return getRawScore(trackNumber, POINumber);
        case CUMUL: return getCumulScore(trackNumber, POINumber);
        case AVG:   return getAverageScore(trackNumber, POINumber);
        default:    return -DBL_MAX;
    }
}

void CircularBuffer::getMaxPositiveInfo(ScoreMode sm, size_t trackNumber, int& index, double& score)
{
    score = -DBL_MAX;
    index = -1;

    if (latestWindowScores[trackNumber].size() < 1) return;

    for (size_t POINumber = 0; POINumber < latestWindowScores[trackNumber][0].size(); ++POINumber)
    {
        double s = getScore(sm, trackNumber, POINumber);
        if (s > score) {
            score = s;
            index = (int)POINumber;
        }
    }
}

double CircularBuffer::getMaxPositiveScore(ScoreMode sm, size_t trackNumber, size_t POINumber)
{    
    int index = -1; double score = -DBL_MAX;
    getMaxPositiveInfo(sm, trackNumber, index, score);
    return score;
}

int CircularBuffer::getMaxPositiveIndex(ScoreMode sm, size_t trackNumber)
{
    int index = -1; double score = -DBL_MAX;
    getMaxPositiveInfo(sm, trackNumber, index, score);
    return index;
}

std::vector<double> CircularBuffer::getWindowScores(size_t trackNumber, size_t POINumber)
{
    size_t nScores = latestWindowScores[trackNumber].size();
    std::vector<double> scores(nScores);
    for (size_t s = 0; s < nScores; ++s)
        scores[s] = latestWindowScores[trackNumber][s][POINumber];
    return scores;
}
