#include "mainwidget.h"

#include <QApplication>
#include <QString>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWidget w("localhost", 54321);

    w.show();
    w.showStartMenu();
    w.showStartMenuRegistration();
    return a.exec();
}
