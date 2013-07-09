#ifndef VORONOICELL_H
#define VORONOICELL_H

#include <QList>
#include <iostream>
#include "voronoivertex.h"
#include "voronoiedge.h"
#include "voronoiface.h"
#include "voronoihalfedge.h"
#include "voronoisplitedge.h"
#include "voronoiplane.h"

class VoronoiCell
{
public:
    VoronoiCell();
    ~VoronoiCell();

    void buildMesh(QList<float *> *, QList< QList<int> * > *);
    void pairHalfEdges(QList<VoronoiHalfEdge *> *);
    void createEdges(QList<VoronoiHalfEdge *> *, QList<VoronoiEdge *> *);
    void splitMesh(VoronoiPlane *);
    QList<VoronoiSplitEdge *> * retrieveSplitEdges(VoronoiPlane *);

    QList<VoronoiVertex *> vertices;
    QList<VoronoiFace *> faces;
    QList<VoronoiHalfEdge *> halfEdges;
    QList<VoronoiEdge *> edges;

    VoronoiVertex * center;

protected:
    int isCut(VoronoiPlane *);
    void flushCell();
    void splitFace(VoronoiFace *, VoronoiPlane *, QList<VoronoiVertex *> *, QList<VoronoiHalfEdge *> *, QList<VoronoiFace *> *);

    /*
    void pushVertex(float *);

    float * center;
    QList<VoronoiFace *> facets;

protected:
    void buildTetra();
    void addVertex(int);
    void recalculateCenter();

    QList<float *> vertices;
    */
};

#endif // VORONOICELL_H
