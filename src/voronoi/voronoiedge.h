#ifndef VORONOIEDGE_H
#define VORONOIEDGE_H

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
};


#endif // VORONOIEDGE_H
