# CS 225 Final Project
### Team Members: fcoron5, svalen27, ada6, abdisl2

## Locations

Our code is stored in three classes, `Edge`, `Graph`, and `vertexTraversal`. Each class has a `.h` file as well as a `.cpp` file. The dataset that we use is located at `tests/fullDataset.csv`. `Edge` contains the edge object, which maintains data such as the source, destination, and weight of the edge. Inside `Graph`, the graph object is kept, which is maintained by an `unordered_map`. The code which is responsible for our BFS traversal is kept inside `vertexTraversal`.

## Executable instructions

To build and run the executable, you will use `./finalproj` after compiling with `make`. This executable will present the shortest path between two vertices, as well as print their PageRank statistics. You will be prompted to enter a starting vertex in the console. If the vertex is not valid, you must try again until selecting a valid one. Then, you will be given a list of choices for the end vertex. This list will contain all vertices located on the same 'island' as the starting vertex. After entering a valid end vertex into the console, the shortest path between the two vertices will be printed. Below that, the PageRank statistics will be printed for every vertex on said path.

## Test Suite

Our `tests.cpp` file is located in the `tests` folder. In this file, we have included testing on the `Graph` structure, `readfromFile`, our `Edge` class, as well as the `vertexTraversal` which is iterates through the graph with BFS. The `tests.cpp` file also includes testing on Dijkstra's algorithm. To run the test suite, enter `make` into the console and press enter, followed by `make test`.

## Link to Presentation

Our presentation can be found in the document `LinkToPresentation`
