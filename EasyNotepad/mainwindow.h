#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTextCharFormat>
#include <QLabel>
#include <QMenu>
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
    void updateMessage(QString message);
    void updateAutoSave(bool checked);
    enum ACTION {
        CHANGEFONTSIZE, CHANGEFONT, CHANGECOLOR, CLOSE, SAVE, DELETE, SETAUTOSAVE,
        SETHNORMAL, SETH1, SETH2, SETH3, SETH4, SETH5, SETH6,
        LISTDISK, LISTCIRCLE, LISTSQUARE, LISTUNCHECKED, LISTCHECKED, LISTDECIMAL,
        LISTALPHALOWER, LISTALPHAUPPER, LISTROMANLOWER, LISTROMANUPPER,
        ALIGNLEFT, ALIGNCENTER, ALIGNRIGHT, ALIGNJUSTIFY
    };

private slots:
    void updateTime();
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
    void on_actionOpen_triggered();
    void on_actionSave_triggered();
    void on_actionSave_as_triggered();
    void on_actionDelete_file_triggered();
    void on_actionClose_all_triggered();
    void on_actionForce_Quit_triggered();
    void on_actionAutosave_triggered();
    void on_actionRemeber_opened_files_triggered();
    void on_actionStandard_triggered();
    void on_actionHeading_1_triggered();
    void on_actionHeading_2_triggered();
    void on_actionHeading_3_triggered();
    void on_actionHeading_4_triggered();
    void on_actionHeading_5_triggered();
    void on_actionHeading_6_triggered();
    void on_actionCircle_triggered();
    void on_actionSquare_triggered();
    void on_actionDisc_triggered();
    void on_actionAlpha_lower_triggered();
    void on_actionAlpha_upper_triggered();
    void on_actionRoman_lower_triggered();
    void on_actionRoman_upper_triggered();
    void on_actionStandard_numeric_triggered();
    void on_actionCheckbox_triggered();
    void on_actionCheckbox_checked_triggered();
    void on_actionLeft_triggered();
    void on_actionRight_triggered();
    void on_actionCenter_triggered();
    void on_actionJustify_triggered();

private:
    Ui::MainWindow *ui;
    QLabel *lblStatus;
    QLabel *lblClock;
    QString tempfile;
    void closeEvent(QCloseEvent *event);
    void showEvent(QShowEvent* event);
    void setFontOnSelected(const QTextCharFormat &format);
    void openTab(QString title);
    void updateActions();
    void changeTab(ACTION action, int argument = 0);
    int index;
    bool remember;
    void loadTempFile();
    void saveTempFile();
    void toggleMenu(QMenu* menu, bool disable = true);
};
#endif // MAINWINDOW_H
