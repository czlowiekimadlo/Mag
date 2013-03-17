#ifndef VIEWWIDGET_H
#define VIEWWIDGET_H

#include <QGLWidget>
#include <QMouseEvent>
#include <QFileDialog>
#include <iostream>
#include <GL/glu.h>
#include <math.h>
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
    bool showSuperTetra;
    bool showTriangulation;
    bool showTriangulationNodes;
    bool showVoronoi;
    int voronoiCell;


protected:
    SolidObject object;

    int lastX, lastY;
    double angleX, angleY, angleZ, distance;
    int width, height;

    void drawAxles();
    void drawModel();
    void drawBoundingBox();
    void drawRandomPoints();
    void drawSuperTetra();
    void drawTriangulation();
    void drawVoronoi();

    void mousePressEvent(QMouseEvent *);
    void mouseMoveEvent(QMouseEvent *);
    void keyPressEvent(QKeyEvent *);
    void wheelEvent(QWheelEvent *);

};

#endif // VIEWWIDGET_H
