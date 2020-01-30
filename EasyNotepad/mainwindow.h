#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTextCharFormat>
#include <QLabel>
#include <iostream>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void updateStatusLabel(int line, int col);
    void updateActions(const QTextCharFormat &format);

private slots:
    void on_actionBold_triggered();

    void on_actionItalic_triggered();

    void on_actionUnderline_triggered();

    void on_action_New_triggered();

    void on_action_Close_triggered();

    void on_action_Exit_triggered();

    void on_actionStrikeout_triggered();

    void on_actionBigger_triggered();

    void on_actionSmaller_triggered();

    void on_actionFont_family_triggered();

    void on_actionColor_triggered();

    void on_actionStay_topmost_triggered();

private:
    Ui::MainWindow *ui;
    QLabel *lblStatus;
    void setFontOnSelected(const QTextCharFormat &format);
    void openTab(QString title);
    void closeTab();
    void updateActions();
    void changeFontSize(bool increase);
    int index;
};
#endif // MAINWINDOW_H
