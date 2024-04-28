#include <stdio.h>
#include <stdlib.h>
#include <GL/glut.h>
#include <math.h> // 包含math.h以使用cos和sin函数
#include <stdbool.h>
#include <string.h>
#define DEG2RAD (3.14159/180.0) // 定义将度转换为弧度的常量

#define GRID_SIZE 16  // 棋盘大小
int **grid; // 棋盘状态数组，0表示空，1表示黑棋，2表示白棋
int currentPlayer; // 当前玩家，1为黑棋，2为白棋

void drawChessboard();

void drawChessPiece(int x, int y, int player);

void displayCounts();

void display();

bool isValidPosition(int x, int y);

void flipInDirection(int startX, int startY, int deltaX, int deltaY);

void score(int gridX, int gridY);

void mouse(int button, int state, int x, int y);

void init();

void init_game(int argc, char** argv);

typedef void (*SendDataCallback)(void *data,int type, size_t size);

void register_send_data_callback(SendDataCallback callback);

void init_board();