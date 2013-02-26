#ifndef CHARTITEM_H
#define CHARTITEM_H

#include <QObject>

class ChartItem : public QObject
{
    Q_OBJECT
public:
    explicit ChartItem(QObject *parent = 0);
};

#endif // CHARTITEM_H
