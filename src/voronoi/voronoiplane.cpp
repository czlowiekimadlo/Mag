#include "voronoiplane.h"


VoronoiPlane::VoronoiPlane(VoronoiVertex * point, VoronoiVertex * normal)
{
    VoronoiVertex * n = normal->getCopy();
    n->normalize();
    this->a = n->coords[0];
    this->b = n->coords[1];
    this->c = n->coords[2];
    this->d = -(this->a * point->coords[0] + this->b * point->coords[1] + this->c * point->coords[2]);

    delete n;

    this->referencePoint = point;
    this->normal = normal;
}

VoronoiPlane::~VoronoiPlane()
{
    delete this->referencePoint;
    delete this->normal;
}

/**
 * Get new instance of plane object that is a copy of current one
 *
 * @brief VoronoiPlane::getCopy
 * @return plane object
 */
VoronoiPlane * VoronoiPlane::getCopy()
{
    return new VoronoiPlane(this->referencePoint->getCopy(), this->normal->getCopy());
}

/**
 * Check on which side of the plane is the vertex
 *  1 - in front of plane
 *  0 - on the plane
 * -1 - behind the plane
 *
 * @brief VoronoiPlane::side
 * @param v vertex
 * @return position status
 */
int VoronoiPlane::side(VoronoiVertex *v)
{
    VoronoiVertex v1;
    VoronoiVertex v2;
    VoronoiVertex  * reverted;
    float distanceNormal, distanceReverted;
    reverted = this->normal->getCopy();
    reverted->revert();

    v1.add(this->referencePoint, this->normal);
    v2.add(this->referencePoint, reverted);

    delete reverted;

    distanceNormal = v->dist(&v1);
    distanceReverted = v->dist(&v2);

    if (distanceNormal < distanceReverted) {
        return 1;
    }
    if (distanceNormal > distanceReverted) {
        return -1;
    }

    return 0;
}

/**
 * Calculate the distance from vertex to the plane
 *
 * @brief VoronoiPlane::dist
 * @param v vertex object
 * @return absolute distance value
 */
float VoronoiPlane::dist(VoronoiVertex * v)
{
    float d = this->a * v->coords[0] + this->b * v->coords[1] + this->c * v->coords[2] + this->d;
    if (d < 0) d = -d;
    return d;
}
