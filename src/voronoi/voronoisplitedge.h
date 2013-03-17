#ifndef VORONOISPLITEDGE_H
#define VORONOISPLITEDGE_H

#include "voronoivertex.h"
#include "voronoiedge.h"

class VoronoiSplitEdge
{
public:
    VoronoiSplitEdge(VoronoiEdge *, VoronoiVertex *);

    VoronoiEdge * edge;
    VoronoiVertex * splitVertex;
};

#endif // VORONOISPLITEDGE_H
