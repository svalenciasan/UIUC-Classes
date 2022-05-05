### **Project Proposal (fcoron5-svalen27-ada6-abdisl2)**


### Leading Question:



*   The first question we want to answer is to be able to find the shortest connection path between 2 sellers through their connections.
*   The second question we want to solve is finding the correlation between how often a person is traded with and their ranking.


### Dataset Acquisition and Processing:



*   Link to dataset: [http://snap.stanford.edu/data/soc-sign-bitcoin-otc.html](http://snap.stanford.edu/data/soc-sign-bitcoin-otc.html)
*   Our data comes in the form of a .csv file. We will download it from the Stanford University database, and import it into VSCode. 
*   If there are errors with the data file, we will re-import it from Stanford’s database. Missing entries in the file can be ignored safely for the purposes of our algorithms.
*   Data is organized in lines where the entries in every line are separated by commas. The first entry is the ID of the rater, the second is the ID of the person being rated, the third is the rating from -10 to 10, and the last is a timestamp of the rating. Using this format we can easily read and grab the data we need from the dataset, this data being the first three entries.


### Graph Algorithms:



*   We will traverse the graph by BFS. We will input a node/seller and traverse through the graph based on their transactions. We expect that the whole dataset will not be connected, and there will be islands of connected people. So if we want to traverse an island, then we will use someone that belongs to that island. For our algorithms, we will use Dijkstra’s Algorithm to find the shortest path between two users, with the weights of all edges set to the ratings each user has received. The input for this algorithm will be a starting user, and an end-user. We will use the PageRank algorithm for our second question, which is to return the probability that a new user trades with a known user along with its correlation with their ratings on the site. For both algorithms, we intend for our code to be O(n) or faster.

### Timeline:


*   Our tasks will be to implement Dijkstra’s algorithm, the PageRank algorithm, and our traversal of the graph. We will also need to convert the .csv file into VSCode, in a way that we can reference the data with C++ (JSON or string manipulation for example). We will complete at least one of the two algorithms by the Mid-Project Check-in. We will work on our project as agreed upon in the Team Contract to meet these goals.
