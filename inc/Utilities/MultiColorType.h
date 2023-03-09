#ifndef FACE_RECOG_MULTI_COLOR_TYPE_H
#define FACE_RECOG_MULTI_COLOR_TYPE_H

#include "opencv2/imgproc.hpp"
#include <vector>

typedef cv::Scalar ColorCode;

enum ColorType
{
    BLACK,
    DARK_BLUE,
    BLUE,
    DARK_GREEN,    
    TEAL,
    LIGHT_BLUE,
    GREEN,
    LIGHT_GREEN,
    CYAN,
    DARK_RED,
    PURPLE,
    LIGHT_PURPLE,
    DARK_YELLOW,
    GRAY,
    DARK_GRAY,
    FADE_BLUE,
    BRIGHT_GREEN,
    FADE_GREEN,
    LIGHT_CYAN,
    RED,
    DARK_PINK,
    PINK,
    ORANGE,
    SALMON,
    LIGHT_PINK,
    YELLOW,
    LIGHT_YELLOW,
    WHITE,
};

const ColorCode rgbColorCode(const ColorType& colorType);
const ColorCode bgrColorCode(const ColorType& colorType);

class MultiColorType
{
public:    
    enum Value
    {
        UNDEFINED_DEFAULT_BLACK  = -1,
        GREEN_ORANGE_RED         =  0,
        RED_ORANGE_GREEN         =  1,         
    };
    inline MultiColorType() { MultiColorType(0); }
    inline MultiColorType(Value type) { _type = type; }
    MultiColorType(int value);
    inline virtual bool operator== (const Value& multiColorType) const { return _type == multiColorType; }
    inline virtual bool operator!= (const Value& multiColorType) const { return _type != multiColorType; }
    const std::vector<ColorCode> getColorCodes() const;
    const ColorCode getColorCode(size_t index = 0) const;
    static const std::vector<ColorCode> getColorCodes(const Value& multiColorType);
    static const ColorCode getColorCode(const Value& multiColorType, size_t index = 0);
    static const ColorCode getColorCode(const ColorType& colorType);
private:    
    Value _type;
};

#endif/*FACE_RECOG_MULTI_COLOR_TYPE_H*/
