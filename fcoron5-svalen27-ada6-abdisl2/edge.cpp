#include "edge.h"
//The dataset is seperated by commas --- (ID of Rater, ID of Ratee, Rating[-10,10], Timestamp of Transaction)

Edge::Edge(Vertex src, Vertex dest, int weight) {
    //0 is discovery, 1 is cross, 2 is undeclared
    src_ = src;
    dest_ = dest;
    weight_ = weight;
    label_ = Undeclared; //undeclared edge
}

// Edge::Edge(Vertex src, Vertex dest, int weight, string label) {
//     src_ = src;
//     dest_ = dest;
//     weight_ = weight;
//     label_ = label;
// }

bool Edge::operator==(Edge& other) const {
    //compare weights also?dit
    if (src_ != other.src_ || dest_ != other.dest_) {
        return false;
    }
    return true;
}
