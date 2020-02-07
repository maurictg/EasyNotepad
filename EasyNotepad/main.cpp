#include "mainwindow.h"

#include <QApplication>
#include <QFile>
#include <QTextStream>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    //Load dark theme
    QFile f(":qdarkstyle/style.qss");

    if (!f.exists())   {
        std::cout << "Unable to set stylesheet, file not found" << std::endl;
    }
    else   {
        f.open(QFile::ReadOnly | QFile::Text);
        QTextStream ts(&f);
        a.setStyleSheet(ts.readAll());
    }

    MainWindow w;
    w.setWindowTitle("EasyNotepad");
    w.show();
    return a.exec();
}
