#include "mainwindow.h"
#include <QApplication>
#include "cdatabase.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    CDataBase::getInstance().db_connect();
    w.show();

    return a.exec();
}

