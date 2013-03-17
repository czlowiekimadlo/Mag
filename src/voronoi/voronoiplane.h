#ifndef VORONOIPLANE_H
#define VORONOIPLANE_H

#include "voronoivertex.h"

class VoronoiVertex;

class VoronoiPlane
{
public:
    VoronoiPlane(VoronoiVertex *, VoronoiVertex *);
    ~VoronoiPlane();
    int side(VoronoiVertex *);
    float dist(VoronoiVertex *);

    float a,b,c,d;
    VoronoiVertex * referencePoint;
    VoronoiVertex * normal;
};

#endif // VORONOIPLANE_H
