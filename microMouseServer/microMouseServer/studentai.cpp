#include "micromouseserver.h"
#include <unistd.h>
#include <limits.h>

struct vec2 { // a struct that represents a point in a coordinate plane with an x-value and a y-value
    int xPos; // x-value of the coordinate
    int yPos; // y-value of the coordinate
};
int fixDir(int curr) { // constrains the direction to a value between 0 and 3
    if (curr < 0) return 4+curr; // if the direction is negative (meaning we turned left when we were facing forward)
    else return curr % 4; // if the direction is positive, we use the modulus operator to limit it to a value between 0 and 3
}
bool fixPos(int dir, struct vec2 *pos, int v[20][20], std::vector<vec2> *moves) { // changes the position of the mouse in our 2d array to match what happened on the screen
    struct vec2 move = { (dir % 2 == 0) ? 0 : -(dir-2) , (dir % 2 == 0) ? -(dir - 1): 0}; // calculates how far we have to move along the x-axis and y-axis
    *pos = {pos->xPos + move.xPos, pos->yPos + move.yPos}; // modifies the position (passed as a pointer) accordingly
    v[pos->xPos][pos->yPos] += 1; // increments the value of the new location in the 2d array so we know that we visited that point again
    moves->push_back({pos->xPos, pos->yPos}); // adds the current position to our vector that contains the last 5 positions (we need to know this to find the finish)
    if (moves->size() > 5) moves->erase(moves->begin()); // removes the first element of our vector because we really only need the last 5 to know whether we found the finish or not
    return moves->size() > 4 && (moves->at(0).xPos == pos->xPos && moves->at(0).yPos == pos->yPos) && (abs(moves->at(2).xPos - pos->xPos) == 1 && abs(moves->at(2).yPos - pos->yPos) == 1) && ((abs(moves->at(1).xPos - pos->xPos) + abs(moves->at(1).yPos - pos->yPos)) == 1) && ((abs(moves->at(3).xPos - pos->xPos) + abs(moves->at(3).yPos - pos->yPos)) == 1); // calculates whether the finish was found or not and returns a boolean
}

void microMouseServer::studentAI()
{
    static int dir = 0; // represents direction
    static struct vec2 pos = {0, 0}; // represents position
    static int visited[20][20] = {}; // a 2d array to represent each cell and information about it
    static bool backtracking = false; // a boolean that represents whether we are backtracking from a dead end or not
    static std::vector<vec2> steps = {}; // the vector that monitors each position

    struct vec2 left = {(dir % 2 == 0) ? (dir - 1) : 0, (dir % 2 == 0) ? 0 : (2 - dir)}; // left factor
    struct vec2 right = {-left.xPos, -left.yPos}; // right factor
    struct vec2 fwd = {(dir % 2 == 0) ? right.yPos : left.yPos, (dir % 2 == 0) ? right.xPos : left.xPos}; // forward factor
    int leftVal = visited[pos.xPos+left.xPos][pos.yPos+left.yPos]; // value of matrix at left
    int rightVal = visited[pos.xPos+right.xPos][pos.yPos+right.yPos]; // value of matrix at right
    int fwdVal = visited[pos.xPos+fwd.xPos][pos.yPos+fwd.yPos]; // value of matrix forward
    bool wallLeft = isWallLeft() || leftVal == INT_MAX; // is there a wall or a dead end to the left?
    bool wallRight = isWallRight() || rightVal == INT_MAX; // is there a wall or a dead end to the right?
    bool wallForward = isWallForward() || fwdVal == INT_MAX; // is there a wall or a dead end forward?

    int numWalls = int(wallLeft) + int(wallRight) + int(wallForward); // how many walls surround the mouse (doesn't include whether a wall is behind the mouse or not)

    if (numWalls == 3) { // No choice (dead end)
        backtracking = true; // we set backtracking to true because we reached a dead end and need to get back to a decision point
        turnRight(); // turn around 1/2
        turnRight(); // turn around fully
        visited[pos.xPos][pos.yPos] = INT_MAX; // set that point in the 2d array to the maximum value of an integer so that we know it is a dead end and it will never take priority
        dir = fixDir(dir+2); // fix direction accordingly (we turned 180 degrees)
        steps.push_back({pos.xPos, pos.yPos}); // add this position to our steps tracker (we didn't move forward and call the fixpos function so we do this manually)
        if (steps.size() > 5) steps.erase(steps.begin()); // resize our vector
    } else if (numWalls == 2) { // Only one choice
        if (!wallLeft) { // if there is no wall left, then that is the only possible path
            turnLeft();
            dir = fixDir(dir-1);
        } else if (!wallRight ) { // if there is no wall right, then that is the only possible path
            turnRight();
            dir = fixDir(dir+1);
        }
        if (backtracking) // if we are backtracking, then we still haven't come to a decision point, so we set this point in the 2d array to the max value of an int so we know that going here leads to a dead end
            visited[pos.xPos][pos.yPos] = INT_MAX;
    } else if (numWalls < 2 ) { // More than one choice
        backtracking = false; // we have reached a decision point so we can safely conclude that we don't need to backtrack anymore
        short priority = 0; // a variable to represent which route will take priority ( 0 = left, 1 = right, 2 = forward )
        if (wallLeft) // if there is a wall to the left, then we know we are deciding between right and forward
            priority = rightVal <= fwdVal ? 1 : 2; // if we have visited right less than we have forward, we go right and vice versa (if they have both been visited the same amount, right will take priority over forward)
        else if (wallRight) // if there is a wall to the right, then we know we are deciding between left and forward
            priority = leftVal <= fwdVal ? 0 : 2; // if we have visited left less than we have forward, we go left and vice versa (if they have both been visited the same amount, left will take priority over forward)
        else if (wallForward) // if there is a wall in front of us, then we know we are deciding between right and left
            priority = leftVal <= rightVal ? 0 : 1; // if we have visited left less than we have right, we go left and vice versa (if they have both been visited the same amount, left will take priority over right)
        if (priority == 0) { // if left is taking priority, we turn left
            turnLeft();
            dir = fixDir(dir-1);
        } else if (priority == 1) { // otherwise, if right is taking priority, we turn right
            turnRight();
            dir = fixDir(dir+1);
        }
    }
    moveForward(); // now that we've calculated which route to go to depending on the number of walls surrounding the mouse and turned accordingly, we can move forward
    if (fixPos(dir, &pos, visited, &steps)) // we fix the position since we've moved forward and need to represent that in the array
        foundFinish(); // if our function returns true (meaning we found the finish), we call the foundFinish() method
}
