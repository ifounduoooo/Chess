/*****************************************************************
                          Header file
******************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <GL/glut.h>
#include <math.h> // 包含math.h以使用cos和sin函数
#include <stdbool.h>
#include "client_opengl.h"
#include <string.h>
/*****************************************************************
                         Protocol parameter
******************************************************************/
#define USERNAME_TYPE 1
#define USERNAMELIST_TYPE 2
#define COORDINATE_TYPE 3
#define SIZE_TYPE 4
#define GRID_TYPE 5

/*****************************************************************
                           Global variable
******************************************************************/
#define DEG2RAD (3.14159/180.0) // 定义将度转换为弧度的常量
int **grid; // 棋盘状态数组，0表示空，1表示黑棋，2表示白棋
int size = 0;
int currentPlayer = 1; // 当前玩家，1为黑棋，2为白棋

/************************************************************************
                  Function prototype declaration
*************************************************************************/
static SendDataCallback sendDataCallback = NULL;

void register_send_data_callback(SendDataCallback callback) {
    sendDataCallback = callback;
}


void drawChessboard() {
    int i;
    glClear(GL_COLOR_BUFFER_BIT);

    glColor3f(0.0, 0.0, 0.0); // 黑色绘制棋盘线

    // 绘制棋盘的垂直线
    for (i = 0; i < size; i++) {
        glBegin(GL_LINES);
            glVertex2f(-((size-1)*0.1)/2 + i * 0.1, -((size-1)*0.1)/2);
            glVertex2f(-((size-1)*0.1)/2 + i * 0.1, ((size-1)*0.1)/2);
        glEnd();
    }

    // 绘制棋盘的水平线
    for (i = 0; i < size; i++) {
        glBegin(GL_LINES);
            glVertex2f(-((size-1)*0.1)/2, -((size-1)*0.1)/2 + i * 0.1);
            glVertex2f(((size-1)*0.1)/2, -((size-1)*0.1)/2 + i * 0.1);
        glEnd();
    }
}

void drawChessPiece(int x, int y, int player) {
    float fx = -((size-1)*0.1)/2 + x * 0.1;
    float fy = -((size-1)*0.1)/2 + y * 0.1;

    if (player == 1) glColor3f(0.0, 0.0, 0.0); // 黑棋
    else glColor3f(1.0, 1.0, 1.0); // 白棋

    glBegin(GL_POLYGON);
    for (int i = 0; i < 360; i++) {
        float degInRad = i * DEG2RAD;
        glVertex2f(cos(degInRad) * 0.05 + fx, sin(degInRad) * 0.05 + fy);
    }
    glEnd();
}

void displayCounts() {
    int countPlayer1 = 0, countPlayer2 = 0;
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            if (grid[i][j] == 1) countPlayer1++;
            else if (grid[i][j] == 2) countPlayer2++;
        }
    }

    char text[80];
    sprintf(text, "Player 1: %d, Player 2: %d", countPlayer1, countPlayer2);
    glColor3f(8.0, 0.4, 0.4); // 设置文字颜色
    glRasterPos2f(-0.9, -0.9); // 设置文字位置
    for (char* p = text; *p; p++) {
        glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, *p);
    }

    // 检查是否填满并宣布胜利者
    if (countPlayer1 + countPlayer2 == size * size) {
        sprintf(text, "Game Over: %s Wins!", (countPlayer1 > countPlayer2) ? "Player 1" : "Player 2");
        glRasterPos2f(-0.5, -0.8); // 调整位置显示游戏结束信息
        for (char* p = text; *p; p++) {
            glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, *p);
        }
    }
}


void display() {
    drawChessboard();
    // 绘制棋子
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            if (grid[i][j] != 0) {
                drawChessPiece(i, j, grid[i][j]);
            }
        }
    }
    displayCounts();
    glFlush(); // 确保前面的OpenGL命令立即执行
}

bool isValidPosition(int x, int y) {
    return x >= 0 && x < size && y >= 0 && y < size;
}

void flipInDirection(int startX, int startY, int deltaX, int deltaY) {
    printf("message send! Func:%s,Line:%d\n",__func__,__LINE__);
    int x = startX + deltaX;
    int y = startY + deltaY;

    // 检查沿着(deltaX, deltaY)方向是否有可能翻转棋子
    int count = 0;
    while (isValidPosition(x, y) && grid[x][y] == 3 - currentPlayer) {
        x += deltaX;
        y += deltaY;
        count++;
    }

    // 如果找到了与当前玩家相同颜色的棋子，并且中间没有空位
    if (isValidPosition(x, y) && grid[x][y] == currentPlayer && count > 0) {
        // 回退一步开始翻转棋子
        x -= deltaX;
        y -= deltaY;
        while (count-- > 0) {
            grid[x][y] = currentPlayer;
            x -= deltaX;
            y -= deltaY;
        }
    }
}

void score(int gridX, int gridY) {
    printf("message send! Func:%s,Line:%d\n",__func__,__LINE__);
    // 在点击的位置放置当前玩家的棋子
    grid[gridX][gridY] = currentPlayer;

    // 8个可能的方向向量
    int directions[8][2] = {{-1, 0}, {1, 0}, {0, -1}, {0, 1}, {-1, -1}, {1, 1}, {-1, 1}, {1, -1}};
    for (int i = 0; i < 8; i++) {
        int deltaX = directions[i][0];
        int deltaY = directions[i][1];
        flipInDirection(gridX, gridY, deltaX, deltaY);
    }

    // 切换当前玩家
    currentPlayer = 3 - currentPlayer;
}

void mouse(int button, int state, int x, int y) {
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
        // 获取当前窗口大小
        int windowWidth = glutGet(GLUT_WINDOW_WIDTH);
        int windowHeight = glutGet(GLUT_WINDOW_HEIGHT);

        // 将屏幕坐标转换为OpenGL坐标
        float nx = (float)x / (windowWidth / 2.0f) - 1.0f;
        float ny = 1.0f - (float)y / (windowHeight / 2.0f); // OpenGL的Y轴与屏幕坐标系相反

        // 考虑棋盘在OpenGL坐标系中的位置和大小，计算格子索引
        // 假设棋盘完全填充在-((size-1)*0.1)/2到((size-1)*0.1)/2的范围内，每个格子的大小为0.1
        float boardSize = (0.1*(size-1))*1.0f; // 棋盘大小，从-((size-1)*0.1)/2到((size-1)*0.1)/2
        float gridSize = boardSize / (size-1); // 每个格子的OpenGL大小
        int gridX = (int)round((nx + ((size-1)*0.1)/2) / gridSize);
        int gridY = (int)round((ny + ((size-1)*0.1)/2) / gridSize);

        // 检查点击是否在棋盘范围内
        if (gridX >= 0 && gridX < size && gridY >= 0 && gridY < size) {

            if (grid[gridX][gridY] == 0) { // 确保该位置为空
                if (sendDataCallback != NULL) {
                    int data[2] = {gridX, gridY}; // 假设x和y是你想发送的数据
                    sendDataCallback(data, COORDINATE_TYPE, sizeof(data));
                    printf("message send! Func:%s,Line:%d\n",__func__,__LINE__);
                }
                //glutPostRedisplay(); // 请求重绘OpenGL窗口
            }
        }
    }
}

void init_board() {
    printf("message send! Func:%s,Line:%d\n",__func__,__LINE__);
    // 先释放之前的棋盘（如果存在）
    if (grid != NULL) {
        for (int i = 0; i < size; ++i) {
            free(grid[i]);
        }
        free(grid);
    }
    // 分配新棋盘的内存
    grid = (int **)malloc(sizeof(int *) * size);
    for (int i = 0; i < size; ++i) {
        grid[i] = (int *)malloc(sizeof(int) * size);
        memset(grid[i], 0, sizeof(int) * size); // 初始化棋盘状态为0
    }
}

void init() {
    printf("message send! Func:%s,Line:%d\n",__func__,__LINE__);
    glClearColor(0.8, 0.8, 0.8, 1.0); // 设置背景颜色为灰色
    glColor3f(0.0, 0.0, 0.0); // 设置绘制颜色为黑色
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(-1.0, 1.0, -1.0, 1.0); // 设置视图投影参数
}

void idleFunc() {
    glutPostRedisplay();
}

void init_game(int argc, char** argv) {
    printf("message send! Func:%s,Line:%d\n",__func__,__LINE__);
    init_board();
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
    glutInitWindowSize(400, 400);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("棋盘");
    init();
    glutDisplayFunc(display);
    glutMouseFunc(mouse); // 注册鼠标回调函数
    glutIdleFunc(idleFunc); // 注册空闲时的回调函数
    glutMainLoop(); // 进入GLUT事件处理循环
}