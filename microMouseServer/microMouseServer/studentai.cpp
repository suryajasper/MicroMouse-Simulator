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
void fixPos(int dir, struct vec2 *pos, int v[20][20]) {
    struct vec2 move = { (dir % 2 == 0) ? 0 : -(dir-2) , (dir % 2 == 0) ? -(dir - 1): 0};
    *pos = {pos->xPos + move.xPos, pos->yPos + move.yPos};
    v[pos->xPos][pos->yPos] += 1;
}

void microMouseServer::studentAI()
{
    static int dir = 0;
    static struct vec2 pos = {0, 0};
    static int visited[20][20] = {};
    static bool backtracking = false;

    //left
    struct vec2 left = {0, 0};
    if (dir % 2 == 0) left = {dir - 1, 0};
    else left = {0, 2 - dir};

    //right
    struct vec2 right = {-left.xPos, -left.yPos};

    //fwd
    struct vec2 fwd = {0, 0};
    if (dir % 2 == 0) fwd = {right.yPos, right.xPos};
    else fwd = {left.yPos, left.xPos};

    //values
    int leftVal = visited[pos.xPos+left.xPos][pos.yPos+left.yPos];
    int rightVal = visited[pos.xPos+right.xPos][pos.yPos+right.yPos];
    int fwdVal = visited[pos.xPos+fwd.xPos][pos.yPos+fwd.yPos];
    bool wallLeft = isWallLeft() || leftVal == INT_MAX;
    bool wallRight = isWallRight() || rightVal == INT_MAX;
    bool wallForward = isWallForward() || fwdVal == INT_MAX;

    int numWalls = int(wallLeft) + int(wallRight) + int(wallForward);
    short priority = 0;
    // No choice - deadend
    if (numWalls == 3) {
        backtracking = true;
        turnRight();
        turnRight();
        visited[pos.xPos][pos.yPos] = INT_MAX;
        dir = fixDir(dir+2);
        moveForward();
        fixPos(dir, &pos, visited);
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
        moveForward();
        fixPos(dir, &pos, visited);
    }
    // More than one choice
    else if (numWalls < 2 ) {
        backtracking = false;
        if (isWallLeft()) {
            priority = rightVal <= fwdVal ? 1 : 2;
        } else if (wallRight) {
            priority = leftVal <= fwdVal ? 0 : 2;
        } else if (wallForward) {
            priority = leftVal <= rightVal ? 0 : 1;
        }
        if (priority == 0) {
            turnLeft();
            dir = fixDir(dir-1);
        } else if (priority == 1) {
            turnRight();
            dir = fixDir(dir+1);
        }
        moveForward();
        fixPos(dir, &pos, visited);
    }
    printUI(("(" + std::to_string(pos.xPos) + ", " + std::to_string(pos.yPos) + ") curr: " + std::to_string(visited[pos.xPos][pos.yPos]) + " r: " + std::to_string(rightVal) + " l: " + std::to_string(leftVal) + " f: " + std::to_string(fwdVal)).c_str());
}
