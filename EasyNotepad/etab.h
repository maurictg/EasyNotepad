#ifndef ETAB_H
#define ETAB_H

#include <QWidget>
#include <QTimer>
#include <QFile>
#include <QTextListFormat>
#include <QTextCharFormat>
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
    void saveFile();
    void setAutoSave(bool enabled);
    void setStyle(int type);
    void setAlign(int type);

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
    void useFile(bool write);
    void mergeFormat(QTextCharFormat format);
    QString getName();
};

#endif // ETAB_H
