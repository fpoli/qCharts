#ifndef BASECHART_H
#define BASECHART_H

#include <QtGui>
#include "animatedchart.h"

// QObject non supporta l'ereditarietà a diamante :(
class BaseChart : public AnimatedChart
{
    Q_OBJECT
public:
    explicit BaseChart(QWidget *parent = 0);

    QRect visualRect(const QModelIndex &index) const;
    void scrollTo(const QModelIndex &index, ScrollHint hint = EnsureVisible);

    QModelIndex indexAt(const QPoint &point) const;

public slots:
    void updateProperties();

protected:
    QModelIndex moveCursor(CursorAction cursorAction, Qt::KeyboardModifiers modifiers);

    void updatePropertiesHelper(bool color = true, bool normalize = false, bool normalizeIndipendent = false, bool stacked = false);

    int horizontalOffset() const;
    int verticalOffset() const;

    bool isIndexHidden(const QModelIndex &index) const;

    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);

    void setSelection(const QRect &rect, QItemSelectionModel::SelectionFlags command);
    QRegion visualRegionForSelection(const QItemSelection &selection) const;

    virtual QPainterPath itemPath(const QModelIndex &index) const;
    void drawItem(QPainter& painter, const QModelIndex &index) const;
    void paintEvent(QPaintEvent *event);

signals:

protected slots:
    void currentChanged(const QModelIndex & current, const QModelIndex & previous);
    void selectionChanged(const QItemSelection & selected, const QItemSelection & deselected);

private:
    QPoint origin;
    QRubberBand *rubberBand;
};

#endif // BASECHART_H
