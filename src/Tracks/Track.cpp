/*
 *  TU Eindhoven
 *  Eindhoven, The Netherlands
 *
 *  Name            :   Track.h (old 'Target.h')
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

#include "Tracks/Track.h"
#include "FaceRecog.h"

Track::Track(ConfigFile *configFile)
    : _createCount(0)
    , _removeCount(0)
    , _isMatched(false)
    , _isValidatedWithEyeDetection(false)
{
    configCheckAndSet(configFile);
    resetTracker();
    setTrackSize(_config->roiAccumulationSize);
    _trackNumber = -1;
    _recognizedState = UNKWOWN;
    _recognizedPOIName = "";
}

Track::Track(ConfigFile *configFile, const cv::Rect& rect, int targetNumber)
    : _createCount(0)
    , _removeCount(0)
    , _isMatched(false)
    , _isValidatedWithEyeDetection(false)
{
    configCheckAndSet(configFile);
    setTrackSize(_config->roiAccumulationSize);
    insertROI(rect);
    resetTracker();
    _trackNumber = targetNumber;   // defaults to -1 if not specified
    _recognizedState = UNKWOWN;
    _recognizedPOIName = "";
}

Track::Track(const Track& track)
    : _createCount(track._createCount)
    , _removeCount(track._removeCount)
    , _isMatched(track._isMatched)
    , _isValidatedWithEyeDetection(false)
{
    insertROI(track.bbox());
    _tracker = track._tracker;
    _bboxes = track._bboxes;
    _trackNumber = track._trackNumber;
    _recognizedState = track._recognizedState;
    _recognizedPOIName = track._recognizedPOIName;
}

Track& Track::operator= (const Track& track)
{
    // do the copy
    _createCount = track._createCount;
    _removeCount = track._removeCount;
    _recognizedState = track._recognizedState;
    _recognizedPOIName = track._recognizedPOIName;
    _bboxes = track._bboxes;
    _trackNumber = track._trackNumber;
    _isMatched = track._isMatched;
    _isValidatedWithEyeDetection = track._isValidatedWithEyeDetection;
    _tracker = track._tracker;
    return *this;
}

Track::~Track()
{
    // the tracker destroyer will be invoked automatically
}

void Track::resetTracker()
{
    #ifdef FACE_RECOG_HAS_CAMSHIFT
    if (_config->Camshift)
        _tracker.reset(new TrackerCamshift(_config));
    #endif/*FACE_RECOG_HAS_CAMSHIFT*/
    #ifdef FACE_RECOG_HAS_COMPRESSIVE
    if (_config->Compressive)
        _tracker.reset(new TrackerCompressive(_config));
    #endif/*FACE_RECOG_HAS_COMPRESSIVE*/
    #ifdef FACE_RECOG_HAS_KCF
    if (_config->KCF)
        _tracker.reset(new TrackerKCF(_config));
    #endif/*FACE_RECOG_HAS_KCF*/
    #ifdef FACE_RECOG_HAS_STRUCK
    if (_config->STRUCK)
        _tracker.reset(new TrackerSTRUCK(_config));
    #endif/*FACE_RECOG_HAS_STRUCK*/
}

void Track::configCheckAndSet(ConfigFile *configFile)
{
    ASSERT_LOG(configFile, "Configuration file not specified for track");
    _config = configFile;
}

void Track::reInitTracking(const ImageRep& frame)
{
    cv::Rect b = bbox();
    if ((b.width == 0) && (b.height == 0))
    {
        cout << "Error in reInitTracking, the track has no bbox assigned" << endl;
        return;
    }
    FloatRect fbbox(b.x, b.y, b.width, b.height);
    _tracker->initialize(frame, fbbox);
}

void Track::track(const ImageRep& frame)
{
    cv::Rect b = bbox();
    if ((b.width == 0) && (b.height == 0))
    {
        cout << "Error in track, the track has no bbox assigned" << endl;
        return;
    }
    if (!_tracker->isInitialized())
    {
        cout << "Error in track, the track has no initialized tracker" << endl;
        return;
    }
    insertROI(_tracker->track(frame));
}
