#include "voronoiedge.h"

VoronoiEdge::VoronoiEdge()
{
    this->halfEdge = 0;
    /*
    this->adjacentEdge = NULL;
    this->parent = NULL;
    this->v1 = NULL;
    this->v2 = NULL;
    */
}

/*
bool VoronoiEdge::operator==(const VoronoiEdge &edge) const
{
    if ((this->v1 == edge.v1 && this->v2 == edge.v2)
            || (this->v1 == edge.v2 && this->v2 == edge.v1)) {
        return true;
    }
    return false;
}
*/

VoronoiVertex * VoronoiEdge::planeEdgeIntersection(VoronoiPlane *p)
{
    VoronoiVertex * v0 = this->halfEdge->v;
    VoronoiVertex * v1 = this->halfEdge->pair->v;
    VoronoiVertex * v;
    VoronoiVertex direction;
    float d;

    int side0 = p->side(v0);
    int side1 = p->side(v1);
    float distance, distance0, distance1;

    // same side, do nothing
    if (side0 * side1 == 1) return 0;

    // different sides, find point of intersection
    if (side0 * side1 == -1) {
        distance = v0->dist(v1);
        distance0 = p->dist(v0);
        distance1 = p->dist(v1);
        d = distance0 / (distance0 + distance1);
        d = d * distance;
        direction.sub(v1, v0);
        direction.normalize();
        direction.scale(d);

        v = new VoronoiVertex();
        v->add(v0, &direction);
        return v;
    }

    // plane going through one of the points
    if (side0 * side1 == 0) {
        if (side0 == 0 && side1 == 1) {
            return v0;
        }
        if (side0 == 1 && side1 == 0) {
            return v1;
        }
    }

    return 0;
}