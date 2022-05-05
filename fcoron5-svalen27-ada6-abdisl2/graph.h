#pragma once

#include <unordered_map>
#include <unordered_set>
#include <string>
#include <vector>
#include <iostream>
#include <sstream>
#include <cmath>

#include "edge.h"
#include "readFromFile.h"
//#include "vertexTraversal.h"

using std::pair;
using std::unordered_map;
using std::vector;
using std::cout;
using std::endl;
using std::stringstream;
using std::abs;

//typedef string Vertex;

class Graph {
private:
    //The dataset is seperated by commas --- (ID of Rater, ID of Ratee, Rating[-10,10], Timestamp of Transaction)
public:
    Graph() = default;

    /**
     * 
     */
    void loadFromFile(string filepath);

    /**
     * 
     */
    void insertVertex(Vertex vertex);

    /**
     * 
     */
    Vertex removeVertex(Vertex vertex);

    /**
     * Will create vertices if not existent before.
     */
    bool insertEdge(Vertex src, Vertex dest, int weight);

    /**
     * 
     */
    Edge removeEdge(Vertex src, Vertex dest);

    /**
     * In first map
     */
    bool vertexExists(Vertex vertex) const;
    
    /**
     * In src and dest
     */
    bool edgeExists(Vertex src, Vertex dest) const;

    /**
     *  Gets the rating of a specified vertex
     */ 
    double getAvgRating(Vertex vertex) const;

    /**
     *  Returns a map from a vertex in the graph to its pageRank value
     */
    unordered_map<Vertex, double> getPageRank(float dampingFactor = 0.85);

    /**
     *  Prints the PageRank of each vertex in the graph and compares it to the rating of that vertex
     */
    void printPageRank(vector<Vertex> vertices);

    /**
     *  gets outgoing neighbors of a specific vertex
     */
    std::vector<Vertex> getNeighbors(Vertex start);

    /**
     *  gets outgoing edges of a specific vertex
     */
    std::vector<Edge> getEdges(Vertex start);

    /**
     *  gets specific edge from src to dest
     */
    Edge getEdge(Vertex src, Vertex dest);

    //--------------------------------------------------------------------------------------------------

    void getSize(Vertex src, std::unordered_set<Vertex>& s);

    void printDijkstras(vector<int> m);

    vector<int> dikstras(Vertex src);

    unordered_map<Vertex, int> getShortestPath(Vertex src);
    //int shortestPath(Vertex v) const; 
private:
    /** 
     * Maps vertices to a pair containing incoming and outgoing edges.
     * Pair contains incoming edges first, then outgoing edges second.
     */
    unordered_map<Vertex, pair<unordered_map<Vertex, Edge>, unordered_map<Vertex, Edge>>> graph_;

    /* 
                        |     Vertex                   Vertex     |
                        |       ^                        ^        |
        Vertex ------>  |       |         ,              |        |
                        |       v                        v        |
                        |      Edge                     Edge      |
                        |   (INCOMING)               (OUTGOING)   |
    */

    bool pageRankIteration(unordered_map<Vertex, double>& prev, float dampingFactor, double error = 0.00001);
};