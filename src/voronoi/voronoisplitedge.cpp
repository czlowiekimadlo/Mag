#include "voronoisplitedge.h"

VoronoiSplitEdge::VoronoiSplitEdge(VoronoiEdge *e, VoronoiVertex *v)
{
    this->edge = e;
    this->splitVertex = v->getCopy();
}
