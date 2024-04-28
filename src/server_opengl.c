#include <stdio.h>
#include <stdlib.h>
#include <GL/glut.h>
#include <math.h> // 包含math.h以使用cos和sin函数
#include <stdbool.h>
#include <string.h>
#include "server_opengl.h"
#define DEG2RAD (3.14159/180.0) // 定义将度转换为弧度的常量

bool isValidPosition(int x, int y, game_data *game) {
    return x >= 0 && x < game->size && y >= 0 && y < game->size;
}

void flipInDirection(int startX, int startY, int deltaX, int deltaY ,game_data *game) {
    int x = startX + deltaX;
    int y = startY + deltaY;

    // 检查沿着(deltaX, deltaY)方向是否有可能翻转棋子
    int count = 0;
    while (isValidPosition(x, y, game) && game->grid[x][y] == 3 - game->currentPlayer) {
        x += deltaX;
        y += deltaY;
        count++;
    }

    // 如果找到了与当前玩家相同颜色的棋子，并且中间没有空位
    if (isValidPosition(x, y, game) && game->grid[x][y] == game->currentPlayer && count > 0) {
        // 回退一步开始翻转棋子
        x -= deltaX;
        y -= deltaY;
        while (count-- > 0) {
            game->grid[x][y] = game->currentPlayer;
            x -= deltaX;
            y -= deltaY;
        }
    }
}

void score(int gridX, int gridY, game_data *game) {
    game->grid[gridX][gridY] = game->currentPlayer;

    // 8个可能的方向向量
    int directions[8][2] = {{-1, 0}, {1, 0}, {0, -1}, {0, 1}, {-1, -1}, {1, 1}, {-1, 1}, {1, -1}};
    for (int i = 0; i < 8; i++) {
        int deltaX = directions[i][0];
        int deltaY = directions[i][1];
        flipInDirection(gridX, gridY, deltaX, deltaY, game);
    }
    switch_player(game);
}

void switch_player(game_data *game){

    game->currentPlayer = 3 - game->currentPlayer;
}

void free_board(game_data *game){

    for (int i = 0; i < game->size; ++i) {
        free(game->grid[i]);
    }
    free(game->grid);
}

void init_board(game_data *game) {
    printf("%s,%d\ngrid size:%d\n",__func__,__LINE__,game->size);
    game->currentPlayer = 1;
    // 分配新棋盘的内存
    game->grid = (int **)malloc(sizeof(int *) * game->size);
    for (int i = 0; i < game->size; ++i) {
        game->grid[i] = (int *)malloc(sizeof(int) * game->size);
        memset(game->grid[i], 0, sizeof(int) * game->size); // 初始化棋盘状态为0
    }
}