#include "solidobject.h"

SolidObject::SolidObject()
{
}

SolidObject::~SolidObject() {
    float *v;
    QList<int> * f;
    while (!this->vertices.empty()) {
        v = this->vertices.first();
        delete v;
        this->vertices.pop_front();
    }

    while (!this->normals.empty()) {
        v = this->normals.first();
        delete v;
        this->normals.pop_front();
    }

    while (!this->BBvertices.empty()) {
        v = this->BBvertices.first();
        delete v;
        this->BBvertices.pop_front();
    }

    while (!this->faces.empty()) {
        f = this->faces.first();
        delete f;
        this->faces.pop_front();
    }

    while (!this->BBfaces.empty()) {
        f = this->BBfaces.first();
        delete f;
        this->BBfaces.pop_front();
    }
}

void SolidObject::loadModel(QString fileName) {
    QFile file(fileName);
    QString line;
    QStringList elements;
    QList<int> * face;
    float *vertex;
    float maxX, maxY, maxZ;
    float minX, minY, minZ;

    if (file.open(QIODevice::ReadOnly)) {
        QTextStream t(&file);

        // read file by lines
        while (!t.atEnd()) {
          line = t.readLine();

          // vertex found
          if (line.startsWith("v")) {
              vertex = new float[3];
              elements = line.split(' ');
              vertex[0] = elements.at(1).toFloat();
              vertex[1] = elements.at(2).toFloat();
              vertex[2] = elements.at(3).toFloat();

              // find extreme points for bounding box
              if (this->vertices.size() == 0)
              {
                  maxX = minX = vertex[0];
                  maxY = minY = vertex[1];
                  maxZ = minZ = vertex[2];
              }
              if (vertex[0] > maxX) maxX = vertex[0];
              if (vertex[0] < minX) minX = vertex[0];
              if (vertex[1] > maxY) maxY = vertex[1];
              if (vertex[1] < minY) minY = vertex[1];
              if (vertex[2] > maxZ) maxZ = vertex[2];
              if (vertex[2] < minZ) minZ = vertex[2];

              this->vertices.push_back(vertex);
          }

          // face found
          if (line.startsWith("f")) {
              elements = line.split(' ');
              face = new QList<int>;
              for (int i = 1; i < elements.size(); i++) {
                face->push_back(elements.at(i).toInt());
              }
              this->faces.push_back(face);
          }
        }
        file.close();

        // build bounding box vertices
        vertex = new float[3];
        vertex[0] = maxX;
        vertex[1] = maxY;
        vertex[2] = maxZ;
        this->BBvertices.push_back(vertex);
        vertex = new float[3];
        vertex[0] = maxX;
        vertex[1] = minY;
        vertex[2] = maxZ;
        this->BBvertices.push_back(vertex);
        vertex = new float[3];
        vertex[0] = maxX;
        vertex[1] = minY;
        vertex[2] = minZ;
        this->BBvertices.push_back(vertex);
        vertex = new float[3];
        vertex[0] = maxX;
        vertex[1] = maxY;
        vertex[2] = minZ;
        this->BBvertices.push_back(vertex);
        vertex = new float[3];
        vertex[0] = minX;
        vertex[1] = maxY;
        vertex[2] = maxZ;
        this->BBvertices.push_back(vertex);
        vertex = new float[3];
        vertex[0] = minX;
        vertex[1] = maxY;
        vertex[2] = minZ;
        this->BBvertices.push_back(vertex);
        vertex = new float[3];
        vertex[0] = minX;
        vertex[1] = minY;
        vertex[2] = minZ;
        this->BBvertices.push_back(vertex);
        vertex = new float[3];
        vertex[0] = minX;
        vertex[1] = minY;
        vertex[2] = maxZ;
        this->BBvertices.push_back(vertex);

        // build bounding box faces
        face = new QList<int>;
        face->push_back(1);
        face->push_back(2);
        face->push_back(3);
        face->push_back(4);
        this->BBfaces.push_back(face);
        face = new QList<int>;
        face->push_back(5);
        face->push_back(6);
        face->push_back(7);
        face->push_back(8);
        this->BBfaces.push_back(face);
        face = new QList<int>;
        face->push_back(1);
        face->push_back(5);
        face->push_back(8);
        face->push_back(2);
        this->BBfaces.push_back(face);
        face = new QList<int>;
        face->push_back(2);
        face->push_back(8);
        face->push_back(7);
        face->push_back(3);
        this->BBfaces.push_back(face);
        face = new QList<int>;
        face->push_back(3);
        face->push_back(7);
        face->push_back(6);
        face->push_back(4);
        this->BBfaces.push_back(face);
        face = new QList<int>;
        face->push_back(5);
        face->push_back(1);
        face->push_back(4);
        face->push_back(6);
        this->BBfaces.push_back(face);
    }
}
