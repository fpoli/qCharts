#include "animatedchart.h"

AnimatedChart::AnimatedChart(QWidget *parent, bool _useAnimation) : AbstractChart(parent)
{
    useAnimation = _useAnimation;

    animationTimer = new QTimer(this);
    animationTimer->setInterval(1000/30);
    connect(animationTimer, SIGNAL(timeout()), this, SLOT(animate()));
    stopAnimationTimer = new QTimer(this);
    stopAnimationTimer->setSingleShot(true);
    connect(stopAnimationTimer, SIGNAL(timeout()), this, SLOT(endAnimation()));
}

void AnimatedChart::setModel(DataModel *model)
{
    AbstractChart::setModel(model);

    // Migliora un po' le performance di setItemProperty
    // anche se il costo maggiore sono le "new ChartItem"
    itemProperty.reserve(model->rowCount() * model->columnCount());
}

void AnimatedChart::animate()
{
    viewport()->update();
}

void AnimatedChart::beginAnimation(int msecs)
{
    animationTimer->start();
    stopAnimationTimer->start(msecs+100);
}

void AnimatedChart::endAnimation()
{
    animationTimer->stop();
}

QVariant AnimatedChart::getItemProperty(const QModelIndex &index, const char* name) const
{
    if (itemProperty.find(index) == itemProperty.end()) {
        return QVariant();
    } else {
        return itemProperty[index]->property(name);
    }
}

bool AnimatedChart::setItemProperty(const QModelIndex &index, const char* name, const QVariant& value, const QVariant& defaultValue)
{
    //qDebug() << name << value.toString() << defaultValue.toString();

    ChartItem *item;
    if (itemProperty.find(index) == itemProperty.end()) {
        item = new ChartItem(this);
        itemProperty[index] = item;
    } else {
        item = itemProperty[index];
    }

    if (useAnimation) {
        if (!item->property(name).isValid()) {
            item->setProperty(name, defaultValue);
        }

        QPropertyAnimation *animation = new QPropertyAnimation(item, name, this);
        // Varia un po' l'animazione
        if (activeColorHeader()==Qt::Horizontal)
            animation->setDuration(500+1500/(2+index.row()));
        else
            animation->setDuration(500+1500-1500/(1+index.column()));

        animation->setEndValue(value.isValid()?value:defaultValue);
        animation->setEasingCurve(QEasingCurve::InOutQuad);

        beginAnimation(2000);
        animation->start(QAbstractAnimation::DeleteWhenStopped);
        return true;
    } else {
        return item->setProperty(name, value.isValid()?value:defaultValue);
    }

}
