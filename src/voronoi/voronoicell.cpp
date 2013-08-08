#include "voronoicell.h"

VoronoiCell::VoronoiCell()
{

}

/**
 * Destructor, center is not deleted on purpose
 *
 * @brief VoronoiCell::~VoronoiCell
 */
VoronoiCell::~VoronoiCell()
{
    this->flushCell();
    this->center = 0;
}

/**
 * Method clearing all data structures allowing to generate different cell configuration
 *
 * @brief VoronoiCell::flushCell
 */
void VoronoiCell::flushCell()
{
    VoronoiVertex * v;
    VoronoiHalfEdge * he;
    VoronoiEdge * e;
    VoronoiFace * f;

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

/**
 * Build initial cell mesh based on SolidObject data structures
 *
 * @brief VoronoiCell::buildMesh
 * @param verts list of float[3] vertices
 * @param facets list of facets
 */
void VoronoiCell::buildMesh(QList<float *> *verts, QList<QList<int> *> *facets)
{
    VoronoiVertex * v;
    VoronoiHalfEdge * he;
    VoronoiFace * f;
    QList<VoronoiHalfEdge *> faceEdges;

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


/**
 * Method for finding matching pairs of directed half edges into whole edges
 *
 * @brief VoronoiCell::pairHalfEdges
 * @param l list of half-edges to pair
 */
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
/**
 * Method for creating whole edges objects from half-edges
 * Needs half-edges to be paired
 *
 * @brief VoronoiCell::createEdges
 * @param hel list of half-edges
 * @param el list of edges, new edges will be appended to this list
 */
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
/**
 * Search current list of edges for intersections with plane and
 * calculate intersection points
 *
 * @brief VoronoiCell::retrieveSplitEdges
 * @param p plane object
 * @return list of split-edge objects with intersection points
 */
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

/**
 * Verify if plane is intersecting with current mesh
 *  1 - whole mesh in front of plane
 *  0 - plane cutting mesh
 * -1 - whole mesh behind plane
 *
 * @brief VoronoiCell::isCut
 * @param p plane object
 * @return intersection status
 */
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

/**
 * Find instance of given vertex in list or create new one and add it
 *
 * @brief VoronoiCell::getNewVertex
 * @param v vertex object
 * @param newVertices list of newly created vertices
 * @param newFaceVertices list of newly created vertices for new cut face
 * @return vertex object
 */
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

/**
 * Search split-edge for given half-edge
 *
 * @brief VoronoiCell::getSplitEdge
 * @param halfEdge half-edge object
 * @param splitEdges list of calculated split-edges
 * @return split-edge for half-edge or NULL if not found
 */
VoronoiSplitEdge * VoronoiCell::getSplitEdge(VoronoiHalfEdge * halfEdge, QList<VoronoiSplitEdge *> * splitEdges)
{
    for(int i = 0; i < splitEdges->size(); i++) {
        if (splitEdges->at(i)->edge == halfEdge->edge) {
            return splitEdges->at(i);
        }
    }

    return NULL;
}

/**
 * Create new half-edge for face
 *
 * @brief VoronoiCell::createNewHalfEdge
 * @param baseVertex vertex
 * @param previousHalfEdge previous half-edge in chain
 * @param face face for which half-edge is added
 * @param newVertices list of new vertices
 * @param newHalfEdges list of new half-edges
 * @param newFaceVertices list of new cut-face vertices
 * @return new half-edge object
 */
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

/**
 * Analyze face and create new one split by plane
 *
 * @brief VoronoiCell::splitFace
 * @param f old face
 * @param p plane
 * @param newVertices list of new vertices
 * @param newHalfEdges list of new half-edges
 * @param newFaces list of new faces
 * @param splitEdges list of calculated split-edges
 * @param newFaceVertices list of new cut-face vertices
 */
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
        //std::cout << "copying face " << newFace << std::endl;
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
        //std::cout << "cutting face " << newFace << std::endl;
        int side;
        bool inFront;
        VoronoiSplitEdge * splitEdge;

        side = p->side(halfEdgeHandler->v);
        if (side == 1) {
            inFront = true;
            //std::cout << "starting in front" << std::endl;
        } else {
            inFront = false;
            //std::cout << "starting in back" << std::endl;
        }

        do {
            splitEdge = this->getSplitEdge(halfEdgeHandler, splitEdges);

            if (inFront) {
                newHalfEdge = this->createNewHalfEdge(halfEdgeHandler->v, lastCreatedEdge, newFace, newVertices, newHalfEdges, NULL);

                lastCreatedEdge = newHalfEdge;
                if (newFace->halfEdge == NULL && newHalfEdge != NULL) {
                    //std::cout << "assigned first edge" << std::endl;
                    newFace->halfEdge = newHalfEdge;
                }
                //std::cout << "copied edge" << std::endl;
            } //else {
                //std::cout << "skipping edge" << std::endl;
            //}

            if (splitEdge != NULL) {
                newHalfEdge = this->createNewHalfEdge(splitEdge->splitVertex, lastCreatedEdge, newFace, newVertices, newHalfEdges, newFaceVertices);

                lastCreatedEdge = newHalfEdge;

                //std::cout << "created edge, reversed" << std::endl;

                inFront = !inFront;
            }

            halfEdgeHandler = halfEdgeHandler->next;
            if (newFace->halfEdge == NULL && newHalfEdge != NULL) {
                //std::cout << "assigned first edge" << std::endl;
                newFace->halfEdge = newHalfEdge;
            }
        } while (halfEdgeHandler != f->halfEdge);
    }
    lastCreatedEdge->next = newFace->halfEdge;

    newFace->plane = f->plane;
    f->plane = 0;

    newFaces->push_back(newFace);
}

/**
 * Build new cut-face on plane
 *
 * @brief VoronoiCell::buildCutFace
 * @param newHalfEdges list of new half-edges
 * @param newFaces list of new faces
 * @param newFaceVertices list of new cut-face vertices
 * @param plane cut plane
 * @param newEdges list of new edges
 */
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
                //std::cout << "fail?" << std::endl;
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
                //std::cout << "found pair for new edge! " << i << "/" << newFaceVertices->size() << std::endl;
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

/**
 * Replace current cell structure with provided one
 *
 * @brief VoronoiCell::persistElements
 * @param newVertices list of new vertices
 * @param newHalfEdges list of new half-edges
 * @param newEdges list of new edges
 * @param newFaces list of new faces
 */
void VoronoiCell::persistElements(QList<VoronoiVertex *> * newVertices, QList<VoronoiHalfEdge *> * newHalfEdges, QList<VoronoiEdge *> * newEdges, QList<VoronoiFace *> * newFaces)
{
    this->flushCell();

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

/**
 * Split mesh using plane
 *
 * @brief VoronoiCell::splitMesh
 * @param p plane object
 */
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
}
