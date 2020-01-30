#ifndef ETAB_H
#define ETAB_H

#include <QWidget>
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
    void setFileName(QString name);
    void changeFontSize(bool increase);
    void changeFont();
    void changeColor();

private slots:
    void on_textEdit_currentCharFormatChanged(const QTextCharFormat &format);

    void on_textEdit_cursorPositionChanged();

private:
    Ui::ETab *ui;
    MainWindow *main;
    QString file;
};

#endif // ETAB_H
