#ifndef VORONOIEDGE_H
#define VORONOIEDGE_H

//#include <cstddef>
//class VoronoiFace;

#include <iostream>
#include "math.h"
#include "voronoihalfedge.h"
#include "voronoiplane.h"
#include "voronoivertex.h"

class VoronoiHalfEdge;
class VoronoiVertex;
class VoronoiPlane;

class VoronoiEdge
{
public:
    VoronoiEdge();
    VoronoiVertex * planeEdgeIntersection(VoronoiPlane *);

    int id;
    VoronoiHalfEdge * halfEdge;

    /*
    bool operator==(const VoronoiEdge &) const;

    float * v1;
    float * v2;

    VoronoiEdge * adjacentEdge;
    VoronoiFace * parent;
    */
};

//#include "voronoiface.h"

#endif // VORONOIEDGE_H
