#ifndef SOLIDOBJECT_H
#define SOLIDOBJECT_H

#include <QFile>
#include <QTextStream>
#include <QStringList>
#include <iostream>
#include "tetrahedron.h"
#include "voronoi/voronoicell.h"

class SolidObject
{
public:
    SolidObject();
    ~SolidObject();

    void loadModel(QString);
    void generateRandomPoints(int);
    void destroyRandomPoints();
    void createTriangulation();
    void createVoronoi();

    QList<float *> vertices;
    QList<float *> normals;
    QList< QList<int> * > faces;

    QList<float *> BBvertices;
    QList< QList<int> * > BBfaces;
    float sidesSizes[3];

    QList<float *> randomPoints;
    QList<float *> superTetraVertices;
    QList< QList<int> * > superTetraFaces;

    QList<float *> triangulationVertices;
    QList< QList<int> * > triangulationFaces;
    QList<Tetrahedron *> triangulationCells;
    QList<int> triangulationWallsUsage;

    QList<float *> voronoiVertices;
    QList< QList<int> * > voronoiFaces;
    QList< QList<int> * > voronoiCells;
    QList<VoronoiCell *> vCells;


protected:
    void flushPointsList(QList<float *> *);
    void flushFacesList(QList< QList<int> * > *);
    void flushValuesList(QList<int> *);

    void cpyVertices(QList<float *> *, QList<float *> *);
    void cpyFaces(QList< QList<int> * > *, QList< QList<int> * > *);
};

#endif // SOLIDOBJECT_H
