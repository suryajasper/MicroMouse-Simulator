#include "micromouseserver.h"

struct vec2 {
    int xPos;
    int yPos;
};
int fixDir(int curr) {
    if (curr < 0) return 4+curr;
    else return curr % 4;
}
void fixPos(int dir, struct vec2 *pos, int v[20][20], int marker) {
    struct vec2 move = { (dir % 2 == 0) ? 0 : -(dir-2) , (dir % 2 == 0) ? -(dir - 1): 0};
    *pos = {pos->xPos + move.xPos, pos->yPos + move.yPos};
    v[pos->xPos][pos->yPos] = marker;
}

void microMouseServer::studentAI()
{
    static int dir = 0;

    static struct vec2 pos = {0, 0};

    static int visited[20][20] = {{0}};

    static bool backtracking = false;

    struct vec2 left = {0, 0};
    if (dir % 2 == 0) left = {dir - 1, 0};
    else left = {0, 2 - dir};
    struct vec2 right = {-left.xPos, -left.yPos};
    int leftVal = visited[pos.xPos+left.xPos][pos.yPos+left.yPos];
    int rightVal = visited[pos.xPos+right.xPos][pos.yPos+right.yPos];
    short priority = rightVal < leftVal ? 1 : 0;
    if (rightVal < 0 || leftVal < 0) priority = rightVal < leftVal ? 0 : 1;
    int deadEndTest = int(!isWallLeft()) + int(!isWallRight()) + int(!isWallForward());

    if (priority == 0 && !isWallLeft() && leftVal != -1) {
        turnLeft();
        dir = fixDir(dir-1);
    }
    else if (!isWallRight() && rightVal != -1) {
        turnRight();
        dir = fixDir(dir+1);
    }
    if (!isWallForward()) {
        moveForward();
        if (backtracking && deadEndTest <= 1) {
            fixPos(dir, &pos, visited, -1);
        }
        else {
            backtracking = false;
            fixPos(dir, &pos, visited, 1);
        }
    }
    else {
        turnLeft();
        dir = fixDir(dir-1);
        visited[pos.xPos][pos.yPos] = -1;
        backtracking = true;
    }

    printUI(std::to_string(backtracking).c_str());
    printUI(("rightVal: " + std::to_string(rightVal)).c_str());
    printUI(("leftVal: " + std::to_string(leftVal)).c_str());
    //printUI(("(" + std::to_string(pos.xPos) + ", " + std::to_string(pos.yPos) + ")").c_str());
    //printUI(("direction: " + std::to_string(dir)).c_str());
}
