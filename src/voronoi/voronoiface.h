#ifndef VORONOIFACE_H
#define VORONOIFACE_H

//#include "voronoiedge.h"
#include "voronoihalfedge.h"
#include "voronoiplane.h"

class VoronoiFace
{
public:
    VoronoiFace();
    ~VoronoiFace();

    int id;
    VoronoiHalfEdge * halfEdge;
    VoronoiPlane * plane;

    /*
    void fill(float *, float *, float *);
    void calibrate(float *);
    bool facing(float *);
    bool removed;

    float normal[3];
    float middle[3];
    VoronoiEdge edges[3];
    */
};

#endif // VORONOIFACE_H
