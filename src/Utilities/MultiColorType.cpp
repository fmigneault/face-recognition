#include "Utilities/MultiColorType.h"

const ColorCode rgbColorCode(const ColorType& colorType)
{
    switch (colorType) {
        case ColorType::BLACK:          return CV_RGB(  0,   0,   0);
        case ColorType::DARK_BLUE:      return CV_RGB(  0,   0, 128);
        case ColorType::BLUE:           return CV_RGB(  0,   0, 255);
        case ColorType::DARK_GREEN:     return CV_RGB(  0, 128,   0);        
        case ColorType::TEAL:           return CV_RGB(  0, 128, 128);
        case ColorType::LIGHT_BLUE:     return CV_RGB(  0, 128, 255);
        case ColorType::GREEN:          return CV_RGB(  0, 255,   0);
        case ColorType::LIGHT_GREEN:    return CV_RGB(  0, 255, 128);
        case ColorType::CYAN:           return CV_RGB(  0, 255, 255);
        case ColorType::DARK_RED:       return CV_RGB(128,   0,   0);
        case ColorType::PURPLE:         return CV_RGB(128,   0, 128);
        case ColorType::LIGHT_PURPLE:   return CV_RGB(128,   0, 255);
        case ColorType::DARK_YELLOW:    return CV_RGB(128, 128,   0);
        case ColorType::DARK_GRAY:      return CV_RGB(128, 128, 128);
        case ColorType::FADE_BLUE:      return CV_RGB(128, 128, 255);
        case ColorType::BRIGHT_GREEN:   return CV_RGB(128, 255,   0);
        case ColorType::FADE_GREEN:     return CV_RGB(128, 255, 128);
        case ColorType::LIGHT_CYAN:     return CV_RGB(128, 255, 255);
        case ColorType::RED:            return CV_RGB(255,   0,   0); 
        case ColorType::DARK_PINK:      return CV_RGB(255,   0, 128);
        case ColorType::PINK:           return CV_RGB(255,   0, 255);        
        case ColorType::ORANGE:         return CV_RGB(255, 128,   0);        
        case ColorType::SALMON:         return CV_RGB(255, 128, 128);
        case ColorType::LIGHT_PINK:     return CV_RGB(255, 128, 255);
        case ColorType::YELLOW:         return CV_RGB(255, 255,   0);
        case ColorType::LIGHT_YELLOW:   return CV_RGB(255, 255, 128);
        case ColorType::WHITE:          return CV_RGB(255, 255, 255);
        default:                        return rgbColorCode(BLACK);
    }
}

const ColorCode bgrColorCode(const ColorType& colorType)
{
    ColorCode rgb = rgbColorCode(colorType);
    return ColorCode(rgb[2], rgb[1], rgb[0], rgb[3]);
}

const ColorCode MultiColorType::getColorCode(const ColorType& colorType)
{
    return rgbColorCode(colorType);
}

MultiColorType::MultiColorType(int value)
{
    if      (value == (int)MultiColorType::GREEN_ORANGE_RED)    _type = MultiColorType::GREEN_ORANGE_RED;
    else if (value == (int)MultiColorType::RED_ORANGE_GREEN)    _type = MultiColorType::RED_ORANGE_GREEN;
    else                                                        _type = UNDEFINED_DEFAULT_BLACK;
}

const std::vector<ColorCode> MultiColorType::getColorCodes(const MultiColorType::Value& multiColorType)
{
    switch (multiColorType) {
        case MultiColorType::GREEN_ORANGE_RED:      
            return std::vector<ColorCode>{ MultiColorType::getColorCode(GREEN),
                                           MultiColorType::getColorCode(ORANGE),
                                           MultiColorType::getColorCode(RED) };
        case MultiColorType::RED_ORANGE_GREEN:
            return std::vector<ColorCode>{ MultiColorType::getColorCode(RED),
                                           MultiColorType::getColorCode(ORANGE), 
                                           MultiColorType::getColorCode(GREEN) };
        default:
            return std::vector<ColorCode>{ MultiColorType::getColorCode(BLACK) };
    }
}

const ColorCode MultiColorType::getColorCode(const MultiColorType::Value& multiColorType, size_t index)
{
    std::vector<ColorCode> colorCodes = MultiColorType::getColorCodes(multiColorType);
    if (index < 0 || index >= colorCodes.size())
        return colorCodes[0];
    return colorCodes[index];
}

const std::vector<ColorCode> MultiColorType::getColorCodes() const
{
    return getColorCodes(this->_type);
}

const ColorCode MultiColorType::getColorCode(size_t index) const
{
    return getColorCode(this->_type, index);
}