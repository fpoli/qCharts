#ifndef M_PI
#define M_PI 3.1415927
#endif

#include "linechart.h"

LineChart::LineChart(QWidget *parent) : BaseChart(parent)
{
    margin = 8;
    maxWidth = 100;
    maxHeight = 100;
}

QVariant LineChart::staticType()
{
    return "line";
}

QVariant LineChart::dynamicType() const
{
    return LineChart::staticType();
}

Qt::Orientation LineChart::activeColorHeader() const
{
    return Qt::Horizontal;
}


void LineChart::updateProperties()
{
    updatePropertiesHelper(true, true, false, false);
    int rowCount = model()->rowCount();
    int columnCount = model()->columnCount();
    double step = 1.0/(rowCount-1);
    double radius = qMax(1.0/400, 1.0/(100+rowCount));

    QModelIndex index;
    for (int column = 1; column < columnCount; ++column) {
        for (int row = 0; row < rowCount; ++row) {
            index = model()->index(row, column);
            if (rowCount == 1)
                setItemProperty(index, "x", 0.5, 0.0);
            else
                setItemProperty(index, "x", step*row, 0.0);
            setItemProperty(index, "radius", radius, 0.0);
        }
    }
}

QPainterPath LineChart::itemPath(const QModelIndex &index) const
{
    double value = getItemProperty(index, "value").toDouble() * maxHeight;
    double offset = getItemProperty(index, "offset").toDouble() * maxHeight;
    double x = getItemProperty(index, "x").toDouble() * maxWidth;
    double radius = getItemProperty(index, "radius").toDouble() * maxWidth;

    QPainterPath path;
    path.addEllipse(QPointF(x, maxHeight-(offset+value)), radius, radius);
    path.translate(margin, margin);
    return path;
}

void LineChart::paintEvent(QPaintEvent *event)
{
    QStyleOptionViewItem option = viewOptions();

    QBrush background = option.palette.base();
    QPen foreground(option.palette.color(QPalette::WindowText));
    QPen penpath(option.palette.color(QPalette::WindowText));
    penpath.setWidth(3);
    penpath.setColor(Qt::black);
    QPen textPen(option.palette.color(QPalette::Text));
    QPen highlightedPen(option.palette.color(QPalette::HighlightedText));

    QPainter painter(viewport());
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setRenderHint(QPainter::SmoothPixmapTransform);

    painter.fillRect(event->rect(), background);
    foreground.setColor(Qt::transparent);
    painter.setPen(foreground);

    maxWidth = viewport()->geometry().width() - 2*margin;
    maxHeight = viewport()->geometry().height() - 2*margin;

    QPainterPath path;

    painter.save();
    painter.translate(margin, margin);

    painter.setPen(QColor(0,0,0,40));
    path.moveTo(0.5, maxHeight+0.5);
    path.lineTo(maxWidth+0.5, maxHeight+0.5);
    painter.drawPath(path);

    QModelIndex index, oldindex;

    painter.setPen(QColor(0,0,0,20));
    for (int row = 0; row < model()->rowCount(); ++row) {
        index = model()->index(row, 1);
        path = QPainterPath();
        path.moveTo(static_cast<int>(getItemProperty(index, "x").toDouble()*maxWidth)+0.5, maxHeight+0.5);
        path.lineTo(static_cast<int>(getItemProperty(index, "x").toDouble()*maxWidth)+0.5, 0.5);
        painter.drawPath(path);
    }

    painter.restore();

    QPointF curr(0,0), old(0,0);
    QColor color;
    for (int column = 1; column < model()->columnCount(); ++column) {
        for (int row = 0; row < model()->rowCount(); ++row) {
            oldindex = index;
            index = model()->index(row, column);

            old = curr;
            curr = QPointF(
                margin+ maxWidth*getItemProperty(index, "x").toDouble(),
                margin+ maxHeight*(1 - getItemProperty(index, "value").toDouble() - getItemProperty(index, "offset").toDouble())
            );
            if (row ==0) {
                color = qvariant_cast<QColor>(getItemProperty(index, "color"));
                color.setAlpha(128);
                penpath.setColor(color);
            } else {
                painter.setPen(penpath);
                painter.drawLine(old, curr);

                painter.setPen(foreground);
                drawItem(painter, oldindex);
            }
        }
        painter.setPen(foreground);
        drawItem(painter, index);
    }
}

