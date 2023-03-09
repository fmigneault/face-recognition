#ifndef FACE_RECOG_CLASSIFIER_TYPE
#define FACE_RECOG_CLASSIFIER_TYPE

#include <iostream>
#include <fstream>

class ClassifierType
{
public:
    enum Value
    {
        UNDEFINED       = -1,
        ENSEMBLE_ESVM   =  0,
        ENSEMBLE_TM     =  1,
        CNN_FACE_NET    =  2,
    };
    inline ClassifierType() { ClassifierType(-1); }
    inline ClassifierType(Value type) { _type = type; }
    ClassifierType(int value);
    const std::string name() const;
    inline bool isDefined() const { return _type != UNDEFINED; }
    inline virtual bool operator== (const Value& classifierType) const { return _type == classifierType; }
    inline virtual bool operator!= (const Value& classifierType) const { return _type != classifierType; }    
    friend std::ostream& operator<< (std::ostream& os, const ClassifierType& ct);
    friend std::istream& operator>> (std::istream& is, ClassifierType& ct);
    friend std::istream& operator>> (std::istream& is, Value& v);
private:
    Value _type;
};

#endif/*FACE_RECOG_CLASSIFIER_TYPE*/
