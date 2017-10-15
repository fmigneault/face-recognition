/*
 *  TU Eindhoven
 *  Eindhoven, The Netherlands
 *
 *  Name            :   ImageRep.cpp
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

#include "Tracks/ImageRep.h"

#include <cassert>

#include <opencv/highgui.h>
#include <opencv2/imgproc/imgproc.hpp>
#include <stdio.h>

using namespace std;
using namespace cv;

static const int kNumBins = 16;

ImageRep::ImageRep(const Mat& image, bool computeIntegral, bool computeIntegralHist, bool colour) :
    m_channels(colour ? 3 : 1),
    m_rect(0, 0, image.cols, image.rows)
{
    m_images.clear();
    m_integralImages.clear();
    m_integralHistImages.clear();
    for (int i = 0; i < m_channels; ++i)
    {
        m_images.push_back(Mat(image.rows, image.cols, CV_8UC1));
        if (computeIntegral) m_integralImages.push_back(Mat(image.rows + 1, image.cols + 1, CV_32SC1));
        if (computeIntegralHist)
        {
            for (int j = 0; j < kNumBins; ++j)
            {
                m_integralHistImages.push_back(Mat(image.rows + 1, image.cols + 1, CV_32SC1));
            }
        }
    }

    if (colour)
    {
        assert(image.channels() == 3);
        colour_image = image.clone(); // .getMat(ACCESS_READ);
        split(image, m_images);
    }
    else
    {
        colour_image = image.clone(); // .getMat(ACCESS_READ);
        assert(image.channels() == 1 || image.channels() == 3);
        if (image.channels() == 3)
        {
            FACE_RECOG_NAMESPACE::cvtColor(image, m_images[0], CV_RGB2GRAY);
        }
        else if (image.channels() == 1)
        {
            image.copyTo(m_images[0]);
        }
    }

    if (computeIntegral)
    {
        for (int i = 0; i < m_channels; ++i)
        {
            //equalizeHist(m_images[i], m_images[i]);
            integral(m_images[i], m_integralImages[i]);
        }
    }

    if (computeIntegralHist)
    {
        Mat tmp(image.rows, image.cols, CV_8UC1);
        tmp.setTo(0);
        for (int j = 0; j < kNumBins; ++j)
        {
            for (int y = 0; y < image.rows; ++y)
            {
                const uchar* src = m_images[0].ptr(y);
                uchar* dst = tmp.ptr(y);
                for (int x = 0; x < image.cols; ++x)
                {
                    int bin = (int)(((float)*src / 256)*kNumBins);
                    *dst = (bin == j) ? 1 : 0;
                    ++src;
                    ++dst;
                }
            }

            integral(tmp, m_integralHistImages[j]);
        }
    }
}

int ImageRep::Sum(const IntRect& rRect, int channel) const
{
    assert(rRect.xmin() >= 0 && rRect.ymin() >= 0 && rRect.xmax() <= m_images[0].cols && rRect.ymax() <= m_images[0].rows);
    return m_integralImages[channel].at<int>(rRect.ymin(), rRect.xmin()) +
        m_integralImages[channel].at<int>(rRect.ymax(), rRect.xmax()) -
        m_integralImages[channel].at<int>(rRect.ymax(), rRect.xmin()) -
        m_integralImages[channel].at<int>(rRect.ymin(), rRect.xmax());
}

void ImageRep::Hist(const IntRect& rRect, Eigen::VectorXd& h) const
{
    assert(rRect.xmin() >= 0 && rRect.ymin() >= 0 && rRect.xmax() <= m_images[0].cols && rRect.ymax() <= m_images[0].rows);
    int norm = rRect.area();
    for (int i = 0; i < kNumBins; ++i)
    {
        int sum = m_integralHistImages[i].at<int>(rRect.ymin(), rRect.xmin()) +
            m_integralHistImages[i].at<int>(rRect.ymax(), rRect.xmax()) -
            m_integralHistImages[i].at<int>(rRect.ymax(), rRect.xmin()) -
            m_integralHistImages[i].at<int>(rRect.ymin(), rRect.xmax());
        h[i] = (float)sum / norm;
    }
}
