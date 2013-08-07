#ifndef VORONOIFACE_H
#define VORONOIFACE_H

#include "voronoihalfedge.h"
#include "voronoiplane.h"
#include "voronoivertex.h"

class VoronoiFace
{
public:
    VoronoiFace();
    ~VoronoiFace();

    int id;
    VoronoiHalfEdge * halfEdge;
    VoronoiPlane * plane;

    int isCut(VoronoiPlane *);
};

#endif // VORONOIFACE_H
