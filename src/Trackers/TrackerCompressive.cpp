#ifdef FACE_RECOG_HAS_COMPRESSIVE

#include "Trackers/TrackerCompressive.h"
#include "FaceRecog.h"

TrackerCompressive::TrackerCompressive(ConfigFile* configFile) :
    m_initialized(false)
{
    updateConfig(configFile);
    featureMinNumRect = 2;
    featureMaxNumRect = 4;  // number of rectangle from 2 to 4
    featureNum = 50;    // number of all weaker classifiers, i.e,feature pool
    rOuterPositive = 4; // radical scope of positive samples
    rSearchWindow = 25; // size of search window
    muPositive = vector<float>(featureNum, 0.0f);
    muNegative = vector<float>(featureNum, 0.0f);
    sigmaPositive = vector<float>(featureNum, 1.0f);
    sigmaNegative = vector<float>(featureNum, 1.0f);
    learnRate = 0.85f;  // Learning rate parameter
}

TrackerCompressive::TrackerCompressive(const TrackerCompressive &obj)
{
    this->m_initialized = obj.m_initialized;
    this->featureMinNumRect = obj.featureMinNumRect;
    this->featureMaxNumRect = obj.featureMaxNumRect;
    this->featureNum = obj.featureNum;
    this->rOuterPositive = obj.rOuterPositive;
    this->rSearchWindow = obj.rSearchWindow;
    this->muPositive = obj.muPositive;
    this->muNegative = obj.muNegative;
    this->sigmaPositive = obj.sigmaPositive;
    this->sigmaNegative = obj.sigmaNegative;
    this->learnRate = obj.learnRate;
}

TrackerCompressive & TrackerCompressive::operator=(const TrackerCompressive &obj)
{
  // check for "self assignment" and do nothing in that case
    if (this == &obj) return *this;
    else
    {
        this->m_initialized = obj.m_initialized;
        this->featureMinNumRect = obj.featureMinNumRect;
        this->featureMaxNumRect = obj.featureMaxNumRect;
        this->featureNum = obj.featureNum;
        this->rOuterPositive = obj.rOuterPositive;
        this->rSearchWindow = obj.rSearchWindow;
        this->muPositive = obj.muPositive;
        this->muNegative = obj.muNegative;
        this->sigmaPositive = obj.sigmaPositive;
        this->sigmaNegative = obj.sigmaNegative;
        this->learnRate = obj.learnRate;
        return *this;                   // return this IntList
    }
}

TrackerCompressive::~TrackerCompressive() { }

void TrackerCompressive::reset() { } // TO-DO

void TrackerCompressive::initialize(const ImageRep& image, FloatRect bb)
{
    bbox = bb.toCvRect();
    float fROP = (float)rOuterPositive;
    float fRSW = (float)rSearchWindow;

    // compute feature template
    computeHaarFeature(bbox, featureNum);

    // compute sample templates
    cv::Mat img = image.getImage();
    sampleRect(img, bbox, fROP, 0.0f, 1000000.0f, samplePositiveBox);
    sampleRect(img, bbox, fRSW * 1.5f, fROP + 4.0f, 100.0f, sampleNegativeBox);

    integral(image.getImage(), imageIntegral, CV_32F);

    getFeatureValue(imageIntegral, samplePositiveBox, samplePositiveFeatureValue);
    getFeatureValue(imageIntegral, sampleNegativeBox, sampleNegativeFeatureValue);
    classifierUpdate(samplePositiveFeatureValue, muPositive, sigmaPositive, learnRate);
    classifierUpdate(sampleNegativeFeatureValue, muNegative, sigmaNegative, learnRate);

    m_initialized = true;
}


cv::Rect TrackerCompressive::track(const ImageRep& image)
{
    // predict
    cv::Mat img = image.getImage();
    float fROP = (float)rOuterPositive;
    float fRSW = (float)rSearchWindow;
    sampleRect(img, bbox, fRSW, detectBox);
    integral(img, imageIntegral, CV_32F);
    getFeatureValue(imageIntegral, detectBox, detectFeatureValue);
    int radioMaxIndex;
    float radioMax;
    radioClassifier(muPositive, sigmaPositive, muNegative, sigmaNegative, detectFeatureValue, radioMax, radioMaxIndex);
    bbox = detectBox[radioMaxIndex];

    // update
    sampleRect(img, bbox, fROP, 0.0f, 1000000.0f, samplePositiveBox);
    sampleRect(img, bbox, fRSW * 1.5f, fROP + 4.0f, 100.0f, sampleNegativeBox);

    getFeatureValue(imageIntegral, samplePositiveBox, samplePositiveFeatureValue);
    getFeatureValue(imageIntegral, sampleNegativeBox, sampleNegativeFeatureValue);
    classifierUpdate(samplePositiveFeatureValue, muPositive, sigmaPositive, learnRate);
    classifierUpdate(sampleNegativeFeatureValue, muNegative, sigmaNegative, learnRate);

    return bbox;
}

// ================== Private Functions =====================

void TrackerCompressive::computeHaarFeature(Rect& _objectBox, int _numFeature)
/*Description: compute Haar features
  Arguments:
  -_objectBox: [x y width height] object rectangle
  -_numFeature: total number of features.The default is 50.
*/
{
    features = vector<vector<Rect>>(_numFeature, vector<Rect>());
    featuresWeight = vector<vector<float>>(_numFeature, vector<float>());

    int numRect;
    Rect rectTemp;
    float weightTemp;

    for (int i = 0; i < _numFeature; i++)
    {
        numRect = cvFloor(rng.uniform((double)featureMinNumRect, (double)featureMaxNumRect));

        for (int j = 0; j < numRect; j++)
        {

            rectTemp.x = cvFloor(rng.uniform(0.0, (double)(_objectBox.width - 3)));
            rectTemp.y = cvFloor(rng.uniform(0.0, (double)(_objectBox.height - 3)));
            rectTemp.width = cvCeil(rng.uniform(0.0, (double)(_objectBox.width - rectTemp.x - 2)));
            rectTemp.height = cvCeil(rng.uniform(0.0, (double)(_objectBox.height - rectTemp.y - 2)));
            features[i].push_back(rectTemp);

            weightTemp = (float)pow(-1.0, cvFloor(rng.uniform(0.0, 2.0))) / sqrt(float(numRect));
            featuresWeight[i].push_back(weightTemp);

        }
    }
}


void TrackerCompressive::sampleRect(const Mat& _image, Rect& _objectBox, float _rInner, float _rOuter, int _maxSampleNum, vector<Rect>& _sampleBox)
/* Description: compute the coordinate of positive and negative sample image templates
   Arguments:
   -_image:        processing frame
   -_objectBox:    recent object position
   -_rInner:       inner sampling radius
   -_rOuter:       Outer sampling radius
   -_maxSampleNum: maximal number of sampled images
   -_sampleBox:    Storing the rectangle coordinates of the sampled images.
*/
{
    int rowsz = _image.rows - _objectBox.height - 1;
    int colsz = _image.cols - _objectBox.width - 1;
    float inradsq = _rInner*_rInner;
    float outradsq = _rOuter*_rOuter;
    int minrow = max(0, (int)_objectBox.y - (int)_rInner);
    int maxrow = min((int)rowsz - 1, (int)_objectBox.y + (int)_rInner);
    int mincol = max(0, (int)_objectBox.x - (int)_rInner);
    int maxcol = min((int)colsz - 1, (int)_objectBox.x + (int)_rInner);
    int i = 0;
    float prob = ((float)(_maxSampleNum)) / (maxrow - minrow + 1) / (maxcol - mincol + 1);
    int r, c, dist;

    _sampleBox.clear();//important
    Rect rec(0, 0, 0, 0);

    for (r = minrow; r <= (int)maxrow; r++) {
        for (c = mincol; c <= (int)maxcol; c++) {
            dist = (_objectBox.y - r)*(_objectBox.y - r) + (_objectBox.x - c)*(_objectBox.x - c);

            if (rng.uniform(0., 1.) < prob && dist < inradsq && dist >= outradsq) {

                rec.x = c;
                rec.y = r;
                rec.width = _objectBox.width;
                rec.height = _objectBox.height;

                _sampleBox.push_back(rec);

                i++;
            }
        }
    }
    _sampleBox.resize(i);
}

void TrackerCompressive::sampleRect(const Mat& _image, Rect& _objectBox, float _srw, vector<Rect>& _sampleBox)
/* Description: Compute the coordinate of samples when detecting the object.*/
{
    int rowsz = _image.rows - _objectBox.height - 1;
    int colsz = _image.cols - _objectBox.width - 1;
    float inradsq = _srw*_srw;


    int dist;

    int minrow = max(0, (int)_objectBox.y - (int)_srw);
    int maxrow = min((int)rowsz - 1, (int)_objectBox.y + (int)_srw);
    int mincol = max(0, (int)_objectBox.x - (int)_srw);
    int maxcol = min((int)colsz - 1, (int)_objectBox.x + (int)_srw);

    int i = 0;

    int r;
    int c;

    Rect rec(0, 0, 0, 0);
    _sampleBox.clear();//important

    for (r = minrow; r <= (int)maxrow; r++)
        for (c = mincol; c <= (int)maxcol; c++) {
            dist = (_objectBox.y - r)*(_objectBox.y - r) + (_objectBox.x - c)*(_objectBox.x - c);

            if (dist < inradsq) {

                rec.x = c;
                rec.y = r;
                rec.width = _objectBox.width;
                rec.height = _objectBox.height;

                _sampleBox.push_back(rec);

                i++;
            }
        }

    _sampleBox.resize(i);

}
// Compute the features of samples
void TrackerCompressive::getFeatureValue(Mat& _imageIntegral, vector<Rect>& _sampleBox, Mat& _sampleFeatureValue)
{
    int sampleBoxSize = (int)_sampleBox.size();
    _sampleFeatureValue.create(featureNum, sampleBoxSize, CV_32F);
    float tempValue;
    int xMin;
    int xMax;
    int yMin;
    int yMax;

    for (int i = 0; i < featureNum; i++)
    {
        for (int j = 0; j < sampleBoxSize; j++)
        {
            tempValue = 0.0f;
            for (size_t k = 0; k < features[i].size(); k++)
            {
                xMin = _sampleBox[j].x + features[i][k].x;
                xMax = _sampleBox[j].x + features[i][k].x + features[i][k].width;
                yMin = _sampleBox[j].y + features[i][k].y;
                yMax = _sampleBox[j].y + features[i][k].y + features[i][k].height;
                tempValue += featuresWeight[i][k] *
                    (_imageIntegral.at<float>(yMin, xMin) +
                    _imageIntegral.at<float>(yMax, xMax) -
                    _imageIntegral.at<float>(yMin, xMax) -
                    _imageIntegral.at<float>(yMax, xMin));
            }
            _sampleFeatureValue.at<float>(i, j) = tempValue;
        }
    }
}

// Update the mean and variance of the gaussian classifier
void TrackerCompressive::classifierUpdate(Mat& _sampleFeatureValue, vector<float>& _mu, vector<float>& _sigma, float _learnRate)
{
    Scalar muTemp;
    Scalar sigmaTemp;

    for (int i = 0; i < featureNum; i++)
    {
        meanStdDev(_sampleFeatureValue.row(i), muTemp, sigmaTemp);

        // equation 6 in paper
        _sigma[i] = (float)sqrt(_learnRate*_sigma[i] * _sigma[i] + (1.0f - _learnRate)*sigmaTemp.val[0] * sigmaTemp.val[0] +
                                _learnRate*(1.0f - _learnRate)*(_mu[i] - muTemp.val[0])*(_mu[i] - muTemp.val[0]));
        _mu[i] = _mu[i] * _learnRate + (1.0f - _learnRate)*muTemp.val[0];
    }
}

// Compute the ratio classifier
void TrackerCompressive::radioClassifier(vector<float>& _muPos, vector<float>& _sigmaPos, vector<float>& _muNeg, vector<float>& _sigmaNeg,
                                         Mat& _sampleFeatureValue, float& _radioMax, int& _radioMaxIndex)
{
    float sumRadio;
    _radioMax = -FLT_MAX;
    _radioMaxIndex = 0;
    float pPos;
    float pNeg;
    int sampleBoxNum = _sampleFeatureValue.cols;

    for (int j = 0; j < sampleBoxNum; j++)
    {
        sumRadio = 0.0f;
        for (int i = 0; i < featureNum; i++)
        {
            pPos = exp((_sampleFeatureValue.at<float>(i, j) - _muPos[i])*(_sampleFeatureValue.at<float>(i, j) - _muPos[i]) / -(2.0f*_sigmaPos[i] * _sigmaPos[i] + 1e-30)) / (_sigmaPos[i] + 1e-30);
            pNeg = exp((_sampleFeatureValue.at<float>(i, j) - _muNeg[i])*(_sampleFeatureValue.at<float>(i, j) - _muNeg[i]) / -(2.0f*_sigmaNeg[i] * _sigmaNeg[i] + 1e-30)) / (_sigmaNeg[i] + 1e-30);
            sumRadio += log(pPos + 1e-30) - log(pNeg + 1e-30);  // equation 4
        }
        if (_radioMax < sumRadio)
        {
            _radioMax = sumRadio;
            _radioMaxIndex = j;
        }
    }
}

#endif/*FACE_RECOG_HAS_COMPRESSIVE*/
