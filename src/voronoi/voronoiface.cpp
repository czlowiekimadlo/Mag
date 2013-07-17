#include "voronoiface.h"

VoronoiFace::VoronoiFace()
{
    this->halfEdge = 0;
    this->plane = 0;
    /*
    this->edges[0].parent = this;
    this->edges[1].parent = this;
    this->edges[2].parent = this;
    this->removed = false;
    */
}

VoronoiFace::~VoronoiFace()
{
    if (this->plane != 0) {
        delete this->plane;
    }
}

int VoronoiFace::isCut(VoronoiPlane *p)
{
    VoronoiVertex * v;
    bool front = false;
    bool back = false;
    int side;
    VoronoiHalfEdge * e = this->halfEdge;

    do {
        v = e->v;
        side = p->side(v);
        if (side == 1) {
            front = true;
        }
        if (side == -1) {
            back = true;
        }
        if (front && back) {
            return 0;
        }

        e = e->next;
    } while (e != this->halfEdge);

    if (front && !back) {
        return 1;
    }
    if (back && !front) {
        return -1;
    }

    // there was some error or whole face is on plane
    throw 2;
}

/*
void VoronoiFace::fill(float *v1, float *v2, float *v3)
{
    float u[3];
    float v[3];

    // fill edges
    this->edges[0].v1 = v1;
    this->edges[0].v2 = v2;
    this->edges[1].v1 = v2;
    this->edges[1].v2 = v3;
    this->edges[2].v1 = v3;
    this->edges[2].v2 = v1;

    // middle of triangle
    this->middle[0] = (v1[0] + v2[0] + v3[0]) / 3.0;
    this->middle[1] = (v1[1] + v2[1] + v3[1]) / 3.0;
    this->middle[2] = (v1[2] + v2[2] + v3[2]) / 3.0;

    // calculate normal to this facet
    u[0] = v1[0] - v2[0];
    u[1] = v1[1] - v2[1];
    u[2] = v1[2] - v2[2];
    v[0] = v1[0] - v3[0];
    v[1] = v1[1] - v3[1];
    v[2] = v1[2] - v3[2];

    this->normal[0] = u[1]*v[2] - u[2]*v[1];
    this->normal[1] = u[2]*v[0] - u[0]*v[2];
    this->normal[2] = u[0]*v[1] - u[1]*v[0];
}

void VoronoiFace::calibrate(float * context)
{
    float normalMiddle[3];
    float revertMiddle[3];
    float vec[3];
    float normalDist, revertDist;

    normalMiddle[0] = this->middle[0] + this->normal[0];
    normalMiddle[1] = this->middle[1] + this->normal[1];
    normalMiddle[2] = this->middle[2] + this->normal[2];

    revertMiddle[0] = this->middle[0] - this->normal[0];
    revertMiddle[1] = this->middle[1] - this->normal[1];
    revertMiddle[2] = this->middle[2] - this->normal[2];

    vec[0] = normalMiddle[0] - context[0];
    vec[1] = normalMiddle[1] - context[1];
    vec[2] = normalMiddle[2] - context[2];
    normalDist = vec[0] * vec[0] + vec[1] * vec[1] + vec[2] * vec[2];

    vec[0] = revertMiddle[0] - context[0];
    vec[1] = revertMiddle[1] - context[1];
    vec[2] = revertMiddle[2] - context[2];
    revertDist = vec[0] * vec[0] + vec[1] * vec[1] + vec[2] * vec[2];

    if (revertDist > normalDist) {
        this->normal[0] = -this->normal[0];
        this->normal[1] = -this->normal[1];
        this->normal[2] = -this->normal[2];
    }
}

bool VoronoiFace::facing(float * point)
{
    float vec[3];
    float angle;

    vec[0] = point[0] - this->middle[0];
    vec[1] = point[1] - this->middle[1];
    vec[2] = point[2] - this->middle[2];

    angle = vec[0] * this->normal[0] + vec[1] * this->normal[1] + vec[2] * this->normal[2];
    if (angle < 0) return true;
    else return false;
}
*/
