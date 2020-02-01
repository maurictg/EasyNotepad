#include "etab.h"
#include "ui_etab.h"

#include <QAction>
#include <QFontDialog>
#include <QColorDialog>
#include <QFileInfo>
#include <QFile>
#include <QTextDocumentWriter>
#include <QTextCodec>
#include <QMimeDatabase>
#include <QTextStream>

ETab::ETab(MainWindow *mainwindow, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ETab)
{
    ui->setupUi(this);
    this->main = mainwindow;
    ui->textEdit->setFrameStyle(QFrame::NoFrame);
    QFont font("Consolas", 14);
    ui->textEdit->setFont(font);

    timer = new QTimer();
    connect(timer, &QTimer::timeout, this, &ETab::timerTick);
    changes = false;
}

ETab::~ETab()
{
    delete ui;
    delete timer;
}

/*
 * Event handlers
 */


void ETab::on_textEdit_currentCharFormatChanged(const QTextCharFormat &format) { main->updateActions(format); }
void ETab::on_textEdit_textChanged() { changes = true; }

void ETab::on_textEdit_cursorPositionChanged()
{
    QTextCursor cursor = ui->textEdit->textCursor();
    if(main != NULL){
        main->updateStatusLabel((cursor.blockNumber()+1), (cursor.columnNumber()+1));
    } else{
        std::cout << "Main is null" << std::endl;
    }
}

void ETab::timerTick(){
    if(!autosave){
        timer->stop();
    } else{
        if(changes){
            useFile(true);
        }
    }
}


/*
 * Logic
 */

//Set font format on selected tab
void ETab::setFontFormat(const QTextCharFormat &format){
    //Get cursor and set charFormat
    QTextCursor cursor = ui->textEdit->textCursor();
    cursor.mergeCharFormat(format);
    ui->textEdit->mergeCurrentCharFormat(format);
}

//Enable/disable autosave on file
void ETab::setAutoSave(bool enabled){
    autosave = enabled;
    if(enabled == true && !timer->isActive()){
        timer->start(10000);
    }
}

//Write/read file
void ETab::useFile(bool write){
    QFileInfo fi(file);
    if(!fi.exists()){
        std::cout << "File does not exist!" << std::endl;
        return;
    }

    QFile f(file);

    if(!write){
        //Read data from file
        if(!f.open(QIODevice::ReadOnly)){
            std::cout << "Failed to read file" << std::endl;
            return;
        }
        QByteArray data = f.readAll();
        QTextCodec *codec = Qt::codecForHtml(data);
        QString str = codec->toUnicode(data);
        QUrl baseUrl = (file.front() == QLatin1Char(':') ? QUrl(file) : QUrl::fromLocalFile(file)).adjusted(QUrl::RemoveFilename);
        ui->textEdit->document()->setBaseUrl(baseUrl);
        if (Qt::mightBeRichText(str)) {
            ui->textEdit->setHtml(str);
        } else {
            QMimeDatabase db;
            if (db.mimeTypeForFileNameAndData(file, data).name() == QLatin1String("text/markdown"))
                ui->textEdit->setMarkdown(str);
            else
                ui->textEdit->setPlainText(QString::fromLocal8Bit(data));
        }
    } else{
        //Write data to file
        QTextDocumentWriter writer(file);
        bool result = writer.write(ui->textEdit->document());
        if(result)
            main->updateMessage(" \U0001F5CE "+fi.fileName()+" saved!");
        else {
            QString text = ui->textEdit->toPlainText();
            if(!f.open(QIODevice::WriteOnly)){
                std::cout << "Failed to open file" << std::endl;
                return;
            }
            f.write(text.toUtf8());
            main->updateMessage(" \U0001F5CE "+fi.fileName()+" saved!");
            std::cout << "Saved plain text with textstream" << std::endl;
        }
    }

    f.close();
    //Set modified to false
    ui->textEdit->document()->setModified(false);

    //1000 kb max
    if(((fi.size() / 1024) < 1000) && !autosave){
        setAutoSave(true);
        main->updateAutoSave(true);
    }

    changes = false;
}

void ETab::openFile() { this->useFile(false);}
void ETab::saveFile() {
    if(changes) //Maybe exist check? QFile(Info) as private class variable?
        this->useFile(true);
}

//Getter/setter
QString ETab::getFileName(){ return file; }
void ETab::setFileName(QString name){ file = name; }

//Increase/decrease fontsize
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

//Change font format
void ETab::changeFont(){
    bool ok;
    QFont font = QFontDialog::getFont(&ok, ui->textEdit->textCursor().charFormat().font(), this);
    if(ok){
        QTextCursor cursor = ui->textEdit->textCursor();
        QTextCharFormat format;
        format.setFont(font);
        cursor.mergeCharFormat(format);
        ui->textEdit->mergeCurrentCharFormat(format);
    }
}

//Change font color
void ETab::changeColor(){
    QColor color = QColorDialog::getColor(ui->textEdit->textColor(), this);
    if(color.isValid()){
        QTextCursor cursor = ui->textEdit->textCursor();
        QTextCharFormat format;
        format.setForeground(color);
        cursor.mergeCharFormat(format);
        ui->textEdit->mergeCurrentCharFormat(format);
    }
}

