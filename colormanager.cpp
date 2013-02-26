#include "colormanager.h"

double randUnit() {
    return static_cast<double>(qrand())/RAND_MAX;
}

const double ColorManager::golden_angle = 0.618033988749895;
double ColorManager::hue = randUnit();

QColor ColorManager::getColor()
{
    hue += golden_angle;
    if (hue >= 1) hue -= 1;

    QColor color;
    color.setHsv(static_cast<int>(hue*256), 255*0.9, 255*0.95);
    return color;
}

ColorManager::ColorManager() {}
