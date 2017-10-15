/*
 *  TU Eindhoven
 *  Eindhoven, The Netherlands
 *
 *  Name            :   Rect.h
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

#ifndef FACE_RECOG_RECT_H
#define FACE_RECOG_RECT_H

#include "FaceRecog.h"

template <typename T>
class struckRect
{
public:
    struckRect() :
        m_xmin(0),
        m_ymin(0),
        m_width(0),
        m_height(0)
    {
    }

    struckRect(T xmin, T ymin, T width, T height) :
        m_xmin(xmin),
        m_ymin(ymin),
        m_width(width),
        m_height(height)
    {
    }

    template <typename T2>
    struckRect(const struckRect<T2>& rOther) :
        m_xmin((T)rOther.xmin()),
        m_ymin((T)rOther.ymin()),
        m_width((T)rOther.width()),
        m_height((T)rOther.height())
    {
    }

    inline void set(T xmin, T ymin, T width, T height)
    {
        m_xmin = xmin;
        m_ymin = ymin;
        m_width = width;
        m_height = height;
    }

    inline T xmin() const { return m_xmin; }
    inline void xmin(T val) { m_xmin = val; }
    inline T ymin() const { return m_ymin; }
    inline void ymin(T val) { m_ymin = val; }
    inline T width() const { return m_width; }
    inline void width(T val) { m_width = val; }
    inline T height() const { return m_height; }
    inline void height(T val) { m_height = val; }

    inline void translate(T x, T y) { m_xmin += x; m_ymin += y; }

    inline T xmax() const { return m_xmin + m_width; }
    inline T ymax() const { return m_ymin + m_height; }
    inline float xCentre() const { return (float)m_xmin + (float)m_width / 2; }
    inline float yCentre() const { return (float)m_ymin + (float)m_height / 2; }
    inline T area() const { return m_width * m_height; }

    inline cv::Rect toCvRect() const {
        cv::Rect bbox;
        bbox.x = (int)m_xmin;
        bbox.y = (int)m_ymin;
        bbox.width = (int)m_width;
        bbox.height = (int)m_height;
        return bbox;
    }

    template <typename T2>
    friend std::ostream& operator <<(std::ostream &rOS, const struckRect<T2>& rRect);

    template <typename T2>
    float overlap(const struckRect<T2>& rOther) const;
    template <typename T2>
    bool isInside(const struckRect<T2>& rOther) const;

private:
    T m_xmin;
    T m_ymin;
    T m_width;
    T m_height;
};

template <typename T>
std::ostream& operator <<(std::ostream &rOS, const struckRect<T>& rRect)
{
    rOS << "[origin: (" << rRect.m_xmin << ", " << rRect.m_ymin << ") size: (" << rRect.m_width << ", " << rRect.m_height << ")]";
    return rOS;
}

template <typename T>
template <typename T2>
float struckRect<T>::overlap(const struckRect<T2>& rOther) const
{
    float x0 = std::max((float)xmin(), (float)rOther.xmin());
    float x1 = std::min((float)xmax(), (float)rOther.xmax());
    float y0 = std::max((float)ymin(), (float)rOther.ymin());
    float y1 = std::min((float)ymax(), (float)rOther.ymax());

    if (x0 >= x1 || y0 >= y1) return 0.f;

    float areaInt = (x1 - x0)*(y1 - y0);
    return areaInt / ((float)area() + (float)rOther.area() - areaInt);
}

template <typename T>
template <typename T2>
bool struckRect<T>::isInside(const struckRect<T2>& rOther) const
{
    return (xmin() >= rOther.xmin()) && (ymin() >= rOther.ymin()) && (xmax() <= rOther.xmax()) && (ymax() <= rOther.ymax());
}


#endif /*FACE_RECOG_RECT_H*/
