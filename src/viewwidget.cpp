#define _USE_MATH_DEFINES
#include "viewwidget.h"

ViewWidget::ViewWidget() : QGLWidget()
{
    this->setFocusPolicy(Qt::ClickFocus);

    this->angleX = this->angleY = 0.0;
    this->width = this->height = 1;
    this->distance = 0.0;

    this->showAxles = false;
    this->showBoundingBox = false;
}

ViewWidget::~ViewWidget()
{
}

void ViewWidget::initializeGL()
{
    glDisable(GL_TEXTURE_2D);
    glEnable(GL_DEPTH_TEST);
    glDisable(GL_COLOR_MATERIAL);
    glEnable(GL_POLYGON_SMOOTH);
    glEnable(GL_CULL_FACE);
    glFrontFace(GL_CCW);
    glCullFace(GL_BACK);
    glClearColor(0.1f, 0.1f, 0.1f, 1);
}

void ViewWidget::paintGL()
{
    glMatrixMode(GL_PROJECTION);
    glRotatef((360.0 * this->angleX) / 3.14, 0, 1, 0);
    glRotatef((360.0 * this->angleY) / 3.14, 0, 0, 1);
    //glTranslatef(this->distance, this->distance, this->distance);
    //this->distance = 0.0;

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if (this->showAxles) this->drawAxles();
    if (this->showBoundingBox) this->drawBoundingBox();
    this->drawModel();

    glFlush();
}

void ViewWidget::resizeGL(int w, int h)
{
    this->width = w;
    this->height = h;

    glViewport(0, 0, (GLint)w, (GLint)h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0, 1.0, 0.1, 100.0);
    gluLookAt(10.0, 10.0, 10.0,  0.0, 0.0, 0.0,  0.0, 1.0, 0.0);
    //gluOrtho2D(0, (GLint)w, 0, (GLint)h);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

void ViewWidget::loadModel(QString fileName)
{
    this->object.loadModel(fileName);
}

void ViewWidget::drawModel()
{
    QList<int> * face;
    float * vertex;

    for (int i = 0; i < this->object.faces.size(); i++) {
        glColor3f((float)i / (float)this->object.faces.size(),0,0);
        glBegin(GL_POLYGON);
        face = this->object.faces.at(i);
        for (int j = 0; j < face->size(); j++) {
            vertex = this->object.vertices.at(face->at(j) - 1);
            glVertex3f(vertex[0], vertex[1], vertex[2]);
        }
        glEnd();
    }
}

void ViewWidget::drawBoundingBox()
{
    QList<int> * face;
    float * vertex;

    for (int i = 0; i < this->object.BBfaces.size(); i++) {
        glColor3f(0, 255.0,0);
        glBegin(GL_LINE_LOOP);
        face = this->object.BBfaces.at(i);
        for (int j = 0; j < face->size(); j++) {
            vertex = this->object.BBvertices.at(face->at(j) - 1);
            glVertex3f(vertex[0], vertex[1], vertex[2]);
        }
        glEnd();
    }
}

void ViewWidget::drawAxles()
{
    glBegin(GL_LINES);
    glColor3f(1,0,0);
    glVertex3f(0, 0, 0);
    glVertex3f(10, 0, 0);
    glColor3f(0,1,0);
    glVertex3f(0, 0, 0);
    glVertex3f(0, 10, 0);
    glColor3f(0,0,1);
    glVertex3f(0, 0, 0);
    glVertex3f(0, 0, 10);
    glEnd();
}

void ViewWidget::mouseMoveEvent(QMouseEvent *e)
{
    if(e->buttons() & Qt::RightButton & Qt::LeftButton)
    {
        this->angleX = 0.0;
        this->angleY = 0.0;
        this->distance += (double)(e->x() - this->lastX) * (double)this->width / 10.0;
        this->lastX = e->x();
        this->lastY = e->y();

        this->updateGL();
    }
    else if(e->buttons() & Qt::LeftButton)
    {
        this->angleX = (double)(e->x() - this->lastX) / (double)this->width;
        this->angleY = 0.0;
        this->lastX = e->x();
        this->lastY = e->y();

        this->updateGL();
    }
    else if(e->buttons() & Qt::RightButton)
    {
        this->angleX = 0.0;
        this->angleY = (double)(this->lastY - e->y()) / (double)this->height;
        this->lastX = e->x();
        this->lastY = e->y();

        this->updateGL();
    }
}

void ViewWidget::mousePressEvent(QMouseEvent *e)
{
    this->lastX = e->x();
    this->lastY = e->y();
}

void ViewWidget::mouseReleaseEvent(QMouseEvent *e)
{

}

void ViewWidget::keyPressEvent(QKeyEvent *e)
{
    switch (e->key())
    {
        case Qt::Key_A:
            this->showAxles = !this->showAxles;
            this->updateGL();
            break;
        case Qt::Key_B:
            this->showBoundingBox = !this->showBoundingBox;
            this->updateGL();
            break;
        default:
            break;
   }
   e->accept();
   QGLWidget::keyPressEvent(e);
}

void ViewWidget::wheelEvent(QWheelEvent *e)
{
    this->distance = (double)e->delta() / 80.0;
    this->updateGL();
}
