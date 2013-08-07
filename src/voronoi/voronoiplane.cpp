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

VoronoiPlane * VoronoiPlane::getCopy()
{
    return new VoronoiPlane(this->referencePoint->getCopy(), this->normal->getCopy());
}

//  1 - in the front
// -1 - in the back
//  0 - on the plane
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

float VoronoiPlane::dist(VoronoiVertex * v)
{
    float d = this->a * v->coords[0] + this->b * v->coords[1] + this->c * v->coords[2] + this->d;
    //std::cout << this->referencePoint->coords[0] << " " << v->coords[0] << std::endl;
    //std::cout << this->a << " " << this->b << " " << this->c << " " << this->d << " " << std::endl;
    if (d < 0) d = -d;
    return d;
}
