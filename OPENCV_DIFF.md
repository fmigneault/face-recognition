## OpenCV Diff ##

Add to `opencv/sources/modules/objdetect/src/cascadedetect.hpp` the following method declaration inside `class CascadeClassifierImpl : public BaseCascadeClassifier`:

```C++
double classify(InputArray image, double &weight);
```

Add to `opencv/sources/modules/objdetect/src/cascadedetect.cpp` the following functions definitions:

```C++
double CascadeClassifierImpl::classify(InputArray image, double &weight)
{
    double confidence = -1;
    Ptr<FeatureEvaluator> evaluator = featureEvaluator->clone();
    Size newSize = getOriginalWindowSize();
    Mat dst, grayImage;
    resize(image, dst, newSize);

    if (dst.channels() > 1)
        cvtColor(dst, grayImage, COLOR_BGR2GRAY);
    else
        dst.copyTo(grayImage);

    if(!evaluator->setImage(grayImage, std::vector<float>(1, 1.0))){
        return -1;
    }
    int result = runAt(evaluator, Point(0,0), 0, weight);


    if( result != 1 ){
        double numberStages = data.stages.size();
        double currentStage = -result;
        confidence = currentStage /numberStages;
    }
    else 
        confidence = (double)result;

    return confidence;
}

double CascadeClassifier::classify(InputArray image, double &weight)
{
    Ptr<CascadeClassifierImpl> ccimpl = cc.dynamicCast<CascadeClassifierImpl>();
    return ccimpl->classify(image, weight);
}
```
