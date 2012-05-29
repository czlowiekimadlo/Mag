#include <QtGui/QApplication>
#include "src/mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    ViewWidget vWidget;

    vWidget.loadModel("cube.obj");
    vWidget.resize(800, 600);
    w.setCentralWidget(&vWidget);

    w.resize(800, 600);
    w.show();
    
    return a.exec();
}
