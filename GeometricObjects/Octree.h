#pragma once


#include<glm/glm.hpp>
#include <vector>
#include <memory>
#include <array>
#include <algorithm>
#include"FlattenedNode.h"
#include"FlattenedMultiNode.h"
#include"Point.h"
#include <iostream>

using namespace glm;

#include <map>
#include <queue>
class OctreeNode {
public:
    OctreeNode(const vec3& minBound, const vec3& maxBound, unsigned int depth, const std::vector<Point>& pts)
        : minBound(minBound), maxBound(maxBound), depth(depth), points(pts) {
        for (auto& child : children) {
            child = nullptr;
        }
    }

    OctreeNode(){
    }

    ~OctreeNode() {
        for (auto& child : children) {
            delete child;
        }
    }

    bool hasChildren() const {
        for (const auto& child : children) {
            if (child) {
                return true;
            }
        }
        return false;
    }

    bool hasSameLabelType(int type) {

        if (type == 0) {
            this->typeNode = -1;
            return false;
        }

        if (points.empty()) {
            return true;
        }

        if (type == 1) {
            int firstLabelType = static_cast<int>(points[0].labels.x);
            for (const Point& point : points) {
                if (static_cast<int>(point.labels.x) != firstLabelType) {
                    return false;
                }
            }
            this->typeNode = firstLabelType;
        }

        if (type == 2) {
            int firstLabelType = static_cast<int>(points[0].labels.y);
            for (const Point& point : points) {
                if (static_cast<int>(point.labels.y) != firstLabelType) {
			        return false;
				}
			}
            this->typeNode = firstLabelType;
        }
        std::cout << this->typeNode << "min " << this->minBound.x <<" " << this->minBound.y << " " << this->minBound.z << std::endl;
        std::cout << this->depth << "max " << this->maxBound.x <<" "<< this->maxBound.y << " " << this->maxBound.z << std::endl;
        return true;
    }

    std::vector<Point> points;
    vec3 minBound;
    vec3 maxBound;
    unsigned int pointIndex;
    unsigned int pointCount;
    unsigned int depth;
    unsigned int altura;
    unsigned int typeNode;
    bool leaf;
    std::array<OctreeNode*, 8> children;
    
    //bool hasError;
};


struct OctreeData {
    std::vector<Point> points;
    unsigned int pointIndex;
    unsigned int pointCount;
    bool depends;
    bool leaf;
    unsigned int typeNode;
};

class OctreeNodeMulti {
public:
    OctreeNodeMulti(const vec3& minBound, const vec3& maxBound, unsigned int depth)
        : minBound(minBound), maxBound(maxBound), depth(depth) {
        for (auto& child : children) {
            child = nullptr;
        }
    }

    ~OctreeNodeMulti() {
        for (auto& child : children) {
            delete child;
        }
    }

    bool hasChildren() const {
        for (const auto& child : children) {
            if (child) {
                return true;
            }
        }
        return false;
    }

    // A�ade aqu� funciones para manejar los datos del octree como las necesites

    vec3 minBound;
    vec3 maxBound;
    unsigned int depth;
    bool isLeafForAnyOctree;
    unsigned int altura;
    std::array<OctreeNodeMulti*, 8> children;

    // Aqu� es donde almacenamos los datos para cada octree
    std::map<int, OctreeData> octreeDataMap;
};





class Octree {
public:
    Octree(const vec3& minBound, const vec3& maxBound, unsigned int maxDepth, const std::vector<Point>& points, unsigned int maxPointsPerNode, unsigned int type = 0)
        : root(new OctreeNode(minBound, maxBound, 0, points)), maxDepth(maxDepth) {
        root->pointCount = points.size();
        allNodesInOctree(root);
        build(root, this->flattenedPoints, maxPointsPerNode, type);
    }

    std::pair<std::vector<FlattenedNodeFloats>, std::vector<FlattenedNodeInts>> getFlattenedNodes(){
        std::vector<FlattenedNodeFloats> flattenedNodesFloats;
        std::vector<FlattenedNodeInts> flattenedNodesInts;
        flattenOctree(this->root, flattenedNodesFloats, flattenedNodesInts);
        return std::make_pair(flattenedNodesFloats, flattenedNodesInts);
    }

    std::vector<Point> getFlattenedPoints()
    {
        return this->flattenedPoints;
    }

    ~Octree() {
        delete root;
    }

    OctreeNode* root;

    std::vector<OctreeNode*> getLeafNodes() {
        std::vector<OctreeNode*> leafNodes;

        // M�todo recursivo para recorrer el �rbol
        getLeafNodesHelper(this->root, leafNodes);

        return leafNodes;
    }

    void getLeafNodesHelper(OctreeNode* node, std::vector<OctreeNode*>& leafNodes) {
        if (node == nullptr) {
            return;
        }

        if (node->leaf) {
            // Este es un nodo hoja, por lo que lo a�adimos a la lista
            leafNodes.push_back(node);
        }
        else {
            // Este es un nodo interno, por lo que llamamos a la funci�n recursivamente en sus hijos
            for (auto& child : node->children) {
                getLeafNodesHelper(child, leafNodes);
            }
        }
    }

private:

    std::vector<Point> flattenedPoints;

    unsigned int maxDepth;

    int flattenOctree(const OctreeNode* node, std::vector<FlattenedNodeFloats>& flattenedNodesFloats, std::vector<FlattenedNodeInts>& flattenedNodesInts) {
        if (!node) {
            return -1;
        }

        FlattenedNodeFloats flatNodeFloat;
        FlattenedNodeInts flatNodeInt;
        flatNodeFloat.minBound = node->minBound;
        flatNodeFloat.maxBound = node->maxBound;
        flatNodeInt.pointIndex = node->pointIndex;
        flatNodeInt.pointCount = node->pointCount;
        flatNodeInt.depth = node->depth;
        flatNodeInt.extra2 = 0;

        int currentNodeIndex = flattenedNodesInts.size();
        flattenedNodesInts.push_back(flatNodeInt);
        flattenedNodesFloats.push_back(flatNodeFloat);

        for (int i = 0; i < 8; ++i) {
            if (node->children[i]) {
                flatNodeInt.childrenIndices[i] = flattenOctree(node->children[i], flattenedNodesFloats, flattenedNodesInts);
            }
            else {
                flatNodeInt.childrenIndices[i] = -1; // Indica que no hay hijo en esta posici�n
            }
        }

        // Actualizar el nodo aplanado en la lista con los �ndices de los hijos correctos
        flattenedNodesInts[currentNodeIndex] = flatNodeInt;
        flattenedNodesFloats[currentNodeIndex] = flatNodeFloat;
        
        return currentNodeIndex; 
    }

    void allNodesInOctree(OctreeNode* node)
    { 
        for (const auto& point : node->points) {
            if (!inBounds(point.position, node->minBound, node->maxBound)) {
                std::cout << "Nodo fuera del OCTREE, aumentar precision" << point.position.x << point.position.y << point.position.z << std::endl;
            }
        }
    }


    void build(OctreeNode* node, std::vector<Point>& flattenedPoints,unsigned int maxPointsPerNode, int type) {
        
        //Noda fulla.
        if (node->depth >= maxDepth || node->points.size() <= maxPointsPerNode){// || node->hasSameLabelType(type)) {
            node->leaf = true;
            return;
        }
        
        int pointsCount = 0;
        vec3 midPoint = (node->minBound + node->maxBound) * 0.5f;
        int childContainingAllPoints = -1;
        int nChilds = 0;
        for (int i = 0; i < 8; ++i) {
            
            vec3 childMinBound = mix(node->minBound, midPoint, vec3((i & 1) > 0, (i & 2) > 0, (i & 4) > 0));
            vec3 childMaxBound = mix(midPoint, node->maxBound, vec3((i & 1) > 0, (i & 2) > 0, (i & 4) > 0));

            std::vector<Point> childPoints;
            for (const auto& point : node->points) {
                if (inBounds(point.position, childMinBound, childMaxBound)) {
                    childPoints.push_back(point);
                }
            }
            if (!childPoints.empty()) {
                if (childPoints.size() == node->points.size()) {
                    childContainingAllPoints = i;
                    break;
                }
                node->children[i] = new OctreeNode(childMinBound, childMaxBound, node->depth + 1, childPoints);
                //std::cout << "Creating child node at depth: " << (node->depth + 1) << std::endl;
                //std::cout << "Child node points: " << childPoints.size() << std::endl;
                nChilds++;
                // Guarda el �ndice del primer punto en el nodo
                node->children[i]->pointIndex = flattenedPoints.size();

                // Guarda la cantidad de puntos en el nodo
                node->children[i]->pointCount = childPoints.size();

                build(node->children[i], flattenedPoints,maxPointsPerNode, type);
                // Verifica si el nodo hijo creado es un nodo hoja
                if (!node->children[i]->hasChildren()) {
                    std::sort(node->children[i]->points.begin(), node->children[i]->points.end(),
                        [](const Point& a, const Point& b) {
                            return a.labels.z < b.labels.z;
                        });

                    // A�ade los puntos del nodo hoja a flattenedPoints
                    for (const auto& point : node->children[i]->points) {
                        flattenedPoints.push_back(point);
                    }
                }
            }
            
        }
        if (nChilds == 0) {
            node->leaf = true;
        }
        /*if (childContainingAllPoints != -1) {
            return;
        }*/
        
        /*if ((node->points.size()) - pointsCount > 0.000001)
        {
            std::cout << "##################################################################################################" << std::endl;

        }*/
        //std::cout << "Number of Points: " << (node->points.size()) << std::endl;
        //std::cout << "Number of the sum of child points: " << (pointsCount) << std::endl;
        pointsCount = 0;
        
    }

    bool inBounds(const vec3& point, const vec3& minBound, const vec3& maxBound, const float epsilon = 1e-6f) {
        return point.x >= minBound.x - epsilon && point.x <= maxBound.x + epsilon &&
            point.y >= minBound.y - epsilon && point.y <= maxBound.y + epsilon &&
            point.z >= minBound.z - epsilon && point.z <= maxBound.z + epsilon;
    }



};

class MultiOctree {
public:
    MultiOctree(const Octree* octree) {
        // Construir el Octree Multi con base en el Octree existente
        root = new OctreeNodeMulti(octree->root->minBound, octree->root->maxBound, 0);
        constructMultiOctree(root, octree->root);
        this->nOctrees++;
    }

    ~MultiOctree() {
        delete root;
    }

    // Aqu� es donde se realizar� la conversi�n de Octree a MultiOctree
    void constructMultiOctree(OctreeNodeMulti* multiNode, OctreeNode* node) {
        // Aqu� est� c�mo copiar�as la informaci�n de los puntos de OctreeNode a OctreeNodeMulti para el Octree original
        OctreeData data;
        data.points = node->points;
        data.pointIndex = node->pointIndex;
        data.pointCount = node->pointCount;
        data.depends = true;
        data.typeNode = node->typeNode;
        data.leaf = node->leaf;
        if (data.leaf) {
            multiNode->isLeafForAnyOctree = true;
        }

        multiNode->octreeDataMap[0] = data; // Considerando que el ID del primer Octree es 0

        // Luego debes hacer lo mismo para cada uno de los hijos del nodo
        for (int i = 0; i < 8; i++) {
            if (node->children[i] != nullptr) {
                multiNode->children[i] = new OctreeNodeMulti(node->children[i]->minBound, node->children[i]->maxBound, node->depth + 1);
                constructMultiOctree(multiNode->children[i], node->children[i]);
            }
        }
    }

    bool inBounds(const vec3& point, const vec3& minBound, const vec3& maxBound, const float epsilon = 1e-6f) {
        return point.x >= minBound.x - epsilon && point.x <= maxBound.x + epsilon &&
            point.y >= minBound.y - epsilon && point.y <= maxBound.y + epsilon &&
            point.z >= minBound.z - epsilon && point.z <= maxBound.z + epsilon;
    }

    void populateNullNodeData(OctreeNodeMulti* multiNode, int octreeIndex) {
        for (int i = 0; i < 8; i++) {
            if (multiNode->children[i]) {
                populateNullNodeData(multiNode->children[i], octreeIndex);
            }
        }
        if (multiNode) {
            OctreeData data;
            data.depends = false;
            data.typeNode = 0;
            data.points = std::vector<Point>();
            data.pointIndex = -1;
            data.pointCount = 0;
            data.leaf = false;
            multiNode->octreeDataMap[octreeIndex] = data;
        }
    }

    void populateLeafNodeData(OctreeNodeMulti* multiNode, OctreeNode* node, int octreeIndex) {

        //primero comprobamos que ya no sea un nodo hoja el multiNode
        int contador = 0;
        for (int i = 0; i < 8; i++) {
            if (multiNode->children[i] == nullptr) {
				contador++;
			}
        }
        if (contador == 8) {
            return;
        }

        for (int i = 0; i < 8; i++) {

            //Primeramente, buscamos a que quadrante tendrian que ir estos nodos.
            int depthDifference = multiNode->depth - node->depth;
            vec3 childMinBound, childMaxBound;
            if (depthDifference == 0) {
                vec3 midPoint = (multiNode->minBound + multiNode->maxBound) * 0.5f;
                childMinBound = mix(multiNode->minBound, midPoint, vec3((i & 1) > 0, (i & 2) > 0, (i & 4) > 0));
                childMaxBound = mix(midPoint, multiNode->maxBound, vec3((i & 1) > 0, (i & 2) > 0, (i & 4) > 0));
            }
            // Si la diferencia de profundidad es mayor que 0, ajustamos los l�mites para un nivel de profundidad m�s profundo
            else {
                vec3 midPoint = (multiNode->minBound + multiNode->maxBound) * 0.5f;
                for (int j = 0; j < depthDifference; j++) {
                    midPoint = (childMinBound + childMaxBound) * 0.5f;
                    childMinBound = mix(childMinBound, midPoint, vec3((i & 1) > 0, (i & 2) > 0, (i & 4) > 0));
                    childMaxBound = mix(midPoint, childMaxBound, vec3((i & 1) > 0, (i & 2) > 0, (i & 4) > 0));
                }
            }
            std::vector<Point> childPoints;
            for (const auto& point : node->points) {
                if (inBounds(point.position, childMinBound, childMaxBound)) {
                    childPoints.push_back(point);
                }
            }

            //Por otro lado, miramos si existe un hijo en el nodo multi.
            bool existSMultiNodeChild = multiNode->children[i] != nullptr;


            //Ahora hay el caso donde existe un hijo en el noto multi y hay puntos que se pueden asignar a ese hijo.
            if(childPoints.size() != 0 && existSMultiNodeChild){
                OctreeData data;
                data.points = childPoints;
                data.depends = true;
                data.leaf = true;
                data.typeNode = node->typeNode;
                multiNode->children[i]->octreeDataMap[octreeIndex] = data;
				populateLeafNodeData(multiNode->children[i], node, octreeIndex);
			}

            //Caso 2: Existe un hijo en el nodo multi y NO hay puntos que se pueden asignar a ese hijo.
            if (childPoints.size() == 0 && existSMultiNodeChild) {
                populateNullNodeData(multiNode->children[i], octreeIndex);
            }

            //Caso 3: NO existe un hijo en el nodo multi y hay puntos que se pueden asignar a ese hijo.
            if (childPoints.size() != 0 && !existSMultiNodeChild) {
                multiNode->children[i] = new OctreeNodeMulti(childMinBound, childMaxBound, multiNode->depth + 1);

                // Rellenamos los datos de este nodo para los dem�s octrees con datos vac�os
                for (auto it = multiNode->octreeDataMap.begin(); it != multiNode->octreeDataMap.end(); ++it) {
                    const auto& key = it->first;
                    const auto& val = it->second;
                    if (it->first != octreeIndex) {
                        std::vector<Point> containedPoints;

                        OctreeData existingOctreeData;
                        existingOctreeData.points = containedPoints;
                        existingOctreeData.depends = false;
                        existingOctreeData.pointCount = -1;
                        existingOctreeData.pointIndex = 0;
                        existingOctreeData.typeNode = val.typeNode;
                        existingOctreeData.leaf = false;
                        multiNode->children[i]->octreeDataMap[key] = existingOctreeData;
                    }
                }
                OctreeData data;
                data.points = childPoints;
                data.depends = true;
                data.leaf = true;
                data.typeNode = node->typeNode;
                multiNode->children[i]->octreeDataMap[octreeIndex] = data;
			}

            //Caso 4: NO existe un hijo en el nodo multi y NO hay puntos que se pueden asignar a ese hijo.
            if (childPoints.size() == 0 && !existSMultiNodeChild) {
                //NADA
            }
        }
    }



    std::vector<Point> getFlattenedPoints(int n)
    {
        return this->flattenedPointsPerOctree.at(n);
    }

    void mergeOctree(Octree* newOctree) {
        
        this->mergeOctree(this->root, newOctree->root, this->nOctrees);
        this->nOctrees++;
        this->updateFlattenedPointsPerOctree();
    }

    std::pair<std::vector<FlattenedMultiNodeFloats>, std::vector<FlattenedMultiNodeInts>> getFlattenedNodesMultiOctree(int n) {
        std::vector<FlattenedMultiNodeFloats> flattenedNodesFloats;
        std::vector<FlattenedMultiNodeInts> flattenedNodesInts;
        flattenMultiOctree(this->root, flattenedNodesFloats, flattenedNodesInts, n);
        return std::make_pair(flattenedNodesFloats, flattenedNodesInts);
    }

private:
    OctreeNodeMulti* root;
    int nOctrees = 0;
    std::map<int, std::vector<Point>> flattenedPointsPerOctree;

    void mergeOctree(OctreeNodeMulti* multiNode, OctreeNode* node, int octreeIndex) {
        if (node && multiNode) {
            OctreeData data;
            data.points = node->points;
            data.pointIndex = node->pointIndex;
            data.pointCount = node->pointCount;
            data.depends = true;
            data.leaf = node->leaf;
            data.typeNode = node->typeNode;
            multiNode->octreeDataMap[octreeIndex] = data;

            //Si es un leaf y por lo tanto no tiene hijos, entonces.
            if (node->leaf) {

                // No va a tener mas hijos, por lo tanto, tenemos que tratar este nodo como un leaf y crear mas posibles subleaf segun los otros octrees.
                populateLeafNodeData(multiNode, node, octreeIndex);
                
            }
            else {
                for (int i = 0; i < 8; i++) {
                    if (node->children[i]) {
                        if (multiNode->children[i]) {
                            // El hijo existe para el nodo y exista para el multiNodo, entonces seguimos avanzando.
                            mergeOctree(multiNode->children[i], node->children[i], octreeIndex);
                        }
                        else {
                            // El hijo no existe para el multiNodo, creamos uno nuevo con los bounds de este hijo.
                            multiNode->children[i] = new OctreeNodeMulti(node->children[i]->minBound, node->children[i]->maxBound, multiNode->depth + 1);



                            // Rellenar la informaci�n de los otros octrees en el multiNode
                            for (auto it = multiNode->octreeDataMap.begin(); it != multiNode->octreeDataMap.end(); ++it) {
                                const auto& key = it->first;
                                const auto& val = it->second;

                                if (key != octreeIndex) {
                                    std::vector<Point> containedPoints;
                                    for (const auto& point : val.points) {
                                        if (inBounds(point.position, node->children[i]->minBound, node->children[i]->maxBound)) {
                                            containedPoints.push_back(point);
                                        }
                                    }

                                    if (!containedPoints.empty()) {
                                        OctreeData existingOctreeData;
                                        existingOctreeData.points = containedPoints;
                                        existingOctreeData.depends = true;
                                        existingOctreeData.pointCount = val.pointCount;
                                        existingOctreeData.pointIndex = val.pointIndex;
                                        existingOctreeData.typeNode = val.typeNode;
                                        existingOctreeData.leaf = true;
                                        multiNode->children[i]->octreeDataMap[key] = existingOctreeData;
                                    }
                                    else {
                                        OctreeData existingOctreeData;
                                        existingOctreeData.points = containedPoints;
                                        existingOctreeData.depends = false;
                                        existingOctreeData.pointCount = -1;
                                        existingOctreeData.pointIndex = 0;
                                        existingOctreeData.typeNode = val.typeNode;
                                        existingOctreeData.leaf = false;
                                        multiNode->children[i]->octreeDataMap[key] = existingOctreeData;
                                    }
                                }
                            }
                            OctreeData data;
                            data.points = node->children[i]->points;
                            data.pointIndex = node->children[i]->pointIndex;
                            data.pointCount = node->children[i]->pointCount;
                            data.depends = true;
                            data.leaf = node->children[i]->leaf;
                            data.typeNode = node->children[i]->typeNode;
                            multiNode->children[i]->octreeDataMap[octreeIndex] = data;


                            mergeOctree(multiNode->children[i], node->children[i], octreeIndex);
                            

                            //FALTA TENER EN CUENTA AL REVES... ES DECIR, QUE EL NODO SEA HOJA PARA TODOS LOS OCTREES.
                        }
                    }
                    else {
                        //Si no existe hijo pero si existe hijo del multi, lo rellenamos de null.
                        if (multiNode->children[i]) {
                            populateNullNodeData(multiNode->children[i], octreeIndex);
                        }
                        
                    }
                }
            }
        }
    }

    void updateFlattenedPointsPerOctreeDFS(OctreeNodeMulti* node, int octreeIndex, unsigned int& currentPointIndex, std::vector<Point>& flattenedPoints) {
        if (node == nullptr) return;

        // Comprobamos si el nodo actual tiene datos para el octreeIndex
        if (node->octreeDataMap.count(octreeIndex)) {
            OctreeData& nodeData = node->octreeDataMap[octreeIndex];

            if(nodeData.points.size() == 0) return;
            // Si el nodo es una hoja y no tiene hijos con nodos hojas para el mismo octreeIndex, agregamos sus puntos
            if (nodeData.leaf && !hasLeafChildren(node, octreeIndex)) {
                flattenedPoints.insert(flattenedPoints.end(), nodeData.points.begin(), nodeData.points.end());
                nodeData.pointIndex = currentPointIndex;
                nodeData.pointCount = nodeData.points.size();
                currentPointIndex += nodeData.points.size();
            }
            // Si el nodo no es una hoja o tiene hijos con nodos hojas para el mismo octreeIndex, recorremos sus hijos
            else {
                for (int i = 0; i < 8; i++) {
                    if (node->children[i]) {
                        updateFlattenedPointsPerOctreeDFS(node->children[i], octreeIndex, currentPointIndex, flattenedPoints);
                    }
                }

                // Actualizamos la informaci�n del nodo bas�ndonos en los hijos
                int sum = 0;
                for (int i = 0; i < 8; i++) {
                    if (node->children[i] && node->children[i]->octreeDataMap.count(octreeIndex)) {
                        sum += node->children[i]->octreeDataMap[octreeIndex].pointCount;
                    }
                }
                nodeData.pointCount = nodeData.points.size();
                nodeData.pointIndex = flattenedPoints.size()-nodeData.pointCount;
            }
        }
        // Si el nodo actual no contiene datos para el octreeIndex, seguimos buscando en sus hijos
        else {
            for (int i = 0; i < 8; i++) {
                updateFlattenedPointsPerOctreeDFS(node->children[i], octreeIndex, currentPointIndex, flattenedPoints);
            }
        }
    }

    bool hasLeafChildren(OctreeNodeMulti* node, int octreeIndex) {
        for (int i = 0; i < 8; i++) {
            if (node->children[i] && node->children[i]->octreeDataMap.count(octreeIndex) && node->children[i]->octreeDataMap[octreeIndex].leaf) {
                return true;
            }
        }
        return false;
    }


    void updateFlattenedPointsPerOctree() {
        flattenedPointsPerOctree.clear();

        for (const auto& entry : root->octreeDataMap) {
            int octreeIndex = entry.first;
            unsigned int currentPointIndex = 0;
            std::vector<Point> flattenedPoints;

            updateFlattenedPointsPerOctreeDFS(root, octreeIndex, currentPointIndex, flattenedPoints);

            flattenedPointsPerOctree[octreeIndex] = flattenedPoints;
        }
    }

    int flattenMultiOctree(const OctreeNodeMulti* node, std::vector<FlattenedMultiNodeFloats>& flattenedNodesFloats, std::vector<FlattenedMultiNodeInts>& flattenedNodesInts, int n) {
        if (!node) {
            return -1;
        }

        FlattenedMultiNodeFloats flatNodeFloat;
        FlattenedMultiNodeInts flatNodeInt;
        flatNodeFloat.minBound = node->minBound;
        flatNodeFloat.maxBound = node->maxBound;

        for (int octreeIndex = 0; octreeIndex < n; ++octreeIndex) {
            if (node->octreeDataMap.count(octreeIndex)) {
                flatNodeInt.octreeInfo[octreeIndex].pointIndex = node->octreeDataMap.at(octreeIndex).pointIndex;
                flatNodeInt.octreeInfo[octreeIndex].pointCount = node->octreeDataMap.at(octreeIndex).pointCount;
                flatNodeInt.octreeInfo[octreeIndex].depends = node->octreeDataMap.at(octreeIndex).depends;
                flatNodeInt.octreeInfo[octreeIndex].leaf = node->octreeDataMap.at(octreeIndex).leaf;
            }
            else {
                // llenar con datos por defecto si el �ndice de octree no est� presente
                flatNodeInt.octreeInfo[octreeIndex].pointIndex = -1;
                flatNodeInt.octreeInfo[octreeIndex].pointCount = 0;
                flatNodeInt.octreeInfo[octreeIndex].depends = false;
                flatNodeInt.octreeInfo[octreeIndex].leaf = false;
            }
        }

        int currentNodeIndex = flattenedNodesInts.size();
        flattenedNodesInts.push_back(flatNodeInt);
        flattenedNodesFloats.push_back(flatNodeFloat);

        for (int i = 0; i < 8; ++i) {
            if (node->children[i]) {
                flatNodeInt.childrenIndices[i] = flattenMultiOctree(node->children[i], flattenedNodesFloats, flattenedNodesInts,n);
            }
            else {
                flatNodeInt.childrenIndices[i] = -1; // Indica que no hay hijo en esta posici�n
            }
        }

        // Actualizar el nodo aplanado en la lista con los �ndices de los hijos correctos
        flattenedNodesInts[currentNodeIndex] = flatNodeInt;
        flattenedNodesFloats[currentNodeIndex] = flatNodeFloat;

        return currentNodeIndex;
    }

};
