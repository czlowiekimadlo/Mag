#ifndef SOLIDOBJECT_H
#define SOLIDOBJECT_H

#include <QFile>
#include <QTextStream>
#include <QStringList>

class SolidObject
{
public:
    SolidObject();
    ~SolidObject();

    void loadModel(QString);

    QList<float *> vertices;
    QList<float *> normals;
    QList< QList<int> * > faces;

    QList<float *> BBvertices;
    QList< QList<int> * > BBfaces;
};

#endif // SOLIDOBJECT_H
