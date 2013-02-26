#ifndef DATAMODEL_H
#define DATAMODEL_H

#include <QtGui>
#include <QAbstractTableModel>
#include <QVector>
#include <QVariant>
#include "exceptions.h"
#include "colormanager.h"

class DataModel : public QAbstractTableModel {
    Q_OBJECT

public:
    explicit DataModel(QObject *parent = 0);

    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;

    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole);
    bool setHeaderData(int section, Qt::Orientation orientation, const QVariant &value, int role = Qt::EditRole);
    Qt::ItemFlags flags(const QModelIndex &index) const;

    bool insertRows(int row, int count, const QModelIndex &parent = QModelIndex());
    bool insertColumns(int column, int count, const QModelIndex &parent = QModelIndex());
    bool removeRows(int row, int count, const QModelIndex &parent = QModelIndex());
    bool removeColumns(int column, int count, const QModelIndex &parent = QModelIndex());

    bool insertRow(int row, const QModelIndex &parent = QModelIndex());
    bool insertColumn(int column, const QModelIndex &parent = QModelIndex());
    bool removeRow(int row, const QModelIndex &parent = QModelIndex());
    bool removeColumn(int column, const QModelIndex &parent = QModelIndex());

    bool moveRow(int source, int destination, const QModelIndex &parent = QModelIndex());
    bool moveColumn(int source, int destination, const QModelIndex &parent = QModelIndex());

    void clear();
    bool isModified() const;
    void setModified(bool flag);
    QVariant headerColor(int section, Qt::Orientation orientation) const;
    bool setHeaderColor(int section, Qt::Orientation orientation, const QVariant &value);

    QVariant chartType() const;
    bool setChartType(const QVariant &type);
    bool setActiveColorHeader(Qt::Orientation orientation);
    Qt::Orientation activeColorHeader() const;

    void loadXML(QFile &file) throw(XMLParserException, FileException);
    void saveXML(QFile &file) const throw(FileException);

signals:
    // Emesso ogni volta che il modello cambia
    void contentChanged();
    void chartTypeChanged(const QVariant &data);

private:
    void notifyChange();

    // La tabella con i dati
    QVector<QVector<QVariant> > table;

    // L'intestazione delle colonne, cioè il nome delle serie
    QVector<QVariant> horizontalHeader;
    //QVector<QVariant> verticalHeader;

    // Il colore della riga o colonna
    QVector<QVariant> rowColor;
    QVector<QVariant> columnColor;

    // I colori da visualizzare sono quelli nella header orientata
    Qt::Orientation activeColorHeaderOrient;

    // Il tipo di grafico (torta, barre, linee...)
    QVariant chartTypeVariant;

    // Il numero di colonne della tabella, usato (anche) per sapere quante colonne da dare ad una nuova riga quando viene inserita
    int numCols;

    // Indica se ci sono delle modifiche non salvate
    bool modified;
};

#endif // DATAMODEL_H
