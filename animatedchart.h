#ifndef ANIMATEDCHART_H
#define ANIMATEDCHART_H

#include <Qt>
#include <QtGui>
#include "abstractchart.h"

class AnimatedChart : public AbstractChart
{
    Q_OBJECT
public:
    explicit AnimatedChart(QWidget *parent = 0, bool _useAnimation = true);
    virtual void setModel(DataModel *model);

protected slots:
    virtual void animate();

private slots:
    void beginAnimation(int msecs);
    void endAnimation();

protected:
    virtual QVariant getItemProperty(const QModelIndex &index, const char* name) const;
    virtual bool setItemProperty(const QModelIndex &index, const char* name, const QVariant& value, const QVariant& defaultValue = double(0));

private:
    bool useAnimation;
    QTimer *animationTimer;
    QTimer *stopAnimationTimer;

    QHash<IndexHash, ChartItem*> itemProperty;
};

#endif // ANIMATEDCHART_H
