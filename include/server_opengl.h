#include <stdio.h>
#include <stdlib.h>
#include <GL/glut.h>
#include <math.h> // 包含math.h以使用cos和sin函数
#include <stdbool.h>
#define DEG2RAD (3.14159/180.0) // 定义将度转换为弧度的常量

typedef struct
{
    int currentPlayer;
    int size;
    int **grid;
} game_data;

bool isValidPosition(int x, int y, game_data *game);
void flipInDirection(int startX, int startY, int deltaX, int deltaY ,game_data *game);
void score(int gridX, int gridY, game_data *game);
void init_board(game_data *game);
void free_board(game_data *game);
void switch_player(game_data *game);