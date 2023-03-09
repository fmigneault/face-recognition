#ifndef FACE_RECOG_CIRCULAR_BUFFER_H
#define FACE_RECOG_CIRCULAR_BUFFER_H

#include "Utilities/MatDefines.h"
#include <boost/circular_buffer.hpp>
#include <map>
#include <vector>

enum ScoreMode { RAW = 0, CUMUL = 1, AVG = 2 };

class CircularBuffer
{
public:
	CircularBuffer(size_t scoreWindowSizeReceived = 1);
	void addPredictions(size_t trackNumber, std::vector<double> scores);                    // add newly predicted raw scores
    void removeTrackScores(size_t trackNumber);                                             // remove track number scores buffers if existing
    std::vector<double> getWindowScores(size_t trackNumber, size_t POINumber);              // raw scores memorized along 'scoreWindowSize'
    // general methods
    double getMaxScore(size_t trackNumber, size_t POINumber);                               // maximum raw score memorized along 'scoreWindowSize'
    double getScore(ScoreMode sm, size_t trackNumber, size_t POINumber);                    // general getter for any score type (dispatch calls)
    double getMaxPositiveScore(ScoreMode sm, size_t trackNumber, size_t POINumber);         
    int getMaxPositiveIndex(ScoreMode sm, size_t trackNumber);
    void getMaxPositiveInfo(ScoreMode sm, size_t trackNumber, int& index, double& score);   // index as 'int' to allow '-1' on unmatched info
    // calling methods
    double getRawScore(size_t trackNumber, size_t POINumber);
    double getCumulScore(size_t trackNumber, size_t POINumber);
    double getAverageScore(size_t trackNumber, size_t POINumber);

private:
    void updateMax(size_t trackNumber, std::vector<double> scores);
    void updateCumul(size_t trackNumber);
    size_t scoreWindowSize;
	std::map<size_t, boost::circular_buffer<std::vector<double> > > latestWindowScores;    // latest raw scores memorized along 'scoreWindowSize'	
    std::map<size_t, std::vector<double> > maxScores;                                      // max score obtained for a given trackNumber/POINumber
    std::map<size_t, std::vector<double> > latestCumulScores;                              // most recent calculated accumulation scores
};

#endif /*FACE_RECOG_CIRCULAR_BUFFER_H*/
