#include "mainwindow.h"

#include <QApplication>
#include <QDir>
#include <QFile>
#include <QSettings>
#include <QStyleFactory>
#include <QTextStream>
#include <qjsonarray.h>
#include <qjsondocument.h>
#include <qjsonobject.h>
#include <qstandardpaths.h>

QJsonObject loadTheme(QApplication *a) {
    int theme = 0; //Default
    QJsonObject json;

    //Read settings file
    QString tempfile = QDir::cleanPath(QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation) + QDir::separator() + "temp.enff");
    QFile s(tempfile);

    if(s.exists()){
        if(!s.open(QIODevice::ReadOnly)){
            std::cerr << "ERROR: Failed to open temp file" << std::endl;
            return QJsonObject();
        }

        QString res = s.readAll();
        s.close();

        if(res.length() > 0) {
            QJsonDocument doc = QJsonDocument::fromJson(res.toUtf8());
            if(!doc.isNull() && doc.isObject()) {
                //Read document to object
                json = doc.object();
                if(json.contains("theme")) {
                    theme = json["theme"].toInt();
                }
            }
        }
    }

    //If windows and system default: choose from system
#if defined(Q_OS_WIN)
    if(theme == 0) {
        QSettings settings("HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize",QSettings::NativeFormat);
        theme = (settings.value("AppsUseLightTheme")==0) ? MainWindow::THEME::DARK : MainWindow::THEME::LIGHT;
    }
#endif



    //Set theme
    if(theme != 0) {
        //Set fusion theme
        qApp->setStyle(QStyleFactory::create("Fusion"));

        //Create color palette
        QPalette p;

        if(theme == MainWindow::THEME::DARK) {
            QColor darkColor = QColor(45,45,45);
            QColor disabledColor = QColor(127,127,127);
            p.setColor(QPalette::Window, darkColor);
            p.setColor(QPalette::WindowText, Qt::white);
            p.setColor(QPalette::Base, QColor(18,18,18));
            p.setColor(QPalette::AlternateBase, darkColor);
            p.setColor(QPalette::ToolTipBase, Qt::white);
            p.setColor(QPalette::ToolTipText, Qt::white);
            p.setColor(QPalette::Text, Qt::white);
            p.setColor(QPalette::Disabled, QPalette::Text, disabledColor);
            p.setColor(QPalette::Button, darkColor);
            p.setColor(QPalette::ButtonText, Qt::white);
            p.setColor(QPalette::Disabled, QPalette::ButtonText, disabledColor);
            p.setColor(QPalette::BrightText, Qt::red);
            p.setColor(QPalette::Link, QColor(42, 130, 218));

            p.setColor(QPalette::Highlight, QColor(42, 130, 218));
            p.setColor(QPalette::HighlightedText, Qt::black);
            p.setColor(QPalette::Disabled, QPalette::HighlightedText, disabledColor);

            qApp->setStyleSheet("QToolTip { color: #ffffff; background-color: #2a82da; border: 1px solid white; }");
        } else if(theme == MainWindow::THEME::BLUE) {
            QFile f(":qdarkstyle/style.qss");

            if (!f.exists()) {
                std::cout << "Unable to set stylesheet, file not found" << std::endl;
            }
            else {
                f.open(QFile::ReadOnly | QFile::Text);
                QTextStream ts(&f);
                a->setStyleSheet(ts.readAll());
            }
        }

        //Set palette
        qApp->setPalette(p);
    }

    return json;
}


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QJsonObject json = loadTheme(&a);

    //Get parameters from CLI
    QStringList params;
    for(int i = 0; i < argc; i++){
        if(i == 0)
            continue;

        std::cout << argv[i] << std::endl;
        params << QString::fromLocal8Bit(argv[i]);
    }


    MainWindow w(&params, &json);
    w.setWindowTitle("EasyNotepad");
    w.show();
    return a.exec();
}
