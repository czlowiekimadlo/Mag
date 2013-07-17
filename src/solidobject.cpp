#include "solidobject.h"

SolidObject::SolidObject()
{
}

SolidObject::~SolidObject() {
    Tetrahedron * t;
    VoronoiCell * v;

    this->flushPointsList(&this->vertices);
    this->flushPointsList(&this->normals);
    this->flushPointsList(&this->BBvertices);
    this->flushFacesList(&this->faces);
    this->flushFacesList(&this->BBfaces);
    this->flushPointsList(&this->triangulationVertices);
    this->flushFacesList(&this->triangulationFaces);

    while (!this->triangulationCells.empty()) {
        t = this->triangulationCells.first();
        delete t;
        this->triangulationCells.pop_front();
    }

    this->flushValuesList(&this->triangulationWallsUsage);

    while (!this->voronoiVertices.empty()) {
        this->voronoiVertices.pop_front();
    }
    this->flushFacesList(&this->voronoiFaces);
    this->flushFacesList(&this->voronoiCells);
    while (!this->vCells.empty()) {
        v = this->vCells.first();
        delete v;
        this->vCells.pop_front();
    }

    this->destroyRandomPoints();
}

void SolidObject::flushPointsList(QList<float *> *list) {
    if (list == NULL) return;
    float *v;
    while (!list->empty()) {
        v = list->first();
        delete v;
        list->pop_front();
    }
}
void SolidObject::flushFacesList(QList< QList<int> *> *list) {
    if (list == NULL) return;
    QList<int> * f;
    while (!list->empty()) {
        f = list->first();
        delete f;
        list->pop_front();
    }
}
void SolidObject::flushValuesList(QList<int> *list) {
    if (list == NULL) return;
    while (!list->empty()) {
        list->pop_front();
    }
}

void SolidObject::cpyFaces(QList< QList<int> * > *source, QList< QList<int> * > *destination) {
    QList<int> * f;

    this->flushFacesList(destination);
    for (QList< QList<int> * >::iterator i = source->begin(); i != source->end(); i++) {
        f = new QList<int>;
        for (QList<int>::iterator j = (*i)->begin(); j != (*i)->end(); j++) {
            f->push_back(*j);
        }
        destination->push_back(f);
    }
}
void SolidObject::cpyVertices(QList<float *> *source, QList<float *> *destination) {
    float *v;

    this->flushPointsList(destination);
    for (QList<float *>::iterator i = source->begin(); i != source->end(); i++) {
        v = new float[3];
        v[0] = (*i)[0];
        v[1] = (*i)[1];
        v[2] = (*i)[2];
        destination->push_back(v);
    }

}

void SolidObject::loadModel(QString fileName) {
    QFile file(fileName);
    QString line;
    QStringList elements;
    QList<int> * face;
    float *vertex;
    float maxX, maxY, maxZ;
    float minX, minY, minZ;
    float maxSize;

    maxX = maxY = maxZ = minX = minY = minZ = 0;

    if (file.open(QIODevice::ReadOnly)) {
        QTextStream t(&file);

        // read file by lines
        while (!t.atEnd()) {
          line = t.readLine();

          // vertex found
          if (line.startsWith("v")) {
              vertex = new float[3];
              elements = line.split(' ');
              vertex[0] = elements.at(1).toFloat();
              vertex[1] = elements.at(2).toFloat();
              vertex[2] = elements.at(3).toFloat();

              // find extreme points for bounding box
              if (this->vertices.size() == 0)
              {
                  maxX = minX = vertex[0];
                  maxY = minY = vertex[1];
                  maxZ = minZ = vertex[2];
              }
              if (vertex[0] > maxX) maxX = vertex[0];
              if (vertex[0] < minX) minX = vertex[0];
              if (vertex[1] > maxY) maxY = vertex[1];
              if (vertex[1] < minY) minY = vertex[1];
              if (vertex[2] > maxZ) maxZ = vertex[2];
              if (vertex[2] < minZ) minZ = vertex[2];

              this->vertices.push_back(vertex);
          }

          // face found
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

        // build bounding box vertices
        vertex = new float[3];
        vertex[0] = maxX;
        vertex[1] = maxY;
        vertex[2] = maxZ;
        this->BBvertices.push_back(vertex);
        vertex = new float[3];
        vertex[0] = maxX;
        vertex[1] = minY;
        vertex[2] = maxZ;
        this->BBvertices.push_back(vertex);
        vertex = new float[3];
        vertex[0] = maxX;
        vertex[1] = minY;
        vertex[2] = minZ;
        this->BBvertices.push_back(vertex);
        vertex = new float[3];
        vertex[0] = maxX;
        vertex[1] = maxY;
        vertex[2] = minZ;
        this->BBvertices.push_back(vertex);
        vertex = new float[3];
        vertex[0] = minX;
        vertex[1] = maxY;
        vertex[2] = maxZ;
        this->BBvertices.push_back(vertex);
        vertex = new float[3];
        vertex[0] = minX;
        vertex[1] = maxY;
        vertex[2] = minZ;
        this->BBvertices.push_back(vertex);
        vertex = new float[3];
        vertex[0] = minX;
        vertex[1] = minY;
        vertex[2] = minZ;
        this->BBvertices.push_back(vertex);
        vertex = new float[3];
        vertex[0] = minX;
        vertex[1] = minY;
        vertex[2] = maxZ;
        this->BBvertices.push_back(vertex);

        // calculate size
        this->sidesSizes[0] = maxX - minX;
        this->sidesSizes[1] = maxY - minY;
        this->sidesSizes[2] = maxZ - minZ;
        if (this->sidesSizes[0] < 0.0) this->sidesSizes[0] = -this->sidesSizes[0];
        if (this->sidesSizes[1] < 0.0) this->sidesSizes[1] = -this->sidesSizes[1];
        if (this->sidesSizes[2] < 0.0) this->sidesSizes[2] = -this->sidesSizes[2];

        // build bounding box faces
        face = new QList<int>;
        face->push_back(1);
        face->push_back(2);
        face->push_back(3);
        face->push_back(4);
        this->BBfaces.push_back(face);
        face = new QList<int>;
        face->push_back(5);
        face->push_back(6);
        face->push_back(7);
        face->push_back(8);
        this->BBfaces.push_back(face);
        face = new QList<int>;
        face->push_back(1);
        face->push_back(5);
        face->push_back(8);
        face->push_back(2);
        this->BBfaces.push_back(face);
        face = new QList<int>;
        face->push_back(2);
        face->push_back(8);
        face->push_back(7);
        face->push_back(3);
        this->BBfaces.push_back(face);
        face = new QList<int>;
        face->push_back(3);
        face->push_back(7);
        face->push_back(6);
        face->push_back(4);
        this->BBfaces.push_back(face);
        face = new QList<int>;
        face->push_back(5);
        face->push_back(1);
        face->push_back(4);
        face->push_back(6);
        this->BBfaces.push_back(face);

        // build super tetra
        if (this->sidesSizes[0] > this->sidesSizes[1]) {
            if (this->sidesSizes[0] > this->sidesSizes[2]) {
                maxSize = this->sidesSizes[0];
            } else {
                maxSize = this->sidesSizes[2];
            }
        } else {
            if (this->sidesSizes[1] > this->sidesSizes[2]) {
                maxSize = this->sidesSizes[1];
            } else {
                maxSize = this->sidesSizes[2];
            }
        }
        vertex = new float[3];
        vertex[0] = maxX + 0.5 * maxSize;
        vertex[1] = minY - 0.5 * maxSize;
        vertex[2] = minZ - 0.5 * maxSize;
        this->superTetraVertices.push_back(vertex);
        vertex = new float[3];
        vertex[0] = minX - 3.5 * maxSize;
        vertex[1] = minY - 0.5 * maxSize;
        vertex[2] = minZ - 0.5 * maxSize;
        this->superTetraVertices.push_back(vertex);
        vertex = new float[3];
        vertex[0] = maxX + 0.5 * maxSize;
        vertex[1] = minY - 0.5 * maxSize;
        vertex[2] = maxZ + 3.5 * maxSize;
        this->superTetraVertices.push_back(vertex);
        vertex = new float[3];
        vertex[0] = maxX + 0.5 * maxSize;
        vertex[1] = maxY + 3.5 * maxSize;
        vertex[2] = minZ - 0.5 * maxSize;
        this->superTetraVertices.push_back(vertex);
        face = new QList<int>;
        face->push_back(1);
        face->push_back(2);
        face->push_back(3);
        this->superTetraFaces.push_back(face);
        face = new QList<int>;
        face->push_back(1);
        face->push_back(2);
        face->push_back(4);
        this->superTetraFaces.push_back(face);
        face = new QList<int>;
        face->push_back(1);
        face->push_back(4);
        face->push_back(3);
        this->superTetraFaces.push_back(face);
        face = new QList<int>;
        face->push_back(2);
        face->push_back(3);
        face->push_back(4);
        this->superTetraFaces.push_back(face);
    }
}

void SolidObject::generateRandomPoints(int amount)
{
    QList<float *>::iterator it;
    float * point;
    int sum, scale, use;

    if (amount <= 0) return;

    this->destroyRandomPoints();

//    //generate two points to split mesh in halfs
//    float midX, midY, midZ, midX1, midX2;
//    midX = (this->BBvertices.at(0)[0] + this->BBvertices.at(6)[0]) / 2.0;
//    midY = (this->BBvertices.at(0)[1] + this->BBvertices.at(6)[1]) / 2.0;
//    midZ = (this->BBvertices.at(0)[2] + this->BBvertices.at(6)[2]) / 2.0;
//    midX1 = (midX + this->BBvertices.at(6)[0]) / 2.0;
//    midX2 = (midX + this->BBvertices.at(0)[0]) / 2.0;

//    //point 1
//    point = new float[3];
//    point[0] = midX1;
//    point[1] = midY;
//    point[2] = midZ;
//    this->randomPoints.push_back(point);

//    //point 2
//    point = new float[3];
//    point[0] = midX2;
//    point[1] = midY;
//    point[2] = midZ;
//    this->randomPoints.push_back(point);


    for (int i = 0; i < amount; i++)
    {
        point = new float[3];
        point[0] = point[1] = point[2] = 0.0;
        sum = 0;
        for (it = this->vertices.begin(); it != this->vertices.end(); it++)
        {
            use = qrand() % 100;
            if (use <= 50) continue;

            scale = qrand() % 100;
            sum += scale;
            point[0] += (double)scale * (*it)[0];
            point[1] += (double)scale * (*it)[1];
            point[2] += (double)scale * (*it)[2];
        }
        point[0] = point[0] / (double)sum;
        point[1] = point[1] / (double)sum;
        point[2] = point[2] / (double)sum;
        this->randomPoints.push_back(point);
    }

}

void SolidObject::destroyRandomPoints()
{
    this->flushPointsList(&this->randomPoints);
}


void SolidObject::createTriangulation() {
    if (this->randomPoints.empty()) return;

    Tetrahedron *t;
    int verticeIterator = 6;
    int faceIterator = 10;
    int WTFIterator = 0;
    int WTFSize = 0;
    int currentFace[4];
    int face;
    int k;
    float * v;
    QList<int> * f;
    QList<int> faceStack;
    bool facePresent;
    QList<int> * tempFace;
    QList<float *>::iterator it;
    it = this->randomPoints.begin();

    // clear triangulation structures
    this->flushPointsList(&this->triangulationVertices);
    this->flushFacesList(&this->triangulationFaces);
    while (!this->triangulationCells.empty()) {
        t = this->triangulationCells.first();
        delete t;
        this->triangulationCells.pop_front();
    }
    this->flushValuesList(&this->triangulationWallsUsage);

    // add super tetra
    this->cpyVertices(&this->superTetraVertices, &this->triangulationVertices);
    this->cpyFaces(&this->superTetraFaces, &this->triangulationFaces);
    for (int j = 0; j < 4; j++) {
        this->triangulationWallsUsage.push_back(1);
    }

    // add first point
    v = new float[3];
    v[0] = (*it)[0];
    v[1] = (*it)[1];
    v[2] = (*it)[2];
    this->triangulationVertices.push_back(v);

    // create inner faces with first point
    f = new QList<int>;
    f->push_back(1);
    f->push_back(2);
    f->push_back(5);
    this->triangulationFaces.push_back(f);
    this->triangulationWallsUsage.push_back(2);
    f = new QList<int>;
    f->push_back(2);
    f->push_back(3);
    f->push_back(5);
    this->triangulationFaces.push_back(f);
    this->triangulationWallsUsage.push_back(2);
    f = new QList<int>;
    f->push_back(3);
    f->push_back(1);
    f->push_back(5);
    this->triangulationFaces.push_back(f);
    this->triangulationWallsUsage.push_back(2);
    f = new QList<int>;
    f->push_back(1);
    f->push_back(4);
    f->push_back(5);
    this->triangulationFaces.push_back(f);
    this->triangulationWallsUsage.push_back(2);
    f = new QList<int>;
    f->push_back(2);
    f->push_back(4);
    f->push_back(5);
    this->triangulationFaces.push_back(f);
    this->triangulationWallsUsage.push_back(2);
    f = new QList<int>;
    f->push_back(3);
    f->push_back(4);
    f->push_back(5);
    this->triangulationFaces.push_back(f);
    this->triangulationWallsUsage.push_back(2);

    // create first four cells
    t = new Tetrahedron(1, 5, 6, 7, &this->triangulationFaces, &this->triangulationVertices);
    this->triangulationCells.push_back(t);
    t = new Tetrahedron(2, 5, 8, 9, &this->triangulationFaces, &this->triangulationVertices);
    this->triangulationCells.push_back(t);
    t = new Tetrahedron(3, 7, 8, 10, &this->triangulationFaces, &this->triangulationVertices);
    this->triangulationCells.push_back(t);
    t = new Tetrahedron(4, 6, 9, 10, &this->triangulationFaces, &this->triangulationVertices);
    this->triangulationCells.push_back(t);

    // go to next point
    it++;

    // add rest of points
    while (it != this->randomPoints.end()) {
        this->flushValuesList(&faceStack);
        v = new float[3];
        v[0] = (*it)[0];
        v[1] = (*it)[1];
        v[2] = (*it)[2];
        this->triangulationVertices.push_back(v);
        WTFSize = this->triangulationCells.size();
        for (WTFIterator = 0; WTFIterator < WTFSize; WTFIterator++) {
            if (this->triangulationCells.at(WTFIterator) == NULL) continue;
            if (this->triangulationCells.at(WTFIterator)->isInRange(v)) {
                for (int j = 0; j < 4; j++) {
                    face = this->triangulationCells.at(WTFIterator)->faces[j];
                    if (face > 4) {
                        this->triangulationWallsUsage.replace(face -1, this->triangulationWallsUsage.at(face -1) - 1);
                    }
                    if (faceStack.indexOf(face) == -1) {
                        faceStack.push_back(face);
                    }
                }
                delete this->triangulationCells.at(WTFIterator);
                this->triangulationCells.replace(WTFIterator, NULL);
            }
        }
        for (QList<int>::iterator sit = faceStack.begin(); sit != faceStack.end(); sit++) {
            if (this->triangulationWallsUsage.at(*sit - 1) == 0) continue;
            currentFace[0] = *sit;
            for (int j = 0; j < 3; j++) {
                facePresent = false;
                // check if face already exists
                for (k = 0; k < this->triangulationFaces.size(); k++) {
                    tempFace = this->triangulationFaces.at(k);
                    if (tempFace->indexOf(this->triangulationFaces.at(*sit - 1)->at(j)) != -1
                     && tempFace->indexOf(this->triangulationFaces.at(*sit - 1)->at((j + 1) % 3)) != -1
                     && tempFace->indexOf(verticeIterator) != -1) {
                        facePresent = true;
                        currentFace[j + 1] = k + 1;
                        if (k > 3) this->triangulationWallsUsage.replace(k, this->triangulationWallsUsage.at(k) + 1);
                        break;
                    }
                }
                if (!facePresent) {
                    f = new QList<int>;
                    f->push_back(this->triangulationFaces.at(*sit - 1)->at(j));
                    f->push_back(this->triangulationFaces.at(*sit - 1)->at((j + 1) % 3));
                    f->push_back(verticeIterator);
                    this->triangulationFaces.push_back(f);
                    this->triangulationWallsUsage.push_back(1);
                    currentFace[j + 1] = faceIterator++;
                }
            }
            t = new Tetrahedron(currentFace[0], currentFace[1], currentFace[2], currentFace[3], &this->triangulationFaces, &this->triangulationVertices);
            this->triangulationCells.push_back(t);
        }
        it++;
        verticeIterator++;
    }
}

void SolidObject::createVoronoi()
{
    VoronoiCell * c;
    VoronoiVertex * v1, * v2, * n, *m;
    VoronoiPlane *p;

    v1 = new VoronoiVertex();
    v2 = new VoronoiVertex();

    while (!this->vCells.empty()) {
        c = this->vCells.first();
        delete c;
        this->vCells.pop_front();
    }

    for (int i = 0; i < this->randomPoints.size(); i++) {
        c = new VoronoiCell();
        c->buildMesh(&(this->BBvertices), &(this->BBfaces));
        c->center = new VoronoiVertex();
        c->center->fill(this->randomPoints.at(i));

        for (int j = 0; j < this->randomPoints.size(); j++) {
            if (i == j) continue;

            v1->fill(this->randomPoints.at(i));
            v2->fill(this->randomPoints.at(j));
            m = new VoronoiVertex();
            n = new VoronoiVertex();
            m->mid(v1, v2);
            n->sub(v2, v1);

            p = new VoronoiPlane(m, n);

            c->splitMesh(p);
        }

        this->vCells.push_back(c);
    }

    delete v1;
    delete v2;




















    /*
    QList<int> adjacentVertices;
    QList<int> templist;
    QList<int> * face;
    QList<int> * cell;
    int index, last;
    float distance, x, y, z;
    float minDistance;
    bool faceMatched;
    float * vertex;


    VoronoiCell * cell;

    // clear data
    while (!this->voronoiVertices.empty()) {
        this->voronoiVertices.pop_front();
    }
    this->flushFacesList(&this->voronoiFaces);
    this->flushFacesList(&this->voronoiCells);
    while (!this->vCells.empty()) {
        cell = this->vCells.first();
        delete cell;
        this->vCells.pop_front();
    }

    for (int i = 4; i < this->triangulationVertices.size(); i++) {

        cell = new VoronoiCell();

        // search for cell vertices
        for (int j = 0; j < this->triangulationCells.size(); j++) {
            if (this->triangulationCells.at(j) == NULL) continue;
            if (this->triangulationCells.at(j)->containsVertex(i+1)) {
                cell->pushVertex(this->triangulationCells.at(j)->center);
            }
        }

        this->vCells.push_back(cell);


        // search for adjacent vertices
        this->flushValuesList(&adjacentVertices);
        for (int j = 0; j < this->triangulationFaces.size(); j++) {
            if (this->triangulationWallsUsage.at(j) < 1) continue;
            index = this->triangulationFaces.at(j)->indexOf(i+1);
            if (index != -1) {
                for (int k = 0; k < 3; k++) {
                    if (this->triangulationFaces.at(j)->at(k) < 5) continue;
                    if (k != index && adjacentVertices.indexOf(this->triangulationFaces.at(j)->at(k)) == -1) {
                        adjacentVertices.push_back(this->triangulationFaces.at(j)->at(k));
                    }
                }
            }
        }

        //std::cout << "calculating cell " << i << std::endl;
        //std::cout << "walls " << adjacentVertices.size() << std::endl;
        //for (int h = 0; h < adjacentVertices.size(); h++) {
        //    std::cout << adjacentVertices.at(h) << " ";
        //}
        //std::cout << std::endl;

        // create cell

        cell = new QList<int>;

        // build cell walls, one for each adjacent vertex
        for (int j = 0; j < adjacentVertices.size(); j++) {
            this->flushValuesList(&templist);
            // search for cells containing this edge
            for (int k = 0; k < this->triangulationCells.size(); k++) {
                if (this->triangulationCells.at(k) == NULL) continue;
                for (int l = 0; l < 4; l++) {
                    if (this->triangulationFaces.at(this->triangulationCells.at(k)->faces[l] - 1)->indexOf(i+1) != -1
                     && this->triangulationFaces.at(this->triangulationCells.at(k)->faces[l] - 1)->indexOf(adjacentVertices.at(j)) != -1) {
                        vertex = this->triangulationCells.at(k)->center;
                        index = this->voronoiVertices.indexOf(vertex);
                        if (index == -1) {
                            this->voronoiVertices.push_back(vertex);
                            index = this->voronoiVertices.indexOf(vertex);
                        }
                        templist.push_back(index + 1);
                        break;
                    }
                }
            }
            if (templist.empty()) continue;
            // see if this wall already exists
            faceMatched = false;
            for (int k = 0; k < this->voronoiFaces.size(); k++) {
                faceMatched = true;
                for (int l = 0; l < templist.size(); l++) {
                    if (this->voronoiFaces.at(k)->indexOf(templist.at(l)) == -1) {
                        faceMatched = false;
                        break;
                    }
                }
                if (faceMatched) {
                    index = k;
                    break;
                }
            }
            if (faceMatched) {
                cell->push_back(index);
                continue;
            }

            // put vertices into face in order
            face = new QList<int>;
            face->push_back(templist.at(0));
            last = 0;
            index = 1;
            for (int k = 1; k < templist.size(); k++) {
                minDistance = -1.0;
                for (int l = 1; l < templist.size(); l++) {
                    if (l == last) continue;
                    x = this->voronoiVertices.at(templist.at(last)-1)[0] - this->voronoiVertices.at(templist.at(l)-1)[0];
                    y = this->voronoiVertices.at(templist.at(last)-1)[1] - this->voronoiVertices.at(templist.at(l)-1)[1];
                    z = this->voronoiVertices.at(templist.at(last)-1)[2] - this->voronoiVertices.at(templist.at(l)-1)[2];

                    distance = x*x + y*y + z*z;
                    if (distance < minDistance || minDistance < 0.0) {
                        minDistance = distance;
                        index = l;
                    }
                }
                last = index;
                face->push_back(templist.at(last));
            }

            this->voronoiFaces.push_back(face);
            cell->push_back(this->voronoiFaces.indexOf(face));
        }

        this->voronoiCells.push_back(cell);
    }
    */
}
