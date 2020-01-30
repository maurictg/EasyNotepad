#include "etab.h"
#include "ui_etab.h"

#include <QAction>
#include <QFontDialog>
#include <QColorDialog>

ETab::ETab(MainWindow *mainwindow, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ETab)
{
    ui->setupUi(this);
    this->main = mainwindow;
}

ETab::~ETab()
{
    delete ui;
}

/*
 * Event handlers
 */

void ETab::setFontFormat(const QTextCharFormat &format){
    //Get cursor and set charFormat
    QTextCursor cursor = ui->textEdit->textCursor();
    cursor.mergeCharFormat(format);
    ui->textEdit->mergeCurrentCharFormat(format);
}

void ETab::on_textEdit_currentCharFormatChanged(const QTextCharFormat &format)
{
    main->updateActions(format);
}

void ETab::on_textEdit_cursorPositionChanged()
{
    QTextCursor cursor = ui->textEdit->textCursor();
    if(main != NULL){
        main->updateStatusLabel((cursor.blockNumber()+1), (cursor.columnNumber()+1));
    } else{
        std::cout << "Main is null" << std::endl;
    }
}

/*
 * Logic
 */

QString ETab::getFileName(){
    return file;
}

void ETab::setFileName(QString name){
    file = name;
}

void ETab::changeFontSize(bool increase){
    QTextCursor cursor = ui->textEdit->textCursor();

    QTextCharFormat fmt;
    QFont font = cursor.charFormat().font();

    float size = font.pointSizeF() + ((increase) ? 1 : -1);

    if(size > 0){
        font.setPointSizeF(size);
        fmt.setFont(font);
        cursor.mergeCharFormat(fmt);
        ui->textEdit->mergeCurrentCharFormat(fmt);
    }
}

void ETab::changeFont(){
    bool ok;
    QFont font = QFontDialog::getFont(
                    &ok, ui->textEdit->textCursor().charFormat().font(), this);
    if(ok){
        QTextCursor cursor = ui->textEdit->textCursor();
        QTextCharFormat format;
        format.setFont(font);
        cursor.mergeCharFormat(format);
        ui->textEdit->mergeCurrentCharFormat(format);
    }
}

void ETab::changeColor(){
    bool ok;
    QColor color = QColorDialog::getColor(ui->textEdit->textColor(), this);

    if(color.isValid()){
        QTextCursor cursor = ui->textEdit->textCursor();
        QTextCharFormat format;
        format.setForeground(color);
        cursor.mergeCharFormat(format);
        ui->textEdit->mergeCurrentCharFormat(format);
    }
}
