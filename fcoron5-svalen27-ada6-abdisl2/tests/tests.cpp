#include <string>
#include <vector>

#include "../catch/catch.hpp"
#include "../readFromFile.h"
#include "../graph.h"
#include "../edge.h"
#include "../vertexTraversal.h"

using std::string;
using std::vector;
using std::cout;
using std::endl;


/**
 * READ FILE TESTS
 */

TEST_CASE("Verify that file_to_string works on a small example") {
	// always check "expected" == "actual" --> be consistent
	string res = file_to_string("tests/smallSample.txt");
	string expected = "hello\nstudents\nof\ncs\n225\n!!!";
	REQUIRE(expected == res);
}

TEST_CASE("Verify that file_to_vector works on a small example") {
	vector<string> res = file_to_vector("tests/smallSample.txt");
	string expected[] = {"hello", "students", "of", "cs", "225", "!!!"};
	
	REQUIRE(6 == res.size());

	for (size_t i = 0; i < res.size(); i++) {
		REQUIRE(expected[i] == res[i]);
	}
}

TEST_CASE("Verify that file_to_string works on a 10 csv") {
	// always check "expected" == "actual" --> be consistent
	string res = file_to_string("tests/10Dataset.csv");
	string expected = "6,2,4,1289241911.72836\n6,5,2,1289241941.53378\n1,15,1,1289243140.39049\n4,3,7,1289245277.36975\n13,16,8,1289254254.44746\n13,10,8,1289254300.79514\n7,5,1,1289362700.47913\n2,21,5,1289370556.80938\n2,20,5,1289370622.21473\n21,2,5,1289380981.52787";
	REQUIRE(expected == res);
}

TEST_CASE("Verify that file_to_vector works on a 10 csv") {
	vector<string> res = file_to_vector("tests/10Dataset.csv");
	string expected[] = {"6,2,4,1289241911.72836","6,5,2,1289241941.53378","1,15,1,1289243140.39049","4,3,7,1289245277.36975","13,16,8,1289254254.44746","13,10,8,1289254300.79514","7,5,1,1289362700.47913","2,21,5,1289370556.80938","2,20,5,1289370622.21473","21,2,5,1289380981.52787"};
	
	REQUIRE(10 == res.size());

	for (size_t i = 0; i < res.size(); i++) {
		REQUIRE(expected[i] == res[i]);
	}
}

TEST_CASE("Verify that file_to_string works on a 5 csv") {
	// always check "expected" == "actual" --> be consistent
	string res = file_to_string("tests/5Dataset.csv");
	string expected = "6,2,4,1289241911.72836\n6,5,2,1289241941.53378\n1,15,1,1289243140.39049\n4,3,7,1289245277.36975\n13,16,8,1289254254.44746";
	REQUIRE(expected == res);
}

TEST_CASE("Verify that file_to_vector works on a 5 csv") {
	vector<string> res = file_to_vector("tests/5Dataset.csv");
	string expected[] = {"6,2,4,1289241911.72836","6,5,2,1289241941.53378","1,15,1,1289243140.39049","4,3,7,1289245277.36975","13,16,8,1289254254.44746"};
	
	REQUIRE(5 == res.size());

	for (size_t i = 0; i < res.size(); i++) {
		REQUIRE(expected[i] == res[i]);
	}
}

/**
 * EDGE TESTS
 */

TEST_CASE("Create Edge") {
	Edge edge_("1", "2", 3);
	REQUIRE("1" == edge_.src_);
	REQUIRE("2" == edge_.dest_);
	REQUIRE(3 == edge_.weight_);
	REQUIRE(Edge::Undeclared == edge_.label_);
}

/**
 * GRAPH VERTICES 
 */

TEST_CASE("Insert vertex") {
	Graph graph_;
	string v1 = "1";
	string v2 = "2";
	graph_.insertVertex(v1);
	REQUIRE(true == graph_.vertexExists(v1));
	REQUIRE(false == graph_.vertexExists(v2));
}

/**
 * GRAPH EDGES 
 */

TEST_CASE("Insert edge") {
	Graph graph_;

	string v1 = "1";
	string v2 = "2";

	graph_.insertVertex(v1);
	graph_.insertVertex(v2);

	REQUIRE(true == graph_.vertexExists(v1));
	REQUIRE(true == graph_.vertexExists(v2));

	REQUIRE(true == graph_.insertEdge("1", "2", 5));

	REQUIRE(true == graph_.edgeExists("1", "2"));
	REQUIRE(false == graph_.edgeExists("2", "1"));
}

TEST_CASE("Insert edge already exists") {
	Graph graph_;

	string v1 = "1";
	string v2 = "2";

	graph_.insertVertex(v1);
	graph_.insertVertex(v2);

	REQUIRE(true == graph_.vertexExists(v1));
	REQUIRE(true == graph_.vertexExists(v2));

	REQUIRE(true == graph_.insertEdge("1", "2", 5));
	REQUIRE(false == graph_.insertEdge("1", "2", 5));

	REQUIRE(true == graph_.edgeExists("1", "2"));
	REQUIRE(false == graph_.edgeExists("2", "1"));

	REQUIRE(true == graph_.vertexExists(v1));
	REQUIRE(true == graph_.vertexExists(v2));
}

TEST_CASE("Insert edge destination missing") {
	Graph graph_;

	string v1 = "1";

	graph_.insertVertex(v1);

	REQUIRE(true == graph_.vertexExists(v1));
	REQUIRE(false == graph_.vertexExists("2"));

	REQUIRE(false == graph_.edgeExists("1", "2"));
	REQUIRE(false == graph_.edgeExists("2", "1"));

	REQUIRE(true == graph_.insertEdge("1", "2", 5));
	REQUIRE(false == graph_.insertEdge("1", "2", 5));

	REQUIRE(true == graph_.edgeExists("1", "2"));
	REQUIRE(false == graph_.edgeExists("2", "1"));

	REQUIRE(true == graph_.vertexExists(v1));
	REQUIRE(true == graph_.vertexExists("2"));
}

TEST_CASE("Insert edge source missing") {
	Graph graph_;

	string v2 = "2";

	graph_.insertVertex(v2);

	REQUIRE(false == graph_.vertexExists("1"));
	REQUIRE(true == graph_.vertexExists(v2));

	REQUIRE(false == graph_.edgeExists("1", "2"));
	REQUIRE(false == graph_.edgeExists("2", "1"));

	REQUIRE(true == graph_.insertEdge("1", "2", 5));
	REQUIRE(false == graph_.insertEdge("1", "2", 5));

	REQUIRE(true == graph_.edgeExists("1", "2"));
	REQUIRE(false == graph_.edgeExists("2", "1"));

	REQUIRE(true == graph_.vertexExists("1"));
	REQUIRE(true == graph_.vertexExists(v2));
}

TEST_CASE("Insert edge source and destination missing") {
	Graph graph_;

	REQUIRE(false == graph_.vertexExists("1"));
	REQUIRE(false == graph_.vertexExists("2"));

	REQUIRE(false == graph_.edgeExists("1", "2"));
	REQUIRE(false == graph_.edgeExists("2", "1"));

	REQUIRE(true == graph_.insertEdge("1", "2", 5));
	REQUIRE(false == graph_.insertEdge("1", "2", 5));

	REQUIRE(true == graph_.edgeExists("1", "2"));
	REQUIRE(false == graph_.edgeExists("2", "1"));

	REQUIRE(true == graph_.vertexExists("1"));
	REQUIRE(true == graph_.vertexExists("2"));
}

/**
 * LOADING GRAPH
 */

TEST_CASE("Loading 5 Dataset") {
	string fileStr = "tests/5Dataset.csv";
	Graph graph_;
	graph_.loadFromFile(fileStr);

	REQUIRE(true == graph_.vertexExists("1"));
	REQUIRE(true == graph_.vertexExists("2"));
	REQUIRE(true == graph_.vertexExists("3"));
	REQUIRE(true == graph_.vertexExists("4"));
	REQUIRE(true == graph_.vertexExists("5"));
	REQUIRE(true == graph_.vertexExists("6"));
	REQUIRE(true == graph_.vertexExists("13"));
	REQUIRE(true == graph_.vertexExists("15"));
	REQUIRE(true == graph_.vertexExists("16"));

	REQUIRE(false == graph_.insertEdge("1", "15", 1));
	REQUIRE(true == graph_.edgeExists("1", "15"));
	REQUIRE(false == graph_.insertEdge("4", "3", 7));
	REQUIRE(true == graph_.edgeExists("4", "3"));
	REQUIRE(false == graph_.insertEdge("6", "2", 4));
	REQUIRE(true == graph_.edgeExists("6", "2"));
	REQUIRE(false == graph_.insertEdge("6", "5", 2));
	REQUIRE(true == graph_.edgeExists("6", "5"));
	REQUIRE(false == graph_.insertEdge("13", "16", 8));
	REQUIRE(true == graph_.edgeExists("13", "16"));
}

//Iterator tests

TEST_CASE("!= operator works") {
	string fileStr = "tests/10Dataset.csv";
	Graph theGraph;
	theGraph.loadFromFile(fileStr);
	vertexTraversal *traversal;
	vertexTraversal *traversal2;
	auto one = traversal->begin("1", theGraph);
	auto two = traversal2->begin("4", theGraph);
	REQUIRE((one != two) == true);
}

TEST_CASE("* operator works") {
	string fileStr = "tests/10Dataset.csv";
	Graph theGraph;
	theGraph.loadFromFile(fileStr);
	vertexTraversal *traversal;
	auto one = traversal->begin("4", theGraph);
	REQUIRE(*one == "4");
}

TEST_CASE("++ operator works") {
	string fileStr = "tests/10Dataset.csv";
	Graph theGraph;
	theGraph.loadFromFile(fileStr);
	vertexTraversal *traversal;
	vector<Vertex> nodesVisited;

	//largest section of the sample
	for (auto it = traversal->begin("6", theGraph); it != traversal->end(); ++it) {
		nodesVisited.push_back(*it);
		/*
					6
				  /   \
				 5     2
				     /   \
					20    21
		*/
		//std::cout << *it << std::endl;
	}
	//std::cout << "\n" << std::endl;
	REQUIRE(nodesVisited.size() == 5); //adds 5 nodes
	REQUIRE(nodesVisited[0] == "6"); //adds them in the order of the file
	REQUIRE(nodesVisited[1] == "5");
	REQUIRE(nodesVisited[2] == "2");
	REQUIRE(nodesVisited[3] == "20");
	REQUIRE(nodesVisited[4] == "21");
	nodesVisited.clear();

	//smaller section
	for (auto it = traversal->begin("21", theGraph); it != traversal->end(); ++it) {
		nodesVisited.push_back(*it);
		/*
					21
				   /
				  2
			     /
			    20
		*/
		//std::cout << *it << std::endl;
	}
	//std::cout << "\n" << std::endl;
	REQUIRE(nodesVisited.size() == 3);
	REQUIRE(nodesVisited[0] == "21");
	REQUIRE(nodesVisited[1] == "2");
	REQUIRE(nodesVisited[2] == "20");
	nodesVisited.clear();

	//two nodes only
	for (auto it = traversal->begin("1", theGraph); it != traversal->end(); ++it) {
		nodesVisited.push_back(*it);
		/*
					1
				   /
				 15
		*/
		//std::cout << *it << std::endl;
	}
	REQUIRE(nodesVisited.size() == 2);
	REQUIRE(nodesVisited[0] == "1");
	REQUIRE(nodesVisited[1] == "15");
}

TEST_CASE("++ operator works with only one Vertex") {
	Graph theGraph;
	vertexTraversal *traversal;
	vector<Vertex> nodesVisited;

	theGraph.insertVertex("1");

	for (auto it = traversal->begin("1", theGraph); it != traversal->end(); ++it) {
		nodesVisited.push_back(*it);
	}

	REQUIRE(nodesVisited.size() == 1);
	REQUIRE(nodesVisited[0] == "1");
}

TEST_CASE("++ operator works with more connected graph") {
	Graph theGraph;

	theGraph.insertVertex("1");
	theGraph.insertVertex("2");
	theGraph.insertVertex("3");
	theGraph.insertEdge("1", "3", 1);
	theGraph.insertEdge("1", "2", 1);
	theGraph.insertEdge("2", "3", 1);
	
	theGraph.insertVertex("4");
	theGraph.insertVertex("5");
	theGraph.insertEdge("2", "4", 1);
	theGraph.insertEdge("3", "4", 1);
	theGraph.insertEdge("3", "5", 1);
	theGraph.insertEdge("4", "5", 1);

	theGraph.insertVertex("6");
	theGraph.insertEdge("5", "6", 1);

	vertexTraversal *traversal;
	vector<Vertex> nodesVisited;

	for (auto it = traversal->begin("1", theGraph); it != traversal->end(); ++it) {
		nodesVisited.push_back(*it);
		/*
							   1
							  / \
							 3---2
							 |\  |
							 | \ |
							 |	\|
							 5---4
							 |	 
							 6	 
		*/
		//std::cout << *it << std::endl;
	}
	REQUIRE(nodesVisited.size() == 6);
	REQUIRE(nodesVisited[0] == "1");
	REQUIRE((bool) ((nodesVisited[1] == "2") || (nodesVisited[1] == "3")) );
	REQUIRE((bool) ((nodesVisited[2] == "3") || (nodesVisited[2] == "2")) );
	REQUIRE((bool) ((nodesVisited[3] == "5") || (nodesVisited[3] == "4")) );
	REQUIRE((bool) ((nodesVisited[4] == "4") || (nodesVisited[4] == "5")) );
	REQUIRE(nodesVisited[5] == "6");
}

/*
 * PageRank tests
 */

TEST_CASE("Verify PageRank works on a small (5) dataset") {
	string fileStr = "tests/5Dataset.csv";
	Graph graph_;
	graph_.loadFromFile(fileStr);
	unordered_map<Vertex, double> pageRank = graph_.getPageRank();

	REQUIRE(pageRank["1"] == Approx(0.0806469644));
	REQUIRE(pageRank["2"] == Approx(0.1173671412));
	REQUIRE(pageRank["3"] == Approx(0.1491912945));
	REQUIRE(pageRank["4"] == Approx(0.0806469644));
	REQUIRE(pageRank["5"] == Approx(0.1124711177));
	REQUIRE(pageRank["6"] == Approx(0.0806469644));
	REQUIRE(pageRank["13"] == Approx(0.0806469644));
	REQUIRE(pageRank["15"] == Approx(0.1491912945));
	REQUIRE(pageRank["16"] == Approx(0.1491912945));

	double sum = 0;
	for (auto it = pageRank.begin(); it != pageRank.end(); it++) {
		sum += it->second;
	}

	REQUIRE(sum == Approx(1));
}

TEST_CASE("Verify PageRank works on a small (10) dataset") {
	string fileStr = "tests/10Dataset.csv";
	Graph graph_;
	graph_.loadFromFile(fileStr);
	unordered_map<Vertex, double> pageRank = graph_.getPageRank();

	REQUIRE(pageRank["1"] == Approx(0.0445004208));
	REQUIRE(pageRank["2"] == Approx(0.1606078545));
	REQUIRE(pageRank["3"] == Approx(0.082326281));
	REQUIRE(pageRank["4"] == Approx(0.0445004208));
	REQUIRE(pageRank["5"] == Approx(0.0998882874));
	REQUIRE(pageRank["6"] == Approx(0.0445004208));
	REQUIRE(pageRank["13"] == Approx(0.0445004208));
	REQUIRE(pageRank["15"] == Approx(0.082326281));
	REQUIRE(pageRank["16"] == Approx(0.0634133509));
	REQUIRE(pageRank["10"] == Approx(0.0634133509));
	REQUIRE(pageRank["7"] == Approx(0.0445004208));
	REQUIRE(pageRank["21"] == Approx(0.1127612451));
	REQUIRE(pageRank["20"] == Approx(0.1127612451));

	double sum = 0;
	for (auto it = pageRank.begin(); it != pageRank.end(); it++) {
		sum += it->second;
	}

	REQUIRE(sum == Approx(1));
}

/*
* DIJKSTRA ALGO
*/
TEST_CASE("Dijkstras Testing") {
	string fileStr = "tests/10Dataset.csv";
	Graph graph_;
	graph_.loadFromFile(fileStr);
	REQUIRE(true == graph_.vertexExists("6"));
	vector<int> m = graph_.dikstras("6");
	std::cout << "test";
	for (int i = 0, n = m.size(); i < n; i++)
		if (m[i] != INT_MAX)
			std::cout << "Source: 6 Destination: " << i << " Distance: " << m[i] << std::endl;
	// some tests here
}

//shortest path
TEST_CASE("Test shortest path with BFS") {
	Graph theGraph;
	vertexTraversal *traversal;

	theGraph.insertEdge("1", "2", 1);
	theGraph.insertEdge("1", "11", 1);
	theGraph.insertEdge("1", "7", 1);

	theGraph.insertEdge("2", "1", 1);
	theGraph.insertEdge("2", "13", 1);
	theGraph.insertEdge("2", "5", 1);
	
	theGraph.insertEdge("4", "5", 1);

	theGraph.insertEdge("5", "4", 1);
	theGraph.insertEdge("5", "9", 1);
	theGraph.insertEdge("5", "10", 1);

	theGraph.insertEdge("6", "10", 1);

	theGraph.insertEdge("7", "3", 1);
	theGraph.insertEdge("7", "13", 1);

	theGraph.insertEdge("8", "12", 1);

	theGraph.insertEdge("9", "8", 1);
	theGraph.insertEdge("9", "5", 1);

	theGraph.insertEdge("10", "12", 1);

	theGraph.insertEdge("12", "8", 1);

	theGraph.insertEdge("13", "2", 1);
	theGraph.insertEdge("13", "6", 1);

	vector<Vertex> path;
	Vertex start = "4";
	Vertex end = "10";
	Vertex endTemp = end;
	string shortestPath = "";

	for (auto it = traversal->begin(start, theGraph); it != traversal->end(); ++it) {
		if (*it == end) {
			path.push_back(end);
			while (end != "") {
				end = it.previousNodes[end];
				path.push_back(end);
			}
			for (unsigned index = path.size() - 1; index >= 0; index--) {
				if (index == -1) {
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
	REQUIRE(shortestPath == start + " -> 5 -> " + endTemp);
	REQUIRE(path.size() == 3); //size should be amount of vertices involved
}

TEST_CASE("Test shortest path with BFS #2") {
	Graph theGraph;
	vertexTraversal *traversal;

	theGraph.insertEdge("1", "2", 1);
	theGraph.insertEdge("1", "11", 1);
	theGraph.insertEdge("1", "7", 1);

	theGraph.insertEdge("2", "1", 1);
	theGraph.insertEdge("2", "13", 1);
	theGraph.insertEdge("2", "5", 1);
	
	theGraph.insertEdge("4", "5", 1);

	theGraph.insertEdge("5", "4", 1);
	theGraph.insertEdge("5", "9", 1);
	theGraph.insertEdge("5", "10", 1);

	theGraph.insertEdge("6", "10", 1);

	theGraph.insertEdge("7", "3", 1);
	theGraph.insertEdge("7", "13", 1);

	theGraph.insertEdge("8", "12", 1);

	theGraph.insertEdge("9", "8", 1);
	theGraph.insertEdge("9", "5", 1);

	theGraph.insertEdge("10", "12", 1);

	theGraph.insertEdge("12", "8", 1);

	theGraph.insertEdge("13", "2", 1);
	theGraph.insertEdge("13", "6", 1);

	vector<Vertex> path;
	Vertex start = "13";
	Vertex end = "4";
	Vertex endTemp = end;
	string shortestPath = "";

	for (auto it = traversal->begin(start, theGraph); it != traversal->end(); ++it) {
		if (*it == end) {
			path.push_back(end);
			while (end != "") {
				end = it.previousNodes[end];
				path.push_back(end);
			}
			for (unsigned index = path.size() - 1; index >= 0; index--) {
				if (index == -1) {
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
	REQUIRE(shortestPath == start + " -> 2 -> 5 -> " + endTemp);
	REQUIRE(path.size() == 4); //size should be amount of vertices involved
}