#ifndef TABLEVIEW_H
#define TABLEVIEW_H

#include <QTableView>
#include <QtGui>
#include "datamodel.h"
#include "headerview.h"
#include "headerdelegate.h"

class TableView : public QTableView
{
    Q_OBJECT
public:
    explicit TableView(QWidget *parent = 0);
    DataModel* model() const;

signals:
    
public slots:
    void adjustTableSize();

private slots:
    void contextMenuRow(const QPoint&);
    void contextMenuColumn(const QPoint&);
    void columnMoved(int, int, int);
    void rowMoved(int, int, int);
};

#endif // TABLEVIEW_H
