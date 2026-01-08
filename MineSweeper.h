#pragma once //防止头文件被重复包含 
#include <graphics.h>
#include <conio.h>//控制台交互 
#include <ctime>
#include<iostream>
#include<windows.h>
using namespace std;

// 难度定义
#define DIFFICULTY_EASY 1
#define DIFFICULTY_MID 2
#define DIFFICULTY_HARD 3

// 地图参数（按难度）
#define BLOCK_SIZE 40 //一个地雷格子 
#define EASY_ROW 9//row行 
#define EASY_COL 9//col列 
#define EASY_MINE 10
#define MID_ROW 16
#define MID_COL 16
#define MID_MINE 40
#define HARD_COL 30
#define HARD_ROW 16
#define HARD_MINE 99

// 窗口参数（含信息栏50像素）
#define INFO_AREA_HEIGHT 50
#define EASY_WIN_C (EASY_COL * BLOCK_SIZE)
#define EASY_WIN_R (EASY_ROW * BLOCK_SIZE+INFO_AREA_HEIGHT)
#define MID_WIN_C (MID_COL * BLOCK_SIZE)
#define MID_WIN_R (MID_ROW * BLOCK_SIZE+INFO_AREA_HEIGHT)
#define HARD_WIN_C (HARD_COL * BLOCK_SIZE)
#define HARD_WIN_R (HARD_ROW * BLOCK_SIZE+INFO_AREA_HEIGHT)

// 方块状态枚举
#define EMPTY 0
#define MINE 9
#define FLAGGED 1
#define QUESTION 2
#define OPENED 3
  
// 数字颜色宏（经典配色）
#define DARKBLUE RGB(0,0,128)
#define GRAY RGB(192,192,192)

#define NUM_COLOR_1 BLUE
#define NUM_COLOR_2 GREEN
#define NUM_COLOR_3 RED
#define NUM_COLOR_4 DARKBLUE
#define NUM_COLOR_5 BROWN
#define NUM_COLOR_6 CYAN
#define NUM_COLOR_7 BLACK
#define NUM_COLOR_8 GRAY
//main.cpp
void initAll();
void draw_Win_and_Buffer(int map[2][16][30],int selectedDifficulty,int gametime,int flagCount,bool isShowPopup);

// 数据层 MineData.cpp
void initMineMap(int map[2][16][30], int difficulty, int firstClickRow, int firstClickCol);
bool updateBlockState(int map[2][16][30], int row, int col, int newState, int maxRow, int maxCol);
bool checkGameWin(int map[2][16][30], int maxRow, int maxCol, int mineCount);
int getMineCountByDifficulty(int difficulty);
void getMapSizeByDifficulty(int difficulty, int &maxRow, int &maxCol);

// 交互层 MineInteraction.cpp
void handleMouseEvent(int map[2][16][30], int difficulty, bool &isGameOver, int &isGameWin, 
                     int &flagCount, bool &isFirstClick, bool &isShowPopup, 
					 bool &isRestart,clock_t &startTime,bool &isTimerRunning);
void expandBlankArea(int map[2][16][30], int row, int col, int maxRow, int maxCol);
void startGameTimer(clock_t &startTime, bool &isTimerRunning, bool isReset);
int calcGameTime(clock_t startTime, bool isTimerRunning);
void handleFlagOperation(int map[2][16][30], int row, int col, int &flagCount);
void clear();

// 绘制层 MineGraphics.cpp
void initGraphicsWindow(int difficulty);
void drawMineMap(int map[2][16][30], int selectedDifficulty,bool isShowPopup);
void drawSingleBlock(int pixelX, int pixelY,int state,int mineCountAround,bool isShowPopup);
void drawGameInfo(int difficulty, int gameTime, int flagCount, int totalMine);
void drawGameOverUI(int isWin, int gameTime, int winWidth, int winHeight, bool &isRestart, bool &isQuit);
void drawDifficultySelect(int &selectedDifficulty);
