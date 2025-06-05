#include "mainwindow.h"
#include <QApplication>
#include <QTranslator>
#include <QDir>

int main(int argc, char *argv[])
{

    QApplication app(argc, argv);

    QTranslator translator;
    if (translator.load("app_pl.qm", QDir::currentPath() + "/translations")) {
        app.installTranslator(&translator);
    }


    MainWindow w;
    w.show();
    return app.exec();
}
