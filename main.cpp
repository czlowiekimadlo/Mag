#include <QtGui/QApplication>
#include "mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    ViewWidget vWidget;

    vWidget.loadModel("c:\\Users\\Hrabia Quak Metzger\\Dropbox\\Magisterka\\gl\\cube.obj");
    vWidget.resize(800, 600);
    w.setCentralWidget(&vWidget);

    w.resize(800, 600);
    w.show();
    
    return a.exec();
}
