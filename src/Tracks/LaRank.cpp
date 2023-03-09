/*
 *  TU Eindhoven
 *  Eindhoven, The Netherlands
 *
 *  Name            :   LaRank.cpp
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

//#define EIGEN2_SUPPORT
#include <Eigen/Core>
#include <opencv/highgui.h>

#include "Configs/ConfigFile.h"
#include "Tracks/HaarFeatures.h"
#include "Tracks/Kernels.h"
#include "Tracks/LaRank.h"
#include "Tracks/Rect.h"
#include "Tracks/Sample.h"

using namespace cv;
using namespace std;
using namespace Eigen;
using namespace config;

static const int kMaxSVs = 2000; // TODO (only used when no budget)


LaRank::LaRank(ConfigFile *conf, HaarFeatures features, Kernel kernel)
{
    assert(conf);
    m_config = conf;
    m_features = features;
    m_kernel = kernel;
    m_C = conf->svmC;

	int N = conf->svmBudgetSize > 0 ? conf->svmBudgetSize+2 : kMaxSVs;
	m_K = MatrixXd::Zero(N, N);
}

LaRank::LaRank(const LaRank &obj)
{
	this->m_config = obj.m_config;
	this->m_C =obj.m_C;
	this->m_K = obj.m_K;
	this->m_features = obj.m_features;
	this->m_kernel = obj.m_kernel;
	vector<SupportPattern*> pointerList;
	for (int i = 0; i < (int)m_svs.size(); ++i)
	{
		if (m_svs[i] != NULL)
		{
			if((std::find(pointerList.begin(), pointerList.end(),m_svs[i]->x ) == pointerList.end()))
			{
				// if not cleaned yet
				pointerList.push_back(m_svs[i]->x);
				delete m_svs[i]->x;
				m_svs[i]->x = NULL;
			}
			delete m_svs[i];
			m_svs[i] = NULL;
		}
	}
	m_svs.clear();
	for (int i = 0; i < (int)m_sps.size(); ++i)
	{
		if (m_sps[i] != NULL)
		{
			if((std::find(pointerList.begin(), pointerList.end(),m_sps[i] ) == pointerList.end()))
			{
				pointerList.push_back(m_sps[i]);
				delete m_sps[i];
				m_sps[i] = NULL;
			}
		}
	}
	m_sps.clear();
	pointerList.clear();

	vector<SupportPattern*> objPointerList;
	vector<SupportPattern*> thisPointerList;
	vector<SupportPattern*>::iterator it;

	m_svs.resize(obj.m_svs.size());
	for (std::size_t i = 0; i < obj.m_svs.size(); i++)
	{
		m_svs[i] = new SupportVector;
		*m_svs[i] = *obj.m_svs[i];
		it = find(objPointerList.begin(), objPointerList.end(), obj.m_svs[i]->x);
		if (it == objPointerList.end())
		{
			// not in list
			// if not on the old list, allocate space (deep copy)
			m_svs[i]->x = new SupportPattern;
			*(m_svs[i]->x) = *(obj.m_svs[i]->x);
			// add old object pointer to list
			objPointerList.push_back(obj.m_svs[i]->x);
			// add new object pointer to list
			thisPointerList.push_back(m_svs[i]->x);
		}
		else // make a shallow copy from the new object
		{
			size_t pos = std::distance(objPointerList.begin(), it);
			// make shallow copy
			m_svs[i]->x = thisPointerList[pos];
		}
	}
	m_sps.resize(obj.m_sps.size());
	for (std::size_t i = 0; i < obj.m_sps.size(); i++)
	{
		it = find(objPointerList.begin(), objPointerList.end(), obj.m_sps[i]);
		if (it == objPointerList.end())
		{
			m_sps[i] = new SupportPattern;
			*m_sps[i] = *obj.m_sps[i];
			objPointerList.push_back(obj.m_sps[i]);
			thisPointerList.push_back(m_sps[i]);
		}
		else
		{
			size_t pos = std::distance(objPointerList.begin(), it);
			// make shallow copy
			m_sps[i] = thisPointerList[pos];
		}
	}
}

LaRank & LaRank::operator=(const LaRank &obj)
{
	// check for "self assignment" and do nothing in that case
	if (this == &obj) return *this;
	else
	{
		this->m_config = obj.m_config;
		this->m_C =obj.m_C;
		this->m_K = obj.m_K;
		this->m_features = obj.m_features;
		this->m_kernel = obj.m_kernel;
		vector<SupportPattern*> pointerList;
		for (int i = 0; i < (int)m_svs.size(); ++i)
		{
			if (m_svs[i] != NULL)
			{
				if((std::find(pointerList.begin(), pointerList.end(),m_svs[i]->x ) == pointerList.end()))
				{
					// if not cleaned yet
					pointerList.push_back(m_svs[i]->x);
					delete m_svs[i]->x;
					m_svs[i]->x = NULL;
				}
				delete m_svs[i];
				m_svs[i] = NULL;
			}
		}
		m_svs.clear();
		for (int i = 0; i < (int)m_sps.size(); ++i)
		{
			if (m_sps[i] != NULL)
			{
				if((std::find(pointerList.begin(), pointerList.end(),m_sps[i] ) == pointerList.end()))
				{
					pointerList.push_back(m_sps[i]);
					delete m_sps[i];
					m_sps[i] = NULL;
				}
			}
		}
		m_sps.clear();
		pointerList.clear();
		vector<SupportPattern*> objPointerList;
		vector<SupportPattern*> thisPointerList;
		vector<SupportPattern*>::iterator it;

		m_svs.resize(obj.m_svs.size());
		for (std::size_t i = 0; i < obj.m_svs.size(); i++)
		{
			m_svs[i] = new SupportVector;
			*m_svs[i] = *obj.m_svs[i];
			it = find(objPointerList.begin(), objPointerList.end(), obj.m_svs[i]->x);
			if (it == objPointerList.end())
			{
				// not in list
				// if not on the old list, allocate space (deep copy)
				m_svs[i]->x = new SupportPattern;
				*(m_svs[i]->x) = *(obj.m_svs[i]->x);
				// add old object pointer to lits
				objPointerList.push_back(obj.m_svs[i]->x);
				// add new object pointer to list
				thisPointerList.push_back(m_svs[i]->x);
			}
			else // make a shallow copy from the new object
			{
				size_t pos = std::distance(objPointerList.begin(), it);
				// make shallow copy
				m_svs[i]->x = thisPointerList[pos];
			}
		}

		m_sps.resize(obj.m_sps.size());

		for (std::size_t i = 0; i < obj.m_sps.size(); i++)
		{
			it = find(objPointerList.begin(), objPointerList.end(), obj.m_sps[i]);
			if (it == objPointerList.end())
			{
				m_sps[i] = new SupportPattern;
				*m_sps[i] = *obj.m_sps[i];
				objPointerList.push_back(obj.m_sps[i]);
				thisPointerList.push_back(m_sps[i]);
			}
			else
			{
				size_t pos = std::distance(objPointerList.begin(), it);
				// make shallow copy
				m_sps[i] = thisPointerList[pos];
			}
		}
		return *this;
	}
}

LaRank::~LaRank()
{
	vector<SupportPattern*> pointerList;
	for (int i = 0; i < (int)m_svs.size(); i++)
	{
		// check if the corresponding support pattern m_svs[i]->x has already been cleaned
		if((std::find(pointerList.begin(), pointerList.end(),m_svs[i]->x ) == pointerList.end()))
		{
			//			if not add to list of cleaned supportPatterns
			pointerList.push_back(m_svs[i]->x);
			// if not, clean
			delete m_svs[i]->x;
		}
		delete m_svs[i];

	}
	m_svs.clear();
	for (int i = 0; i < (int)m_sps.size(); i++)
	{
		if((std::find(pointerList.begin(), pointerList.end(),m_sps[i] ) == pointerList.end()))
		{
			pointerList.push_back(m_sps[i]);
			delete m_sps[i];
			m_sps[i] = NULL;
		}
	}
	m_sps.clear();
	pointerList.clear();
}

double LaRank::evaluate(const Eigen::VectorXd& x) const
{
	double f = 0.0;
	for (int i = 0; i < (int)m_svs.size(); ++i)
	{
		const SupportVector& sv = *m_svs[i];
		f += sv.b*m_kernel.Eval(x, sv.x->x[sv.y]);
	}
	return f;
}

void LaRank::eval(const MultiSample& sample, std::vector<double>& results)
{
	const FloatRect& centre(sample.GetRects()[0]);
	vector<VectorXd> fvs;
	const_cast<HaarFeatures&>(m_features).eval(sample, fvs);
	results.resize(fvs.size());
	for (int i = 0; i < (int)fvs.size(); ++i)
	{
		// express y in coord frame of centre sample
		FloatRect y(sample.GetRects()[i]);
		y.translate(-centre.xmin(), -centre.ymin());
		results[i] = evaluate(fvs[i]);
	}
}

void LaRank::update(const MultiSample& sample, int y)
{
	// add new support pattern
	SupportPattern* sp = new SupportPattern; // it is the current frame
	const vector<FloatRect>& rects = sample.GetRects();
	FloatRect centre = rects[y]; //center is the positive sample (current target location)
	for (int i = 0; i < (int)rects.size(); ++i)
	{
		// express r in coord frame of centre sample
		FloatRect r = rects[i];
		r.translate(-centre.xmin(), -centre.ymin());
		sp->yv.push_back(r); // yv is a vector of bounding boxes
	}
	// evaluate features for each sample
	sp->x.resize(rects.size());
	const_cast<HaarFeatures&>(m_features).eval(sample, sp->x);
	sp->y = y;
	sp->refCount = 0;
	m_sps.push_back(sp);
	processNew((int)m_sps.size()-1);
	budgetMaintenance();
	for (int i = 0; i < 10; ++i)
	{
		reprocess();
		budgetMaintenance();
	}
}

void LaRank::budgetMaintenance()
{
	if (m_config->svmBudgetSize > 0)
	{
		while ((int)m_svs.size() > m_config->svmBudgetSize)
		{
			budgetMaintenanceRemove();
		}
	}
}

void LaRank::reprocess()
{
	processOld();
	for (int i = 0; i < 10; ++i)
		optimize();
}

void LaRank::SMOStep(int ipos, int ineg)
{
	if (ipos == ineg) return;

	SupportVector* svp = m_svs[ipos];
	SupportVector* svn = m_svs[ineg];
	assert(svp->x == svn->x);
	SupportPattern* sp = svp->x; // they refer to the same support pattern

	if ((svp->g - svn->g) < 1e-5)
	{

	}
	else
	{
		double kii = m_K(ipos, ipos) + m_K(ineg, ineg) - 2*m_K(ipos, ineg);
		double lu = (svp->g-svn->g)/kii;
		// no need to clamp against 0 since we'd have skipped in that case
		double l = min(lu, m_C*(int)(svp->y == sp->y) - svp->b);// b is the beta coefficient in the paper

		svp->b += l;
		svn->b -= l;

		// update gradients
		for (int i = 0; i < (int)m_svs.size(); ++i)
		{
			SupportVector* svi = m_svs[i];
			svi->g -= l*(m_K(i, ipos) - m_K(i, ineg));
		}
	}

	// check if we should remove either sv now

	if (fabs(svp->b) < 1e-8)
	{
		removeSupportVector(ipos);
		if (ineg == (int)m_svs.size())
		{
			// ineg and ipos will have been swapped during sv removal
			ineg = ipos;
		}
	}

	if (fabs(svn->b) < 1e-8)
		removeSupportVector(ineg);
}

pair<int, double> LaRank::minGradient(int ind)
{
	const SupportPattern* sp = m_sps[ind];
	pair<int, double> minGrad(-1, DBL_MAX);
	for (int i = 0; i < (int)sp->yv.size(); ++i)
	{
		double grad = -loss(sp->yv[i], sp->yv[sp->y]) - evaluate(sp->x[i]);
		if (grad < minGrad.second)
		{
			minGrad.first = i;
			minGrad.second = grad;
		}
	}
	return minGrad;
}

void LaRank::processNew(int ind)
{
	// gradient is -f(x,y) since loss=0
	int ip = addSupportVector(m_sps[ind], m_sps[ind]->y, -evaluate(m_sps[ind]->x[m_sps[ind]->y]));

	pair<int, double> minGrad = minGradient(ind);// mingrad has the index of thebbox and
	int in = addSupportVector(m_sps[ind], minGrad.first, minGrad.second);

	SMOStep(ip, in);
}

void LaRank::processOld()
{
	if (m_sps.size() == 0) return;

	// choose pattern to process (random frame from the past)
	int ind = rand() % m_sps.size();

	// find existing sv with largest grad and nonzero beta
	int ip = -1;
	double maxGrad = -DBL_MAX;
	for (int i = 0; i < (int)m_svs.size(); ++i)
	{
		if (m_svs[i]->x != m_sps[ind]) continue;

		const SupportVector* svi = m_svs[i];
		if (svi->g > maxGrad && svi->b < m_C*(int)(svi->y == m_sps[ind]->y))
		{
			ip = i;
			maxGrad = svi->g;
		}
	}
	assert(ip != -1);
	if (ip == -1) return;

	// find potentially new sv with smallest grad
	pair<int, double> minGrad = minGradient(ind);
	int in = -1;
	for (int i = 0; i < (int)m_svs.size(); ++i)
	{
		if (m_svs[i]->x != m_sps[ind]) continue;

		if (m_svs[i]->y == minGrad.first)
		{
			in = i;
			break;
		}
	}
	if (in == -1)
	{
		// add new sv
		in = addSupportVector(m_sps[ind], minGrad.first, minGrad.second);
	}

	SMOStep(ip, in);
}

void LaRank::optimize()
{
	if (m_sps.size() == 0) return;

	// choose pattern to optimize
	int ind = rand() % m_sps.size();

	int ip = -1;
	int in = -1;
	double maxGrad = -DBL_MAX;
	double minGrad = DBL_MAX;
	for (int i = 0; i < (int)m_svs.size(); ++i)
	{
		if (m_svs[i]->x != m_sps[ind]) continue;

		const SupportVector* svi = m_svs[i];
		if (svi->g > maxGrad && svi->b < m_C*(int)(svi->y == m_sps[ind]->y))
		{
			ip = i;
			maxGrad = svi->g;
		}
		if (svi->g < minGrad)
		{
			in = i;
			minGrad = svi->g;
		}
	}
	assert(ip != -1 && in != -1);
	if (ip == -1 || in == -1)
	{
		// this shouldn't happen
		cout << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" << endl;
		return;
	}

	SMOStep(ip, in);
}

int LaRank::addSupportVector(SupportPattern* x, int y, double g)
{
	SupportVector* sv = new SupportVector;
	sv->b = 0.0;
	sv->x = x;
	//	sv->x = new SupportPattern;
	//	*(sv->x) = *x;
	sv->y = y;
	sv->g = g;

	int ind = (int)m_svs.size();
	m_svs.push_back(sv);
	x->refCount++;



	// update kernel matrix.. its size depends on the number of support vectors
	for (int i = 0; i < ind; ++i)
	{
		m_K(i,ind) = m_kernel.Eval(m_svs[i]->x->x[m_svs[i]->y], x->x[y]);
		m_K(ind,i) = m_K(i,ind);
	}
	m_K(ind,ind) = m_kernel.Eval(x->x[y]);

	return ind;
}

void LaRank::swapSupportVectors(int ind1, int ind2)
{
	SupportVector* tmp = m_svs[ind1];
	m_svs[ind1] = m_svs[ind2];
	m_svs[ind2] = tmp;

	VectorXd row1 = m_K.row(ind1);
	m_K.row(ind1) = m_K.row(ind2);
	m_K.row(ind2) = row1;

	VectorXd col1 = m_K.col(ind1);
	m_K.col(ind1) = m_K.col(ind2);
	m_K.col(ind2) = col1;
}

void LaRank::removeSupportVector(int ind)
{


	m_svs[ind]->x->refCount--;
	if (m_svs[ind]->x->refCount == 0)
	{
		// also remove the support pattern
		for (int i = 0; i < (int)m_sps.size(); ++i)
		{
			if (m_sps[i] == m_svs[ind]->x)
			{
				delete m_sps[i];
				m_sps.erase(m_sps.begin()+i);
				break;
			}
		}
	}

	// make sure the support vector is at the back, this
	// lets us keep the kernel matrix cached and valid
	if (ind < (int)m_svs.size()-1)
	{
		swapSupportVectors(ind, (int)m_svs.size()-1);
		ind = (int)m_svs.size()-1;
	}
	delete m_svs[ind];
	m_svs.pop_back();
}

void LaRank::budgetMaintenanceRemove()
{
	// find negative sv with smallest effect on discriminant function if removed
	double minVal = DBL_MAX;
	int in = -1;
	int ip = -1;
	for (int i = 0; i < (int)m_svs.size(); ++i)
	{
		if (m_svs[i]->b < 0.0)
		{
			// find corresponding positive sv
			int j = -1;
			for (int k = 0; k < (int)m_svs.size(); ++k)
			{
				if (m_svs[k]->b > 0.0 && m_svs[k]->x == m_svs[i]->x)
				{
					j = k;
					break;
				}
			}
			double val = m_svs[i]->b*m_svs[i]->b*(m_K(i,i) + m_K(j,j) - 2.0*m_K(i,j));
			if (val < minVal)
			{
				minVal = val;
				in = i;
				ip = j;
			}
		}
	}

	// adjust weight of positive sv to compensate for removal of negative
	m_svs[ip]->b += m_svs[in]->b;

	// remove negative sv
	removeSupportVector(in);
	if (ip == (int)m_svs.size())
	{
		// ip and in will have been swapped during support vector removal
		ip = in;
	}

	if (m_svs[ip]->b < 1e-8)
	{
		// also remove positive sv
		removeSupportVector(ip);
	}

	// update gradients
	// TODO: this could be made cheaper by just adjusting incrementally rather than recomputing
	for (int i = 0; i < (int)m_svs.size(); ++i)
	{
		SupportVector& svi = *m_svs[i];
		svi.g = -loss(svi.x->yv[svi.y],svi.x->yv[svi.x->y]) - evaluate(svi.x->x[svi.y]);
	}	
}


