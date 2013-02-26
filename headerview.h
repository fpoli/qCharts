#ifndef HEADERVIEW_H
#define HEADERVIEW_H

#include <QHeaderView>
#include <QtGui>

class HeaderView : public QHeaderView
{
    Q_OBJECT

public:
    HeaderView(Qt::Orientation orientation, QWidget* parent = 0);
    QRect sectionRect(int logicalIndex) const;
    QSize sizeHint () const;

protected:
    void mouseDoubleClickEvent(QMouseEvent* event);
    void paintSection(QPainter* painter, const QRect& rect, int logicalIndex) const;

private:
    int pick(const QPoint& pos) const;

    mutable QHash<int, QRect> sections;
};

#endif // HEADERVIEW_H
