/* Your code here! */
#pragma once

#include <vector>
#include <utility>
#include <stdlib.h>
#include <queue>
#include<iostream>

#include "cs225/PNG.h"
#include "cs225/HSLAPixel.h"
#include "dsets.h"

using std::vector;
using std::pair;
using std::queue;
using std::cout;
using std::endl;

using cs225::PNG;
using cs225::HSLAPixel;

class SquareMaze {
    public:
        SquareMaze();
        void makeMaze(int width, int height);

        /**
         * dir = 0 represents a rightward step (+1 to the x coordinate)
         * dir = 1 represents a downward step (+1 to the y coordinate)
         * dir = 2 represents a leftward step (-1 to the x coordinate)
         * dir = 3 represents an upward step (-1 to the y coordinate)
         */ 
        bool canTravel (int x, int y, int dir) const; 
        void setWall (int x, int y, int dir, bool exists);
        vector<int> solveMaze();
        PNG* drawMaze() const;
        PNG* drawMazeWithSolution();

    struct Cell {
		Cell(bool bottom, bool right) {
			bottom_ = bottom;
			right_ = right;
		}

        //true means wall
		bool bottom_ = true;
		bool right_ = true;
	};

    private:
        //Height by Width. Rows by Columns
        vector<vector<Cell>> maze_;
        DisjointSets sets_;
        //Coordinates of the end
        pair<int,int> end_;
        int height_;
        int width_;

        //Helper to turn a 2D coordinate to a 1D value corresponding to the DisjointSet
        int find1DIndex(int x, int y);
        //Return in (x,y) format
        pair<int,int> find2DIndex(int i);
        //If cycle and perimeter. 0-right, 1-bottom
        bool isValid(int x, int y, int wall);
};
