#include "voronoiedge.h"

VoronoiEdge::VoronoiEdge()
{
    this->halfEdge = 0;
}

/**
 * Calculate intersection of edge with a plane
 *
 * @brief VoronoiEdge::planeEdgeIntersection
 * @param p plane object
 * @return intersection vertex or null if no intersection
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
    float distance0, distance1;

    // same side, do nothing
    if (side0 * side1 == 1) return 0;

    // different sides, find point of intersection
    if (side0 * side1 == -1) {
        //distance = sqrt(v0->dist(v1));
        distance0 = p->dist(v0);
        distance1 = p->dist(v1);

        d = distance0 / (distance0 + distance1);
        direction.sub(v0, v1);
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
