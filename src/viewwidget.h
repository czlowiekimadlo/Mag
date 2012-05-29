#ifndef VIEWWIDGET_H
#define VIEWWIDGET_H

#include <QGLWidget>
#include <QMouseEvent>
#include <iostream>
#include "solidobject.h"

class ViewWidget : public QGLWidget
{
public:
    ViewWidget();
    ~ViewWidget();
    void initializeGL();
    void resizeGL(int, int);
    void paintGL();
    void loadModel(QString);

    bool showAxles;
    bool showBoundingBox;


protected:
    SolidObject object;

    int lastX, lastY;
    double angleX, angleY, distance;
    int width, height;

    void drawAxles();
    void drawModel();
    void drawBoundingBox();

    void mouseReleaseEvent(QMouseEvent *);
    void mousePressEvent(QMouseEvent *);
    void mouseMoveEvent(QMouseEvent *);
    void keyPressEvent(QKeyEvent *);
    void wheelEvent(QWheelEvent *);

};

#endif // VIEWWIDGET_H
