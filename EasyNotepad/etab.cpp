#include "etab.h"
#include "ui_etab.h"

#include <QAction>
#include <QFontDialog>
#include <QFileInfo>
#include <QFile>
#include <QTextDocumentWriter>
#include <QTextCodec>
#include <QMimeDatabase>
#include <QTextStream>
#include <QTextListFormat>
#include <QTextList>

#include <colorpicker.h>
#include <urlpicker.h>

ETab::ETab(MainWindow *mainwindow, QWidget *parent) : QWidget(parent), ui(new Ui::ETab)
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
    delete file;
}

void ETab::focus() {
    ui->textEdit->setFocus();
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
        std::cerr << "ERROR: Nullpointerexception" << std::endl;
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

void ETab::setContent(QString text, bool doSave) {
    ui->textEdit->setHtml(text);
    this->dontSave = !doSave;
}

QString ETab::getContent() {
    return ui->textEdit->toHtml();
}

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
    if(!file->exists()){
        std::cout << "WARNING: File does not exist!" << std::endl;
        return;
    }

    if(!write){
        //Read data from file
        if(!file->open(QIODevice::ReadOnly)){
            std::cerr << "ERROR: Failed to read file" << std::endl;
            return;
        }

        //Use mimetypes. Can also check on endsWith
        QByteArray data = file->readAll();
        QTextCodec *codec = Qt::codecForHtml(data);
        QString str = codec->toUnicode(data);
        QUrl baseUrl = (file->fileName().front() == QLatin1Char(':') ? QUrl(file->fileName()) : QUrl::fromLocalFile(file->fileName())).adjusted(QUrl::RemoveFilename);
        ui->textEdit->document()->setBaseUrl(baseUrl);
        if (Qt::mightBeRichText(str)) {
            ui->textEdit->setHtml(str);
        } else {
            QMimeDatabase db;
            if (db.mimeTypeForFileNameAndData(file->fileName(), data).name() == QLatin1String("text/markdown")){
                ui->textEdit->setMarkdown(str);
            }
            else
                ui->textEdit->setPlainText(QString::fromLocal8Bit(data));
        }
    } else{
        //Write data to file
        QTextDocumentWriter writer(file->fileName());
        bool result = writer.write(ui->textEdit->document());
        if(result)
            main->updateMessage(" \U0001F5CE "+getName()+" saved!");
        else {
            QString text = ui->textEdit->toPlainText();
            if(!file->open(QIODevice::WriteOnly)){
                std::cerr << "ERROR: Failed to open file" << std::endl;
                return;
            }
            file->write(text.toUtf8());
            main->updateMessage(" \U0001F5CE "+getName()+" saved!");
            std::cout << "INFO: Saved file as plain text" << std::endl;
        }
    }

    file->close();
    //Set modified to false
    ui->textEdit->document()->setModified(false);

    //1000 kb max
    if(((file->size() / 1024) < 1000) && !autosave){
        setAutoSave(true);
        main->updateAutoSave(true);
    }

    changes = false;
}

void ETab::openFile() { this->useFile(false);}
void ETab::saveFile(bool force) {
    if(changes || force)
        this->useFile(true);
}

//Getter/setter
QString ETab::getFileName() { return file->fileName(); }
void ETab::setFileName(QString name){ file = new QFile(name); }

QString ETab::getName(){
    QFileInfo i(getFileName());
    return i.fileName();
}

bool ETab::hasChanges() {
    if(ui->textEdit->toPlainText().length() == 0 || dontSave)
        return false;

    return changes;
}

bool ETab::isAutosave() {
    return autosave;
}

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
void ETab::changeFont() {
    bool ok;
    QFont font = QFontDialog::getFont(&ok, ui->textEdit->textCursor().charFormat().font(), this);
    if(ok){
        QTextCharFormat format;
        format.setFont(font);
        mergeFormat(format);
    }
}

//Change font color
void ETab::changeColor(){
    ColorPicker *p = new ColorPicker(this);
    p->exec();
}

//Merge font format into current cursor
void ETab::mergeFormat(QTextCharFormat format){
    QTextCursor cursor = ui->textEdit->textCursor();
    cursor.mergeCharFormat(format);
    ui->textEdit->mergeCurrentCharFormat(format);
}

void ETab::insertLink(QString text, QString url) {
    QTextCursor cursor = ui->textEdit->textCursor();
    cursor.insertHtml("<a href=\""+url+"\">"+text+"</a>&nbsp;");
}

void ETab::createLink() {
    UrlPicker *p = new UrlPicker(this);
    p->exec();
}

QString ETab::getSelection() {
    QTextCursor cursor = ui->textEdit->textCursor();
    if(cursor.hasSelection())
        return cursor.selectedText();
    else
        return "";
}

QColor ETab::foreground() {
    return ui->textEdit->textCursor().charFormat().foreground().color();
}

QColor ETab::background() {
    return ui->textEdit->textCursor().charFormat().background().color();
}

//Set special style like header or list
void ETab::setStyle(int type){
    QTextCursor cursor = ui->textEdit->textCursor();
    QTextListFormat::Style style = QTextListFormat::ListStyleUndefined;
    QTextBlockFormat::MarkerType marker = QTextBlockFormat::MarkerType::NoMarker;

    switch (type) {
    case 1:
        style = QTextListFormat::ListDisc;
        break;
    case 2:
        style = QTextListFormat::ListCircle;
        break;
    case 3:
        style = QTextListFormat::ListSquare;
        break;
    case 4:
        if (cursor.currentList())
            style = cursor.currentList()->format().style();
        else
        {
            style = QTextListFormat::ListDisc;
            marker = QTextBlockFormat::MarkerType::Unchecked;
        }
        break;
    case 5:
        if (cursor.currentList())
            style = cursor.currentList()->format().style();
        else
        {
            style = QTextListFormat::ListDisc;
            marker = QTextBlockFormat::MarkerType::Checked;
        }

        break;
    case 6:
        style = QTextListFormat::ListDecimal;
        break;
    case 7:
        style = QTextListFormat::ListLowerAlpha;
        break;
    case 8:
        style = QTextListFormat::ListUpperAlpha;
        break;
    case 9:
        style = QTextListFormat::ListLowerRoman;
        break;
    case 10:
        style = QTextListFormat::ListUpperRoman;
        break;
    default:
        break;
    }

    cursor.beginEditBlock();

    QTextBlockFormat format = cursor.blockFormat();

    if (style == QTextListFormat::ListStyleUndefined) {
        format.setObjectIndex(-1);
        int headingLevel = type >= 11 ? type - 11 + 1 : 0; // H1 to H6, or Standard
        format.setHeadingLevel(headingLevel);
        cursor.setBlockFormat(format);

        int sizeAdjustment = headingLevel ? 4 - headingLevel : 0; // H1 to H6: +3 to -2
        QTextCharFormat fmt;
        fmt.setFontWeight(headingLevel ? QFont::Bold : QFont::Normal);
        fmt.setProperty(QTextFormat::FontSizeAdjustment, sizeAdjustment);
        cursor.select(QTextCursor::LineUnderCursor);
        cursor.mergeCharFormat(fmt);
        ui->textEdit->mergeCurrentCharFormat(fmt);
    } else {
        format.setMarker(marker);
        cursor.setBlockFormat(format);
        QTextListFormat listFmt;
        if (cursor.currentList()) {
            listFmt = cursor.currentList()->format();
        } else {
            listFmt.setIndent(format.indent() + 1);
            format.setIndent(0);
            cursor.setBlockFormat(format);
        }

        listFmt.setStyle(style);
        cursor.createList(listFmt);
    }

    cursor.endEditBlock();
}

void ETab::setAlign(int type){
    switch (type) {
        case 0:
        ui->textEdit->setAlignment(Qt::AlignLeft | Qt::AlignAbsolute);
            break;
        case 1:
        ui->textEdit->setAlignment(Qt::AlignCenter);
            break;
        case 2:
        ui->textEdit->setAlignment(Qt::AlignRight | Qt::AlignAbsolute);
            break;
        case 3:
            ui->textEdit->setAlignment(Qt::AlignJustify);
        break;
    }
}

bool ETab::fileExists() {
    return file->exists();
}
