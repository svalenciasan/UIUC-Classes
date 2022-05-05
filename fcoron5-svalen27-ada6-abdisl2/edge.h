#pragma once

#include <string>
#include <iostream>

using std::string;
using std::cout;
using std::endl;

typedef string Vertex;
//The dataset is seperated by commas --- (ID of Rater, ID of Ratee, Rating[-10,10], Timestamp of Transaction)
//

class Edge {
public:
    Vertex src_;
    Vertex dest_;
    int weight_;
    enum Label {Discovery, Cross, Undeclared} label_;
    
    /**
     * 
     */
    Edge() = default;

    /**
     * 
     */
    Edge(Vertex src, Vertex dest, int weight);

    /**
     * 
     */
    //Edge(Vertex src, Vertex dest, int weight, string label);

    /**
     * 
     */
    bool operator==(Edge& other) const;

};