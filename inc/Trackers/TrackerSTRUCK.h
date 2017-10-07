/*
 *  TU Eindhoven
 *  Eindhoven, The Netherlands
 *
 *  Name            :   Tracker.h
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
 *  This file is part of FAST-DT.
 *
 *  This code is a modified version of the work provided in:
 *  Struck: Structured Output Tracking with Kernels
 *  Sam Hare, Amir Saffari, Philip H. S. Torr
 *  International Conference on Computer Vision (ICCV), 2011
 *  Copyright (C) 2011 Sam Hare, Oxford Brookes University, Oxford, UK
 *
 *  I have modified and redistributed this code
 *  under the terms of the GNU General Public License
 *  as published by the Free Software Foundation.
 *
 *  I hereby proclaim that this file is *NOT* part of the
 *  "Struck: Structured Output Tracking with Kernels" distribution!
 *
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

#ifndef FACE_RECOG_TRACKER_STRUCK_H
#define FACE_RECOG_TRACKER_STRUCK_H

#include "FaceRecog.h"


class TrackerSTRUCK final : public ITracker
{
public:
	TrackerSTRUCK(ConfigFile *configFile);
	virtual ~TrackerSTRUCK();
	TrackerSTRUCK(const TrackerSTRUCK &obj);		   // copy constructor
	TrackerSTRUCK &operator=(const TrackerSTRUCK &T);  // assignment operator
	virtual void initialize(const ImageRep& frame, FloatRect bb);
	virtual void reset();
	virtual cv::Rect track(const ImageRep& frame);
private:
	std::vector<HaarFeatures*> m_features;
	std::vector<Kernel*> m_kernels;
	LaRank* m_pLearner;
	bool m_needsIntegralImage;
	bool m_needsIntegralHist;
	void updateLearner(const ImageRep& image);
};

#endif /*FACE_RECOG_TRACKER_STRUCK_H*/