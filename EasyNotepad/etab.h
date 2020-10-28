#ifndef ETAB_H
#define ETAB_H

#include <QWidget>
#include <QTimer>
#include <QFile>
#include <QTextListFormat>
#include <QTextCharFormat>
#include <QColor>
#include <mainwindow.h>

namespace Ui {
class ETab;
}

class ETab : public QWidget
{
    Q_OBJECT

public:
    explicit ETab(MainWindow *mainwindow, QWidget *parent = nullptr);
    ~ETab();
    void setFontFormat(const QTextCharFormat &format);
    QString getFileName();
    bool hasChanges();
    bool isAutosave();
    void setFileName(QString name);
    void changeFontSize(bool increase);
    void changeFont();
    void changeColor();
    void openFile();
    void saveFile(bool force = false);
    void setAutoSave(bool enabled);
    void setStyle(int type);
    void setAlign(int type);
    void focus();
    void setContent(QString text, bool doSave = true);
    void mergeFormat(QTextCharFormat format);
    void insertLink(QString title, QString url);
    void createLink();
    QString getSelection();
    QString getContent();
    bool fileExists();
    QColor foreground();
    QColor background();

private slots:
    void timerTick();
    void on_textEdit_currentCharFormatChanged(const QTextCharFormat &format);
    void on_textEdit_cursorPositionChanged();
    void on_textEdit_textChanged();

private:
    Ui::ETab *ui;
    MainWindow *main;
    QFile *file;
    QTimer *timer;
    bool autosave;
    bool changes;
    bool dontSave;
    void useFile(bool write);
    QString getName();
};

#endif // ETAB_H
