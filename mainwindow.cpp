/****************************************************************************
**
** Copyright (C) 2012 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** You may use this file under the terms of the BSD license as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of Digia Plc and its Subsidiary(-ies) nor the names
**     of its contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "mainwindow.h"

MainWindow::MainWindow()
{
    tabWidget = new QTabWidget(this);
    tabWidget->setTabPosition(QTabWidget::North);
    tabWidget->setDocumentMode(true);
    tabWidget->setMovable(true);
    tabWidget->setTabsClosable(true);
    setCentralWidget(tabWidget);

    QObject::connect(tabWidget, SIGNAL(currentChanged(int)), this, SLOT(onTabChanged(int)));
    QObject::connect(tabWidget, SIGNAL(tabCloseRequested(int)), this, SLOT(onTabCloseRequested(int)));

    setWindowTitle(tr("qCharts"));
    createActions();
    updateActions();
    createMenus();
    createToolBars();
    createStatusBar();
    readSettings();

    // Example data
    //open("hugedata.xml");
    //open("mydata.xml");
    //open("qtdata.xml");
    //open("test.xml");
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    // Chiudi tutte le schede
    bool ignoreCloseEvent = false;
    for (int i=0; i < tabWidget->count(); ++i) {
        if(!tabWidget->widget(i)->close()) {
            ignoreCloseEvent = true;
        }
    }
    if (ignoreCloseEvent) {
        event->ignore();
    } else {
        writeSettings();
        event->accept();
    }
}

void MainWindow::setStatusBarText(const QString &status)
{
    statusBar()->showMessage(status);
}

void MainWindow::updateTabsTitle()
{
    // Per ogni tab, aggiorna il titolo della tab a tab->getTitle()
    Sheet* tab;
    for (int i=0; i < tabWidget->count(); ++i) {
        tab = dynamic_cast<Sheet *>(tabWidget->widget(i));
        if (tab) tabWidget->setTabText(i, tab->getShortTitle());
    }
    // Per la tab corrente aggiorna anche il titolo della finestra
    Sheet* curr = dynamic_cast<Sheet *>(tabWidget->currentWidget());
    if (curr) setWindowTitle(tr("%1 - qCharts").arg(curr->getLongTitle()));
}

void MainWindow::setChooseChartCombo(const QVariant &data)
{
    chooseChart->setCurrentIndex(chooseChart->findData(data));
}

Sheet* MainWindow::newSheet()
{
    Sheet *childSheet = new Sheet(tabWidget);
    tabWidget->addTab(childSheet, childSheet->getShortTitle());
    tabWidget->setCurrentWidget(childSheet);
    updateActions();

    connect(childSheet, SIGNAL(statusEvent(QString)), this, SLOT(setStatusBarText(QString)));
    connect(childSheet, SIGNAL(titleChanged()), this, SLOT(updateTabsTitle()));
    return childSheet;
}

void MainWindow::onTabCloseRequested(int index)
{
    tabWidget->widget(index)->close();
    updateActions();
}

void MainWindow::onTabChanged(int index)
{
    Sheet* curr = dynamic_cast<Sheet *>(tabWidget->widget(index));
    if (curr) setWindowTitle(tr("%1 - qCharts").arg(curr->getLongTitle()));
    updateActions();
}

void MainWindow::addRow()
{
    Sheet* curr = dynamic_cast<Sheet *>(tabWidget->currentWidget());
    if (curr) curr->addRow();
}

void MainWindow::addColumn()
{
    Sheet* curr = dynamic_cast<Sheet *>(tabWidget->currentWidget());
    if (curr) curr->addColumn();
}

void MainWindow::open(const QString &filename)
{
    Sheet *childSheet = newSheet();

    if (!childSheet->open(filename)) {
        childSheet->close();
    }
}

void MainWindow::save()
{
    Sheet* curr = dynamic_cast<Sheet *>(tabWidget->currentWidget());
    if (curr) curr->save();
}

void MainWindow::saveAs()
{
    Sheet* curr = dynamic_cast<Sheet *>(tabWidget->currentWidget());
    if (curr) curr->saveAs();
}

void MainWindow::changeChartType(int index)
{
    Sheet* curr = dynamic_cast<Sheet *>(tabWidget->currentWidget());
    if (curr) curr->getModel()->setChartType(chooseChart->itemData(index));
}

void MainWindow::saveImage()
{
    Sheet* curr = dynamic_cast<Sheet *>(tabWidget->currentWidget());
    if (curr) curr->saveImage();
}

void MainWindow::about()
{
   QMessageBox::about(this, tr("Informazioni"), tr("<b>qCharts</b><p>Realizzato da Federico Poli come progetto per il corso di Programmazione ad Oggetti tenuto dal prof. Ranzato</p><p>Giugno 2013</p>"));
}

void MainWindow::createActions()
{
    // File
    newAction = new QAction(QIcon("./icons/page_blank_add_32.png"), tr("&Nuovo"), this);
    newAction->setShortcuts(QKeySequence::New);
    connect(newAction, SIGNAL(triggered()), this, SLOT(newSheet()));

    openAction = new QAction(QIcon("./icons/folder_32.png"), tr("&Apri file..."), this);
    openAction->setShortcuts(QKeySequence::Open);
    connect(openAction, SIGNAL(triggered()), this, SLOT(open()));

    saveAction = new QAction(QIcon("./icons/save_32.png"), tr("&Salva"), this);
    saveAction->setShortcuts(QKeySequence::Save);
    connect(saveAction, SIGNAL(triggered()), this, SLOT(save()));

    saveAsAction = new QAction(tr("Salva &come..."), this);
    saveAsAction->setShortcuts(QKeySequence::SaveAs);
    connect(saveAsAction, SIGNAL(triggered()), this, SLOT(saveAs()));

    quitAction = new QAction(tr("&Esci"), this);
    quitAction->setShortcuts(QKeySequence::Quit);
    connect(quitAction, SIGNAL(triggered()), qApp, SLOT(quit()));

    // Aiuto
    aboutAction = new QAction(tr("Informazioni"), this);
    aboutAction->setShortcuts(QKeySequence::HelpContents);
    connect(aboutAction, SIGNAL(triggered()), this, SLOT(about()));

    // Dati
    addRowAction = new QAction(QIcon("./icons/add_32.png"), tr("Aggiungi &riga"), this);
    connect(addRowAction, SIGNAL(triggered()), this, SLOT(addRow()));

    addColumnAction = new QAction(QIcon("./icons/add_32.png"), tr("Aggiungi &colonna"), this);
    connect(addColumnAction, SIGNAL(triggered()), this, SLOT(addColumn()));

    // Grafico
    saveImageAction = new QAction(QIcon("./icons/save_download_32.png"), tr("Salva immagine"), this);
    connect(saveImageAction, SIGNAL(triggered()), this, SLOT(saveImage()));

    chooseChart = new QComboBox(this);
    chooseChart->addItem(QIcon("./icons/chart_pie_32.png"), tr("Grafico a torta"), PieChart::staticType());
    chooseChart->addItem(QIcon("./icons/chart_bar_32.png"), tr("Grafico a barre impilate"), StackChart::staticType());
    chooseChart->addItem(QIcon("./icons/chart_bar_32.png"), tr("Grafico a barre affiancate"), BarChart::staticType());
    chooseChart->addItem(QIcon("./icons/chart_32.png"), tr("Grafico a linee"), LineChart::staticType());
    //chooseChart->addItem(QIcon("./icons/chart_32.png"), tr("Grafico a radar"), RadarChart::staticType());
    connect(chooseChart, SIGNAL(currentIndexChanged(int)), this, SLOT(changeChartType(int)));
}

void MainWindow::updateActions()
{
    bool anySheet = tabWidget->currentWidget();
    saveAction->setEnabled(anySheet);
    saveAsAction->setEnabled(anySheet);
    addRowAction->setEnabled(anySheet);
    addColumnAction->setEnabled(anySheet);
    saveImageAction->setEnabled(anySheet);
    chooseChart->setEnabled(anySheet);

    Sheet* curr = dynamic_cast<Sheet *>(tabWidget->currentWidget());
    if (curr) {
        chooseChart->setCurrentIndex(chooseChart->findData(curr->getModel()->chartType()));
        if (chooseChart->currentIndex() < 0) {
            chooseChart->setCurrentIndex(chooseChart->findData(PieChart::staticType()));
        }
    }
}

void MainWindow::createMenus()
{
    fileMenu = new QMenu(tr("&File"), this);
    fileMenu->addAction(newAction);
    fileMenu->addAction(openAction);
    fileMenu->addAction(saveAction);
    fileMenu->addAction(saveAsAction);
    fileMenu->addSeparator();
    fileMenu->addAction(quitAction);
    menuBar()->addMenu(fileMenu);

    helpMenu = new QMenu(tr("&Aiuto"), this);
    helpMenu->addAction(aboutAction);
    menuBar()->addMenu(helpMenu);
}

void MainWindow::createToolBars()
{
    // File
    QToolBar *fileToolBar = addToolBar(tr("Dati"));
    fileToolBar->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    fileToolBar->addAction(newAction);
    fileToolBar->addAction(openAction);
    fileToolBar->addAction(saveAction);

    // Dati
    dataToolBar = addToolBar(tr("Dati"));
    dataToolBar->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    dataToolBar->addAction(addRowAction);
    dataToolBar->addAction(addColumnAction);

    // Grafico
    chartToolBar = addToolBar(tr("Grafico"));
    chartToolBar->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    chartToolBar->addWidget(chooseChart);
    chartToolBar->addAction(saveImageAction);
}

void MainWindow::createStatusBar()
{
    statusBar()->showMessage(tr("Pronto"));
}

void MainWindow::readSettings()
{
    QSettings settings("MyOrganization", "qCharts");
    QPoint pos = settings.value("pos", QPoint(200, 200)).toPoint();
    QSize size = settings.value("size", QSize(900, 600)).toSize();
    move(pos);
    resize(size);
}

void MainWindow::writeSettings()
{
    QSettings settings("MyOrganization", "qCharts");
    settings.setValue("pos", pos());
    settings.setValue("size", size());
}
