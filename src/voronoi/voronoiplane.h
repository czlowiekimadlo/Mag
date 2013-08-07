#ifndef VORONOIPLANE_H
#define VORONOIPLANE_H

#include "math.h"
#include "voronoivertex.h"

class VoronoiVertex;

class VoronoiPlane
{
public:
    VoronoiPlane(VoronoiVertex *, VoronoiVertex *);
    ~VoronoiPlane();
    int side(VoronoiVertex *);
    float dist(VoronoiVertex *);
    VoronoiPlane * getCopy();

    float a,b,c,d;
    VoronoiVertex * referencePoint;
    VoronoiVertex * normal;
};

#endif // VORONOIPLANE_H
