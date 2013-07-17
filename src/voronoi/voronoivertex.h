#ifndef VORONOIVERTEX_H
#define VORONOIVERTEX_H

#include "math.h"
#include "voronoihalfedge.h"

class VoronoiHalfEdge;

class VoronoiVertex
{
public:
    VoronoiVertex();
    void fill(float *);

    void add(VoronoiVertex *, VoronoiVertex *);
    void sub(VoronoiVertex *, VoronoiVertex *);
    void mid(VoronoiVertex *, VoronoiVertex *);
    float dot(VoronoiVertex *);
    float dist(VoronoiVertex *);
    VoronoiVertex * getCopy();
    void scale(float);
    void normalize();
    void revert();
    bool equals(VoronoiVertex *);

    float coords[3];
    int id;
    VoronoiHalfEdge * halfEdge;
};

#endif // VORONOIVERTEX_H
