#ifndef VIEWWIDGET_H
#define VIEWWIDGET_H

#include <QGLWidget>
#include <QFile>
#include <QTextStream>
#include <iostream>
#include "trackball.h"
class ViewWidget : public QGLWidget
{
public:
    ViewWidget();
    ~ViewWidget();
    void initializeGL();
    void resizeGL(int, int);
    void paintGL();
    void loadModel(QString);

protected:
    QList<float *> vertices;
    QList< QList<int> * > faces;
    TrackBall track;

    void mouseReleaseEvent(QMouseEvent *);
    void mousePressEvent(QMouseEvent *);
    void mouseMoveEvent(QMouseEvent *);

};

#endif // VIEWWIDGET_H
