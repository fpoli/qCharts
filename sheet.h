#ifndef SHEET_H
#define SHEET_H

#include <QWidget>
#include "datamodel.h"
#include "headerview.h"
#include "tableview.h"
#include "abstractchart.h"

#include "piechart.h"
#include "stackchart.h"
#include "barchart.h"
#include "linechart.h"

class Sheet : public QWidget
{
    Q_OBJECT
public:
    explicit Sheet(QWidget *parent = 0);
    explicit Sheet(const QString &path, QWidget *parent = 0);

    QString getBaseTitle();
    QString getShortTitle();
    QString getLongTitle();
    DataModel* getModel();

signals:
    void statusEvent(const QString&);
    void titleChanged();

public slots:
    bool addRow();
    bool addColumn();
    bool open(const QString &path = QString());
    bool save();
    bool saveAs(const QString &path = QString());
    bool saveImage(const QString &path = QString());

private slots:
    void updateChartType();

protected:
    void closeEvent(QCloseEvent *event);

private:
    bool maybeSave();
    void init(QWidget *parent = 0);

    QString fileName;
    QSplitter *splitter;
    DataModel *model;
    TableView *tableView;
    AbstractChart *chartView;
    QItemSelectionModel *selectionModel;
};

#endif // SHEET_H
