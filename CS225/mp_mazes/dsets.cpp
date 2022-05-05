/* Your code here! */
#include "dsets.h"

void DisjointSets::addelements(int elements) {
    while (elements > 0) {
        trees.push_back(-1);
        --elements;
    } 
}

int DisjointSets::find(int element) {
    if (element > (int)trees.size()) {
        return -1;
    }
    if (trees[element] < 0) {
        return element;
    } else {
        trees[element] = find(trees[element]);
        return trees[element];
    }
}

void DisjointSets::setunion(int firstIndex,int secondIndex) {
    int root1 = find(firstIndex);
    int root2 = find(secondIndex);
    int newSize = trees[root1] + trees[root2];

    if (trees[root1] < trees[root2]) {
        trees[root2] = root1;
        trees[root1] = newSize;
    } else {
        trees[root1] = root2;
        trees[root2] = newSize;
    }
}

int DisjointSets::size(int element) {
    return trees[find(element)]  * -1;
}