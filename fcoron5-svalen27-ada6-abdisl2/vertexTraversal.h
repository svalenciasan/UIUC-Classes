#pragma once

#include <unordered_map>
#include <string>
#include <vector>
#include <queue>
#include <iostream>

#include "edge.h"
#include "graph.h"

using std::pair;
using std::unordered_map;
using std::queue;
using std::vector;
using std::iterator;
using std::string;
using namespace std;

class vertexTraversal {
    public:
        class Iterator : iterator<forward_iterator_tag, Vertex> {
            public:
                Iterator();
                Iterator(Vertex vertex, Graph theGraph);
                Iterator & operator++();
                Vertex operator*();
                bool operator!=(const Iterator &other);
                bool isVisited(Vertex check);
                unordered_map<Vertex, Vertex> previousNodes; //key is node, value is node that added it to the tree;
            private:
                Vertex vertexID;
                Vertex start;
                vector<Vertex> visited; //a list of all visited vertices, ultimately in the order of the traversal
                queue<Vertex> vertexQueue;
                Graph bitcoinGraph;
        };
    Iterator begin(Vertex vertex, Graph theGraph); //specific vertex to begin from
    Iterator end();
};