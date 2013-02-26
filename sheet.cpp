#include "sheet.h"

void Sheet::init(QWidget *parent)
{
    setAttribute(Qt::WA_DeleteOnClose);

    splitter = new QSplitter(this);
    model = new DataModel(splitter);
    tableView = new TableView(splitter);
    chartView = NULL;

    connect(model, SIGNAL(contentChanged()), this, SIGNAL(titleChanged()));
    connect(model, SIGNAL(chartTypeChanged(const QVariant&)), this, SLOT(updateChartType()));
    // Perchè deve ridimensionare gli header
    connect(model, SIGNAL(chartTypeChanged(const QVariant&)), tableView, SLOT(adjustTableSize()));
    // Cambia la ComboBox
    connect(model, SIGNAL(chartTypeChanged(const QVariant&)), parent->parent(), SLOT(setChooseChartCombo(const QVariant&)));

    tableView->setModel(model);
    splitter->addWidget(tableView);

    selectionModel = new QItemSelectionModel(model, this);
    //selectionModel = tableView->selectionModel();
    tableView->setSelectionModel(selectionModel);

    QHeaderView *headerView = tableView->horizontalHeader();
    headerView->setStretchLastSection(true);

    QGridLayout* grid = new QGridLayout;
    grid->addWidget(splitter);
    setLayout(grid);

    updateChartType();
}

Sheet::Sheet(QWidget *parent) : QWidget(parent)
{
    init(parent);
    model->insertRow(0);
    model->insertColumn(0);
    model->setHeaderData(0, Qt::Horizontal, tr("Indice"));
    model->insertColumn(1);
    model->setHeaderData(1, Qt::Horizontal, tr("Dati"));
    model->setModified(false);
}

Sheet::Sheet(const QString &path, QWidget *parent) : QWidget(parent)
{
    init(parent);
    open(path);
}

QString Sheet::getBaseTitle()
{
    QString title;
    if (fileName.isNull()) {
        return tr("Senza nome");
    } else {
        return QFileInfo(fileName).fileName();
    }
}

QString Sheet::getShortTitle()
{
    if (model->isModified())
        return getBaseTitle().prepend("*");
    else
        return getBaseTitle();
}

QString Sheet::getLongTitle()
{
    QString title;
    if (fileName.isNull()) {
        title = getBaseTitle();
    } else {
        title = tr("%1 (%2)").arg(getBaseTitle(), QFileInfo(fileName).absolutePath());
    }

    if (model->isModified())
        return title.prepend("*");
    else
        return title;
}

DataModel* Sheet::getModel()
{
    return model;
}

bool Sheet::addRow()
{
    return model->insertRow(model->rowCount());
}

bool Sheet::addColumn()
{
    bool ret = model->insertColumn(model->columnCount());
    if (ret)
        tableView->adjustTableSize();
    return ret;
}

bool Sheet::open(const QString &path)
{
    QString fileName;
    if (path.isNull())
        fileName = QFileDialog::getOpenFileName(this, tr("Scegli il file con i dati"), "", tr("Dati per il grafico *.xml (*.xml);;Tutti i file (*)"));
    else
        fileName = path;

    if (fileName.isEmpty()) {
        return false;
    }

    QFile file(fileName);
    if (!file.exists()) {
        QMessageBox::critical(this, tr("Caricamento file"), tr("Non è possibile aprire il file '%1'.").arg(fileName));
        emit statusEvent(tr("Errore nell'apertura del file '%1'").arg(fileName));
        return false;
    }

    try {
        model->loadXML(file);
    }
    catch (FileException exc) {
        QMessageBox::critical(this, tr("Caricamento file"), tr("Non è possibile caricare il file '%1': %2").arg(fileName).arg(exc));
        emit statusEvent(tr("Errore nel caricamento del file '%1': %2").arg(fileName).arg(exc));
        return false;
    }
    catch (XMLParserException exc) {
        QMessageBox::critical(this, tr("Caricamento file"), tr("Il formato del file '%1' non è corretto: %2").arg(fileName).arg(exc));
        emit statusEvent(tr("Il formato del file '%1' non è corretto: %2").arg(fileName).arg(exc));
        return false;
    }

    emit statusEvent(tr("Caricato %1").arg(fileName));
    this->fileName = fileName;
    model->setModified(false);
    emit titleChanged();
    tableView->adjustTableSize();
    return true;
}

bool Sheet::save()
{
    if (!this->fileName.isNull())
        return saveAs(this->fileName);
    else
        return saveAs(QString());
}

bool Sheet::saveAs(const QString &path)
{
    QString fileName;
    if (path.isNull())
        fileName = QFileDialog::getSaveFileName(this, tr("Salva con nome"), "", tr("Dati per il grafico *.xml (*.xml);;Tutti i file (*)"));
    else
        fileName = path;

    if (fileName.isEmpty()) {
        return false;
    }

    if (QFileInfo(fileName).suffix().isEmpty())
        fileName += ".xml";

    QFile file(fileName);

    try {
        model->saveXML(file);
    }
    catch (FileException exc) {
        QMessageBox::critical(this, tr("Salvataggio file"), tr("Non è possibile salvare il file '%1': %2").arg(fileName).arg(exc));
        emit statusEvent(tr("Errore nel salvataggio del file '%1': %2").arg(fileName).arg(exc));
        return false;
    }

    emit statusEvent(tr("Salvato %1").arg(fileName));
    this->fileName = fileName;
    model->setModified(false);
    emit titleChanged();
    return true;
}

bool Sheet::saveImage(const QString &path)
{
    QString fileName;
    if (path.isNull())
        fileName = QFileDialog::getSaveFileName(this, tr("Salva immagine"), "", tr("Immagine PNG (*.png);;Tutti i file (*)"));
    else
        fileName = path;

    if (fileName.isEmpty()) {
        return false;
    }

    if (QFileInfo(fileName).suffix().isEmpty())
        fileName += ".png";

    QFile file(fileName);

    try {
        chartView->saveImage(file);
    }
    catch (FileException exc) {
        QMessageBox::critical(this, tr("Salvataggio immagine"), tr("Non è possibile salvare l'immagine' '%1': %2").arg(fileName).arg(exc));
        emit statusEvent(tr("Errore nel salvataggio dell'immagine' '%1': %2").arg(fileName).arg(exc));
        return false;
    }

    emit statusEvent(tr("Salvata l'immagine' %1").arg(fileName));
    return true;
}

void Sheet::closeEvent(QCloseEvent *event)
{
    if (maybeSave()) {
        event->accept();
    } else {
        event->ignore();
    }
}

void Sheet::updateChartType()
{
    if (chartView && chartView->dynamicType().isValid() && chartView->dynamicType() == model->chartType()) {
        return;
    }

    QByteArray splitterSettings;

    if (chartView) {
        // Per ricordarsi le dimensioni dello splitter mentre cambiamo grafico
        splitterSettings = splitter->saveState();

        delete chartView;
        chartView = NULL;
    }

    if (model->chartType() == PieChart::staticType() || model->chartType().isNull()) {
        chartView = new PieChart(this);
    } else
    if (model->chartType() == StackChart::staticType()) {
        chartView = new StackChart(this);
    } else
    if (model->chartType() == BarChart::staticType()) {
        chartView = new BarChart(this);
    } else
    if (model->chartType() == LineChart::staticType()) {
        chartView = new LineChart(this);
    //} else
    //if (model->chartType() ==RadarChart::staticType()) {
    //    chartView = new RadarChart(this);
    } else {
        qWarning() << "Non riconosco il tipo di grafico" << model->chartType();
        chartView = new PieChart(this);
    }

    chartView->setModel(model);
    chartView->setSelectionModel(selectionModel);
    chartView->updateProperties();

    splitter->addWidget(chartView);
    // Ripristina le dimensioni dello spitter
    splitter->restoreState(splitterSettings);
}

bool Sheet::maybeSave()
{
    if (model->isModified()) {
        QMessageBox::StandardButton ret;
        ret = QMessageBox::warning(
            this,
            tr("Conferma"),
            tr("Il documento %1 è stato modificato.\nVuoi salvare le tue modifiche?").arg(getBaseTitle()),
            QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel
        );
        if (ret == QMessageBox::Save)
            return save();
        else if (ret == QMessageBox::Cancel)
            return false;
    }
    return true;
}
