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

    int deadEndTest = 0;

    if (!isWallLeft() && visited[pos.xPos-1][pos.yPos] != -1) {
        deadEndTest++;
        turnLeft();
        dir = fixDir(dir-1);
    }
    else if (!isWallRight() && visited[pos.xPos+1][pos.yPos] != -1) {
        deadEndTest++;
        turnRight();
        dir = fixDir(dir+1);
    }
    if (!isWallForward()) {
        moveForward();
        fixPos(dir, &pos, visited, (backtracking && ++deadEndTest <= 1) ? -1: 1);
    }
    else {
        turnLeft();
        dir = fixDir(dir-1);
        visited[pos.xPos][pos.yPos] = -1;
        backtracking = true;
    }

    printUI(std::to_string(backtracking).c_str());
    printUI(("(" + std::to_string(pos.xPos) + ", " + std::to_string(pos.yPos) + ")").c_str());
    printUI(("direction: " + std::to_string(dir)).c_str());
}
