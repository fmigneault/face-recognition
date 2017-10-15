/*
 *  TU Eindhoven
 *  Eindhoven, The Netherlands
 *
 *  Name            :   LaRank.h
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

#ifndef FACE_RECOG_LARANK_H
#define FACE_RECOG_LARANK_H

#include "FaceRecog.h"

class LaRank
{
public:

    LaRank(ConfigFile *conf, HaarFeatures features, Kernel kernel);
    ~LaRank();
    LaRank(const LaRank &obj);  // copy constructor
    LaRank & operator=(const LaRank &T); // assignment operator


    void eval(const MultiSample& x, std::vector<double>& results);
    void update(const MultiSample& x, int y);


private:

    struct SupportPattern
    {
        std::vector<Eigen::VectorXd> x; // vector of feature repsonses
        std::vector<FloatRect> yv; // vector of samples
        std::vector<cv::Mat> images;
        int y; // y is the translation
        int refCount;
    };

    struct SupportVector
    {
        SupportPattern* x;
        int y;
        double b;
        double g;
        cv::Mat image;
    };

    ConfigFile *m_config;
    HaarFeatures m_features;
    Kernel m_kernel;

    std::vector<SupportPattern*> m_sps;
    std::vector<SupportVector*> m_svs;


    double m_C;
    Eigen::MatrixXd m_K;

    inline double loss(const FloatRect& y1, const FloatRect& y2) const
    {
        // overlap loss
        return 1.0 - y1.overlap(y2);
    }

    void SMOStep(int ipos, int ineg);
    std::pair<int, double> minGradient(int ind);
    void processNew(int ind);
    void reprocess();
    void processOld();
    void optimize();

    int addSupportVector(SupportPattern* x, int y, double g);
    void removeSupportVector(int ind);
    void removeSupportVectors(int ind1, int ind2);
    void swapSupportVectors(int ind1, int ind2);

    void budgetMaintenance();
    void budgetMaintenanceRemove();

    double evaluate(const Eigen::VectorXd& x) const;
};

#endif /*FACE_RECOG_LARANK_H*/
