#include "voronoivertex.h"

VoronoiVertex::VoronoiVertex()
{
    this->coords[0] = this->coords[1] = this->coords[2] = 0.0;
    this->halfEdge = 0;
    this->id = 0;
}

void VoronoiVertex::fill(float *data)
{
    this->coords[0] = data[0];
    this->coords[1] = data[1];
    this->coords[2] = data[2];
}

void VoronoiVertex::sub(VoronoiVertex *v, VoronoiVertex *u)
{
    this->coords[0] = u->coords[0] - v->coords[0];
    this->coords[1] = u->coords[1] - v->coords[1];
    this->coords[2] = u->coords[2] - v->coords[2];
}

void VoronoiVertex::add(VoronoiVertex *v, VoronoiVertex *u)
{
    this->coords[0] = u->coords[0] + v->coords[0];
    this->coords[1] = u->coords[1] + v->coords[1];
    this->coords[2] = u->coords[2] + v->coords[2];
}

void VoronoiVertex::mid(VoronoiVertex *v, VoronoiVertex *u)
{
    this->add(v,u);
    this->coords[0] = this->coords[0] / 2.0;
    this->coords[1] = this->coords[1] / 2.0;
    this->coords[2] = this->coords[2] / 2.0;
}

float VoronoiVertex::dot(VoronoiVertex *v)
{
    float sum = 0.0;
    for (int i = 0; i < 3; i++) {
        sum = sum + this->coords[0] * v->coords[0];
    }
    return sum;
}

VoronoiVertex * VoronoiVertex::getCopy()
{
    VoronoiVertex * v = new VoronoiVertex();
    v->coords[0] = this->coords[0];
    v->coords[1] = this->coords[1];
    v->coords[2] = this->coords[2];
    v->id = this->id;
    v->halfEdge = this->halfEdge;
    return v;
}

void VoronoiVertex::normalize()
{
    float d1, d2, d;

    d1 = sqrt(this->coords[0] * this->coords[0] + this->coords[1] * this->coords[1]);
    d2 = sqrt(this->coords[0] * this->coords[0] + this->coords[2] * this->coords[2]);

    d = sqrt(d1 * d1 + d2 * d2);

    this->coords[0] /= d;
    this->coords[1] /= d;
    this->coords[2] /= d;
}

void VoronoiVertex::revert()
{
    this->coords[0] = -this->coords[0];
    this->coords[1] = -this->coords[1];
    this->coords[2] = -this->coords[2];
}

float VoronoiVertex::dist(VoronoiVertex * v)
{
    float sum = 0.0;
    for (int i = 0; i < 3; i++) {
        sum = sum + (this->coords[i] - v->coords[i]) * (this->coords[i] - v->coords[i]);
    }
    return sum;
}

void VoronoiVertex::scale(float s)
{
    for (int i = 0; i < 3; i++) {
        this->coords[i] = this->coords[i] * s;
    }
}
