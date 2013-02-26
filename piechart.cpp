#ifndef M_PI
#define M_PI 3.1415927
#endif

#include "piechart.h"

PieChart::PieChart(QWidget *parent) : BaseChart(parent)
{
    margin = 8;
    maxRadius = 100;
}

QVariant PieChart::staticType()
{
    return "pie";
}

QVariant PieChart::dynamicType() const
{
    return PieChart::staticType();
}

Qt::Orientation PieChart::activeColorHeader() const
{
    return Qt::Vertical;
}


void PieChart::updateProperties()
{
    updatePropertiesHelper(true, true, true, true);
    int columnCount = model()->columnCount();
    double unit = 1.0/(2*( columnCount-1 + 0.5 ));

    QModelIndex index;
    for (int column = 1; column < columnCount; ++column) {
        for (int row = 0; row < model()->rowCount(); ++row) {
            index = model()->index(row, column);
            if (columnCount > 2) {
                setItemProperty(index, "minr", unit + 2*unit*(column-1), 0.0);
                setItemProperty(index, "maxr", unit + 2*unit*column, 0.0);
            } else {
                setItemProperty(index, "minr", 0.0, 0.0);
                setItemProperty(index, "maxr", 1.0, 0.0);
            }
        }
    }
}

QPainterPath PieChart::itemPath(const QModelIndex &index) const
{
    double amp = getItemProperty(index, "value").toDouble() * 360;
    double deg = getItemProperty(index, "offset").toDouble() * 360;
    double minr = getItemProperty(index, "minr").toDouble() * maxRadius;
    double maxr = getItemProperty(index, "maxr").toDouble() * maxRadius;

    QPainterPath path;
    path.moveTo(minr*cos(deg/180.0*M_PI), -minr*sin(deg/180.0*M_PI));
    path.arcTo( -minr, -minr, 2*minr, 2*minr, deg, amp );
    path.arcTo( -maxr, -maxr, 2*maxr, 2*maxr, deg+amp, -amp );
    path.closeSubpath();
    path.translate(viewport()->geometry().width()/2.0, viewport()->geometry().height()/2.0);
    return path;
}

void PieChart::paintEvent(QPaintEvent *event)
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
    painter.setPen(Qt::white);

    maxRadius = qMin(viewport()->geometry().width(), viewport()->geometry().height())/2.0 - margin;

    QModelIndex index;
    for (int column = 1; column < model()->columnCount(); ++column) {
        for (int row = 0; row < model()->rowCount(); ++row) {
            index = model()->index(row, column);
            drawItem(painter, index);
        }
    }
}

