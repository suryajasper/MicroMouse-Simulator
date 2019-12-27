
#include "micromouseserver.h"
using namespace std;

struct vec2 {
    int xPos;
    int yPos;
};

int fixDir(int curr) {
    if (curr < 0) return 4+curr;
    else return curr % 4;
}

void fixPos(int dir, struct vec2 *pos, int v[20][20], bool isDeadEnd) {
    struct vec2 move = { (dir % 2 == 0) ? 0 : -(dir-2) , (dir % 2 == 0) ? -(dir - 1): 0};
    *pos = {pos->xPos + move.xPos, pos->yPos + move.yPos};
    v[pos->xPos][pos->yPos] = isDeadEnd ? -1: 1;
}

void microMouseServer::studentAI()
{

    static int dir = 0;

    static struct vec2 pos = {0, 0};

    static int visited[20][20] = {{0}};

    int deadEndTest = 0;

    if (!isWallLeft() && pos.yPos != 19 && (pos.xPos > 0 && visited[pos.xPos-1][pos.yPos] != -1) ) {
        deadEndTest++;
        turnLeft();
        dir = fixDir(dir-1);
    }
    else if (!isWallRight() && pos.yPos != 19 && (pos.xPos < 20 && visited[pos.xPos+1][pos.yPos] != -1)) {
        deadEndTest++;
        turnRight();
        dir = fixDir(dir+1);
    }
    if (!isWallForward() && pos.yPos != 19){
        deadEndTest++;
        moveForward();
        fixPos(dir, &pos, visited, deadEndTest <= 1);
    }
    else {
        turnLeft();
        dir = fixDir(dir-1);
        visited[pos.xPos][pos.yPos] = -1;
    }

    printUI(("(" + std::to_string(pos.xPos) + ", " + std::to_string(pos.yPos) + ")").c_str());
    printUI(("direction: " + std::to_string(dir)).c_str());
}
