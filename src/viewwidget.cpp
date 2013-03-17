#define _USE_MATH_DEFINES
#include "viewwidget.h"

ViewWidget::ViewWidget() : QGLWidget()
{
    this->setFocusPolicy(Qt::ClickFocus);

    this->angleX = this->angleY = this->angleZ = 0.0;
    this->width = this->height = 1;
    this->distance = 0.0;

    this->showAxles = false;
    this->showBoundingBox = false;
    this->showSuperTetra = false;
    this->showTriangulation = false;
    this->showTriangulationNodes = false;
    this->showVoronoi = false;
    this->voronoiCell = 0;
}

ViewWidget::~ViewWidget()
{
}

void ViewWidget::initializeGL()
{
    glDisable(GL_TEXTURE_2D);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_COLOR_MATERIAL);
    glEnable(GL_POLYGON_SMOOTH);
    glEnable(GL_CULL_FACE);
    glFrontFace(GL_CCW);
    glCullFace(GL_BACK);
    glClearColor(0.1f, 0.1f, 0.1f, 1);
}

void ViewWidget::paintGL()
{
    glMatrixMode(GL_PROJECTION);
    glRotatef((360.0 * this->angleX) / 3.14, 1, 0, 0);
    glRotatef((360.0 * this->angleY) / 3.14, 0, 1, 0);
    glRotatef((360.0 * this->angleZ) / 3.14, 0, 0, 1);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if (this->showAxles) this->drawAxles();
    if (this->showBoundingBox) this->drawBoundingBox();
    if (this->showSuperTetra) this->drawSuperTetra();
    else if (this->showTriangulation) this->drawTriangulation();
    if (this->showVoronoi) this->drawVoronoi();
    this->drawRandomPoints();
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
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

void ViewWidget::loadModel(QString fileName)
{
    if (fileName.length() == 0)
    {
        fileName = QFileDialog::getOpenFileName(this, tr("Open File"),
                                                "",
                                                tr("Object (*.obj)"));
    }
    if (fileName.length() == 0) {
        exit(0);
    }
    this->object.loadModel(fileName);
}

void ViewWidget::drawModel()
{
    QList<int> * face;
    float * vertex;

    for (int i = 0; i < this->object.faces.size(); i++) {
        //glColor3f((float)i / (float)this->object.faces.size(),0,0);
        glColor3f(255.0, 0, 0);
        glBegin(GL_LINE_LOOP);
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

void ViewWidget::drawSuperTetra()
{
    QList<int> * face;
    float * vertex;

    for (int i = 0; i < this->object.superTetraFaces.size(); i++) {
        glColor3f(255.0, 255.0,0);
        glBegin(GL_LINE_LOOP);
        face = this->object.superTetraFaces.at(i);
        for (int j = 0; j < face->size(); j++) {
            vertex = this->object.superTetraVertices.at(face->at(j) - 1);
            glVertex3f(vertex[0], vertex[1], vertex[2]);
        }
        glEnd();
    }
}

void ViewWidget::drawTriangulation() {
    QList<int> * face;
    float * vertex;

    for (int i = 0; i < this->object.triangulationFaces.size(); i++) {
        if (this->object.triangulationWallsUsage.at(i) == 0) continue;
        glColor3f(255.0, 255.0,0);
        glBegin(GL_LINE_LOOP);
        face = this->object.triangulationFaces.at(i);
        for (int j = 0; j < face->size(); j++) {
            vertex = this->object.triangulationVertices.at(face->at(j) - 1);
            glVertex3f(vertex[0], vertex[1], vertex[2]);
        }
        glEnd();
    }

    if (!this->showTriangulationNodes) return;

    for (int i = 0; i < this->object.triangulationCells.size(); i++) {
        if (this->object.triangulationCells.at(i) == NULL) continue;
        glBegin(GL_LINES);
        glColor3f(0, 255.0, 0);
        glVertex3f(this->object.triangulationCells.at(i)->center[0], this->object.triangulationCells.at(i)->center[1], this->object.triangulationCells.at(i)->center[2]);
        glVertex3f(this->object.triangulationCells.at(i)->v1[0], this->object.triangulationCells.at(i)->v1[1], this->object.triangulationCells.at(i)->v1[2]);

        glVertex3f(this->object.triangulationCells.at(i)->center[0], this->object.triangulationCells.at(i)->center[1], this->object.triangulationCells.at(i)->center[2]);
        glVertex3f(this->object.triangulationCells.at(i)->v2[0], this->object.triangulationCells.at(i)->v2[1], this->object.triangulationCells.at(i)->v2[2]);

        glVertex3f(this->object.triangulationCells.at(i)->center[0], this->object.triangulationCells.at(i)->center[1], this->object.triangulationCells.at(i)->center[2]);
        glVertex3f(this->object.triangulationCells.at(i)->v3[0], this->object.triangulationCells.at(i)->v3[1], this->object.triangulationCells.at(i)->v3[2]);

        glVertex3f(this->object.triangulationCells.at(i)->center[0], this->object.triangulationCells.at(i)->center[1], this->object.triangulationCells.at(i)->center[2]);
        glVertex3f(this->object.triangulationCells.at(i)->v4[0], this->object.triangulationCells.at(i)->v4[1], this->object.triangulationCells.at(i)->v4[2]);
        glEnd();
    }
}

void ViewWidget::drawVoronoi() {

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

void ViewWidget::drawRandomPoints() {
    if (this->object.randomPoints.empty()) return;

    glBegin(GL_POINTS);
    glColor3f(1,0,1);
    for (QList<float *>::iterator i = this->object.randomPoints.begin(); i != this->object.randomPoints.end(); i++) {
        glVertex3f((*i)[0], (*i)[1], (*i)[2]);
    }
    glEnd();
}

void ViewWidget::mouseMoveEvent(QMouseEvent *e)
{
    double distance;
    distance = e->x() - this->lastX;

    if(e->buttons() == (Qt::RightButton | Qt::LeftButton))
    {
        this->angleX = 0.0;
        this->angleY = 0.0;
        this->angleZ = distance / (double)this->width;
        this->lastX = e->x();
        this->lastY = e->y();

        this->updateGL();
    }
    else if(e->buttons() == Qt::RightButton)
    {
        this->angleX = distance / (double)this->width;
        this->angleY = 0.0;
        this->angleZ = 0.0;
        this->lastX = e->x();
        this->lastY = e->y();

        this->updateGL();
    }
    else if(e->buttons() == Qt::LeftButton)
    {
        this->angleX = 0.0;
        this->angleY = distance / (double)this->width;
        this->angleZ = 0.0;
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
        case Qt::Key_C:
            this->object.destroyRandomPoints();
            this->updateGL();
            break;
        case Qt::Key_R:
            this->voronoiCell = 0;
            this->object.generateRandomPoints(2);
            this->object.createTriangulation();
            this->object.createVoronoi();
            this->updateGL();
            break;
        case Qt::Key_T:
            this->showSuperTetra = !this->showSuperTetra;
            this->showTriangulation = false;
            this->updateGL();
            break;
        case Qt::Key_U:
            if (this->object.vCells.size() > 0) {
                this->showVoronoi = true;
                this->voronoiCell++;
                this->voronoiCell = this->voronoiCell % this->object.vCells.size();
                this->updateGL();
            }
            break;
        case Qt::Key_Y:
            this->showTriangulation = !this->showTriangulation;
            this->showSuperTetra = false;
            this->updateGL();
            break;
        case Qt::Key_H:
            this->showTriangulationNodes = !this->showTriangulationNodes;
            this->updateGL();
            break;
        case Qt::Key_Escape:
            exit(0);
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
