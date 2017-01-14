#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QCoreApplication::setApplicationName("nc-sim"); //info for QSettings
    QCoreApplication::setOrganizationName("kwahoo");
    QCoreApplication::setOrganizationDomain("wkupiesila.blogspot.com");
    MainWindow w;
    w.show();

    return a.exec();
}
