#include "voronoicell.h"

VoronoiCell::VoronoiCell()
{
    //this->center = new float[3];
}

VoronoiCell::~VoronoiCell()
{
    this->flushCell();
    /*
    VoronoiFace * f;
    while (!this->facets.empty()) {
        f = this->facets.front();
        delete f;
        this->facets.pop_front();
    }

    delete this->center;
    */
}

void VoronoiCell::flushCell()
{
    VoronoiVertex * v;
    VoronoiHalfEdge * he;
    VoronoiEdge * e;
    VoronoiFace * f;

    this->center = 0;

    while (!this->vertices.empty()) {
        v = this->vertices.front();
        delete v;
        this->vertices.pop_front();
    }
    while (!this->halfEdges.empty()) {
        he = this->halfEdges.front();
        delete he;
        this->halfEdges.pop_front();
    }
    while (!this->edges.empty()) {
        e = this->edges.front();
        delete e;
        this->edges.pop_front();
    }
    while (!this->faces.empty()) {
        f = this->faces.front();
        delete f;
        this->faces.pop_front();
    }
}

void VoronoiCell::buildMesh(QList<float *> *verts, QList<QList<int> *> *facets)
{
    VoronoiVertex * v;
    VoronoiHalfEdge * he;
    VoronoiFace * f;
    QList<VoronoiHalfEdge *> faceEdges;

    this->center = new VoronoiVertex();

    //Add all input vertices to the mesh, the original index of the vertex is stored as an id.
    for(int i=0; i < verts->size(); i++) {
        v = new VoronoiVertex();
        v->fill(verts->at(i));
        v->id = i;
        this->vertices.push_back(v);
    }

    //Create a loop of halfedges for each face. We assume the vertices are given in a consistent order.
    // To extend this for a non-ordered list, sort the vertices of the input vertices here.
    for(int i=0; i < facets->size(); i++) {
        while (!faceEdges.empty()) faceEdges.pop_back();
        f = new VoronoiFace();
        this->faces.push_back(f);
        //TODO: sort face vertices here
        for(int j=0; j < facets->at(i)->size(); j++) {
            he = new VoronoiHalfEdge();
            faceEdges.push_back(he);
            this->halfEdges.push_back(he);
            he->face = f;
            if (f->halfEdge == NULL) f->halfEdge = he;
            he->v = this->vertices.at(facets->at(i)->at(j) - 1);
            if (he->v->halfEdge == NULL) he->v->halfEdge = he;
        }

        for(int j=0; j < faceEdges.size() - 1; j++) {
            faceEdges.at(j)->next = faceEdges.at(j+1);
        }
        faceEdges.last()->next = faceEdges.first();
    }
    //associate each halfedge with its pair (belonging to adjacent face)
    this->pairHalfEdges(&(this->halfEdges));
    //associate each pair of halfedges to a physical edge
    this->createEdges(&(this->halfEdges), &(this->edges));
}


//go through all the halfedges and find matching pairs
void VoronoiCell::pairHalfEdges(QList<VoronoiHalfEdge *> *l)
{
    VoronoiHalfEdge * he1;
    VoronoiHalfEdge * he2;

    //std::cout << "pairing..." << std::endl;
    for (int i = 0; i < l->size(); i++) {
        he1 = l->at(i);
        if (he1->pair == 0) {
            for (int j = 0; j < l->size(); j++) {
                if( i != j ) {
                    he2 = l->at(j);
                    if(he2->pair == 0 && he1->v == he2->next->v && he2->v == he1->next->v) {
                        he1->pair = he2;
                        he2->pair = he1;
                        //std::cout << "paired" << std::endl;

                        break;
                    }
                }
            }
        }
        //if (he1->pair == 0) {
        //    std::cout << "failed to pair" << std::endl;
        //}
    }
}

//associate each pair of halfedges to a physical edge.
void VoronoiCell::createEdges(QList<VoronoiHalfEdge *> *hel, QList<VoronoiEdge *> *el)
{
    VoronoiHalfEdge * he1;
    VoronoiHalfEdge * he2;
    VoronoiEdge * e;

    for (int i = 0; i < hel->size(); i++) {
        he1 = hel->at(i);
        if (he1->edge != 0) continue;
        for(int j = 0; j < hel->size(); j++) {
            if(i != j) {
                he2 = hel->at(j);
                if (he1->pair == he2) {
                    e = new VoronoiEdge();
                    e->halfEdge = he1;
                    el->push_back(e);
                    he1->edge = e;
                    he2->edge = e;
                    break;
                }
            }
        }
    }
}

// check all edges for intersection with a plane
QList<VoronoiSplitEdge *> * VoronoiCell::retrieveSplitEdges(VoronoiPlane *p)
{
    VoronoiEdge * e;
    VoronoiVertex * v;
    QList<VoronoiSplitEdge *> * splitEdges = new QList<VoronoiSplitEdge *>();
    for(int i=0; i < this->edges.size(); i++) {
        e = this->edges.at(i);
        v = e->planeEdgeIntersection(p);
        if (v != 0) {
            splitEdges->push_back(new VoronoiSplitEdge(e, v));
        }
    }

    return splitEdges;
}

int VoronoiCell::isCut(VoronoiPlane * p)
{
    VoronoiVertex * v;
    bool front = false;
    bool back = false;
    int side;

    for(int i = 0; i < this->vertices.size(); i++) {
        v = this->vertices.at(i);
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
    }

    if (front && !back) {
        return 1;
    }
    if (back && !front) {
        return -1;
    }

    // there was some error
    throw 2;
}

VoronoiVertex * VoronoiCell::getNewVertex(VoronoiVertex * v, QList<VoronoiVertex *> * newVertices, QList<VoronoiVertex *> * newFaceVertices)
{
    VoronoiVertex * vertexHandler;

    for (int i = 0; i < newVertices->size(); i++) {
        vertexHandler = newVertices->at(i);
        if (vertexHandler->equals(v)) {
            return vertexHandler;
        }
    }

    vertexHandler = v->getCopy();
    newVertices->push_back(vertexHandler);

    if (newFaceVertices != NULL) {
        newFaceVertices->push_back(vertexHandler);
    }

    return vertexHandler;
}

VoronoiSplitEdge * VoronoiCell::getSplitEdge(VoronoiHalfEdge * halfEdge, QList<VoronoiSplitEdge *> * splitEdges)
{
    for(int i = 0; i < splitEdges->size(); i++) {
        if (splitEdges->at(i)->edge == halfEdge->edge) {
            return splitEdges->at(i);
        }
    }

    return NULL;
}

VoronoiHalfEdge * VoronoiCell::createNewHalfEdge(VoronoiVertex * baseVertex, VoronoiHalfEdge * previousHalfEdge, VoronoiFace * face, QList<VoronoiVertex *> * newVertices, QList<VoronoiHalfEdge *> * newHalfEdges, QList<VoronoiVertex *> * newFaceVertices)
{
    VoronoiVertex * newVertex;
    VoronoiHalfEdge * newHalfEdge;

    newVertex = this->getNewVertex(baseVertex, newVertices, newFaceVertices);
    newHalfEdge = new VoronoiHalfEdge();
    newHalfEdge->v = newVertex;
    newHalfEdge->face = face;

    if (face->halfEdge == NULL) {
        face->halfEdge = newHalfEdge;
    }
    if (previousHalfEdge != NULL) {
        previousHalfEdge->next = newHalfEdge;
    }

    newHalfEdges->push_back(newHalfEdge);

    return newHalfEdge;
}

void VoronoiCell::splitFace(VoronoiFace * f, VoronoiPlane * p, QList<VoronoiVertex *> * newVertices, QList<VoronoiHalfEdge *> * newHalfEdges, QList<VoronoiFace *> * newFaces, QList<VoronoiSplitEdge *> * splitEdges, QList<VoronoiVertex *> * newFaceVertices)
{
    int cut;
    cut = f->isCut(p);
    if (cut == -1) return;

    VoronoiFace * newFace;
    newFace = new VoronoiFace();

    VoronoiHalfEdge * halfEdgeHandler;
    VoronoiHalfEdge * newHalfEdge = NULL;
    VoronoiHalfEdge * lastCreatedEdge = NULL;

    halfEdgeHandler = f->halfEdge;

    if (cut == 1) {
        // copy whole face
        std::cout << "copying face " << newFace << std::endl;
        if (f->halfEdge == NULL) { //broken face?
            return;
        }

        do {
            newHalfEdge = this->createNewHalfEdge(halfEdgeHandler->v, lastCreatedEdge, newFace, newVertices, newHalfEdges, NULL);

            lastCreatedEdge = newHalfEdge;
            halfEdgeHandler = halfEdgeHandler->next;
            if (newFace->halfEdge == NULL && newHalfEdge) {
                newFace->halfEdge = newHalfEdge;
            }
        } while (halfEdgeHandler != f->halfEdge);
    } else {
        // proceed with cut
        std::cout << "cutting face " << newFace << std::endl;
        int side;
        bool inFront;
        VoronoiSplitEdge * splitEdge;

        side = p->side(halfEdgeHandler->v);
        if (side == 1) {
            inFront = true;
            std::cout << "starting in front" << std::endl;
        } else {
            inFront = false;
            std::cout << "starting in back" << std::endl;
        }

        do {
            splitEdge = this->getSplitEdge(halfEdgeHandler, splitEdges);

            if (inFront) {
                newHalfEdge = this->createNewHalfEdge(halfEdgeHandler->v, lastCreatedEdge, newFace, newVertices, newHalfEdges, NULL);

                lastCreatedEdge = newHalfEdge;
                if (newFace->halfEdge == NULL && newHalfEdge != NULL) {
                    std::cout << "assigned first edge" << std::endl;
                    newFace->halfEdge = newHalfEdge;
                }
                std::cout << "copied edge" << std::endl;
            } else {
                std::cout << "skipping edge" << std::endl;
            }

            if (splitEdge != NULL) {
                newHalfEdge = this->createNewHalfEdge(splitEdge->splitVertex, lastCreatedEdge, newFace, newVertices, newHalfEdges, newFaceVertices);

                lastCreatedEdge = newHalfEdge;

                std::cout << "created edge, reversed" << std::endl;

                inFront = !inFront;
            }

            halfEdgeHandler = halfEdgeHandler->next;
            if (newFace->halfEdge == NULL && newHalfEdge != NULL) {
                std::cout << "assigned first edge" << std::endl;
                newFace->halfEdge = newHalfEdge;
            }
        } while (halfEdgeHandler != f->halfEdge);
    }
    lastCreatedEdge->next = newFace->halfEdge;

    newFaces->push_back(newFace);
}

void VoronoiCell::buildCutFace(QList<VoronoiHalfEdge *> * newHalfEdges, QList<VoronoiFace *> * newFaces, QList<VoronoiVertex *> * newFaceVertices, VoronoiPlane * plane, QList<VoronoiEdge *> * newEdges)
{
    int size = newHalfEdges->size();
    VoronoiHalfEdge * newHalfEdge;
    VoronoiEdge * newEdge;
    VoronoiVertex * vertexHandler;
    VoronoiHalfEdge * halfEdgeHandler;
    QList<VoronoiHalfEdge *> newFaceHalfEdges;

    VoronoiFace * newFace = new VoronoiFace();
    newFace->plane = plane;

    for (int i = 0; i < newFaceVertices->size(); i++) {
        vertexHandler = newFaceVertices->at(i);

        newEdge = new VoronoiEdge();

        newHalfEdge = new VoronoiHalfEdge();
        newHalfEdge->face = newFace;
        newHalfEdge->v = vertexHandler;
        newHalfEdge->edge = newEdge;
        newEdge->halfEdge = newHalfEdge;

        //find pair for new edge
        for (int j = 0; j < size; j++) {
            halfEdgeHandler = newHalfEdges->at(j);

            //std::cout << "comparing: " << newHalfEdge->v->coords[0] << "-" << newHalfEdge->v->coords[1] << "-" << newHalfEdge->v->coords[2] << " " << halfEdgeHandler->next->v->coords[0] << "-" << halfEdgeHandler->next->v->coords[1] << "-" << halfEdgeHandler->next->v->coords[2] << "  " << halfEdgeHandler->next->v << " " << newHalfEdge->v << std::endl;

//            if (halfEdgeHandler->next->v == vertexHandler) {
//                std::cout << j << " next could be good" << std::endl;
//            }

            if (!newFaceVertices->contains(halfEdgeHandler->v)) {
                continue;
            }

            if (halfEdgeHandler->next == NULL) {
                std::cout << "fail?" << std::endl;
                continue;
            }
            //std::cout << "still comparing" << std::endl;
//            if (halfEdgeHandler->v == newHalfEdge->v) {
//                std::cout << j << " ok" << std::endl;
//                if (newFaceVertices->contains(halfEdgeHandler->next->v)) std::cout << j << " rev" << std::endl;
//            }

//            if (halfEdgeHandler->v == halfEdgeHandler->next->v) std::cout << "failed?" << std::endl;


            if (halfEdgeHandler->next->v == vertexHandler) {
                halfEdgeHandler->pair = newHalfEdge;
                halfEdgeHandler->edge = newEdge;
                newHalfEdge->pair = halfEdgeHandler;
                std::cout << "found pair for new edge! " << i << "/" << newFaceVertices->size() << std::endl;
                break;
            }
        }
//        if (newHalfEdge->pair == NULL) {
//            std::cout << "could not find pair for new edge! " << i << "/" << newFaceVertices->size() << std::endl;
//        }

        newEdges->push_back(newEdge);
        newHalfEdges->push_back(newHalfEdge);
        newFaceHalfEdges.push_back(newHalfEdge);

        if (newFace->halfEdge == NULL) {
            newFace->halfEdge = newHalfEdge;
        }
    }

    //link new half edges into chain loop
    for (int i = 0; i < newFaceHalfEdges.size(); i++) {
        halfEdgeHandler = newFaceHalfEdges.at(i);

        halfEdgeHandler = halfEdgeHandler->pair; //1
        halfEdgeHandler = halfEdgeHandler->next; //2
        halfEdgeHandler = halfEdgeHandler->pair; //3
        halfEdgeHandler = halfEdgeHandler->next; //4
        halfEdgeHandler = halfEdgeHandler->pair; //5
        halfEdgeHandler->next = newFaceHalfEdges.at(i);
    }
    newFaces->push_back(newFace);
}

void VoronoiCell::persistElements(QList<VoronoiVertex *> * newVertices, QList<VoronoiHalfEdge *> * newHalfEdges, QList<VoronoiEdge *> * newEdges, QList<VoronoiFace *> * newFaces)
{
    this->flushCell();
    this->center = new VoronoiVertex();

    //re-index all new datastructures
    for(int i = 0 ; i < newVertices->size(); i++) {
        newVertices->at(i)->id = i;
    }
    for(int i = 0; i < newFaces->size(); i++) {
        newFaces->at(i)->id = i;
    }
    for(int i = 0; i < newHalfEdges->size(); i++) {
        newHalfEdges->at(i)->id = i;
    }
    for(int i = 0; i < newEdges->size(); i++) {
        newEdges->at(i)->id = i;
    }

    while (!newVertices->empty()) {
        this->vertices.push_back(newVertices->front());
        newVertices->pop_front();
    }
    while (!newFaces->empty()) {
        this->faces.push_back(newFaces->front());
        newFaces->pop_front();
    }
    while (!newHalfEdges->empty()) {
        this->halfEdges.push_back(newHalfEdges->front());
        newHalfEdges->pop_front();
    }
    while (!newEdges->empty()) {
        this->edges.push_back(newEdges->front());
        newEdges->pop_front();
    }
}

void VoronoiCell::splitMesh(VoronoiPlane * p)
{
    int cut;
    QList<VoronoiVertex *> newVertices;
    QList<VoronoiHalfEdge *> newHalfEdges;
    QList<VoronoiEdge *> newEdges;
    QList<VoronoiFace *> newFaces;
    QList<VoronoiVertex *> newFaceVertices;
    QList<VoronoiSplitEdge *> * splitEdges;

    // check if the plane cuts the mesh at all
    cut = this->isCut(p);
    if (cut == 1) return;
    if (cut == -1) {
        this->flushCell();
        return;
    }

    splitEdges = this->retrieveSplitEdges(p);

    // cut each face separately
    for(int i = 0; i < this->faces.size(); i++) {
        this->splitFace(this->faces.at(i), p, &newVertices, &newHalfEdges, &newFaces, splitEdges, &newFaceVertices);
    }

    this->pairHalfEdges(&newHalfEdges);
    this->createEdges(&newHalfEdges, &newEdges);

    this->buildCutFace(&newHalfEdges, &newFaces, &newFaceVertices, p, &newEdges);

    this->persistElements(&newVertices, &newHalfEdges, &newEdges, &newFaces);

    //cleanup
    VoronoiSplitEdge * se;
    while (!splitEdges->empty()) {
        se = splitEdges->front();
        delete se;
        splitEdges->pop_front();
    }
    delete splitEdges;
    this->splitEdges = splitEdges;
}


/*

// Split the mesh in half, retain the part on the same side as the point "center".
// Works only on a convex mesh !!!
void VoronoiCell::splitMesh(VoronoiPlane * p)
{
    float centerside = p->side(this->center);
    bool cut = false;
    VoronoiVertex * v;
    VoronoiHalfEdge * he;
    VoronoiHalfEdge * he2;
    VoronoiHalfEdge * he3;
    VoronoiEdge * e;
    VoronoiFace * f;
    VoronoiSplitEdge * se;
    QList<VoronoiVertex *> newVertices;
    QList<VoronoiVertex *> newFaceVertices1; // vertices on the correct side.
    QList<VoronoiVertex *> newFaceVertices2; // vertices on the wrong side, not used right now.
    QList<VoronoiFace *> newFaces;
    QList<VoronoiHalfEdge *> newHalfEdges;
    QList<VoronoiHalfEdge *> unpairedHalfEdges;
    QList<VoronoiEdge *> newEdges;
    QList<VoronoiHalfEdge *> faceHalfEdges;
    QList<VoronoiSplitEdge *> * splitEdges;

    //check if the plane cuts the mesh at all, at least one point should be on the other side of the plane.
    //compared to the first point
    int * sides = new int[this->vertices.size()];
    for(int i = 0; i < this->vertices.size(); i++) {
        v = this->vertices.at(i);
        sides[i] = p->side(v);
        if (sides[0] * sides[i] <= 0.0) {
            cut = true;
        }
    }
    if (!cut) {
        return;
    }

    // get all split edges
    splitEdges = this->retrieveSplitEdges(p);

    std::cout << splitEdges->size() << std::endl;

    for(int i = 0; i < this->faces.size(); i++) {
        f = this->faces.at(i);
        he = f->halfEdge;

        //for each face, loop through all vertices and retain the vertices on the correct side. If the edge
        //is cut, insert the new point in the appropriate place.
        do {
            if(sides[he->v->id] >= 0.0) {
                newFaceVertices1.push_back(he->v);
            }
            if(sides[he->v->id] < 0.0) {
                newFaceVertices2.push_back(he->v);
            }
            // loop through all split edges to check for the current edge.
            for(int j = 0; j < splitEdges->size(); j++) {
                se = splitEdges->at(j);
                if (he->edge == se->edge) {
                    newFaceVertices1.push_back(se->splitVertex);
                    break;
                }
            }
            he = he->next;
        } while (he != f->halfEdge);
    }

    return;

    // ignore code below, it will be removed

    if (centerside != 0) { // if center is on the plan, we can't decide which part to keep, ignore.
        // get all split edges
        splitEdges = this->retrieveSplitEdges(p);

        std::cout << splitEdges->size() << std::endl;
        return;



        //plane cuts the mesh
        if (cut) {
            //loop through all faces.
            for(int i = 0; i < this->faces.size(); i++) {
                f = this->faces.at(i);
                he = f->halfEdge;

                //for each face, loop through all vertices and retain the vertices on the correct side. If the edge
                //is cut, insert the new point in the appropriate place.
                do {
                    if(sides[he->v->id] * centerside >= 0.0) {
                        newFaceVertices1.push_back(he->v);
                    }
                    if(sides[he->v->id] * centerside <= 0.0) {
                        newFaceVertices2.push_back(he->v);
                    }
                    // loop through all split edges to check for the current edge.
                    for(int j = 0; j < splitEdges->size(); j++) {
                        se = splitEdges->at(j);
                        if (he->edge == se->edge) {
                            newFaceVertices1.push_back(se->splitVertex);
                            newFaceVertices2.push_back(se->splitVertex);
                            break;
                        }
                    }
                    he = he->next;
                } while (he != f->halfEdge);

                //Create a new face form the vertices we retained,ignore degenerate faces with less than 3 vertices.
                //Add all face-related information to the data-structure.
                if (newFaceVertices1.size() > 2) {
                    f = new VoronoiFace();
                    newFaces.push_back(f);
                    for (int j = 0; j < newFaceVertices1.size(); j++) {
                        v = newFaceVertices1.at(j);
                        if(newVertices.indexOf(v) == -1) newVertices.push_back(v);
                        he = new VoronoiHalfEdge();
                        faceHalfEdges.push_back(he);
                        he->v = v;
                        if (v->halfEdge == 0) v->halfEdge = he;
                        he->face = f;
                        if(f->halfEdge == 0) f->halfEdge = he;
                    }
                    for(int j = 0; j < faceHalfEdges.size() - 1; j++) {
                        he = faceHalfEdges.at(j);
                        he->next = faceHalfEdges.at(j + 1);
                    }
                    he = faceHalfEdges.last();
                    he->next = faceHalfEdges.first();
                    while (!faceHalfEdges.empty()) {
                        newHalfEdges.push_back(faceHalfEdges.front());
                        faceHalfEdges.pop_front();
                    }
                }

                while (!newFaceVertices1.empty()) newFaceVertices1.pop_back();
                while (!newFaceVertices2.empty()) newFaceVertices2.pop_back();
            }

            //Add missing information to the datastructure
            this->pairHalfEdges(&newHalfEdges);
            this->createEdges(&newHalfEdges, &newEdges);

            //Cutting the mesh not only cuts the faces, it also creates one new planar face looping through all new cutpoints(in a convex mesh).
            //This hole in the mesh is identified by unpaired halfedges remaining after the pairibg operation.
            //This part needs to rethought to extend to concave meshes!!!
            for(int i = 0; i < newHalfEdges.size(); i++) {
                he = newHalfEdges.at(i);
                if (he->pair == 0) unpairedHalfEdges.push_back(he);
            }
            if (unpairedHalfEdges.size() > 0) {
                //Create a closed loop out of the collection of unpaired halfedges and associate a new face with this.
                //Easy to explain with a drawing, beyond my skill with words.
                f = new VoronoiFace();
                he = unpairedHalfEdges.front();
                he2 = he;
                while (!faceHalfEdges.empty()) faceHalfEdges.pop_back();

                do{
                    he2 = he->next->pair->next;
                    while (unpairedHalfEdges.indexOf(he2) == -1) he2 = he2->pair->next;
                    he3 = new VoronoiHalfEdge();
                    faceHalfEdges.push_back(he3);
                    if (f->halfEdge == 0) f->halfEdge = he3;
                    he3->v = he2->v;
                    he3->pair = he;
                    he->pair = he3;
                    e = new VoronoiEdge;
                    e->halfEdge = he3;
                    he->edge = e;
                    he3->edge = e;
                    newEdges.push_back(e);
                    he3->face = f;
                    he = he2;
                } while (he2 != unpairedHalfEdges.front());

                for(int j = 1; j < faceHalfEdges.size(); j++) {
                    he = faceHalfEdges.at(j);
                    he->next = faceHalfEdges.at(j - 1);
                }
                he = faceHalfEdges.front();
                he->next = faceHalfEdges.last();
                while (!faceHalfEdges.empty()) {
                    newHalfEdges.push_back(faceHalfEdges.front());
                    faceHalfEdges.pop_front();
                }
                newFaces.push_back(f);
            }

            //re-index all new datastructures
            for(int i = 0 ; i < newVertices.size(); i++) {
                newVertices.at(i)->id = i;
            }
            for(int i = 0; i < newFaces.size(); i++) {
                newFaces.at(i)->id = i;
            }
            for(int i = 0; i < newHalfEdges.size(); i++) {
                newHalfEdges.at(i)->id = i;
            }
            for(int i = 0; i < newEdges.size(); i++) {
                newEdges.at(i)->id = i;
            }

            // clear old data
            while (!this->vertices.empty()) this->vertices.pop_back();
            while (!this->halfEdges.empty()) {
                he = this->halfEdges.front();
                delete he;
                this->halfEdges.pop_front();
            }
            while (!this->edges.empty()) {
                e = this->edges.front();
                delete e;
                this->edges.pop_front();
            }
            while (!this->faces.empty()) {
                f = this->faces.front();
                delete f;
                this->faces.pop_front();
            }
            // update mesh
            while (!newVertices.empty()) {
                this->vertices.push_back(newVertices.front());
                newVertices.pop_front();
            }
            while (!newFaces.empty()) {
                this->faces.push_back(newFaces.front());
                newFaces.pop_front();
            }
            while (!newHalfEdges.empty()) {
                this->halfEdges.push_back(newHalfEdges.front());
                newHalfEdges.pop_front();
            }
            while (!newEdges.empty()) {
                this->edges.push_back(newEdges.front());
                newEdges.pop_front();
            }
        } else {
            // if the plane doesn't cut the mesh, keep the mesh if on the same side as "center", otherwise discard everything.
            if(sides[0] * centerside < 0.0) {
                while (!this->vertices.empty()) {
                    v = this->vertices.front();
                    delete v;
                    this->vertices.pop_back();
                }
                while (!this->halfEdges.empty()) {
                    he = this->halfEdges.front();
                    delete he;
                    this->halfEdges.pop_front();
                }
                while (!this->edges.empty()) {
                    e = this->edges.front();
                    delete e;
                    this->edges.pop_front();
                }
                while (!this->faces.empty()) {
                    f = this->faces.front();
                    delete f;
                    this->faces.pop_front();
                }
            }
        }

        while (!splitEdges->empty()) {
            se = splitEdges->front();
            delete se;
            splitEdges->pop_front();
        }
        delete splitEdges;
    }
}

*/















/*
void VoronoiCell::pushVertex(float * v)
{
    this->vertices.push_back(v);

    this->recalculateCenter();

    if (this->vertices.size() == 4) {
        this->buildTetra();
    }
    if (this->vertices.size() > 4) {
        this->addVertex(this->vertices.size() - 1);
    }
}

void VoronoiCell::buildTetra()
{
    VoronoiFace * face;
    bool found;

    // create faces
    for (int i = 0; i < 4; i++) {
        face = new VoronoiFace();
        face->fill(
            this->vertices.at(i),
            this->vertices.at((i+1)%4),
            this->vertices.at((i+2)%4)
        );
        face->calibrate(this->center);
        this->facets.push_back(face);
    }

    // connect edges with neighbours
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 3; j++) {
            if (this->facets.at(i)->edges[j].adjacentEdge != NULL) continue;
            for (int k = 0; k < 4; k++) {
                if (k == i) continue;
                found = false;
                for (int l = 0; l < 3; l++) {
                    if (this->facets.at(i)->edges[j] == this->facets.at(k)->edges[l]) {
                        this->facets.at(i)->edges[j].adjacentEdge = &(this->facets.at(k)->edges[l]);
                        this->facets.at(k)->edges[l].adjacentEdge = &(this->facets.at(i)->edges[j]);
                        found = true;
                        break;
                    }
                }
                if (found) break;
            }
        }
    }
}

void VoronoiCell::addVertex(int index)
{
    QList<int> facing;
    VoronoiFace * face;
    bool found;
    int newFacesStartingIndex = this->facets.size();

    for (int i = 0; i < this->facets.size(); i++) {
        if (this->facets.at(i)->facing(this->vertices.at(index))) {
            facing.push_back(i);
        }
    }

    for (int i = 0; i < facing.size(); i++) {
        this->facets.at(facing.at(i))->removed = true;
        for (int j = 0; j < 3; j++) {
            if (!this->facets.at(facing.at(i))->edges[j].adjacentEdge->parent->facing(this->vertices.at(index))) {
                face = new VoronoiFace();
                face->fill(this->facets.at(facing.at(i))->edges[j].v1, this->facets.at(facing.at(i))->edges[j].v2,
                           this->vertices.at(index));
                face->calibrate(this->center);
                this->facets.at(facing.at(i))->edges[j].adjacentEdge = &(face->edges[0]);
                face->edges[0].adjacentEdge = &(this->facets.at(facing.at(i))->edges[j]);
            }
        }
    }

    for (int i = newFacesStartingIndex; i < this->facets.size(); i++) {
        for (int j = 0; j < 3; j++) {
            if (this->facets.at(i)->edges[j].adjacentEdge != NULL) continue;
            for (int k = newFacesStartingIndex; k < this->facets.size(); k++) {
                if (k == i) continue;
                found = false;
                for (int l = 0; l < 3; l++) {
                    if (this->facets.at(i)->edges[j] == this->facets.at(k)->edges[l]) {
                        this->facets.at(i)->edges[j].adjacentEdge = &(this->facets.at(k)->edges[l]);
                        this->facets.at(k)->edges[l].adjacentEdge = &(this->facets.at(i)->edges[j]);
                        found = true;
                        break;
                    }
                }
                if (found) break;
            }
        }
    }
}

void VoronoiCell::recalculateCenter()
{
    float sum[3];
    sum[0] = sum[1] = sum[2] = 0.0;

    for (int i = 0; i < this->vertices.size(); i++) {
        for (int j = 0; j < 3; j++) {
            sum[j] = sum[j] + this->vertices.at(i)[j];
        }
    }

    for (int i = 0; i < 3; i++) {
        this->center[i] = sum[i] / this->vertices.size();
    }
}
*/
