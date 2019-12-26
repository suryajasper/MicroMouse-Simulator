
#include "micromouseserver.h"

struct vec2 {
    int xPos;
    int yPos;
};

int fixDir(int curr) {
    return curr % 4;
}

void fixPos(int dir, struct vec2 *pos, int v[20][20], bool isDeadEnd) {
    struct vec2 move = { (dir % 2 == 0) ? 0 : -(dir-2) , (dir % 2 == 0) ? dir - 1: 0};
    *pos = {pos->xPos + move.xPos, pos->yPos + move.yPos};
    v[pos->xPos][pos->yPos] = isDeadEnd ? -1: 1;
}

void microMouseServer::studentAI()
{

    static int dir = 0;
    static struct vec2 pos = {0, 0};

    static int visited[20][20] = {{0}};

    if (!isWallLeft() && (pos.xPos > 0 && visited[pos.xPos-1][pos.yPos] != -1) ) {
        turnLeft();
    }
    else if (!isWallRight() && (pos.xPos < 20 && visited[pos.xPos+1][pos.yPos] != -1))
        turnRight();
    bool isDeadEnd = false;
    if (!isWallForward()) {
        moveForward();
    }
    else {
        turnLeft();
        isDeadEnd = true;
    }
    fixPos(dir, &pos, visited, isDeadEnd);
}
