/**
 * @file kdtree.cpp
 * Implementation of KDTree class.
 */

#include <utility>
#include <algorithm>

using namespace std;

template <int Dim>
bool KDTree<Dim>::smallerDimVal(const Point<Dim>& first,
                                const Point<Dim>& second, int curDim) const
{
    /**
     * @todo Implement this function!
     */
    if (first[curDim] < second[curDim]) {
      return true;
    } else if (first[curDim] == second[curDim]) {
      return first < second;
    } else {
      return false;
    }
}

template <int Dim>
bool KDTree<Dim>::shouldReplace(const Point<Dim>& target,
                                const Point<Dim>& currentBest,
                                const Point<Dim>& potential) const
{
    /**
     * @todo Implement this function!
     */
    double currDist = 0;
    double potentialDist = 0;
    for (auto i = 0; i < Dim; ++i) {
      currDist += pow(target[i] - currentBest[i], 2);
      potentialDist += pow(target[i] - potential[i], 2);
    }

    if (potentialDist < currDist) {
      return true;
    } else if (potentialDist == currDist) {
      return potential < currentBest;
    } else {
      return false;
    }
}

template <int Dim>
KDTree<Dim>::KDTree(const vector<Point<Dim>>& newPoints)
{
    /**
     * @todo Implement this function!
     */
    vector<Point<Dim>> points = newPoints;
    root = buildTree(points, 0, newPoints.size() - 1, 0);
    size = newPoints.size();
}

/**
 * @todo HELPERS
 */
template <int Dim>
typename KDTree<Dim>::KDTreeNode* KDTree<Dim>::buildTree(vector<Point<Dim>>& list, int left, int right, int dimension) {
  if (right < left) {
    return nullptr;
  }
  int middleIndex = (left + right) / 2;
  select(list, left, right, middleIndex, dimension);
  KDTreeNode* currRoot = new KDTreeNode(list[middleIndex]);
  currRoot->left = buildTree(list, left, middleIndex - 1, (dimension + 1) % Dim);
  currRoot->right = buildTree(list, middleIndex + 1, right, (dimension + 1) % Dim);
  return currRoot;
}

template <int Dim>
void KDTree<Dim>::select(vector<Point<Dim>>& list, int left, int right, int k, int dimension) {
  if (right < left) {
    return;
  }
  //Could be randomn
  int pivotIndex = (left + right) / 2;
  pivotIndex = partition(list, left, right, pivotIndex, dimension);

  if (k == pivotIndex) {
    return;
  } else if (k < pivotIndex) {
    select(list, left, pivotIndex - 1, k, dimension);
  } else {
    select(list, pivotIndex + 1, right, k, dimension);
  }
}

template <int Dim>
int KDTree<Dim>::partition(vector<Point<Dim>>& list, int left, int right, int pivotIndex, int dimension) {
  //Store pivotValue
  Point<Dim> pivotValue = list[pivotIndex];
  //Swap
  swap(list, pivotIndex, right);
  //Store index
  int storeIndex = left;

  for (int i = left; i <= right - 1; ++i) {
    if (smallerDimVal(list[i], pivotValue, dimension)) {
      swap(list, storeIndex, i);
      ++storeIndex;
    }
  }
      
  swap(list, right, storeIndex);// Move pivot to its final place
  return storeIndex;
}

template <int Dim>
void KDTree<Dim>::swap(vector<Point<Dim>>& list, int first, int second) {
  Point<Dim> temp = list[first];
  list[first] = list[second];
  list[second] = temp;
}

template <int Dim>
KDTree<Dim>::KDTree(const KDTree<Dim>& other) {
  /**
   * @todo Implement this function!
   */
  root = copyTree(other.root);
  size = other.size;
}

/**
 * Copy Helper
 */
template <int Dim>
typename KDTree<Dim>::KDTreeNode* KDTree<Dim>::copyTree(KDTreeNode* currRoot) {
  if (currRoot == nullptr) {
    return nullptr;
  }
  KDTreeNode* newRoot = new KDTreeNode(currRoot->point);
  newRoot->right = copyTree(currRoot->right);
  newRoot->left = copyTree(currRoot->left);
  return newRoot;
}

template <int Dim>
const KDTree<Dim>& KDTree<Dim>::operator=(const KDTree<Dim>& rhs) {
  /**
   * @todo Implement this function!
   */
  if (this == rhs) {
    return *this;
  }
  clearTree(root);
  root = copyTree(rhs);
  size = rhs.size;
  return *this;
}

template <int Dim>
KDTree<Dim>::~KDTree() {
  /**
   * @todo Implement this function!
   */
  clearTree(root);
}

/**
 * Destroy helper.
 */
template <int Dim>
void KDTree<Dim>::clearTree(KDTreeNode* currRoot) {
  if (currRoot == nullptr) {
    return;
  }
  clearTree(currRoot->left);
  clearTree(currRoot->right);
  delete currRoot;
}

template <int Dim>
Point<Dim> KDTree<Dim>::findNearestNeighbor(const Point<Dim>& query) const
{
    /**
     * @todo Implement this function!
     */

    return findNearestNeighborHelper(root, query, 0);
}

/**
 * Nearest Neighbor Helper.
 */
template <int Dim>
Point<Dim> KDTree<Dim>::findNearestNeighborHelper(KDTreeNode* currNode, const Point<Dim>& query, int dimension) const{
  bool hasGoneLeft = false;
  //Recursing fowards
  if (currNode->left == nullptr && currNode->right == nullptr) {
    return currNode->point;
  }

  Point<Dim> currentBest;

  if (smallerDimVal(query, currNode->point, dimension)) {
    if (currNode->left != nullptr) {
      currentBest = findNearestNeighborHelper(currNode->left, query, (dimension + 1) % Dim);
      hasGoneLeft = true;
    } else {
      currentBest = findNearestNeighborHelper(currNode->right, query, (dimension + 1) % Dim);
    }
  } else {
    if (currNode->right != nullptr) {
      currentBest = findNearestNeighborHelper(currNode->right, query, (dimension + 1) % Dim);
    } else {
      currentBest = findNearestNeighborHelper(currNode->left, query, (dimension + 1) % Dim);
      hasGoneLeft = true;
    }
  }
  //Recursing backwards

  //Checks if current node point is better
  if (shouldReplace(query,currentBest,currNode->point)) {
    currentBest = currNode->point;
  }

  //Finds radius between currentBest and query
  double radius = 0;
  for (auto i = 0; i < Dim; ++i) {
    radius += pow(query[i] - currentBest[i], 2);
  }
  //Finds split distance
  double splitDist = pow(currNode->point[dimension] - query[dimension], 2);

  if (radius >= splitDist) {
    Point<Dim> tmpNearest;
    if (hasGoneLeft) {
      if (currNode->right != nullptr) {
        tmpNearest = findNearestNeighborHelper(currNode->right, query, (dimension + 1) % Dim);
        
        if (shouldReplace(query, currentBest, tmpNearest)) {
          currentBest = tmpNearest;
        }
      }
    } else {
      if (currNode->left != nullptr) {
        tmpNearest = findNearestNeighborHelper(currNode->left, query, (dimension + 1) % Dim);

        if (shouldReplace(query, currentBest, tmpNearest)) {
          currentBest = tmpNearest;
        }
      }
    }
  }
  return currentBest;
}