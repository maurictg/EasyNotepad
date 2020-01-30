#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <etab.h>
#include <QMessageBox>
#include <QTextCharFormat>
#include <QTime>
#include <QTimer>
#include <QFileInfo>
#include <QFileDialog>
#include <QStandardPaths>
#include <QCloseEvent>
#include <iostream>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    QFont font("Consolas", 10);
    statusBar()->setFont(font);
    font.setPointSize(13);
    ui->tabs->setFont(font);

    //Initialize status bar
    this->lblClock = new QLabel("Hi!");
    lblClock->setAlignment(Qt::AlignLeft);
    statusBar()->addWidget(lblClock, 1);

    this->lblStatus = new QLabel("ln: 0 col: 0 ");
    lblStatus->setAlignment(Qt::AlignRight);
    statusBar()->addWidget(lblStatus, 1);

    updateTime();
    QTimer *timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &MainWindow::updateTime);
    timer->start(5000);

    //Open new file
    this->index = 0;
    this->openTab("New file");
    statusBar()->clearMessage();

    tempfile = QDir::cleanPath(QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation) + QDir::separator() + "temp.enff");
}

MainWindow::~MainWindow()
{
    delete ui;
}


/*
 * Event handlers
 */

void MainWindow::updateTime(){
    lblClock->setText("  \U0001F550 "+QTime::currentTime().toString("HH:mm"));
}


//Bold, italic, underline events
void MainWindow::on_actionBold_triggered()
{
    QTextCharFormat fmt;
    fmt.setFontWeight(ui->actionBold->isChecked() ? QFont::Bold : QFont::Normal);
    setFontOnSelected(fmt);
}

void MainWindow::on_actionItalic_triggered()
{
    QTextCharFormat fmt;
    fmt.setFontItalic(ui->actionItalic->isChecked());
    setFontOnSelected(fmt);
}

void MainWindow::on_actionUnderline_triggered()
{
    QTextCharFormat fmt;
    fmt.setFontUnderline(ui->actionUnderline->isChecked());
    setFontOnSelected(fmt);
}

void MainWindow::on_actionStrikeout_triggered()
{
    QTextCharFormat fmt;
    fmt.setFontStrikeOut(ui->actionStrikeout->isChecked());
    setFontOnSelected(fmt);
}

//I/O actions
void MainWindow::on_actionOpen_triggered()
{
    QFileDialog fileDialog(this, tr("Open File(s)..."));
    fileDialog.setAcceptMode(QFileDialog::AcceptOpen);
    fileDialog.setFileMode(QFileDialog::ExistingFiles);
    fileDialog.setMimeTypeFilters(QStringList()
#if QT_CONFIG(texthtmlparser)
                                  << "text/html"
#endif
#if QT_CONFIG(textmarkdownreader)

                                  << "text/markdown"
#endif
                                  << "text/plain");
    if (fileDialog.exec() != QDialog::Accepted)
        return;

    for(QString file : fileDialog.selectedFiles()){
        openTab(file);
    }
}

void MainWindow::on_actionSave_as_triggered()
{
    QFileDialog fileDialog(this, tr("Save as..."));
    fileDialog.setAcceptMode(QFileDialog::AcceptSave);
    QStringList mimeTypes;
    mimeTypes << "text/plain"
#if QT_CONFIG(textodfwriter)
              << "application/vnd.oasis.opendocument.text"
#endif
#if QT_CONFIG(textmarkdownwriter)
              << "text/markdown"
#endif
              << "text/html";
    fileDialog.setMimeTypeFilters(mimeTypes);
#if QT_CONFIG(textodfwriter)
    fileDialog.setDefaultSuffix("odt");
#endif

    if (fileDialog.exec() != QDialog::Accepted)
        return;
    const QString filename = fileDialog.selectedFiles().first();

    //Get selected tab
    ETab *selected = ui->tabs->findChild<ETab *>(ui->tabs->currentWidget()->objectName());
    if(selected == NULL){
        std::cout << "Error: selected tab is NULL" << std::endl;
        return;
    }

    selected->setFileName(filename);
    QFileInfo info(filename);
    QFile f(filename);
    f.open(QIODevice::ReadWrite);
    ui->tabs->setTabText(ui->tabs->currentIndex(), info.fileName());
    changeTab(ACTION::SAVE);
}


void MainWindow::on_actionSave_triggered() { changeTab(ACTION::SAVE); }

void MainWindow::on_actionDelete_file_triggered()
{
    QMessageBox::StandardButton reply = QMessageBox::question(this, "Delete file?", "Are you sure you want to delete this file?",
                                    QMessageBox::Yes|QMessageBox::No);
    if(reply == QMessageBox::Yes){
        changeTab(ACTION::DELETE);
    }
}

void MainWindow::closeEvent(QCloseEvent *bar){
    on_actionClose_all_triggered();
    std::cout << "Bye!" << std::endl;
    bar->accept();
}

//Other menu items
void MainWindow::on_action_New_triggered() { this->openTab("New File"); }
void MainWindow::on_action_Close_triggered() { changeTab(ACTION::CLOSE); }
void MainWindow::on_actionBigger_triggered() { changeTab(ACTION::CHANGEFONTSIZE, true); }
void MainWindow::on_actionSmaller_triggered() { changeTab(ACTION::CHANGEFONTSIZE, false); }
void MainWindow::on_actionFont_family_triggered() { changeTab(ACTION::CHANGEFONT); }
void MainWindow::on_actionForce_Quit_triggered() { exit(0); }

void MainWindow::on_action_Exit_triggered()
{
    on_actionClose_all_triggered();
    exit(0);
}

void MainWindow::on_actionColor_triggered()
{
    ETab *selected = ui->tabs->findChild<ETab *>(ui->tabs->currentWidget()->objectName());
    if(selected == NULL){
        std::cout << "Error: selected tab is NULL" << std::endl;
        return;
    }
    selected->changeColor();
}

void MainWindow::on_actionStay_topmost_triggered()
{
    //Maybe other code on windows: https://stackoverflow.com/a/2860768
    if(ui->actionStay_topmost->isChecked()){
        this->setWindowFlags(this->windowFlags() | Qt::WindowStaysOnTopHint);
        std::cout << "Change topmost" << std::endl;
    } else{
        this->setWindowFlags(this->windowFlags() ^ Qt::WindowStaysOnTopHint);
        std::cout << "Change not topmost" << std::endl;
    }
    this->show();
}

void MainWindow::on_actionClose_all_triggered()
{
    int cnt = ui->tabs->count();
    while(ui->tabs->count()>0){
        changeTab(ACTION::CLOSE);
    }
    updateMessage(QString("Closed %1 files").arg(cnt));
}

void MainWindow::on_actionAutosave_triggered()
{
    changeTab(ACTION::SETAUTOSAVE);
}

void MainWindow::on_actionRemeber_opened_files_triggered()
{

}

/*
 * Logic
 */

void MainWindow::loadTempFile(){
    QFile f(tempfile);
    if(f.exists()){
        //Read file to string
        remember = true;
    } else{
        remember = false;
    }
}

void MainWindow::updateAutoSave(bool checked){
    ui->actionAutosave->setChecked(checked);
}

void MainWindow::setFontOnSelected(const QTextCharFormat &format){
    //Get selected tab
    ETab *selected = ui->tabs->findChild<ETab *>(ui->tabs->currentWidget()->objectName());
    if(selected == NULL){
        std::cout << "Error: selected tab is NULL" << std::endl;
        return;
    }

    selected->setFontFormat(format);
}

void MainWindow::openTab(QString file){
    //Add tab to tabs
    ETab *tab = new ETab(this);
    int tabCount = ui->tabs->count();
    tab->setObjectName(QString("tab-%1").arg(index++));
    tab->setFileName(file);

    QFileInfo fi(file);
    QString title = fi.fileName();

    if(fi.exists()){
        tab->openFile();
    }

    ui->tabs->addTab(tab, title);
    ui->tabs->setCurrentIndex(tabCount);
    updateActions();
    updateMessage(title+" opened!");
}

void MainWindow::updateActions() {
    bool enabled = (ui->tabs->count()!=0);

    ui->actionBold->setEnabled(enabled);
    ui->actionItalic->setEnabled(enabled);
    ui->actionUnderline->setEnabled(enabled);
    ui->actionStrikeout->setEnabled(enabled);
    ui->actionBigger->setEnabled(enabled);
    ui->actionSmaller->setEnabled(enabled);
    ui->actionFont_family->setEnabled(enabled);
    ui->actionColor->setEnabled(enabled);
    ui->action_Close->setEnabled(enabled);
    ui->actionSave->setEnabled(enabled);
    ui->actionSave_as->setEnabled(enabled);
    ui->actionDelete_file->setEnabled(enabled);
    ui->actionClose_all->setEnabled(enabled);
}

void MainWindow::updateActions(const QTextCharFormat &format){
    ui->actionBold->setChecked(format.font().bold());
    ui->actionUnderline->setChecked(format.font().underline());
    ui->actionItalic->setChecked(format.font().italic());
    ui->actionStrikeout->setChecked(format.font().strikeOut());
}

void MainWindow::updateStatusLabel(int line, int col){
    lblStatus->setText(QString("ln: %1 col: %2 ").arg(line).arg(col));
}

void MainWindow::updateMessage(QString message){
    ui->statusbar->showMessage(message, 3000);
}

void MainWindow::changeTab(ACTION action, int argument){
    ETab *selected = ui->tabs->findChild<ETab *>(ui->tabs->currentWidget()->objectName());
    if(selected == NULL){
        std::cout << "Error: selected tab is NULL" << std::endl;
        return;
    }

    QFileInfo info(selected->getFileName());

    switch (action) {
        case ACTION::CHANGEFONT:
            selected->changeFont();
        break;
        case ACTION::CHANGECOLOR:
            selected->changeColor();
        break;
        case ACTION::CHANGEFONTSIZE:
            selected->changeFontSize(argument);
        break;
        case ACTION::SAVE:
        {
            if(!info.exists())
                on_actionSave_as_triggered();
            selected->saveFile();
        }
        break;
        case ACTION::DELETE:
        {
            QFile file(selected->getFileName());
            changeTab(ACTION::CLOSE);
            if(!info.exists())
                return;
            file.remove();
            updateMessage("Deleted "+info.fileName());
        }
        break;
        case ACTION::CLOSE:
            //Close current tab
            selected->saveFile();
            ui->tabs->currentWidget()->deleteLater();
            selected->deleteLater();
            ui->tabs->removeTab(ui->tabs->currentIndex());
            updateActions();
            updateMessage(info.fileName()+" closed!");
        break;
        case ACTION::SETAUTOSAVE:
            selected->setAutoSave(ui->actionAutosave->isChecked());
        break;
    }
}

