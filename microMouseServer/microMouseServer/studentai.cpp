#include "micromouseserver.h"
#include <unistd.h>
#include <limits.h>

struct vec2 {
    int xPos;
    int yPos;
};
int fixDir(int curr) {
    if (curr < 0) return 4+curr;
    else return curr % 4;
}
bool fixPos(int dir, struct vec2 *pos, int v[20][20], std::vector<vec2> *moves) {
    struct vec2 move = { (dir % 2 == 0) ? 0 : -(dir-2) , (dir % 2 == 0) ? -(dir - 1): 0};
    *pos = {pos->xPos + move.xPos, pos->yPos + move.yPos};
    v[pos->xPos][pos->yPos] += 1;
    moves->push_back({pos->xPos, pos->yPos});
    if (moves->size() > 5) moves->erase(moves->begin());
    return moves->size() > 4 && (moves->at(0).xPos == pos->xPos && moves->at(0).yPos == pos->yPos) && (abs(moves->at(2).xPos - pos->xPos) == 1 && abs(moves->at(2).yPos - pos->yPos) == 1) && ((abs(moves->at(1).xPos - pos->xPos) + abs(moves->at(1).yPos - pos->yPos)) == 1) && ((abs(moves->at(3).xPos - pos->xPos) + abs(moves->at(3).yPos - pos->yPos)) == 1);
}

void microMouseServer::studentAI()
{
    static int dir = 0;
    static struct vec2 pos = {0, 0};
    static int visited[20][20] = {};
    static bool backtracking = false;
    static std::vector<vec2> steps = {};

    struct vec2 left = {(dir % 2 == 0) ? (dir - 1) : 0, (dir % 2 == 0) ? 0 : (2 - dir)}; // left factor
    struct vec2 right = {-left.xPos, -left.yPos}; // right factor
    struct vec2 fwd = {(dir % 2 == 0) ? right.yPos : left.yPos, (dir % 2 == 0) ? right.xPos : left.xPos}; // forward factor
    int leftVal = visited[pos.xPos+left.xPos][pos.yPos+left.yPos]; // value of matrix at left
    int rightVal = visited[pos.xPos+right.xPos][pos.yPos+right.yPos]; // value of matrix at right
    int fwdVal = visited[pos.xPos+fwd.xPos][pos.yPos+fwd.yPos]; // value of matrix forward
    bool wallLeft = isWallLeft() || leftVal == INT_MAX; // is there a wall or a dead end to the left?
    bool wallRight = isWallRight() || rightVal == INT_MAX; // is there a wall or a dead end to the right?
    bool wallForward = isWallForward() || fwdVal == INT_MAX; // is there a wall or a dead end forward?

    int numWalls = int(wallLeft) + int(wallRight) + int(wallForward);

    // No choice (dead end)
    if (numWalls == 3) {
        backtracking = true;
        turnRight();
        turnRight();
        visited[pos.xPos][pos.yPos] = INT_MAX;
        dir = fixDir(dir+2);
        steps.push_back({pos.xPos, pos.yPos});
        if (steps.size() > 5) steps.erase(steps.begin());
    }
    // Only one choice
    else if (numWalls == 2) {
        if (!wallLeft) {
            turnLeft();
            dir = fixDir(dir-1);
        } else if (!wallRight ) {
            turnRight();
            dir = fixDir(dir+1);
        }
        if (backtracking)
            visited[pos.xPos][pos.yPos] = INT_MAX;
    }
    // More than one choice
    else if (numWalls < 2 ) {
        backtracking = false;
        short priority = 0;
        if (wallLeft)
            priority = rightVal <= fwdVal ? 1 : 2;
        else if (wallRight)
            priority = leftVal <= fwdVal ? 0 : 2;
        else if (wallForward)
            priority = leftVal <= rightVal ? 0 : 1;
        if (priority == 0) {
            turnLeft();
            dir = fixDir(dir-1);
        } else if (priority == 1) {
            turnRight();
            dir = fixDir(dir+1);
        }
    }
    moveForward();
    if (fixPos(dir, &pos, visited, &steps))
        foundFinish();
}
