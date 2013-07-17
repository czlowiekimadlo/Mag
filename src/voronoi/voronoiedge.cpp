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
        distance = sqrt(v0->dist(v1));
        distance0 = p->dist(v0);
        distance1 = p->dist(v1);

        //std::cout << distance << " " << distance0 << " " << distance1 << " " << v0->coords[0] << " " << v1->coords[0] << std::endl;

        d = distance0 / (distance0 + distance1);
        direction.sub(v0, v1);
        direction.scale(d);

        //std::cout << d << std::endl;
        //std::cout << direction.coords[0] << " " << direction.coords[1] << " " << direction.coords[2] << std::endl;

        v = new VoronoiVertex();
        v->add(v0, &direction);

        //std::cout << v->coords[0] << " " << v->coords[1] << " " << v->coords[2] << std::endl;
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
