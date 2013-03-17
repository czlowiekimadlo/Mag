#ifndef TETRAHEDRON_H
#define TETRAHEDRON_H

#include <QList>
#include <iostream>

class Tetrahedron
{
public:
    Tetrahedron(int, int, int, int, QList< QList<int> * > *, QList<float *> *);
    void calculateCircumcenter();
    bool isInRange(float *);
    bool containsVertex(int);

    int faces[4];
    QList< QList<int> * > * facesList;
    QList<float *> * vertexList;

    float center[3];
    float radius;

    float * v1;
    float * v2;
    float * v3;
    float * v4;
};

#endif // TETRAHEDRON_H
