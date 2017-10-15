/*
 *  TU Eindhoven
 *  Eindhoven, The Netherlands
 *
 *  Name            :   Association.cpp
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

#include "FaceRecog.h"

void Association::clear()
{
    if (costMatrix)
    {
        for (size_t i = 0; i < m; ++i)
            free(costMatrix[i]);
        free(costMatrix);
    }
    costMatrix = NULL;
}

Association::Association(ConfigFile *config)
{
    assert(config);
    _config = config;

    costMatrix = NULL;
    detTrackThresh = _config->associationTrackThreshold;
    m = 0;
    n = 0;
}

Association::~Association()
{
    clear();
}

void Association::extendSet(std::vector<Track>& tracks, std::vector<cv::Rect>& detections)
{
    size_t cols = detections.size();
    size_t rows = tracks.size();
    // increase the smaller set
    size_t setDiff = abs(cols - rows);
    if (cols > rows)
    {
        for (size_t i = 0; i < setDiff; ++i)
        {
            // add fake track to tracked objects
            Rect fakeRect(0, 0, 0, 0);
            Track fakeTrack(_config, fakeRect);
            tracks.push_back(fakeTrack);
        }
    }
    else if (cols < rows)
    {
        for (size_t i = 0; i < setDiff; ++i)
        {
            Rect fakeRect(0, 0, 0, 0);
            detections.push_back(fakeRect);
        }
    }
}

int Association::computeCost(std::vector<Track>& tracks, std::vector<cv::Rect>& detections)
{
    size_t rows = tracks.size();
    size_t cols = detections.size();
    if (rows != cols)
    {
        cerr << "ERROR in Association::computeCost, different vectorz size" << endl;
        return -1;
    }
    if (costMatrix)
    {
        clear();
    }
    m = rows;
    n = cols;
    costMatrix = (int **)malloc(m * sizeof(int*));
    for (size_t i = 0; i < m; ++i)
        costMatrix[i] = (int *)malloc(n * sizeof(int));
    double distance;
    // compute costs to fill in the detections-trackers matrix
    for (size_t i = 0; i < m; ++i)
    {
        for (size_t j = 0; j < n; ++j)
        {
            Rect r1 = detections[i];
            //              Rect r2 = Rect(trackers[j].GetBB().XMin(), trackers[j].GetBB().YMin(), trackers[j].GetBB().Width(), trackers[j].GetBB().Height());
            Rect r2 = tracks[j].bbox();
            if ((r1.width != 0) && (r2.width != 0)) // both valid tracks
                distance = util::rectDist(r1, r2);
            else
                distance = 1; // max distance
            costMatrix[i][j] = (int)(distance * 100);
        }
    }
    return 0;
}

int Association::matchTracks(std::vector<Track>& tracks, std::vector<cv::Rect>& detections, std::vector<Rect>& unmatched, ImageRep& frame)
{
    if (!costMatrix)
        computeCost(tracks, detections);
    size_t rows = m;
    size_t cols = n;
    hungarian_problem_t p;
    hungarian_init(&p, costMatrix, rows, cols, HUNGARIAN_MODE_MINIMIZE_COST);
    hungarian_solve(&p);
    for (size_t i = 0; i < rows; ++i)
    {
        for (size_t j = 0; j < cols; ++j)
        {
            if (p.assignment[i][j] == 1) // assigned detection/track
            {
                // the cost matrix is given by the distance (1 - a) so if dist < 0.7 I  consider it a match
                if (costMatrix[i][j] < detTrackThresh)
                {
                    //merge detection and tracking results
                    Rect face;
                    Rect r1 = detections[i];
                    Rect r2 = tracks[j].bbox();
                    face.x = (r1.x + r2.x) / 2; // take the average
                    face.y = (r1.y + r2.y) / 2; // take the average
                    face.width = (r1.width + r2.width) / 2; // take the average
                    face.height = (r1.height + r2.height) / 2; // take the average
                    tracks[j].insertROI(face);
                    // REINIT tracker
                    //                      tracks[j].reInitTracking(frame_gray);
                    tracks[j].reInitTracking(frame);
                    tracks[j].markMatched();
                    // RESET REMOVE COUNT
                    tracks[j].setRemoveCount(0);
                }
                // artificially created detection,
                // i.e. the tracker is potentially drifting
                else if (detections[i].width == 0)
                {
                    // increase remove counter
                    tracks[j].markNotMatched();
                    //                      tracks[j].increaseRemoveCount();
                }
                // artificially created tracker,
                // i.e. the detection is potentially a new candidate
                else if (tracks[j].bbox().width == 0)
                {
                    unmatched.push_back(detections[i]);
                }
                // I have both a candidate for init and for remove
                // (2 mismatches)
                else if (costMatrix[i][j] >= detTrackThresh)
                {
                    // increase remove counter
                    //                      tracks[j].increaseRemoveCount();
                    tracks[j].markNotMatched();
                    unmatched.push_back(detections[i]);
                }
                break;
            } // assignment
        } //j loop
    } // i loop
    /* free used memory */
    hungarian_free(&p);
    clear();
    return 0;
}

void Association::reduceSet(std::vector<Track> & tracks)
{
    /*REMOVE FAKE TRACKS*/
    for (std::vector<Track>::iterator iter = tracks.begin(); iter != tracks.end();)
    {
        if ((*iter).bbox().width == 0)
            iter = tracks.erase(iter);
        else
            ++iter;
    }
}

void Association::matchCandidates(std::vector<Track>& tracks, std::vector<cv::Rect>& detections, std::vector<Track>& newCandidates)
{
    if (!costMatrix)
        computeCost(tracks, detections);
    size_t rows = m;
    size_t cols = n;
    hungarian_problem_t problem;
    int matrix_size = hungarian_init(&problem, costMatrix, rows, cols, HUNGARIAN_MODE_MINIMIZE_COST);
    hungarian_solve(&problem);
    for (size_t i = 0; i < rows; ++i)
    {
        for (size_t j = 0; j < cols; ++j)
        {
            if (problem.assignment[i][j] == 1) // assigned detection/track
            {
                // the cost matrix is given by the distance (1 - a) so if dist < 0.7 I  consider it a match
                if (costMatrix[i][j] <= detTrackThresh)
                {
                    Rect face;
                    Rect r1 = detections[i];
                    Rect r2 = tracks[j].bbox();
                    face.x = (r1.x + r2.x) / 2; // take the average
                    face.y = (r1.y + r2.y) / 2; // take the average
                    face.width = (r1.width + r2.width) / 2; // take the average
                    face.height = (r1.height + r2.height) / 2; // take the average
                    tracks[j].insertROI(face);
                    tracks[j].markMatched();
                    tracks[j].increaseCreateCount();

                }
                // artificially created detection,
                // i.e. no consecutive detections for the candidate for initialization
                else if (detections[i].width == 0)
                {
                    tracks[j].markNotMatched();
                }
                // artificially created candidate
                // i.e. the detection is potentially a new candidate
                else if (tracks[j].bbox().width == 0)
                {
                    // new potential track
                    Track track(_config, detections[i]);
                    // make it valid
                    track.markMatched();
                    // put counter to 1
                    track.increaseCreateCount();
                    // add to new candidates (from this frame)
                    newCandidates.push_back(track);
                }
                // I have both a candidate for init and for remove
                // (2 mismatches)
                else if (costMatrix[i][j] > detTrackThresh)
                {
                    // no consecutive matches for the candidate
                    tracks[j].markNotMatched();
                    // new potential candidate
                    Track track(_config, detections[i]);
                    // mark as valid
                    track.markMatched();
                    // put counter to one
                    track.increaseCreateCount();
                    // add to new candidates from current frame
                    newCandidates.push_back(track);
                }
                break;
            } // assignment
        } //j loop
    } // i loop
    /* free used memory */
    hungarian_free(&problem);
    clear();
}
