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
    this->showFragments = false;
    this->showFullFaces = false;
    this->currentObject = 0;
    this->voronoiCell = 0;
    this->fragment = 0;

    this->lastCenter[0] = this->lastCenter[1] = this->lastCenter[2] = 0.0;
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
    glClearColor(255.0f, 255.0f, 255.0f, 1);
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
    if (this->showFragments) this->drawFragment();
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
    SolidObject * object = new SolidObject();

    if (fileName.length() == 0)
    {
        fileName = QFileDialog::getOpenFileName(this, tr("Open File"),
                                                "",
                                                tr("Object (*.obj)"));
    }
    if (fileName.length() == 0) {
        exit(0);
    }

    object->loadModel(fileName);
    this->objects.push_back(object);
}

void ViewWidget::drawModel()
{
    QList<int> * face;
    float * vertex;

    SolidObject * object;

    for (int i = 0; i < this->objects.size(); i++) {
        object = this->objects.at(i);

        for (int j = 0; j < object->faces.size(); j++) {
            //glColor3f((float)j / (float)object->faces.size(),0,0);
            if (i == this->currentObject) {
                glColor3f(255.0, 0, 0);
            } else {
                glColor3f(0, 0, 255.0);
            }
            if (this->showFullFaces) {
                glBegin(GL_POLYGON);
                if (i == this->currentObject) {
                    glColor3f(0, 0, (float)(j + 5) / (float)(object->faces.size() + 10));
                } else {
                    glColor3f((float)(j + 5) / (float)(object->faces.size() + 10),0,0);
                }
            } else {
                glBegin(GL_LINE_LOOP);
            }

            face = object->faces.at(j);
            for (int k = 0; k < face->size(); k++) {
                vertex = object->vertices.at(face->at(k) - 1);
                glVertex3f(vertex[0], vertex[1], vertex[2]);
            }
            glEnd();
        }
    }

}

void ViewWidget::drawBoundingBox()
{
    QList<int> * face;
    float * vertex;

    SolidObject * object;
    object = this->objects.at(this->currentObject);

    for (int i = 0; i < object->BBfaces.size(); i++) {
        glColor3f(0, 255.0,0);
        glBegin(GL_LINE_LOOP);
        face = object->BBfaces.at(i);
        for (int j = 0; j < face->size(); j++) {
            vertex = object->BBvertices.at(face->at(j) - 1);
            glVertex3f(vertex[0], vertex[1], vertex[2]);
        }
        glEnd();
    }
}

void ViewWidget::drawSuperTetra()
{
    QList<int> * face;
    float * vertex;

    SolidObject * object;
    object = this->objects.at(this->currentObject);

    for (int i = 0; i < object->superTetraFaces.size(); i++) {
        glColor3f(255.0, 255.0,0);
        glBegin(GL_LINE_LOOP);
        face = object->superTetraFaces.at(i);
        for (int j = 0; j < face->size(); j++) {
            vertex = object->superTetraVertices.at(face->at(j) - 1);
            glVertex3f(vertex[0], vertex[1], vertex[2]);
        }
        glEnd();
    }
}

void ViewWidget::drawTriangulation() {
    QList<int> * face;
    float * vertex;

    SolidObject * object;
    object = this->objects.at(this->currentObject);

    for (int i = 0; i < object->triangulationFaces.size(); i++) {
        if (object->triangulationWallsUsage.at(i) == 0) continue;
        glColor3f(255.0, 255.0,0);
        glBegin(GL_LINE_LOOP);
        face = object->triangulationFaces.at(i);
        for (int j = 0; j < face->size(); j++) {
            vertex = object->triangulationVertices.at(face->at(j) - 1);
            glVertex3f(vertex[0], vertex[1], vertex[2]);
        }
        glEnd();
    }

    if (!this->showTriangulationNodes) return;

    for (int i = 0; i < object->triangulationCells.size(); i++) {
        if (object->triangulationCells.at(i) == NULL) continue;
        glBegin(GL_LINES);
        glColor3f(0, 255.0, 0);
        glVertex3f(object->triangulationCells.at(i)->center[0], object->triangulationCells.at(i)->center[1], object->triangulationCells.at(i)->center[2]);
        glVertex3f(object->triangulationCells.at(i)->v1[0], object->triangulationCells.at(i)->v1[1], object->triangulationCells.at(i)->v1[2]);

        glVertex3f(object->triangulationCells.at(i)->center[0], object->triangulationCells.at(i)->center[1], object->triangulationCells.at(i)->center[2]);
        glVertex3f(object->triangulationCells.at(i)->v2[0], object->triangulationCells.at(i)->v2[1], object->triangulationCells.at(i)->v2[2]);

        glVertex3f(object->triangulationCells.at(i)->center[0], object->triangulationCells.at(i)->center[1], object->triangulationCells.at(i)->center[2]);
        glVertex3f(object->triangulationCells.at(i)->v3[0], object->triangulationCells.at(i)->v3[1], object->triangulationCells.at(i)->v3[2]);

        glVertex3f(object->triangulationCells.at(i)->center[0], object->triangulationCells.at(i)->center[1], object->triangulationCells.at(i)->center[2]);
        glVertex3f(object->triangulationCells.at(i)->v4[0], object->triangulationCells.at(i)->v4[1], object->triangulationCells.at(i)->v4[2]);
        glEnd();
    }
}

void ViewWidget::drawVoronoi()
{
    VoronoiCell * cell;
    VoronoiFace * face;
    VoronoiHalfEdge * halfEdgeHandler;
    VoronoiVertex * vertexHandler;
    float * vertex;

    SolidObject * object;
    object = this->objects.at(this->currentObject);

    int j;
    int size;

    if (object->vCells.empty()) return;

    size = object->vCells.size();

    if (this->voronoiCell == size) {
        j = 0;
    } else {
        j = this->voronoiCell;
        size = this->voronoiCell + 1;
    }

    for (; j < size; j++) {
        cell = object->vCells.at(j);

        for (int i = 0; i < cell->faces.size(); i++) {
            face = cell->faces.at(i);
            halfEdgeHandler = face->halfEdge;

            //std::cout << "drawing face " << face << std::endl;

            glColor3f(0, 255.0,0);
            glBegin(GL_LINE_LOOP);

            do {
                if (halfEdgeHandler == NULL) {
                    //std::cout << "empty hander" << std::endl;
                    break;
                }
                if (halfEdgeHandler->next == NULL) {
                    //std::cout << "empty next" << std::endl;
                    break;
                }

                vertexHandler = halfEdgeHandler->v;
                vertex = vertexHandler->coords;
                if (vertex == NULL) {
                    //std::cout << "empty vertex" << std::endl;
                    break;
                }
                glVertex3f(vertex[0], vertex[1], vertex[2]);

                halfEdgeHandler = halfEdgeHandler->next;
            } while (halfEdgeHandler != face->halfEdge);

            glEnd();
        }
    }

//    VoronoiSplitEdge * seHandler;
//    glColor3f(0, 255.0,255.0);
//    glBegin(GL_LINE_LOOP);
//    for (int i = 0; i < cell->splitEdges->size(); i++) {
//        seHandler = cell->splitEdges->at(i);

//        vertex = seHandler->splitVertex->coords;
//        glVertex3f(vertex[0], vertex[1], vertex[2]);
//    }
//    glEnd();
}

void ViewWidget::drawFragment()
{
    VoronoiCell * cell;
    VoronoiFace * face;
    VoronoiHalfEdge * halfEdgeHandler;
    VoronoiVertex * vertexHandler;
    float * vertex;

    SolidObject * object;
    object = this->objects.at(this->currentObject);

    int j;
    int size;

    if (object->fragments.empty()) return;

    size = object->fragments.size();

    if (this->fragment == size) {
        j = 0;
    } else {
        j = this->fragment;
        size = this->fragment + 1;
    }

    for (; j < size; j++) {
        cell = object->fragments.at(j);

        for (int i = 0; i < cell->faces.size(); i++) {
            face = cell->faces.at(i);
            halfEdgeHandler = face->halfEdge;

            glColor3f(0, 255.0,0);
            glBegin(GL_LINE_LOOP);

            do {
                if (halfEdgeHandler == NULL) {
                    break;
                }
                if (halfEdgeHandler->next == NULL) {
                    break;
                }

                vertexHandler = halfEdgeHandler->v;
                vertex = vertexHandler->coords;
                if (vertex == NULL) {
                    break;
                }
                glVertex3f(vertex[0], vertex[1], vertex[2]);

                halfEdgeHandler = halfEdgeHandler->next;
            } while (halfEdgeHandler != face->halfEdge);

            glEnd();
        }

        //        for (int i = 0; i < cell->faces.size(); i++) {
        //            face = cell->faces.at(i);
        //            halfEdgeHandler = face->halfEdge;

        //            do {
        //                if (halfEdgeHandler == NULL) {
        //                    break;
        //                }
        //                if (halfEdgeHandler->next == NULL) {
        //                    break;
        //                }

        //                if (halfEdgeHandler->pair == NULL) {
        //                    glColor3f(0, 0,255.0);
        //                    glBegin(GL_LINE_LOOP);

        //                    vertexHandler = halfEdgeHandler->v;
        //                    vertex = vertexHandler->coords;
        //                    if (vertex == NULL) {
        //                        break;
        //                    }
        //                    glVertex3f(vertex[0], vertex[1], vertex[2]);

        //                    vertexHandler = halfEdgeHandler->next->v;
        //                    vertex = vertexHandler->coords;
        //                    if (vertex == NULL) {
        //                        break;
        //                    }
        //                    glVertex3f(vertex[0], vertex[1], vertex[2]);

        //                    glEnd();
        //                }

        //                halfEdgeHandler = halfEdgeHandler->next;
        //            } while (halfEdgeHandler != face->halfEdge);
        //        }
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

void ViewWidget::drawRandomPoints() {
    SolidObject * object;
    object = this->objects.at(this->currentObject);

    if (object->randomPoints.empty()) return;

    glBegin(GL_POINTS);
    glColor3f(1,0,1);
    for (QList<float *>::iterator i = object->randomPoints.begin(); i != object->randomPoints.end(); i++) {
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
    SolidObject * object;
    object = this->objects.at(this->currentObject);

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
            object->destroyRandomPoints();
            this->updateGL();
            break;
        case Qt::Key_R:
            this->voronoiCell = 0;
            object->generateRandomPoints(5);
            object->createTriangulation();
            object->createVoronoi();
            object->createFragments();
            this->updateGL();
            break;
        case Qt::Key_T:
            this->showSuperTetra = !this->showSuperTetra;
            this->showTriangulation = false;
            this->updateGL();
            break;
        case Qt::Key_U:
            if (object->vCells.size() > 0) {
                this->showVoronoi = true;
                this->voronoiCell++;
                this->voronoiCell = this->voronoiCell % (object->vCells.size() + 1);
                this->updateGL();
            }
            break;
        case Qt::Key_F:
            if (object->fragments.size() > 0) {
                this->showFragments = true;
                this->fragment++;
                this->fragment = this->fragment % (object->fragments.size() + 1);
                this->updateGL();
            }
            break;
        case Qt::Key_I:
            this->showVoronoi = false;
            this->showFragments = false;
            this->updateGL();
            break;
        case Qt::Key_G:
            if (this->objects.size() > 0) {
                this->currentObject++;
                this->currentObject = this->currentObject % this->objects.size();
                this->fragment = 0;
                this->voronoiCell = 0;
                this->showFragments = false;
                this->showVoronoi = false;
                this->showTriangulation = false;
                this->showTriangulationNodes = false;
                this->updateGL();
                std::cout << this->currentObject << std::endl;
            }
            break;
        case Qt::Key_S:
            this->splitCurrentObject();
            this->currentObject = 0;
            this->fragment = 0;
            this->voronoiCell = 0;
            this->showFragments = false;
            this->showVoronoi = false;
            this->showTriangulation = false;
            this->showTriangulationNodes = false;
            this->updateGL();
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
        case Qt::Key_M:
            this->showFullFaces = !this->showFullFaces;
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

void ViewWidget::splitCurrentObject()
{
    QList<SolidObject *> * newObjects;
    SolidObject * object;
    object = this->objects.at(this->currentObject);

    this->lastCenter[0] = object->center[0];
    this->lastCenter[1] = object->center[1];
    this->lastCenter[2] = object->center[2];

    newObjects = object->split();

    if (!newObjects->empty()) {
        this->objects.removeAt(this->currentObject);

        while (!newObjects->empty()) {
            this->objects.push_back(newObjects->front());
            this->moveObject(newObjects->front());
            newObjects->pop_front();
        }
    }

    delete newObjects;
}

void ViewWidget::moveObject(SolidObject * object)
{
    float vector[3];
    float * holder;

    for (int i = 0; i < 3; i++) {
        vector[i] = (object->center[i] - this->lastCenter[i]) * 3;
        object->center[i] = object->center[i] + vector[i];
    }

    for (int i = 0; i < object->vertices.size(); i++) {
        holder = object->vertices.at(i);
        holder[0] = holder[0] + vector[0];
        holder[1] = holder[1] + vector[1];
        holder[2] = holder[2] + vector[2];
    }

    for (int i = 0; i < object->BBvertices.size(); i++) {
        holder = object->BBvertices.at(i);
        holder[0] = holder[0] + vector[0];
        holder[1] = holder[1] + vector[1];
        holder[2] = holder[2] + vector[2];
    }

    for (int i = 0; i < object->superTetraVertices.size(); i++) {
        holder = object->superTetraVertices.at(i);
        holder[0] = holder[0] + vector[0];
        holder[1] = holder[1] + vector[1];
        holder[2] = holder[2] + vector[2];
    }
}


