#include <unordered_map>
#include <string>
#include <vector>
#include <queue>
#include <iostream>

#include "edge.h"
#include "graph.h"
#include "vertexTraversal.h"

using std::pair;
using std::unordered_map;

vertexTraversal::Iterator::Iterator() {
    vertexID = "";
    start = "";
}

vertexTraversal::Iterator::Iterator(Vertex vertex, Graph theGraph) {
    start = vertex;
    vertexID = vertex;
    bitcoinGraph = theGraph;
    visited.push_back(start);
    vertexQueue.push(start);
    previousNodes[start] = "";
}

vertexTraversal::Iterator & vertexTraversal::Iterator::operator++() {
    //updating vertexID is where the ++ happens
    if (vertexQueue.empty() == false) {
        //1. Dequeue vertex
        Vertex current = vertexQueue.front();
        vertexQueue.pop();
        
        vector<Vertex> neighbors = bitcoinGraph.getNeighbors(current);

        for (unsigned i = 0; i < neighbors.size(); i++) {
            Edge currentEdge = bitcoinGraph.getEdge(current, neighbors[i]);

            if (isVisited(neighbors[i]) == false) {
                //2. Process vertex -- Mark visited & Label edges
                currentEdge.label_ = Edge::Discovery;
                visited.push_back(neighbors[i]);
                //3. Add unvisited neighbors to queue
                vertexQueue.push(neighbors[i]);
                previousNodes[neighbors[i]] = current;
            } else {
                //2. Process vertex -- Label edges
                currentEdge.label_ = Edge::Cross;
            }
        }
        if (vertexQueue.empty() == false) {
            vertexID = vertexQueue.front();
            return *this;
        } 
    }
    vertexID = "";
    return *this;
}

Vertex vertexTraversal::Iterator::operator*() {
    return vertexID;
}

bool vertexTraversal::Iterator::operator!=(const vertexTraversal::Iterator & other) {
    return !(vertexID == other.vertexID);
}

vertexTraversal::Iterator vertexTraversal::begin(Vertex vertex, Graph theGraph) {
    return vertexTraversal::Iterator(vertex, theGraph);
}

vertexTraversal::Iterator vertexTraversal::end() {
    return vertexTraversal::Iterator();
}

bool vertexTraversal::Iterator::isVisited(Vertex check) {
    return (find(visited.begin(), visited.end(), check) != visited.end());
}