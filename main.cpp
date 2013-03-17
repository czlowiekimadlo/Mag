#define CGAL_DISABLE_ROUNDING_MATH_CHECK

#include <QtGui/QApplication>
#include "src/mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    ViewWidget vWidget;

    if (argc > 1)
    {
        vWidget.loadModel(argv[1]);
    }
    else
    {
        vWidget.loadModel("");
    }
    vWidget.resize(800, 600);
    w.setCentralWidget(&vWidget);

    w.resize(800, 600);
    w.show();
    
    return a.exec();
}
