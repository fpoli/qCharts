#ifndef ABSTRACTCHART_H
#define ABSTRACTCHART_H

#include <Qt>
#include <QtGui>
#include "exceptions.h"
#include "datamodel.h"
#include "indexhash.h"
#include "chartitem.h"

class AbstractChart : public QAbstractItemView
{
    Q_OBJECT
public:
    explicit AbstractChart(QWidget *parent = 0);
    virtual void setModel(DataModel *model);
    DataModel* model() const;

    void saveImage(QFile &file) throw(FileException);

    // Metodi virtuali puri
    static QVariant staticType();
    virtual QVariant dynamicType() const = 0;
    virtual Qt::Orientation activeColorHeader() const = 0;

public slots:
    virtual void updateProperties();

protected:
    // Metodi virtuali puri
    virtual QVariant getItemProperty(const QModelIndex &index, const char* name) const = 0;
    virtual bool setItemProperty(const QModelIndex &index, const char* name, const QVariant& value, const QVariant& defaultValue = double(0)) = 0;

protected slots:
    virtual void dataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight);

private:
    DataModel *_model;
};

#endif // ABSTRACTCHART_H
