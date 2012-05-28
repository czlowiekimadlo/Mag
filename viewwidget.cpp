#define _USE_MATH_DEFINES
#include "viewwidget.h"

ViewWidget::ViewWidget() : QGLWidget()
{
    this->track = TrackBall(0.05f, QVector3D(0, 1, 0), TrackBall::Sphere);
}

ViewWidget::~ViewWidget()
{
    float *v;
    QList<int> * f;
    while (!this->vertices.empty()) {
        v = this->vertices.first();
        delete v;
        this->vertices.pop_front();
    }

    while (!this->faces.empty()) {
        f = this->faces.first();
        delete f;
        this->faces.pop_front();
    }
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
    QList<int> * face;
    float * vertex;

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    QQuaternion q = this->track.rotation();
    //glRotatef(180.0*q.scalar() / M_PI, 0, 1, 0);
    gluLookAt(10.0, 10.0, 10.0,
              0.0, -sin(q.scalar()), 0.0,
              0.0, 1.0, 0.0);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


    std::cout << (180.0*q.scalar() / M_PI) << " " << q.x() << " " << q.y() << " " << q.z() << std::endl;

    // draw axles
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

    for (int i = 0; i < this->faces.size(); i++) {
        glColor3f((float)i / (float)this->faces.size(),0,0);
        glBegin(GL_POLYGON);
        face = this->faces.at(i);
        for (int j = 0; j < face->size(); j++) {
            vertex = this->vertices.at(face->at(j) - 1);
            glVertex3f(vertex[0], vertex[1], vertex[2]);
            //std::cout << vertex[0] << " " << vertex[1] << " " << vertex[2] << std::endl;
        }
        glEnd();
    }



    glFlush();
}

void ViewWidget::resizeGL(int w, int h)
{
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
    QFile file(fileName);
    QString line;
    QStringList elements;
    QList<int> * face;
    float *vertex;
    if (file.open(QIODevice::ReadOnly)) {
        QTextStream t(&file);
        while (!t.atEnd()) {
          line = t.readLine();
          if (line.startsWith("v")) {
              vertex = new float[3];
              elements = line.split(' ');
              vertex[0] = elements.at(1).toFloat();
              vertex[1] = elements.at(2).toFloat();
              vertex[2] = elements.at(3).toFloat();
              this->vertices.push_back(vertex);
          }

          if (line.startsWith("f")) {
              elements = line.split(' ');
              face = new QList<int>;
              for (int i = 1; i < elements.size(); i++) {
                face->push_back(elements.at(i).toInt());
              }
              this->faces.push_back(face);
          }
        }
        file.close();
    }
}


void ViewWidget::mouseMoveEvent(QMouseEvent *e)
{
    if(e->buttons() & Qt::LeftButton)
    {
        this->track.move(e->posF(), this->track.rotation().conjugate());
        this->paintGL();
    }
}

void ViewWidget::mousePressEvent(QMouseEvent *e)
{
    if(e->buttons() & Qt::LeftButton)
    {
        this->track.push(e->posF(), this->track.rotation().conjugate());
    }
}

void ViewWidget::mouseReleaseEvent(QMouseEvent *e)
{
    if(e->buttons() & Qt::LeftButton)
    {
        this->track.release(e->posF(), this->track.rotation().conjugate());
    }
}

