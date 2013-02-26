#ifndef M_PI
#define M_PI 3.1415927
#endif

#include "barchart.h"

BarChart::BarChart(QWidget *parent) : BaseChart(parent)
{
    margin = 8;
    maxWidth = 100;
    maxHeight = 100;
}

QVariant BarChart::staticType()
{
    return "bar";
}

QVariant BarChart::dynamicType() const
{
    return BarChart::staticType();
}

Qt::Orientation BarChart::activeColorHeader() const
{
    return Qt::Horizontal;
}


void BarChart::updateProperties()
{
    updatePropertiesHelper(true, true, false, false);
    int rowCount = model()->rowCount();
    int columnCount = model()->columnCount();
    double gap = 1.0/(columnCount-1);
    double size = 1.0/(rowCount + (rowCount-1)*gap);
    double step = size * (1+gap);

    QModelIndex index;
    for (int column = 1; column < columnCount; ++column) {
        for (int row = 0; row < rowCount; ++row) {
            index = model()->index(row, column);
            setItemProperty(index, "minx", step*row + (column-1)*size/(columnCount-1), 0.0);
            setItemProperty(index, "maxx", step*row + column*size/(columnCount-1), 0.0);
        }
    }
}

QPainterPath BarChart::itemPath(const QModelIndex &index) const
{
    double value = static_cast<int>(getItemProperty(index, "value").toDouble() * maxHeight);
    double offset = static_cast<int>(getItemProperty(index, "offset").toDouble() * maxHeight);
    double minx = static_cast<int>(getItemProperty(index, "minx").toDouble() * maxWidth);
    double maxx = static_cast<int>(getItemProperty(index, "maxx").toDouble() * maxWidth);

    QPainterPath path;
    path.moveTo(minx, maxHeight-offset);
    path.lineTo(minx, maxHeight-(offset+value));
    path.lineTo(maxx, maxHeight-(offset+value));
    path.lineTo(maxx, maxHeight-offset);
    path.closeSubpath();
    path.translate(margin, margin);
    return path;
}

void BarChart::paintEvent(QPaintEvent *event)
{
    QStyleOptionViewItem option = viewOptions();

    QBrush background = option.palette.base();
    QPen foreground(option.palette.color(QPalette::WindowText));
    QPen textPen(option.palette.color(QPalette::Text));
    QPen highlightedPen(option.palette.color(QPalette::HighlightedText));

    QPainter painter(viewport());
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setRenderHint(QPainter::SmoothPixmapTransform);

    painter.fillRect(event->rect(), background);
    painter.setPen(foreground);
    painter.setPen(Qt::transparent);

    maxWidth = viewport()->geometry().width() - 2*margin;
    maxHeight = viewport()->geometry().height() - 2*margin;

    QModelIndex index;
    for (int column = 1; column < model()->columnCount(); ++column) {
        for (int row = 0; row < model()->rowCount(); ++row) {
            index = model()->index(row, column);
            drawItem(painter, index);
        }
    }
}

