#include "abstractchart.h"

AbstractChart::AbstractChart(QWidget *parent) : QAbstractItemView(parent)
{
    _model = NULL;
}

void AbstractChart::setModel(DataModel *model)
{
    _model = model;
    QAbstractItemView::setModel(model);
    model->setActiveColorHeader(activeColorHeader());
    connect(model, SIGNAL(contentChanged()), this, SLOT(updateProperties()));
}

DataModel* AbstractChart::model() const
{
    return _model;
}

void AbstractChart::saveImage(QFile &file) throw(FileException)
{
    if (!file.open(QIODevice::WriteOnly)) {
        throw FileException(file.errorString());
    }

    QPixmap pixmap(size());
    render(&pixmap);
    pixmap.save(&file, "PNG");

    file.close();
}

void AbstractChart::updateProperties()
{
}

void AbstractChart::dataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight)
{
    QAbstractItemView::dataChanged(topLeft, bottomRight);
    updateProperties();
    viewport()->update();
}
