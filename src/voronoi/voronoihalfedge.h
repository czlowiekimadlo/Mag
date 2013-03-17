#ifndef VORONOIHALFEDGE_H
#define VORONOIHALFEDGE_H

#include "voronoivertex.h"
#include "voronoiedge.h"
#include "voronoiface.h"

class VoronoiFace;
class VoronoiEdge;
class VoronoiVertex;

class VoronoiHalfEdge
{
public:
    VoronoiHalfEdge();

    int id;
    VoronoiVertex * v;
    VoronoiHalfEdge * pair;
    VoronoiFace * face;
    VoronoiHalfEdge * next;
    VoronoiEdge * edge;
};

#endif // VORONOIHALFEDGE_H
