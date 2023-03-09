/*
 *  TU Eindhoven
 *  Eindhoven, The Netherlands
 *
 *  Name            :   Target.h
 *
 *  Author          :   Francesco Comaschi (f.comaschi@tue.nl)
 *
 *  Date            :   August 15, 2015
 *
 *  History         :
 *      15-08-15    :   Initial version.
 *
 *  FAST-DT: FAce STructured Detection and Tracking
 *
 *  Code to accompany the paper:
 *  Online Multi-Face Detection and Tracking using Detector Confidence and Structured SVMs
 *  F. Comaschi, S. Stuijk, T. Basten, H. Corporaal
 *  Advanced Video and Signal-Based Surveillance (AVSS), 2015
 *
 * This file is part of FAST-DT.
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program;  If not, see <http://www.gnu.org/licenses/>
 *
 * In other words, you are welcome to use, share and improve this program.
 * You are forbidden to forbid anyone else to use, share and improve
 * what you give them.   Happy coding!
 */

#ifndef FACE_RECOG_TRACK_H
#define FACE_RECOG_TRACK_H

#include <opencv/cv.h>

#include "Configs/ConfigFile.h"
#include "Trackers/ITracker.h"
#include "Trackers/TrackerCamshift.h"
#include "Trackers/TrackerCompressive.h"
#include "Trackers/TrackerFastDT.h"
#include "Tracks/ImageRep.h"
#include "Tracks/TrackROI.h"

#include <iostream>
#include <memory>

using namespace config;

class Track
{
public:
    Track(ConfigFile *configFile);
    Track(ConfigFile *configFile, const cv::Rect& rect, int trackNumber = -1);
    Track(const Track&);
    Track& operator= (const Track& track);
    ~Track();

    enum RecognizedState { UNKWOWN, CONSIDERED, RECOGNIZED };

    // getters
    inline cv::Rect bbox() const                { return getROI().getRect(); }
    inline ROI getROI(size_t pos = 0) const     { return _bboxes.getROI(pos); }
    inline int getTrackNumber() const           { return _trackNumber; }
    inline int getCreateCount() const           { return _createCount; }
    inline int getRemoveCount() const           { return _removeCount; }
    inline bool isMatched() const               { return _isMatched; }
    // setters
    inline void setTrackNumber(int number)                  { _trackNumber = number; }        
    inline void markMatched()                               { _isMatched = true; _removeCount = 0; } // if matched to detections, reset removal count
    inline void markUnmatched()                             { _isMatched = false; }
    inline void insertROI(const ROI roi, size_t pos = 0)    { _bboxes.addROI(roi, pos); }   // add the ROI to the cumulated list at position or front
    inline void updateROI(const ROI roi, size_t pos = 0)    { _bboxes.setROI(roi, pos); }   // modify the specified ROI (replace, not insert)
    inline void setTrackSize(int trackSize)                 { _bboxes.setTrackSize(trackSize); }
    inline void setCreateCount(const int count)             { _createCount = count; }
    inline void setRemoveCount(const int count)             { _removeCount = count; }
    inline void increaseRemoveCount()                       { _removeCount++; }
    inline void increaseCreateCount()                       { _createCount++; }
    inline void markUnknown()                               { _recognizedState = UNKWOWN; }
    inline bool isUnknown()                                 { return _recognizedState == UNKWOWN; }
    inline void markConsidered()                            { _recognizedState = CONSIDERED; }
    inline bool isConsidered()                              { return _recognizedState == CONSIDERED; }
    inline void markRecognized()                            { _recognizedState = RECOGNIZED; }
    inline bool isRecognized()                              { return _recognizedState == RECOGNIZED; }
    inline void setName(std::string name)                   { _recognizedPOIName = name; }
    inline std::string getName()                            { return _recognizedPOIName; }
    inline void setValidateEyeDetection(bool valid = true)  { _isValidatedWithEyeDetection = valid; }
    inline bool isValidatedEyeDetection()                   { return _isValidatedWithEyeDetection; }
    // operations
    void reInitTracking(const ImageRep& frame);
    void track(const ImageRep& frame);
    // shared config
    ConfigFile *m_config;
    void configCheckAndSet(ConfigFile* configFile);

private:
    int _trackNumber = -1;
    std::string _recognizedPOIName; 
    TrackROI _bboxes;     // accumulation of ROI and sub-ROI
    int _createCount;
    RecognizedState _recognizedState = UNKWOWN;
    bool _isValidatedWithEyeDetection;
    int _removeCount;
    std::shared_ptr<ITracker> _tracker;
    bool _isMatched;    // tells if matched to detection
};

#endif /* FACE_RECOG_TRACK_H */
