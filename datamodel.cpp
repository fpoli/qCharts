#include "datamodel.h"

DataModel::DataModel(QObject *parent) : QAbstractTableModel(parent)
{
    activeColorHeaderOrient = Qt::Horizontal;
    numCols = 0;
    modified = false;
}

int DataModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return table.count();
}

int DataModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return numCols;
}

QVariant DataModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid()) {
        return QVariant();
    }

    if (role == Qt::DisplayRole || role == Qt::EditRole) {
        return table[index.row()][index.column()];
    }

    return QVariant();
}

QVariant DataModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Horizontal && (role == Qt::DisplayRole || role == Qt::EditRole)) {
        if (section < 0 || section >= columnCount()) {
            return QVariant();
        } else {
            return horizontalHeader[section];
        }
    }
    if (orientation == Qt::Vertical && (role == Qt::DisplayRole || role == Qt::EditRole)) {
        return QVariant();
    }
    if (role == Qt::DecorationRole && orientation == activeColorHeader()) {

        // Non c'è il colore per la prima colonna (label)
        if (orientation == Qt::Horizontal && section == 0) {
            return QVariant();
        }

        QVariant color = headerColor(section, orientation);
        if (!color.value<QColor>().isValid()) {
            return QVariant();
        } else {
            QPixmap icon(20,20);
            icon.fill(color.value<QColor>());
            return icon;
        }
    }
    return QAbstractTableModel::headerData(section, orientation, role);
}

bool DataModel::setData (const QModelIndex &index, const QVariant &value, int role)
{
    if (!index.isValid()) {
        return false;
    }

    if (role == Qt::EditRole) {
        table[index.row()][index.column()] = value;
        emit(dataChanged(index, index));
        notifyChange();
        return true;
    }

    return false;
}

bool DataModel::setHeaderData(int section, Qt::Orientation orientation, const QVariant &value, int role)
{
    if (orientation == Qt::Horizontal && role == Qt::EditRole) {
        if (section < 0 || section >= columnCount()) {
            return false;
        } else {
            horizontalHeader[section] = value;
            emit(headerDataChanged(orientation, section, section));
            notifyChange();
            return true;
        }
    }
    return QAbstractTableModel::setHeaderData(section, orientation, value, role);
}

Qt::ItemFlags DataModel::flags(const QModelIndex &index) const
{
    Qt::ItemFlags defaultFlags = QAbstractTableModel::flags(index);

    if (index.isValid()) {
        return Qt::ItemIsEditable | defaultFlags;
    } else {
        return defaultFlags;
    }
}


bool DataModel::insertRows(int row, int count, const QModelIndex &parent)
{
    Q_UNUSED(parent);
    if (row > rowCount()) {
        return false;
    }

    beginInsertRows(QModelIndex(), row, row+count-1);
    table.insert(row, count, QVector<QVariant>(columnCount()));
    //verticalHeader.insert(row, count, QVariant());
    rowColor.insert(row, count, QVariant());
    for (int i=row; i<row+count; ++i) {
        rowColor[i] = ColorManager::getColor();
    }
    endInsertRows();
    notifyChange();
    return true;
}

bool DataModel::insertColumns(int column, int count, const QModelIndex &parent)
{
    Q_UNUSED(parent);
    if (column > columnCount()) {
        return false;
    }

    beginInsertColumns(QModelIndex(), column, column+count-1);
    horizontalHeader.insert(column, count, QVariant());
    columnColor.insert(column, count, QVariant());
    for (int i=column; i<column+count; ++i) {
        columnColor[i] = ColorManager::getColor();
    }
    for (int i = 0; i < rowCount(); ++i) {
        table[i].insert(column, count, QVariant());
    }
    endInsertColumns();
    numCols += count;
    notifyChange();
    return true;
}

bool DataModel::removeRows(int row, int count, const QModelIndex &parent)
{
    Q_UNUSED(parent);
    if (row > rowCount()) {
        return false;
    }

    beginRemoveRows(QModelIndex(), row, row+count-1);
    //verticalHeader.remove(row, count);
    rowColor.remove(row, count);
    table.remove(row, count);
    endRemoveRows();
    notifyChange();
    return true;
}

bool DataModel::removeColumns(int column, int count, const QModelIndex &parent)
{
    Q_UNUSED(parent);
    if (column > columnCount()) {
        return false;
    }

    beginRemoveColumns(QModelIndex(), column, column+count-1);
    horizontalHeader.remove(column, count);
    columnColor.remove(column, count);
    for (int i = 0; i < rowCount(); ++i) {
        table[i].remove(column, count);
    }
    numCols -= count;
    endRemoveColumns();
    notifyChange();
    return true;
}


bool DataModel::insertRow(int row, const QModelIndex &parent)
{
    return insertRows(row, 1, parent);
}

bool DataModel::insertColumn(int column, const QModelIndex &parent)
{
    return insertColumns(column, 1, parent);
}

bool DataModel::removeRow(int row, const QModelIndex &parent)
{
    return removeRows(row, 1, parent);
}

bool DataModel::removeColumn(int column, const QModelIndex &parent)
{
    return removeColumns(column, 1, parent);
}


bool DataModel::moveRow(int source, int destination, const QModelIndex &parent)
{
    if (source < 0 || source >= rowCount() || destination < 0 || destination >= rowCount()) {
        return false;
    }
    if (source == destination) {
        return false;
    }

    beginRemoveRows(parent, source, source);
    beginInsertRows(parent, destination +(source<destination)?0:1, destination +(source<destination)?0:1);

    QVariant temp;
    //temp = verticalHeader[source];
    //verticalHeader.remove(source);
    //verticalHeader.insert(destination, temp);

    temp = rowColor[source];
    rowColor.remove(source);
    rowColor.insert(destination, temp);

    QVector<QVariant> tempRow;
    tempRow = table[source];
    table.remove(source);
    table.insert(destination, tempRow);

    endInsertRows();
    endRemoveRows();
    notifyChange();
    return true;
}

bool DataModel::moveColumn(int source, int destination, const QModelIndex &parent)
{
    if (source < 0 || source >= columnCount() || destination < 0 || destination >= columnCount()) {
        return false;
    }
    if (source == destination) {
        return false;
    }

    beginRemoveColumns(parent, source, source);
    beginInsertColumns(parent, destination +(source<destination)?0:1, destination +(source<destination)?0:1);

    QVariant temp;
    temp = horizontalHeader[source];
    horizontalHeader.remove(source);
    horizontalHeader.insert(destination, temp);

    temp = columnColor[source];
    columnColor.remove(source);
    columnColor.insert(destination, temp);

    for (int i = 0; i < rowCount(); ++i) {
        temp = table[i][source];
        table[i].remove(source);
        table[i].insert(destination, temp);
    }

    endInsertColumns();
    endRemoveColumns();
    notifyChange();
    return true;
}


void DataModel::clear()
{
    if (rowCount() > 0) {
        removeColumns(0,columnCount());
        removeRows(0,rowCount());
    }
    if (numCols > 0) {
        numCols = 0;
        notifyChange();
    }
}

bool DataModel::isModified() const
{
    return modified;
}

void DataModel::setModified(bool flag)
{
    modified = flag;
}

void DataModel::notifyChange()
{
    modified = true;
    emit contentChanged();
}

QVariant DataModel::headerColor(int section, Qt::Orientation orientation) const
{
    if (orientation == Qt::Horizontal) {
        if (section < 0 || section >= columnCount()) {
            return QVariant();
        } else {
           return columnColor[section];
        }
    }
    if (orientation == Qt::Vertical) {
        if (section < 0 || section >= rowCount()) {
            return QVariant();
        } else {
            return rowColor[section];
        }
    }
    return QVariant();
}

bool DataModel::setHeaderColor(int section, Qt::Orientation orientation, const QVariant &value)
{
    if (orientation == Qt::Horizontal) {
        if (section < 0 || section >= columnCount()) {
            return false;
        } else {
            columnColor[section] = value;
            emit(headerDataChanged(orientation, section, section));
            notifyChange();
            return true;
        }
    }
    if (orientation == Qt::Vertical) {
        if (section < 0 || section >= rowCount()) {
            return false;
        } else {
            rowColor[section] = value;
            emit(headerDataChanged(orientation, section, section));
            notifyChange();
            return true;
        }
    }
    return false;
}


QVariant DataModel::chartType() const
{
    return chartTypeVariant;
}

bool DataModel::setChartType(const QVariant &type)
{
    if (chartTypeVariant == type) return true;

    chartTypeVariant = type;
    notifyChange();
    emit chartTypeChanged(type);
    return true;
}

bool DataModel::setActiveColorHeader(Qt::Orientation orientation)
{
    activeColorHeaderOrient = orientation;
    return true;
}

Qt::Orientation DataModel::activeColorHeader() const
{
    return activeColorHeaderOrient;
}

void DataModel::loadXML(QFile &file) throw(XMLParserException, FileException)
{
    if (!file.open(QFile::ReadOnly | QFile::Text)) {
        throw FileException(file.errorString());
    }

    clear();

    QXmlStreamReader xml(&file);

    int currChart = 0;
    int currRow = 0;
    int currCol = 0;

    bool previousBlock = blockSignals(true);
    while(!xml.atEnd() && !xml.hasError()) {
        // Read next element.
        QXmlStreamReader::TokenType token = xml.readNext();

        // If token is just StartDocument, we'll go to next.
        if (token == QXmlStreamReader::StartDocument) {
            continue;
        }

        // If token is StartElement, we'll see if we can read it.
        if (token == QXmlStreamReader::StartElement) {

            if (xml.name() == "chart") {
                if (currChart > 0) {
                    qWarning() << "Può esserci solo un 'chart' nel file xml (per adesso)";
                    break;
                }
                if (xml.attributes().hasAttribute("type")) {
                    setChartType(xml.attributes().value("type").toString());
                }
                continue;
            }
            if (xml.name() == "column") {
                if (currCol >= columnCount()) {
                    insertColumn(columnCount());
                }
                if (xml.attributes().hasAttribute("name")) {
                    setHeaderData(currCol, Qt::Horizontal, xml.attributes().value("name").toString());
                }
                if (xml.attributes().hasAttribute("color")) {
                    setHeaderColor(currCol, Qt::Horizontal, xml.attributes().value("color").toString());
                }
                continue;
            }
            if (xml.name() == "item") {
                if (currRow >= rowCount()) {
                    insertRow(rowCount());
                }
                if (xml.attributes().hasAttribute("color")) {
                    setHeaderColor(currRow, Qt::Vertical, xml.attributes().value("color").toString());
                }
                setData(index(currRow, currCol), xml.readElementText());
                currRow += 1;
                continue;
            }

            qWarning() << "Parser: non riconosco il token " << qPrintable(xml.name().toString());
        }

        if (token == QXmlStreamReader::EndElement) {
            if (xml.name() == "chart") {
                currChart += 1;
                currCol = 0;
                currRow = 0;
                continue;
            }
            if (xml.name() == "column") {
                currCol += 1;
                currRow = 0;
                continue;
            }
            if (xml.name() == "item") {
                currRow += 1;
                continue;
            }

            qWarning() << "Parser: non riconosco il token " << qPrintable(xml.name().toString());
        }
    }
    // Removes any device() or data from the reader
    // and resets its internal state to the initial state.
    xml.clear();
    blockSignals(previousBlock);

    file.close();
    notifyChange();
    emit chartTypeChanged(chartTypeVariant);
    emit headerDataChanged(Qt::Horizontal, 0, columnCount());
    emit headerDataChanged(Qt::Vertical, 0, rowCount());
    emit dataChanged(index(0,0), index(rowCount()-1, columnCount()-1));

    if (xml.hasError()) {
        throw XMLParserException(qPrintable(xml.errorString()));
    }
}

void DataModel::saveXML(QFile &file) const throw(FileException)
{
    QTextStream stream(&file);

    if (!file.open(QFile::WriteOnly)) {
        throw FileException(file.errorString());
    }

    QXmlStreamWriter xmlWriter(&file);
    xmlWriter.setAutoFormatting(true);
    xmlWriter.writeStartDocument();
    xmlWriter.writeStartElement("chart");
    if (!chartType().toString().isEmpty()) {
        xmlWriter.writeAttribute("type", chartType().toString());
    }

    for (int c=0; c<columnCount(); ++c) {
        xmlWriter.writeStartElement("column");
        xmlWriter.writeAttribute("name", headerData(c, Qt::Horizontal, Qt::DisplayRole).toString());
        if (!headerColor(c, Qt::Horizontal).toString().isEmpty()) {
            xmlWriter.writeAttribute("color", headerColor(c, Qt::Horizontal).toString());
        }

        for (int r=0; r<rowCount(); ++r) {
            xmlWriter.writeStartElement("item");
            if (c == 0)
                xmlWriter.writeAttribute("color", headerColor(r, Qt::Vertical).toString());
            xmlWriter.writeCharacters(data(index(r, c), Qt::DisplayRole).toString());
            xmlWriter.writeEndElement();
        }

        xmlWriter.writeEndElement(); // COLUMN
    }

    xmlWriter.writeEndElement(); // CHART
    xmlWriter.writeEndDocument();

    file.close();
}
