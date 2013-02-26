#include "basechart.h"

BaseChart::BaseChart(QWidget *parent) : AnimatedChart(parent)
{
    rubberBand = 0;
}


QRect BaseChart::visualRect(const QModelIndex &index) const
{
    Q_UNUSED(index);
    return QRect();
}

void BaseChart::scrollTo(const QModelIndex &index, ScrollHint hint)
{
    Q_UNUSED(index);
    Q_UNUSED(hint);
}

QModelIndex BaseChart::indexAt(const QPoint &point) const
{
    int rowCount = model()->rowCount();
    int columnCount = model()->columnCount();
    QModelIndex index;
    for (int column = columnCount-1; column >= 1; --column) {
        for (int row = rowCount-1; row >= 0; --row) {
            index = model()->index(row, column, rootIndex());
            if (itemPath(index).contains(point)) {
                return index;
            }
        }
    }
    return QModelIndex();
}

void BaseChart::updateProperties()
{
    updatePropertiesHelper(true, true, false);
}

QModelIndex BaseChart::moveCursor(CursorAction cursorAction, Qt::KeyboardModifiers modifiers)
{
    Q_UNUSED(modifiers);
    QModelIndex current = currentIndex();

    switch (cursorAction) {
        case MoveLeft:
        if (current.column() > 1)
            current = model()->index(current.row(), current.column() - 1, rootIndex());
        else
            current = model()->index(current.row(), model()->columnCount()-1, rootIndex());
        break;

        case MoveUp:
            if (current.row() > 0)
                current = model()->index(current.row() - 1, current.column(), rootIndex());
            else
                current = model()->index(model()->rowCount()-1, current.column(), rootIndex());
            break;

        case MoveRight:
            if (current.column() + 1 < model()->columnCount())
                current = model()->index(current.row(), current.column() + 1, rootIndex());
            else
                current = model()->index(current.row(), 1, rootIndex());
            break;
        case MoveDown:
            if (current.row() + 1 < model()->rowCount())
                current = model()->index(current.row() + 1, current.column(), rootIndex());
            else
                current = model()->index(0, current.column(), rootIndex());
            break;

        default:
            break;
    }

    viewport()->update();
    return current;
}

void BaseChart::updatePropertiesHelper(bool color, bool normalize, bool normalizeIndipendent, bool stacked)
{
    double totalValue = 0;
    QModelIndex index;
    double value;
    double currValue;

    if (color) {
        for (int column = 1; column < model()->columnCount(); ++column) {
            currValue = 0;
            for (int row = 0; row < model()->rowCount(); ++row) {
                index = model()->index(row, column);
                if (activeColorHeader() == Qt::Horizontal)
                    setItemProperty(index, "color", model()->headerColor(column, Qt::Horizontal), QColor("white"));
                else
                    setItemProperty(index, "color", model()->headerColor(row, Qt::Vertical), QColor("white"));
            }
        }
    }

    if (normalize) {
        if (!normalizeIndipendent) {
            totalValue = 0;
            if (activeColorHeader() == Qt::Horizontal) { // raggruppa per righe
                for (int row = 0; row < model()->rowCount(); ++row) {
                    currValue = 0;
                    for (int column = 1; column < model()->columnCount(); ++column) {
                        index = model()->index(row, column);
                        value = model()->data(index).toDouble();
                        if (value < 0) value = 0;

                        if (stacked) {
                            currValue += value;
                        } else {
                            if (value > totalValue)
                                totalValue = value;
                        }
                    }
                    if (currValue > totalValue)
                        totalValue = currValue;
                }
            } else { // raggruppa per colonne
                for (int column = 1; column < model()->columnCount(); ++column) {
                    currValue = 0;
                    for (int row = 0; row < model()->rowCount(); ++row) {
                        index = model()->index(row, column);
                        value = model()->data(index).toDouble();
                        if (value < 0) value = 0;

                        if (stacked) {
                            currValue += value;
                        } else {
                            if (value > totalValue)
                                totalValue = value;
                        }
                    }
                    if (currValue > totalValue)
                        totalValue = currValue;
                }
            }
        }

        if (activeColorHeader() == Qt::Horizontal) { // raggruppa per righe
            for (int row = 0; row < model()->rowCount(); ++row) {
                if (normalizeIndipendent) {
                    totalValue = 0;
                    for (int column = 1; column < model()->columnCount(); ++column) {
                        index = model()->index(row, column);
                        value = model()->data(index).toDouble();
                        if (value < 0) value = 0;

                        if (stacked) {
                            totalValue += value;
                        } else {
                            if (value > totalValue)
                                totalValue = value;
                        }
                    }
                }

                currValue = 0;
                for (int column = 1; column < model()->columnCount(); ++column) {
                    index = model()->index(row, column);
                    value = model()->data(index).toDouble();
                    if (value < 0) value = 0;

                    if (totalValue > 0) {
                        if (stacked) setItemProperty(index, "offset", currValue/totalValue, 0.0);
                        setItemProperty(index, "value", value/totalValue, 0.0);
                    } else {
                        if (stacked) setItemProperty(index, "offset", 0.0, 0.0);
                        setItemProperty(index, "value", 0.0, 0.0);
                    }
                    currValue += value;
                }
            }
        } else { // raggruppa per colonne
            for (int column = 1; column < model()->columnCount(); ++column) {
                if (normalizeIndipendent) {
                    totalValue = 0;
                    for (int row = 0; row < model()->rowCount(); ++row) {
                        index = model()->index(row, column);
                        value = model()->data(index).toDouble();
                        if (value < 0) value = 0;

                        if (stacked) {
                            totalValue += value;
                        } else {
                            if (value > totalValue)
                                totalValue = value;
                        }
                    }
                }

                currValue = 0;
                for (int row = 0; row < model()->rowCount(); ++row) {
                    index = model()->index(row, column);
                    value = model()->data(index).toDouble();
                    if (value < 0) value = 0;

                    if (totalValue > 0) {
                        if (stacked) setItemProperty(index, "offset", currValue/totalValue, 0.0);
                        setItemProperty(index, "value", value/totalValue, 0.0);
                    } else {
                        if (stacked) setItemProperty(index, "offset", 0.0, 0.0);
                        setItemProperty(index, "value", 0.0, 0.0);
                    }
                    currValue += value;
                }
            }
        }
    } else { // Not Normalize
        for (int column = 1; column < model()->columnCount(); ++column) {
            currValue = 0;
            for (int row = 0; row < model()->rowCount(); ++row) {
                index = model()->index(row, column);
                value = model()->data(index).toDouble();
                if (value < 0) value = 0;

                if (stacked) setItemProperty(index, "offset", currValue, 0.0);
                setItemProperty(index, "value", value, 0.0);
                currValue += value;
            }
        }
    }
}


int BaseChart::horizontalOffset() const
{
    return 0;
}

int BaseChart::verticalOffset() const
{
    return 0;
}

bool BaseChart::isIndexHidden(const QModelIndex &index) const
{
    Q_UNUSED(index);
    return false;
}

void BaseChart::mousePressEvent(QMouseEvent *event)
{
    AnimatedChart::mousePressEvent(event);
    origin = event->pos();
    if (!rubberBand)
        rubberBand = new QRubberBand(QRubberBand::Rectangle, viewport());
    rubberBand->setGeometry(QRect(origin, QSize()));
    rubberBand->show();
}

void BaseChart::mouseMoveEvent(QMouseEvent *event)
{
    if (rubberBand) {
        rubberBand->setGeometry(QRect(origin, event->pos()).normalized());
        //setSelection(QRect(origin, event->pos()), selectionCommand(QModelIndex()));
    }
    AnimatedChart::mouseMoveEvent(event);
}

void BaseChart::mouseReleaseEvent(QMouseEvent *event)
{
    AnimatedChart::mouseReleaseEvent(event);
    if (rubberBand)
        rubberBand->hide();
    viewport()->update();
}

void BaseChart::setSelection(const QRect &rect, QItemSelectionModel::SelectionFlags command)
{
    // TODO meglio

    QRect contentsRect = rect.translated(horizontalScrollBar()->value(), verticalScrollBar()->value()).normalized();

    int rows = model()->rowCount(rootIndex());
    int columns = model()->columnCount(rootIndex());
    QItemSelection selection;

    for (int row = 0; row < rows; ++row) {
        for (int column = 1; column < columns; ++column) {
            QModelIndex index = model()->index(row, column, rootIndex());
            if (itemPath(index).intersects(contentsRect)) {
                // Interseca il rettangolo di selezione
                selection.select(index, index);
            }
        }
    }

    selectionModel()->select(selection, command);
    update();
}

QRegion BaseChart::visualRegionForSelection(const QItemSelection &selection) const
{
    Q_UNUSED(selection);
    return QRegion();
}

QPainterPath BaseChart::itemPath(const QModelIndex &index) const
{
    int step = 40;
    int radius = getItemProperty(index, "value").toInt();

    QPainterPath path;
    path.addEllipse(QPoint(step/2 + step*(index.column()-1), step/2 + step*index.row()), radius, radius);
    return path;
}

void BaseChart::drawItem(QPainter& painter, const QModelIndex &index) const
{
    QItemSelectionModel *selections = selectionModel();
    if (currentIndex() == index)
        painter.setBrush(QBrush(qvariant_cast<QColor>(getItemProperty(index, "color")), Qt::Dense4Pattern));
    else if (selections->isSelected(index))
        painter.setBrush(QBrush(qvariant_cast<QColor>(getItemProperty(index, "color")), Qt::Dense3Pattern));
    else
        painter.setBrush(qvariant_cast<QColor>(getItemProperty(index, "color")));

    painter.drawPath(itemPath(index));
}

void BaseChart::paintEvent(QPaintEvent *event)
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

    QModelIndex index;
    for (int column = 1; column < model()->columnCount(); ++column) {
        for (int row = 0; row < model()->rowCount(); ++row) {
            index = model()->index(row, column);
            drawItem(painter, index);
        }
    }
}

void BaseChart::currentChanged(const QModelIndex & current, const QModelIndex & previous)
{
    AnimatedChart::currentChanged(current, previous);
    viewport()->update();
}

void BaseChart::selectionChanged(const QItemSelection & selected, const QItemSelection & deselected)
{
    AnimatedChart::selectionChanged(selected, deselected);
    viewport()->update();
}
