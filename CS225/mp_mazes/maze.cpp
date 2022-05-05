/* Your code here! */
#include "maze.h"

SquareMaze::SquareMaze() {
    height_ = 0;
    width_ = 0;
}

void SquareMaze::makeMaze(int width, int height) {
    width_ = width;
	height_ = height;

    maze_.resize(width_, vector<Cell>(height_, Cell(true, true)));
	sets_.addelements(width_ * height_);

    //Connect maze
    while (sets_.size(0) != width_ * height_) {
        int coordinates1D = rand() % (width_*height_);
        pair<int,int> coordinates2D = find2DIndex(coordinates1D);
        //0-right, 1-bottom
        int wall = rand() % 2;

        if (!isValid(coordinates2D.first,coordinates2D.second, wall)) {
            continue;
        }

        if (wall == 0) {
        maze_[coordinates2D.first][coordinates2D.second].right_ = false;
        sets_.setunion(coordinates1D, find1DIndex(coordinates2D.first + 1, coordinates2D.second));
        } else {
        maze_[coordinates2D.first][coordinates2D.second].bottom_ = false;
        sets_.setunion(coordinates1D, find1DIndex(coordinates2D.first, coordinates2D.second + 1));
        }
    }
}

bool SquareMaze::canTravel (int x, int y, int dir) const {
    switch(dir) {
        case 0 :
        //If at right bound
        if (x >= width_ - 1) {
            return false;
        }
        return (!maze_[x][y].right_);
        
        case 1 :
        if (y >= height_ - 1) {
            return false;
        }
        return (!maze_[x][y].bottom_);
        
        case 2 :
        if (x <= 0) {
            return false;
        }
        return (!maze_[x - 1][y].right_);

        case 3 :
        if (y <= 0) {
            return false;
        }
        return (!maze_[x][y-1].bottom_);

        default :
        return false;
   }
   return false;
}

void SquareMaze::setWall (int x, int y, int dir, bool exists) {
    //Doesn't update disjoint sets
    if (x < 0 || x >= width_ || y < 0 || y >= height_) {
        return;
    }
    switch(dir) {
        case 0 :
        //int nextCell = findLinearIndex(x + 1, y);
        maze_[x][y].right_ = exists;
        return;
        
        case 1 :
        //int nextCell = findLinearIndex(x + 1, y);
        maze_[x][y].bottom_ = exists;
        return;
        
        case 2 :
        //int nextCell = findLinearIndex(x + 1, y);
        if (x - 1 < 0) {
            return;
        }
        maze_[x-1][y].right_ = exists;
        return;

        case 3 :
        if (y - 1 < 0) {
            return;
        }
        //int nextCell = findLinearIndex(x + 1, y);
        maze_[x][y-1].bottom_ = exists;
        return;
    }
}

vector<int> SquareMaze::solveMaze() {
    //Each cell has only one parent. This stores that coordinate
    vector<vector<pair<int,int>>> previous;
    //Unvisited
    previous.resize(width_, vector<pair<int,int>>(height_, pair<int,int>(-1,-1)));
    //Each cell's distance from the beginning
    vector<vector<int>> distance;
    distance.resize(width_, vector<int>(height_, -1));

    previous[0][0] = pair<int,int>(0,0);
    distance[0][0] = 0;

    //Finding all the paths and their lengths
    queue<pair<int,int>> bfs;
    //Starts at top left
    pair<int,int> curr = pair<int,int>(0,0);
    bfs.push(curr);
    while(!bfs.empty()){
        curr = bfs.front();
        int x = curr.first;
        int y = curr.second;
        bfs.pop();

        if(canTravel(x, y, 0) && previous[x + 1][y] == pair<int,int>(-1,-1)){
            bfs.push(pair<int,int>(x + 1, y));
            previous[x + 1][y] = curr;
            distance[x + 1][y] = distance[x][y] + 1;
        }
        if(canTravel(x, y, 1) && previous[x][y + 1] == pair<int,int>(-1,-1)){
            bfs.push(pair<int,int>(x, y + 1));
            previous[x][y + 1] = curr;
            distance[x][y + 1] = distance[x][y] + 1;
        }
        if(canTravel(x, y, 2) && previous[x - 1][y] == pair<int,int>(-1,-1)){
            bfs.push(pair<int,int>(x - 1, y));
            previous[x - 1][y] = curr;
            distance[x - 1][y] = distance[x][y] + 1;
        }
        if(canTravel(x, y, 3) && previous[x][y - 1] == pair<int,int>(-1,-1)){
            bfs.push(pair<int,int>(x, y - 1));
            previous[x][y - 1] = curr;
            distance[x][y - 1] = distance[x][y] + 1;
        }
    }

    //Finding longest path by looking at all the cells in the bottom row
    pair<int,int> longest(0, height_ - 1);

    for (int x = 0; x < width_; ++x) {
        if(distance[x][height_ - 1] > distance[longest.first][longest.second]){
            longest = pair<int, int>(x, height_ - 1);
        }
    }

    end_ = longest;

    vector<int> path;
    int x = longest.first;
    int y = longest.second;
    //Backwards
    while(longest != pair<int,int>(0, 0)){
        x = longest.first;
        y = longest.second;

        if(previous[x][y] == pair<int,int>(x + 1, y)){
            path.push_back(2);
        }
        if(previous[x][y] == pair<int,int>(x, y + 1)){
            path.push_back(3);
        }
        if(previous[x][y] == pair<int,int>(x - 1, y)){
            path.push_back(0);
        }
        if(previous[x][y] == pair<int,int>(x, y - 1)){
            path.push_back(1);
        }
        longest = previous[x][y];
    }
    reverse(path.begin(), path.end());

    /**
     * DELETE THIS
     */
    // for(size_t i = 0; i < path.size(); i++) {
    //     cout << path.at(i) << endl;
    // }
    /**
     * DELETE THIS
     */

    return path;
}

PNG* SquareMaze::drawMaze() const {
    PNG* result = new PNG(width_ * 10 + 1, height_ * 10 + 1);

    //UPPER LEFT
    for (int i = 0; i < width_ * 10 + 1; ++i) {
        if (i < 1 || i > 9) {
        result->getPixel(i, 0).l = 0;
        }
    }
    for (int i = 0; i < height_ * 10 + 1; i++) {
        result->getPixel(0, i).l = 0;
    }

    for (int x = 0; x < width_; ++x) {
        for (int y = 0; y < height_; ++y) {
            //Right
            if (maze_[x][y].right_) {
                for (int k = 0; k <= 10; k++) {
                    result->getPixel((x+1)*10, y*10+k).l = 0;
                }
            }
            //Bottom
            if (maze_[x][y].bottom_) {
                for (int k = 0; k <= 10; ++k) {
                    result->getPixel(x*10+k, (y+1)*10).l = 0;
                }
            }
        }
    }
    
    return result;
}

PNG* SquareMaze::drawMazeWithSolution(){
    PNG* png = drawMaze();
    vector<int> solution = solveMaze();
    //White the exit minus 1 bc single pixel lol
    for (int k = 1; k <= 9; ++k) {
        png->getPixel(end_.first * 10 + k, (end_.second + 1) * 10).l = 1;
    }

    //Start at 5,5
    int x = 5;
    int y = 5;
    for(size_t i = 0; i < solution.size(); ++i){
        switch (solution[i]) {
            case 0:
            for(int k = 0; k <= 10; ++k){
                HSLAPixel& p = png->getPixel(x + k, y);
                p.h = 0;
                p.s = 1;
                p.l = 0.5;
                p.a = 1;
            }
            x += 10;
            continue;

            case 1:
            for(int k = 0; k <= 10; ++k){
                HSLAPixel& p = png->getPixel(x, y + k);
                p.h = 0;
                p.s = 1;
                p.l = 0.5;
                p.a = 1;
            }
            y += 10;
            continue;

            case 2:
            for(int k = 0; k <= 10; ++k){
                HSLAPixel& p = png->getPixel(x - k, y);
                p.h = 0;
                p.s = 1;
                p.l = 0.5;
                p.a = 1;
            }
            x -= 10;
            continue;

            case 3:
            for(int k = 0; k <= 10; ++k){
                HSLAPixel& p = png->getPixel(x, y - k);
                p.h = 0;
                p.s = 1;
                p.l = 0.5;
                p.a = 1;
            }
            y -= 10;
            continue;
        }
    }

    return png;
}

int SquareMaze::find1DIndex(int x, int y) {
    return (y * width_) + x;
}

pair<int,int> SquareMaze::find2DIndex(int i) {
    return pair<int,int>(i % width_, i / width_);
}

bool SquareMaze::isValid(int x, int y, int wall) {
    //Check if perimeter wall
    if ((x >= width_ - 1 && wall == 0) || (y >= height_ - 1 && wall == 1)) {
        return false;
    }

    int current = find1DIndex(x, y);
    int adjacent = 0;
    if (wall == 0) {
        adjacent = find1DIndex(x + 1, y);
    } else {
        adjacent = find1DIndex(x, y + 1);
    }
    
    //Would create a cycle
    if (sets_.find(current) == sets_.find(adjacent)) {
        return false;
    }

    return true;
}
