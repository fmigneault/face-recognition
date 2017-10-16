/*
 *  TU Eindhoven
 *  Eindhoven, The Netherlands
 *
 *  Name            :   HaarFeature.cpp
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

#include "Tracks/HaarFeature.h"
#include "FaceRecog.h"

HaarFeature::HaarFeature(const FloatRect& bb, int type) :
    m_bb(bb)
{
    assert(type < 6);

    switch (type)
    {
        case 0:
        {
            m_rects.push_back(FloatRect(bb.xmin(), bb.ymin(), bb.width(), bb.height() / 2));
            m_rects.push_back(FloatRect(bb.xmin(), bb.ymin() + bb.height() / 2, bb.width(), bb.height() / 2));
            m_weights.push_back(1.f);
            m_weights.push_back(-1.f);
            m_factor = 255 * 1.f / 2;
            break;
        }
        case 1:
        {
            m_rects.push_back(FloatRect(bb.xmin(), bb.ymin(), bb.width() / 2, bb.height()));
            m_rects.push_back(FloatRect(bb.xmin() + bb.width() / 2, bb.ymin(), bb.width() / 2, bb.height()));
            m_weights.push_back(1.f);
            m_weights.push_back(-1.f);
            m_factor = 255 * 1.f / 2;
            break;
        }
        case 2:
        {
            m_rects.push_back(FloatRect(bb.xmin(), bb.ymin(), bb.width() / 3, bb.height()));
            m_rects.push_back(FloatRect(bb.xmin() + bb.width() / 3, bb.ymin(), bb.width() / 3, bb.height()));
            m_rects.push_back(FloatRect(bb.xmin() + 2 * bb.width() / 3, bb.ymin(), bb.width() / 3, bb.height()));
            m_weights.push_back(1.f);
            m_weights.push_back(-2.f);
            m_weights.push_back(1.f);
            m_factor = 255 * 2.f / 3;
            break;
        }
        case 3:
        {
            m_rects.push_back(FloatRect(bb.xmin(), bb.ymin(), bb.width(), bb.height() / 3));
            m_rects.push_back(FloatRect(bb.xmin(), bb.ymin() + bb.height() / 3, bb.width(), bb.height() / 3));
            m_rects.push_back(FloatRect(bb.xmin(), bb.ymin() + 2 * bb.height() / 3, bb.width(), bb.height() / 3));
            m_weights.push_back(1.f);
            m_weights.push_back(-2.f);
            m_weights.push_back(1.f);
            m_factor = 255 * 2.f / 3;
            break;
        }
        case 4:
        {
            m_rects.push_back(FloatRect(bb.xmin(), bb.ymin(), bb.width() / 2, bb.height() / 2));
            m_rects.push_back(FloatRect(bb.xmin() + bb.width() / 2, bb.ymin() + bb.height() / 2, bb.width() / 2, bb.height() / 2));
            m_rects.push_back(FloatRect(bb.xmin(), bb.ymin() + bb.height() / 2, bb.width() / 2, bb.height() / 2));
            m_rects.push_back(FloatRect(bb.xmin() + bb.width() / 2, bb.ymin(), bb.width() / 2, bb.height() / 2));
            m_weights.push_back(1.f);
            m_weights.push_back(1.f);
            m_weights.push_back(-1.f);
            m_weights.push_back(-1.f);
            m_factor = 255 * 1.f / 2;
            break;
        }
        case 5:
        {
            m_rects.push_back(FloatRect(bb.xmin(), bb.ymin(), bb.width(), bb.height()));
            m_rects.push_back(FloatRect(bb.xmin() + bb.width() / 4, bb.ymin() + bb.height() / 4, bb.width() / 2, bb.height() / 2));
            m_weights.push_back(1.f);
            m_weights.push_back(-4.f);
            m_factor = 255 * 3.f / 4;
            break;
        }
    }
}

HaarFeature::~HaarFeature()
{
}

float HaarFeature::Eval(const Sample& s) const
{
    const ImageRep& image = s.GetImage(); // get the image where to evaluate the feature
    const FloatRect& roi = s.GetROI();
    float value = 0.f;
    for (int i = 0; i < (int)m_rects.size(); ++i)// number of rectangles of the feature
    {
        const FloatRect& r = m_rects[i];
        IntRect sampleRect((int)(roi.xmin() + r.xmin()*roi.width() + 0.5f), (int)(roi.ymin() + r.ymin()*roi.height() + 0.5f),
            (int)(r.width()*roi.width()), (int)(r.height()*roi.height()));
        value += m_weights[i] * image.Sum(sampleRect);
    }
    return value / (m_factor*roi.area()*m_bb.area());
}
