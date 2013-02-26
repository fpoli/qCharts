#ifndef COLORMANAGER_H
#define COLORMANAGER_H

#include <QtGui>
#include <cstdlib>

class ColorManager {
public:
    static QColor getColor();
private:
    ColorManager();
    const static double golden_angle;
    static double hue;
};

#endif // COLORMANAGER_H
