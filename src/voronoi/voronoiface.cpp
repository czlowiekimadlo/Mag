#include "voronoiface.h"

VoronoiFace::VoronoiFace()
{
    this->halfEdge = 0;
    this->plane = 0;
}

VoronoiFace::~VoronoiFace()
{
    if (this->plane != 0) {
        delete this->plane;
        this->plane = 0;
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
