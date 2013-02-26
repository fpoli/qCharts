#include "tableview.h"

DataModel* TableView::model() const
{
    return dynamic_cast<DataModel*>(QTableView::model());
}

TableView::TableView(QWidget *parent) : QTableView(parent)
{
    setTabKeyNavigation(true);
    setVerticalScrollMode(ScrollPerPixel);
    setHorizontalScrollMode(ScrollPerPixel);
    setAutoScroll(true);
    //setAlternatingRowColors(true);

    HeaderView *horizHeader = new HeaderView(Qt::Horizontal, this);
    HeaderDelegate* horizontalDelegate = new HeaderDelegate(Qt::Horizontal, horizHeader);
    setHorizontalHeader(horizHeader);
    horizHeader->setItemDelegate(horizontalDelegate);

    HeaderView *vertHeader = new HeaderView(Qt::Vertical, this);
    //HeaderDelegate* verticalDelegate = new HeaderDelegate(Qt::Vertical, vertHeader);
    setVerticalHeader(vertHeader);
    //vertHeader->setItemDelegate(verticalDelegate);

    horizontalHeader()->setMovable(true);
    verticalHeader()->setMovable(true);
    horizontalHeader()->setResizeMode(QHeaderView::Interactive);
    verticalHeader()->setResizeMode(QHeaderView::Interactive);

    horizontalHeader()->setContextMenuPolicy(Qt::CustomContextMenu);
    verticalHeader()->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(horizontalHeader(), SIGNAL(customContextMenuRequested( const QPoint& )), this, SLOT(contextMenuColumn( const QPoint& )));
    connect(verticalHeader(), SIGNAL(customContextMenuRequested( const QPoint& )), this, SLOT(contextMenuRow( const QPoint& )));

    connect(horizontalHeader(), SIGNAL(sectionMoved(int,int,int)), this, SLOT(columnMoved(int,int,int)));
    connect(verticalHeader(), SIGNAL(sectionMoved(int,int,int)), this, SLOT(rowMoved(int,int,int)));
}

void TableView::adjustTableSize()
{
    setVisible(false);
    resizeColumnsToContents();
    setVisible(true);
}

void TableView::contextMenuRow(const QPoint& pos)
{
    QMenu *menu = new QMenu(this);
    QAction *insert = new QAction(QIcon("./icons/add_16.png"), tr("Inserisci una riga"), this);
    insert->setIconVisibleInMenu(true);
    QAction *remove = new QAction(QIcon("./icons/close_16.png"), tr("Cancella la riga"), this);
    remove->setIconVisibleInMenu(true);
    QAction *choosecolor = new QAction(QIcon("./icons/color_circle_16.png"), tr("Scegli un colore"), this);
    choosecolor->setIconVisibleInMenu(true);

    int index = verticalHeader()->logicalIndexAt(pos);
    menu->addAction(insert);
    if (index > -1) {
        menu->addAction(remove);
        if (model()->activeColorHeader() == Qt::Vertical) {
            menu->addAction(choosecolor);
        }
    }

    QAction* selectedItem = menu->exec(QCursor::pos());
    if (!selectedItem) return;

    if (selectedItem == insert) {
        if (index > -1) {
            model()->insertRow(index);
        } else {
            model()->insertRow(model()->rowCount());
        }
    } else
    if (selectedItem == remove) {
        model()->removeRow(index);
    } else
    if (selectedItem == choosecolor) {
        QColor color = QColorDialog::getColor(model()->headerColor(index, Qt::Vertical).toString(), this );
        if (color.isValid())
            model()->setHeaderColor(index, Qt::Vertical, color);
    }
}

void TableView::contextMenuColumn(const QPoint& pos)
{
    QMenu *menu = new QMenu(this);
    QAction *insert = new QAction(QIcon("./icons/add_16.png"), tr("Inserisci una colonna"), this);
    insert->setIconVisibleInMenu(true);
    QAction *remove = new QAction(QIcon("./icons/close_16.png"), tr("Cancella la colonna"), this);
    remove->setIconVisibleInMenu(true);
    QAction *choosecolor = new QAction(QIcon("./icons/color_circle_16.png"), tr("Scegli un colore"), this);
    choosecolor->setIconVisibleInMenu(true);

    int index = horizontalHeader()->logicalIndexAt(pos);
    menu->addAction(insert);
    if (index > -1) {
        menu->addAction(remove);
        if (model()->activeColorHeader() == Qt::Horizontal && index != 0) {
            menu->addAction(choosecolor);
        }
    }

    QAction* selectedItem = menu->exec(QCursor::pos());
    if (!selectedItem) return;

    if (selectedItem == insert) {
        if (index > -1) {
            model()->insertColumn(index);
        } else {
            model()->insertColumn(model()->columnCount());
        }
    } else
    if (selectedItem == remove) {
        model()->removeColumn(index);
        adjustTableSize(); // Senza questo esteticamente è brutto
    } else
    if (selectedItem == choosecolor) {
        QColor color = QColorDialog::getColor(model()->headerColor(index, Qt::Horizontal).toString(), this );
        if (color.isValid())
            model()->setHeaderColor(index, Qt::Horizontal, color);
    }
}

void TableView::columnMoved(int logicalIndex, int oldVisualIndex, int newVisualIndex)
{
    disconnect(horizontalHeader(), SIGNAL(sectionMoved(int,int,int)), this, SLOT(columnMoved(int,int,int)));
    horizontalHeader()->moveSection(newVisualIndex, oldVisualIndex);
    connect(horizontalHeader(), SIGNAL(sectionMoved(int,int,int)), this, SLOT(columnMoved(int,int,int)));

    model()->moveColumn(
        logicalIndex,
        horizontalHeader()->logicalIndex(newVisualIndex)
    );
}

void TableView::rowMoved(int logicalIndex, int oldVisualIndex, int newVisualIndex)
{
    disconnect(verticalHeader(), SIGNAL(sectionMoved(int,int,int)), this, SLOT(rowMoved(int,int,int)));
    verticalHeader()->moveSection(newVisualIndex, oldVisualIndex);
    connect(verticalHeader(), SIGNAL(sectionMoved(int,int,int)), this, SLOT(rowMoved(int,int,int)));

    model()->moveRow(
        logicalIndex,
        verticalHeader()->logicalIndex(newVisualIndex)
    );
}
