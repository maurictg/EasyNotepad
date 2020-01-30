#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <etab.h>
#include <QMessageBox>
#include <QTextCharFormat>
#include <QTime>
#include <QTimer>
#include <iostream>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

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

//Other menu items
void MainWindow::on_action_New_triggered() { this->openTab("New File"); }
void MainWindow::on_action_Close_triggered() { changeTab(ACTION::CLOSE); }
void MainWindow::on_action_Exit_triggered() { exit(0); }
void MainWindow::on_actionBigger_triggered() { changeTab(ACTION::CHANGEFONTSIZE, true); }
void MainWindow::on_actionSmaller_triggered() { changeTab(ACTION::CHANGEFONTSIZE, false); }
void MainWindow::on_actionFont_family_triggered() { changeTab(ACTION::CHANGEFONT); }

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


/*
 * Logic
 */

void MainWindow::setFontOnSelected(const QTextCharFormat &format){
    //Get selected tab
    ETab *selected = ui->tabs->findChild<ETab *>(ui->tabs->currentWidget()->objectName());
    if(selected == NULL){
        std::cout << "Error: selected tab is NULL" << std::endl;
        return;
    }

    selected->setFontFormat(format);
}

void MainWindow::openTab(QString title){
    //Add tab to tabs
    ETab *tab = new ETab(this);
    int tabCount = ui->tabs->count();
    tab->setObjectName(QString("tab-%1").arg(index++));
    tab->setFileName(title);
    ui->tabs->addTab(tab, title);
    ui->tabs->setCurrentIndex(tabCount);
    updateActions();
    ui->statusbar->showMessage(title+" opened!", 3000);
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


void MainWindow::changeTab(ACTION action, int argument){
    ETab *selected = ui->tabs->findChild<ETab *>(ui->tabs->currentWidget()->objectName());
    if(selected == NULL){
        std::cout << "Error: selected tab is NULL" << std::endl;
        return;
    }

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
        case ACTION::CLOSE:
            //Close current tab
            ui->tabs->removeTab(ui->tabs->currentIndex());
            updateActions();
            ui->statusbar->showMessage(selected->getFileName()+" closed!", 3000);
        break;
    }
}
