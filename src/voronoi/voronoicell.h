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
    void splitFace(VoronoiFace *, VoronoiPlane *, QList<VoronoiVertex *> *, QList<VoronoiHalfEdge *> *, QList<VoronoiFace *> *, QList<VoronoiSplitEdge *> *, QList<VoronoiVertex *> *);
    VoronoiVertex * getNewVertex(VoronoiVertex *, QList<VoronoiVertex *> *, QList<VoronoiVertex *> *);
    VoronoiSplitEdge * getSplitEdge(VoronoiHalfEdge *, QList<VoronoiSplitEdge *> *);
    VoronoiHalfEdge * createNewHalfEdge(VoronoiVertex *, VoronoiHalfEdge *, VoronoiFace *, QList<VoronoiVertex *> *, QList<VoronoiHalfEdge *> *, QList<VoronoiVertex *> *);
    void buildCutFace(QList<VoronoiHalfEdge *> *, QList<VoronoiFace *> *, QList<VoronoiVertex *> *, VoronoiPlane *, QList<VoronoiEdge *> *);
    void persistElements(QList<VoronoiVertex *> *, QList<VoronoiHalfEdge *> *, QList<VoronoiEdge *> *, QList<VoronoiFace *> *);

};

#endif // VORONOICELL_H
