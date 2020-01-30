#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <etab.h>

#include <QMessageBox>
#include <QTextCharFormat>
#include <iostream>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    //Initialize status bar
    this->lblStatus = new QLabel("ln: 0 col: 0");
    lblStatus->setAlignment(Qt::AlignRight);
    statusBar()->addWidget(lblStatus, 1);
    statusBar()->showMessage("Welcome!");

    //Open new file
    this->index = 0;
    this->openTab("New file");
}

MainWindow::~MainWindow()
{
    delete ui;
}


/*
 * Event handlers
 */


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
void MainWindow::on_action_Close_triggered() { this->closeTab(); }
void MainWindow::on_action_Exit_triggered() { exit(0); }
void MainWindow::on_actionBigger_triggered() { changeFontSize(true); }
void MainWindow::on_actionSmaller_triggered() { changeFontSize(false); }

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
    ui->tabs->addTab(tab, title);
    ui->tabs->setCurrentIndex(tabCount);
    updateActions();
}

void MainWindow::closeTab(){
    //Get selected tab
    ETab *selected = ui->tabs->findChild<ETab *>(ui->tabs->currentWidget()->objectName());
    if(selected == NULL){
        std::cout << "Error: selected tab is NULL" << std::endl;
        return;
    }

    //std::cout << selected->getFileName().toStdString() << std::endl;

    //Close current tab
    ui->tabs->removeTab(ui->tabs->currentIndex());
    updateActions();
}

void MainWindow::updateActions() {
    bool enabled = (ui->tabs->count()!=0);

    ui->actionBold->setEnabled(enabled);
    ui->actionItalic->setEnabled(enabled);
    ui->actionUnderline->setEnabled(enabled);
    ui->action_Close->setEnabled(enabled);
}

void MainWindow::updateActions(const QTextCharFormat &format){
    ui->actionBold->setChecked(format.font().bold());
    ui->actionUnderline->setChecked(format.font().underline());
    ui->actionItalic->setChecked(format.font().italic());
    ui->actionStrikeout->setChecked(format.font().strikeOut());
}

void MainWindow::updateStatusLabel(int line, int col){
    lblStatus->setText(QString("ln: %1 col: %2").arg(line).arg(col));
}

void MainWindow::changeFontSize(bool increase) {
    ETab *selected = ui->tabs->findChild<ETab *>(ui->tabs->currentWidget()->objectName());
    if(selected == NULL){
        std::cout << "Error: selected tab is NULL" << std::endl;
        return;
    }
    selected->changeFontSize(increase);
}

void MainWindow::on_actionFont_family_triggered()
{
    ETab *selected = ui->tabs->findChild<ETab *>(ui->tabs->currentWidget()->objectName());
    if(selected == NULL){
        std::cout << "Error: selected tab is NULL" << std::endl;
        return;
    }
    selected->changeFont();
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
