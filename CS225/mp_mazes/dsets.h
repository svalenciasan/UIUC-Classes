/* Your code here! */
#pragma once
#include <vector>

using std::vector;

class DisjointSets {
    public:
        DisjointSets() = default;

        void addelements(int element);
        int find(int element);
        void setunion(int firstIndex,int secondIndex);
        int size(int element);

    private:
        vector<int> trees;
};
