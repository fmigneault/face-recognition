/*
 *  TU Eindhoven
 *  Eindhoven, The Netherlands
 *
 *  Name            :  Sampler.cpp
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

#define _USE_MATH_DEFINES   // Must be on top to compile without error on Windows
#include <cmath>

#include "Configs/ConfigFile.h"
#include "Tracks/Sampler.h"

using namespace std;

vector<FloatRect> Sampler::RadialSamples(FloatRect centre, int radius, int nr, int nt)
{
	vector<FloatRect> samples;
	FloatRect s(centre);
	float rstep = (float)radius/nr;
	float tstep = 2*(float)M_PI/nt;
	samples.push_back(centre);
    
    // note here that all the samples (bboxes) have the same width and height
	for (int ir = 1; ir <= nr; ++ir)
	{
		float phase = (ir % 2)*tstep/2;
		for (int it = 0; it < nt; ++it)
		{
			float dx = ir*rstep*cosf(it*tstep+phase);
			float dy = ir*rstep*sinf(it*tstep+phase);
			s.xmin(centre.xmin()+dx);
			s.ymin(centre.ymin()+dy);
			samples.push_back(s);
		}
	}
	
	return samples;
}

vector<FloatRect> Sampler::PixelSamples(FloatRect centre, int radius, bool halfSample)
{
	vector<FloatRect> samples;
	
	IntRect s(centre);
	samples.push_back(s);
	
	int r2 = radius*radius;
	for (int iy = -radius; iy <= radius; ++iy)
	{
		for (int ix = -radius; ix <= radius; ++ix)
		{
			if (ix*ix+iy*iy > r2) continue;
			if (iy == 0 && ix == 0) continue; // already put this one at the start
			
			int x = (int)centre.xmin() + ix;
			int y = (int)centre.ymin() + iy;
			if (halfSample && (ix % 2 != 0 || iy % 2 != 0)) continue;
			
			s.xmin(x);
			s.ymin(y);
			samples.push_back(s);
		}
	}
	
	return samples;
}
