#include "mainwindow.h"

#include <QApplication>
#include <QFile>
#include <QSettings>
#include <QStyleFactory>
#include <QTextStream>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

#if defined(Q_OS_WIN)
    QSettings settings("HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize",QSettings::NativeFormat);
        if(settings.value("AppsUseLightTheme")==0){
            qApp->setStyle(QStyleFactory::create("Fusion"));
            QPalette darkPalette;
            QColor darkColor = QColor(45,45,45);
            QColor disabledColor = QColor(127,127,127);
            darkPalette.setColor(QPalette::Window, darkColor);
            darkPalette.setColor(QPalette::WindowText, Qt::white);
            darkPalette.setColor(QPalette::Base, QColor(18,18,18));
            darkPalette.setColor(QPalette::AlternateBase, darkColor);
            darkPalette.setColor(QPalette::ToolTipBase, Qt::white);
            darkPalette.setColor(QPalette::ToolTipText, Qt::white);
            darkPalette.setColor(QPalette::Text, Qt::white);
            darkPalette.setColor(QPalette::Disabled, QPalette::Text, disabledColor);
            darkPalette.setColor(QPalette::Button, darkColor);
            darkPalette.setColor(QPalette::ButtonText, Qt::white);
            darkPalette.setColor(QPalette::Disabled, QPalette::ButtonText, disabledColor);
            darkPalette.setColor(QPalette::BrightText, Qt::red);
            darkPalette.setColor(QPalette::Link, QColor(42, 130, 218));

            darkPalette.setColor(QPalette::Highlight, QColor(42, 130, 218));
            darkPalette.setColor(QPalette::HighlightedText, Qt::black);
            darkPalette.setColor(QPalette::Disabled, QPalette::HighlightedText, disabledColor);

            qApp->setPalette(darkPalette);
            qApp->setStyleSheet("QToolTip { color: #ffffff; background-color: #2a82da; border: 1px solid white; }");
        }
        else //When not set to dark theme, use custom theme which is darker than light theme
        {
            QFile f(":qdarkstyle/style.qss");

            if (!f.exists())   {
                std::cout << "Unable to set stylesheet, file not found" << std::endl;
            }
            else   {
                f.open(QFile::ReadOnly | QFile::Text);
                QTextStream ts(&f);
                a.setStyleSheet(ts.readAll());
            }
        }
#endif

    //Get parameters from CLI
    QStringList params;
    for(int i = 0; i < argc; i++){
        if(i == 0)
            continue;

        std::cout << argv[i] << std::endl;
        params << QString::fromLocal8Bit(argv[i]);
    }


    MainWindow w(&params);
    w.setWindowTitle("EasyNotepad");
    w.show();
    return a.exec();
}
