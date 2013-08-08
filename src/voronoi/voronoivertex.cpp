#include "voronoivertex.h"

VoronoiVertex::VoronoiVertex()
{
    this->coords[0] = this->coords[1] = this->coords[2] = 0.0;
    this->halfEdge = 0;
    this->id = 0;
}

/**
 * Copy given coordinates data into internal structure
 *
 * @brief VoronoiVertex::fill
 * @param data
 */
void VoronoiVertex::fill(float *data)
{
    this->coords[0] = data[0];
    this->coords[1] = data[1];
    this->coords[2] = data[2];
}


/**
 * Subtract coordinates of u from v and store into internal structure
 *
 * @brief VoronoiVertex::sub
 * @param v
 * @param u
 */
void VoronoiVertex::sub(VoronoiVertex *v, VoronoiVertex *u)
{
    this->coords[0] = u->coords[0] - v->coords[0];
    this->coords[1] = u->coords[1] - v->coords[1];
    this->coords[2] = u->coords[2] - v->coords[2];
}

/**
 * Add coordinates of v and u and strore into internal structure
 *
 * @brief VoronoiVertex::add
 * @param v
 * @param u
 */
void VoronoiVertex::add(VoronoiVertex *v, VoronoiVertex *u)
{
    this->coords[0] = u->coords[0] + v->coords[0];
    this->coords[1] = u->coords[1] + v->coords[1];
    this->coords[2] = u->coords[2] + v->coords[2];
}

/**
 * Calculate coordinates of middle point between v and u and store data
 *
 * @brief VoronoiVertex::mid
 * @param v
 * @param u
 */
void VoronoiVertex::mid(VoronoiVertex *v, VoronoiVertex *u)
{
    this->add(v,u);
    this->coords[0] = this->coords[0] / 2.0;
    this->coords[1] = this->coords[1] / 2.0;
    this->coords[2] = this->coords[2] / 2.0;
}


/**
 * Calculate dot product of v and this point
 *
 * @brief VoronoiVertex::dot
 * @param v
 * @return scalar value
 */
float VoronoiVertex::dot(VoronoiVertex *v)
{
    float sum = 0.0;
    for (int i = 0; i < 3; i++) {
        sum = sum + this->coords[0] * v->coords[0];
    }
    return sum;
}

/**
 * Get new vertex object with exact copy of data
 *
 * @brief VoronoiVertex::getCopy
 * @return vertex object
 */
VoronoiVertex * VoronoiVertex::getCopy()
{
    VoronoiVertex * v = new VoronoiVertex();
    v->fill(this->coords);
    v->id = this->id;
    v->halfEdge = this->halfEdge;
    return v;
}

/**
 * Normalize vector to have length of 1
 *
 * @brief VoronoiVertex::normalize
 */
void VoronoiVertex::normalize()
{
    float d;

    d = sqrt(this->coords[0] * this->coords[0] + this->coords[1] * this->coords[1] + this->coords[2] * this->coords[2]);

    this->coords[0] /= d;
    this->coords[1] /= d;
    this->coords[2] /= d;
}

/**
 * Revert direction of vector
 *
 * @brief VoronoiVertex::revert
 */
void VoronoiVertex::revert()
{
    this->coords[0] = -this->coords[0];
    this->coords[1] = -this->coords[1];
    this->coords[2] = -this->coords[2];
}

/**
 * Calculate distance between points
 *
 * @brief VoronoiVertex::dist
 * @param v
 * @return scalar value
 */
float VoronoiVertex::dist(VoronoiVertex * v)
{
    float sum = 0.0;
    for (int i = 0; i < 3; i++) {
        sum = sum + (this->coords[i] - v->coords[i]) * (this->coords[i] - v->coords[i]);
    }
    return sum;
}


/**
 * Scale vector
 *
 * @brief VoronoiVertex::scale
 * @param s
 */
void VoronoiVertex::scale(float s)
{
    for (int i = 0; i < 3; i++) {
        this->coords[i] = this->coords[i] * s;
    }
}

/**
 * Compare if points have equal position in space
 *
 * @brief VoronoiVertex::equals
 * @param v
 * @return points equality
 */
bool VoronoiVertex::equals(VoronoiVertex * v)
{
    for (int i = 0; i < 3; i++) {
        if (this->coords[i] != v->coords[i]) {
            return false;
        }
    }

    return true;
}
