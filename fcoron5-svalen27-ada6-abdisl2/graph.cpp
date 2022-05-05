//using std::set;

#include <unordered_map>
#include <queue>
#include "graph.h"
#include <set>
#include <unordered_set>
#define INF INT_MAX
//The dataset is seperated by commas --- (ID of Rater, ID of Ratee, Rating[-10,10], Timestamp of Transaction)

void Graph::loadFromFile(string filepath) {
    vector<string> file = file_to_vector(filepath);

    for (auto line : file) {
        //Will contain (ID of Rater, ID of Ratee, Rating[-10,10], Timestamp of Transaction)
        vector<string> data;
        //Splits the string by commas
        stringstream s_stream(line); //create string stream from the string
        while(s_stream.good()) {
            string substr;
            getline(s_stream, substr, ','); //get first string delimited by comma
            data.push_back(substr);
        }
        //If done correctly, create an edge with the data.
        if (data.size() >= 3) {
            int weight = stoi(data[2]);
            insertEdge(data[0], data[1], weight);
        }
    }
}

void Graph::insertVertex(Vertex vertex) {
    //Add if not in the map
    if (!vertexExists(vertex)) {
        graph_[vertex] = pair<unordered_map<Vertex, Edge>, unordered_map<Vertex, Edge>>();
    }
}

Vertex Graph::removeVertex(Vertex vertex) {
    //If exists 
    if (vertexExists(vertex)) {
        graph_.erase(vertex);
        //Could be improved by making a list of adjecent vertices before erasing
        for(auto it = graph_.begin(); it != graph_.end(); it++) {
            //Check if vertex in incoming
            if (it->second.first.find(vertex) != it->second.first.end()) {
                it->second.first.erase(vertex);
            }

            //Check if vertex in outgoing
            if (it->second.second.find(vertex) != it->second.second.end()) {
                it->second.second.erase(vertex);
            }
        }
        return vertex;
    }
    //If doesn't exist
    return "";
}

bool Graph::insertEdge(Vertex src, Vertex dest, int weight) {
    //Checks if source exists, then check if the connection exists
    if(edgeExists(src, dest)) {
        return false;
    }

    //Insert source
    if(!vertexExists(src)) {
        insertVertex(src);
    }
    //Insert edge on source
    //Edge* edj;
    graph_.at(src).second[dest] = Edge(src, dest, weight);

    //Insert destination
    if(!vertexExists(dest)) {
        insertVertex(dest);
    }
    //Insert edge on destination
    graph_.at(dest).first[src] = Edge(src, dest, weight);

    return true;
}

Edge Graph::removeEdge(Vertex src, Vertex dest) {
    if(!edgeExists(src, dest)) {
        return Edge();
    }

    //Remove from source
    Edge removed = graph_[src].second[dest];
    graph_[src].second.erase(dest);

    //Remove from destination
    graph_[dest].first.erase(src);

    return removed;
}

bool Graph::vertexExists(Vertex vertex) const {
    return graph_.find(vertex) != graph_.end();
}
    
bool Graph::edgeExists(Vertex src, Vertex dest) const {
    if (vertexExists(src) && vertexExists(dest)) {
        //Checks edge existence as incoming and outgoing
        return (graph_.at(src).second.find(dest) != graph_.at(src).second.end()) * 
                (graph_.at(dest).first.find(src) != graph_.at(dest).first.end());
    }
    return false;
}

double Graph::getAvgRating(Vertex vertex) const {
    unordered_map<Vertex, Edge> incomingEdges = graph_.at(vertex).first;

    unordered_map<Vertex, Edge>::iterator it;

    double totalRating = 0;

    for (it = incomingEdges.begin(); it != incomingEdges.end(); it++) {
        Edge curr = it->second;
        totalRating += curr.weight_;
    }

    return totalRating / incomingEdges.size();
}

unordered_map<Vertex, double> Graph::getPageRank(float dampingFactor) {
    unordered_map<Vertex, double> pageRank;
    size_t userNum = graph_.size();
    // Evenly distribute pageRank amongst vertices at the start
    for (auto it = graph_.begin(); it != graph_.end(); it++) {
        pageRank.insert(pair<Vertex, double>(it->first, 1.0 / userNum));
    }

    // While pageRank values haven't converged perform pageRank iteration
    size_t n = 1;
    while (!pageRankIteration(pageRank, dampingFactor)) {
        n++;
    }

    cout << "PageRank finished in " << n << " iterations" << endl;

    return pageRank;
}

bool Graph::pageRankIteration(unordered_map<Vertex, double>& prev, float dampingFactor, double error) {
    unordered_map<Vertex, double> tmpRank = prev;

    // PageRank lost to sink nodes (Nodes without outgoing edges)
    double sinkPR = 0;

    for (auto it = graph_.begin(); it != graph_.end(); it++) {
        // if the vertex has no outgoing edges add their pageRank
        if (getEdges(it->first).size() == 0) {
            sinkPR += prev[it->first];
        }
    }

    // Check if pageRank has converged within the givin error margin
    bool hasConverged = true;

    for (auto it = prev.begin(); it != prev.end(); it++) {
        Vertex i = it->first;

        // Map of vertices with edges incoming to vertex i
        unordered_map<Vertex, Edge> incoming = graph_.at(i).first;

        // Add the pageRank that would be lost to vertices without outgoing / incoming edges
        double rank = (1 - dampingFactor + (dampingFactor * sinkPR)) / prev.size();

        // Add the pageRank contributed by each incoming edge
        for (auto it2 = incoming.begin(); it2 != incoming.end(); it2++) {
            Vertex j = it2->first;
            // Value used to avoid negative edge values
            double bound = 11.0;
            // Get the weight of the edge going from j to i
            double w_ji = bound + getEdge(j, i).weight_;
            // Sum the weights of all outgoing edges from vertex j
            double sum_w_jk = 0;
            for (Edge edge : getEdges(j)) {
                sum_w_jk += edge.weight_ + bound;
            }
            // Normalize edge ji using the sum
            double W_ji = w_ji / sum_w_jk;
            // Add the pageRank contribution of edge ji to the rank
            rank += dampingFactor * (prev.at(j) * W_ji);
        }
        // Update the pageRank
        tmpRank.at(i) = rank;

        double diff = abs(rank - prev.at(i));
        if (diff > error) {
            hasConverged = false;
        }  
    }

    prev = tmpRank;
    return hasConverged;
}

void Graph::printPageRank(vector<Vertex> vertices) {
    unordered_map<Vertex, double> pageRank = getPageRank();

    for (auto it = vertices.begin(); it != vertices.end(); it++) {
        cout << "Vertex " << *it << " PageRank: " << pageRank[*it] << " Score: " << getAvgRating(*it) << endl; 
    }
}

std::vector<Vertex> Graph::getNeighbors(Vertex start) {
    unordered_map<Vertex, Edge> myMap = graph_[start].second;
    std::vector<Vertex> result;
    for (auto it = myMap.begin(); it != myMap.end(); ++it) {
        result.push_back(it->first);
    }
    return result;
}

std::vector<Edge> Graph::getEdges(Vertex start) {
    unordered_map<Vertex, Edge> myMap = graph_[start].second;
    std::vector<Edge> result;
    for (auto it = myMap.begin(); it != myMap.end(); ++it) {
        result.push_back(it->second);
    }
    return result;
}

Edge Graph::getEdge(Vertex src, Vertex dest) {
    return graph_[src].second[dest];
}

void Graph::getSize(Vertex src, std::unordered_set<Vertex>& s) {
    if (!s.insert(src).second) return;
    for (auto &x : getNeighbors(src))
        getSize(x, s);
}

std::unordered_map<Vertex, int> Graph::getShortestPath(Vertex src) {
   std::unordered_set<Vertex> s;
   getSize(src, s);
   size_t vertexCount = s.size();
   std::unordered_map<Vertex, int> distance;
   std::queue<Vertex> q;
   distance[src] = 0;
   q.push(src);
   
   while (!q.empty()) {
      Vertex top = q.front();
      q.pop();
      for (auto &x : getNeighbors(top)) {
         if (distance.count(x)) continue;
         distance[x] = distance[top] + 1;
         q.push(x);
         if (distance.size() == vertexCount) return distance;
      }
   }
   return distance;
}

void Graph::printDijkstras(vector<int> m) {
    std::cout << "test";
    m = dikstras("6");
	for (int i = 0, n = m.size(); i < n; i++)
		if (m[i] != INT_MAX)
			std::cout << "Source: 6 Destination: " << i << " Distance: " << m[i] << std::endl;
}      

std::vector<int> Graph::dikstras(Vertex src) {
    std::set<pair<int, Vertex>> extract_set;
    std::vector<int> distance(100, INF);

    extract_set.insert(make_pair(0, src));
    distance[stoi(src)] = 0;

    while(!extract_set.empty()) {
        pair<int, Vertex> tmp = *(extract_set.begin());
        extract_set.erase(extract_set.begin());
        Vertex curr = tmp.second;
        //std::cout<< curr << std::endl;
        for (auto &x : graph_[curr].second) {
            Vertex v = x.first;
            std::cout << v << std::endl;
            int weight = x.second.weight_ + 11;
             std::cout << "weight  = " << (weight - 11) << std::endl;
            if (distance[stoi(v)] > weight + distance[stoi(curr)]) {
                if (distance[stoi(v)] != INF) {
                    extract_set.erase(extract_set.find({distance[stoi(v)], v}));
                }
                distance[stoi(v)] = weight + distance[stoi(curr)];
                extract_set.insert({distance[stoi(v)], v});
            }
           
        }
    }
    return distance;
}

//----------------------------------------------------------------------------------------
/*
dijkstras algrorith implementation
steps:
    1) create a a set of shortest path tree set that keeps trakc of whose minimum distance from source is 
        calculated and finalized. Initially, this starts off as empty.
    
    2) Assign a distance value to all vertices in the input graph. Initialize all distance values as Infinity.
        Assign distance value as 0 for the source vertex so that it is picked first

    3) while the shortest path tree set does't include all vertices
        a) Pick a vertex u which is not there in the (shorted path tree set) and has a min distance value.
        b) include u to the (shorted path tree set)
        c) update the distance value of all adjacent vertices of u. to update the distance values, iterate 
            through all adjacent vertices. for every adjacent vertex v, if sum of distance value of u (from source)
            and weight of edge u-v, is less than the distance value of v, then update the distance vcalue of v.
*/


//finding the nearest neighbor
//start off by taking a starting point. In this case let s be the starting point
// int Graph::shortestPath(Vertex s) const{
//     //create set to store vertices
//     //use this to extract the shortest path
//     std::set<pair<Vertex, Vertex>> extract_set;

//     //vector for distances
//     //All paths are initialized to a large value
//     vector<int> distances(V, INF);

//     //Inser the entry point into the set
//     //Initialize distane to 0
//     extract_set.insert(make_pair(0, s));
//     distances[s] = 0;
// }
