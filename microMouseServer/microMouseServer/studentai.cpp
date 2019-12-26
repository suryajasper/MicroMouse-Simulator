
#include "micromouseserver.h"

struct vec2 {
    int xPos;
    int yPos;
};

int fixDir(int curr) {
    return curr % 4;
}

void fixPos(int dir, struct vec2 *pos) {
    struct vec2 move = { (dir % 2 == 0) ? 0 : -(dir-2) , (dir % 2 == 0) ? dir - 1: 0};
    *pos = {pos->xPos + move.xPos, pos->yPos + move.yPos};
}

void microMouseServer::studentAI()
{

    static int dir = 0;
    static struct vec2 pos = {0, 0};

    static short visited[20][20] = {{0}};

}
