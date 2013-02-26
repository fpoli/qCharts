#include "headerview.h"

HeaderView::HeaderView(Qt::Orientation orientation, QWidget* parent) : QHeaderView(orientation, parent)
{
    setClickable(true);
    setHighlightSections(true);
}

QRect HeaderView::sectionRect(int logicalIndex) const
{
    return sections.value(logicalIndex);
}

void HeaderView::mouseDoubleClickEvent(QMouseEvent* event)
{
    QHeaderView::mouseDoubleClickEvent(event);

    const int idx = visualIndexAt(pick(event->pos()));
    const QModelIndex persistent = model()->index(idx, 0);
    if (event->button() & Qt::LeftButton)
        edit(persistent, QAbstractItemView::DoubleClicked, event);
}

QSize HeaderView::sizeHint () const
{
    QSize proposed = QHeaderView::sizeHint();
    return QSize(qMax(proposed.rwidth(), 20), qMax(proposed.rheight(), 20));
}

void HeaderView::paintSection(QPainter* painter, const QRect& rect, int logicalIndex) const
{
    sections.insert(logicalIndex, rect);
    QHeaderView::paintSection(painter, rect, logicalIndex);
}

int HeaderView::pick(const QPoint& pos) const
{
    return orientation() == Qt::Horizontal ? pos.x() : pos.y();
}
