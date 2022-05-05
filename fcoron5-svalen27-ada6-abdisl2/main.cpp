#include <iostream>
#include <vector>
#include <algorithm>
#include "graph.h"
#include "vertexTraversal.h"


int main() {
	Graph graph_;
	graph_.loadFromFile("tests/fullDataset.csv");
	cout << "WORKS" << endl;

	//shortest path
	Vertex start, end;
	vertexTraversal *traversal = nullptr;

	std::cout << "Insert a vertex to start from (No Letters): "; // get Vertex from user
	std::cin >> start;
	while (graph_.vertexExists(start) == false) {
		std::cout << "Not a valid vertex, insert one that is part of the graph: "; //user re-inputs if vertex is not part of dataset
		std::cin >> start;
	}

	std::cout << "\nInsert a vertex to end at. Choose from: \n"; // gives user all vertices on the island, and they choose one for end Vertex
	vector<int> partofIsland; //int instead of Vertex so that it can be sorted
	for (auto it = traversal->begin(start, graph_); it != traversal->end(); ++it) {
		partofIsland.push_back(stoi(*it));
	}
	std::sort(partofIsland.begin(), partofIsland.end());
	for (unsigned i = 0; i < partofIsland.size(); i++) {
		std::cout << partofIsland[i] << " ";
	}

	std::cout << "\nYour choice for ending vertex: "; //user chooses end Vertex
	std::cin >> end;
	while (find(partofIsland.begin(), partofIsland.end(), stoi(end)) == partofIsland.end()) {
		std::cout << "The end vertex you selected was not part of the given island. Try again (No letters): "; //try again if not from given list
		std::cin >> end;
	}
	std::cout << "Starting Vertex: " << start << ", Ending Vertex: " << end << std::endl; //start and end vertices have been finalized

	vector<Vertex> path;
	string shortestPath = "";

	for (auto it = traversal->begin(start, graph_); it != traversal->end(); ++it) {
		if (*it == end) { //traverse until end node is found
			path.push_back(end); //add end to path
			while (end != "") { //start with end node, and add origin nodes until back at start
				end = it.previousNodes[end];
				path.push_back(end);
			}
			for (unsigned index = path.size() - 1; index >= 0; index--) {
				if ((int) index == -1) {
					break;
				}
				if (index != path.size() - 1 && index != 0) {
					shortestPath +=  path[index] + " -> ";
				} else {
					shortestPath +=  path[index];
				}
			}
			break;
		}
	}
	path.erase(path.end() - 1);
	std::cout << "\n" << shortestPath << std::endl;
	std::cout << path.size() - 1 << " edges crossed" << std::endl;

	std::cout << "-----------PageRank-----------" << std::endl;

	graph_.printPageRank(path);


	//vector<int> src;
	// std::cout << "--------------Dijkstras------------" << std::endl;
	// graph_.dikstras(start);


	return 0;
}