#include "tetrahedron.h"

Tetrahedron::Tetrahedron(int f1, int f2, int f3, int f4, QList<QList<int> *> * facesList, QList<float *> * vertices)
{
    int verts[4];
    int v;
    QList<int> * face;

    this->faces[0] = f1;
    this->faces[1] = f2;
    this->faces[2] = f3;
    this->faces[3] = f4;
    this->facesList = facesList;
    this->vertexList = vertices;

    this->center[0] = this->center[1] = this->center[2] = 0.0;
    this->radius = 0;

    face = facesList->at(f1 -1);
    verts[0] = face->at(0);
    verts[1] = face->at(1);
    verts[2] = face->at(2);
    verts[3] = -1;
    face = facesList->at(f2 -1);

    for (int i = 0; i < 3; i++) {
        v = face->at(i);
        if (v != verts[0] && v != verts[1] && v != verts[2]) {
            verts[3] = v;
            break;
        }
    }
    this->v1 = vertices->at(verts[0] - 1);
    this->v2 = vertices->at(verts[1] - 1);
    this->v3 = vertices->at(verts[2] - 1);
    this->v4 = vertices->at(verts[3] - 1);

    this->calculateCircumcenter();
}

/*****************************************************************************/
/*  Original code taken from:                                                */
/*  http://www.ics.uci.edu/~eppstein/junkyard/circumcenter.html              */
/*                                                                           */
/*  tetcircumcenter()   Find the circumcenter of a tetrahedron.              */
/*                                                                           */
/*  The result is returned both in terms of xyz coordinates and xi-eta-zeta  */
/*  coordinates, relative to the tetrahedron's point `a' (that is, `a' is    */
/*  the origin of both coordinate systems).  Hence, the xyz coordinates      */
/*  returned are NOT absolute; one must add the coordinates of `a' to        */
/*  find the absolute coordinates of the circumcircle.  However, this means  */
/*  that the result is frequently more accurate than would be possible if    */
/*  absolute coordinates were returned, due to limited floating-point        */
/*  precision.  In general, the circumradius can be computed much more       */
/*  accurately.                                                              */
/*                                                                           */
/*  The xi-eta-zeta coordinate system is defined in terms of the             */
/*  tetrahedron.  Point `a' is the origin of the coordinate system.          */
/*  The edge `ab' extends one unit along the xi axis.  The edge `ac'         */
/*  extends one unit along the eta axis.  The edge `ad' extends one unit     */
/*  along the zeta axis.  These coordinate values are useful for linear      */
/*  interpolation.                                                           */
/*                                                                           */
/*  If `xi' is NULL on input, the xi-eta-zeta coordinates will not be        */
/*  computed.                                                                */
/*                                                                           */
/*****************************************************************************/
void Tetrahedron::calculateCircumcenter()
{
    float xba, yba, zba, xca, yca, zca, xda, yda, zda;
    float balength, calength, dalength;
    float xcrosscd, ycrosscd, zcrosscd;
    float xcrossdb, ycrossdb, zcrossdb;
    float xcrossbc, ycrossbc, zcrossbc;
    float denominator;
    float xcirca, ycirca, zcirca;
    /* Use coordinates relative to point `a' of the tetrahedron. */
    xba = this->v2[0] - this->v1[0];
    yba = this->v2[1] - this->v1[1];
    zba = this->v2[2] - this->v1[2];
    xca = this->v3[0] - this->v1[0];
    yca = this->v3[1] - this->v1[1];
    zca = this->v3[2] - this->v1[2];
    xda = this->v4[0] - this->v1[0];
    yda = this->v4[1] - this->v1[1];
    zda = this->v4[2] - this->v1[2];
    /* Squares of lengths of the edges incident to `a'. */
    balength = xba * xba + yba * yba + zba * zba;
    calength = xca * xca + yca * yca + zca * zca;
    dalength = xda * xda + yda * yda + zda * zda;
    /* Cross products of these edges. */
    xcrosscd = yca * zda - yda * zca;
    ycrosscd = zca * xda - zda * xca;
    zcrosscd = xca * yda - xda * yca;
    xcrossdb = yda * zba - yba * zda;
    ycrossdb = zda * xba - zba * xda;
    zcrossdb = xda * yba - xba * yda;
    xcrossbc = yba * zca - yca * zba;
    ycrossbc = zba * xca - zca * xba;
    zcrossbc = xba * yca - xca * yba;

    /* Calculate the denominator of the formulae. */
    /* Take your chances with floating-point roundoff. */
    denominator = 0.5 / (xba * xcrosscd + yba * ycrosscd + zba * zcrosscd);

    /* Calculate offset (from `a') of circumcenter. */
    xcirca = (balength * xcrosscd + calength * xcrossdb + dalength * xcrossbc) * denominator;
    ycirca = (balength * ycrosscd + calength * ycrossdb + dalength * ycrossbc) * denominator;
    zcirca = (balength * zcrosscd + calength * zcrossdb + dalength * zcrossbc) * denominator;
    this->center[0] = xcirca + this->v1[0];
    this->center[1] = ycirca + this->v1[1];
    this->center[2] = zcirca + this->v1[2];

    this->radius = xcirca * xcirca + ycirca * ycirca + zcirca * zcirca;
}

bool Tetrahedron::isInRange(float * v)
{
    float a,b,c,r;

    a = this->center[0] - v[0];
    b = this->center[1] - v[1];
    c = this->center[2] - v[2];
    r = a * a + b * b + c * c;

    if (r > this->radius) {
        return false;
    } else {
        return true;
    }
}

bool Tetrahedron::containsVertex(int index) {
    for (int i = 0; i < 4; i++) {
        if (this->facesList->at(this->faces[i] - 1)->indexOf(index) != -1) {
            return true;
        }
    }
    return false;
}
