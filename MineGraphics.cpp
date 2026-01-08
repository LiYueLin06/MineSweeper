// MineGraphics.cpp
#include "MineSweeper.h"
#include<stdio.h>
#include<string.h>
// 初始化图形窗口
extern IMAGE buffer_img; 
extern int c,r;
void initGraphicsWindow(int difficulty) {
    initgraph(c,r,SHOWCONSOLE);//同时显示绘图和控制台窗口，方便调试 
    setbkcolor(RGB(192, 192, 192)); // 设置背景色为灰色
    cleardevice();
}
// 绘制单个方块
void drawSingleBlock(int pixelX,int pixelY,int state,int mineCountAround,bool isShowPopup){
    // 绘制方块背景
    COLORREF bgColor;
    if (state == OPENED) {
        // 翻开状态：浅灰色背景
        bgColor = RGB(220, 220, 220);
    } else {
        // 未翻开状态：默认灰色背景
        bgColor=RGB(70,130,180);//钢蓝 
    }
    setfillcolor(bgColor);
    solidrectangle(pixelX,pixelY,pixelX+BLOCK_SIZE,pixelY+BLOCK_SIZE);
    
    // 绘制3D边框（凸起效果）//其实重叠了，没用 
    setlinestyle(PS_SOLID,2);
    setlinecolor(RGB(255, 255, 255)); // 左上边框白色
    line(pixelX,pixelY,pixelX+BLOCK_SIZE,pixelY);
    line(pixelX,pixelY,pixelX,pixelY+BLOCK_SIZE);
    setlinecolor(RGB(128, 128, 128)); // 右下边框灰色
    line(pixelX+BLOCK_SIZE,pixelY,pixelX+BLOCK_SIZE,pixelY+BLOCK_SIZE);
    line(pixelX,pixelY+BLOCK_SIZE,pixelX+BLOCK_SIZE,pixelY+BLOCK_SIZE);
    
    // 根据状态绘制内容
    if(isShowPopup&&mineCountAround==9){
    	// 绘制地雷（游戏结束时显示）
        setfillcolor(BLACK);
    	solidcircle(pixelX+BLOCK_SIZE/2,pixelY+BLOCK_SIZE/2,BLOCK_SIZE/3);
	}
    else if(state==FLAGGED){
        // 绘制红旗
        setfillcolor(RED);
        solidrectangle(pixelX+BLOCK_SIZE/4,pixelY+BLOCK_SIZE/4, 
                       pixelX+3*BLOCK_SIZE/4,pixelY+BLOCK_SIZE/2);
            
        setfillcolor(BLACK);
        solidrectangle(pixelX+BLOCK_SIZE/4-2,pixelY+BLOCK_SIZE/4, 
                       pixelX+BLOCK_SIZE/4+2,pixelY+3*BLOCK_SIZE/4);               
	}
	else if(state==QUESTION){
		// 绘制问号
        settextcolor(BLACK);
        settextstyle(BLOCK_SIZE/2, 0, _T("Arial"));
        setbkmode(TRANSPARENT);//背景透明 
            
        TCHAR questionMark[] = _T("?");
		//转为通用字符类型 避免乱码 
        RECT rect={pixelX,pixelY,pixelX+BLOCK_SIZE,pixelY+BLOCK_SIZE};
        //矩形结构体，在指定矩形区域内绘制文字 
		drawtext(questionMark,&rect,DT_CENTER|DT_VCENTER|DT_SINGLELINE);
        //水平居中，垂直居中，一行 
	}
	else if(state==OPENED&&mineCountAround>0){
		// 绘制数字
        COLORREF numColor;
        switch (mineCountAround) {
            case 1: numColor = NUM_COLOR_1; break;
            case 2: numColor = NUM_COLOR_2; break;
            case 3: numColor = NUM_COLOR_3; break;
            case 4: numColor = NUM_COLOR_4; break;
            case 5: numColor = NUM_COLOR_5; break;
            case 6: numColor = NUM_COLOR_6; break;
            case 7: numColor = NUM_COLOR_7; break;
            case 8: numColor = NUM_COLOR_8; break;
            default: numColor = BLACK;
        }
                
        settextcolor(numColor);
        settextstyle(BLOCK_SIZE/2, 0, _T("Arial"));
        setbkmode(TRANSPARENT);
                
        TCHAR numStr[2];
        sprintf(numStr, "%d", mineCountAround);
        //数字转文字显示 "5\0" 
                
        RECT numRect={pixelX,pixelY,pixelX+BLOCK_SIZE,pixelY+BLOCK_SIZE};
        drawtext(numStr,&numRect,DT_CENTER|DT_VCENTER|DT_SINGLELINE);
	}
}

// 绘制整个地雷地图
void drawMineMap(int map[2][16][30], int selectedDifficulty,bool isShowPopup) {
	BeginBatchDraw(); //缓存，一次画完了再显示，免得闪 
    int i,j,pixelX,pixelY,state,maxRow,maxCol;//piexel:picture element像素 
    getMapSizeByDifficulty(selectedDifficulty,maxRow,maxCol);
	for (i = 0; i < maxRow; i++) {
        for (j = 0; j < maxCol; j++) {
            pixelX = j * BLOCK_SIZE;
            pixelY = INFO_AREA_HEIGHT + i * BLOCK_SIZE;
            drawSingleBlock(pixelX,pixelY,map[1][i][j],map[0][i][j],isShowPopup);
        }
    }
	FlushBatchDraw();
	EndBatchDraw();
}

// 绘制游戏信息栏
void drawGameInfo(int difficulty, int gameTime, int flagCount, int totalMine) {
	setfillcolor(RGB(192, 192, 192));
    solidrectangle(0, 0, c, 50);
    settextcolor(BLACK);
    settextstyle(20, 0, _T("Arial"));
    setbkmode(TRANSPARENT);
    
    // 绘制难度信息
    TCHAR difficultyStr[20];
    switch (difficulty) {
        case DIFFICULTY_EASY:
            strcpy(difficultyStr, "简单");
            break;
        case DIFFICULTY_MID:
            strcpy(difficultyStr,"中等");
            break;
        case DIFFICULTY_HARD:
            strcpy(difficultyStr, "困难");
            break;
        default:
            strcpy(difficultyStr, "未知");
    }
    RECT diffRect = {10, 10, 100, 40};
    drawtext(difficultyStr, &diffRect, DT_LEFT | DT_VCENTER);
    
    //画重新开始键
    RECT btnRect = {30,20,180,50};
    drawtext(_T("[重新开始]"), &btnRect, DT_CENTER | DT_VCENTER|DT_SINGLELINE);
    
    // 绘制计时器
    TCHAR timeStr[20];
    sprintf(timeStr, "时间（秒）: %03d", gameTime);
    
    int centerX = getwidth() / 2;
    RECT timeRect = {centerX - 100, 10, centerX + 100, 40};
    drawtext(timeStr, &timeRect, DT_CENTER | DT_VCENTER);
    
    // 绘制地雷计数
    TCHAR mineStr[20];
    sprintf(mineStr, "雷: %02d/%02d", flagCount, totalMine);
    RECT mineRect = {getwidth() - 120, 10, getwidth() - 10, 40};
    drawtext(mineStr, &mineRect, DT_RIGHT | DT_VCENTER);
}

// 绘制游戏结束弹窗
void drawGameOverUI(int isWin, int gameTime, int winWidth, int winHeight, 
                   bool &isRestart, bool &isQuit) {
    // 弹窗背景
    int popupWidth = 300;
    int popupHeight = 200;
    int popupX=0,popupY=0;
    setfillcolor(RGB(240, 240, 240));
    solidrectangle(popupX, popupY, popupX + popupWidth, popupY + popupHeight);
    
    // 绘制边框
    setlinecolor(RGB(100, 100, 100));
    rectangle(popupX, popupY, popupX + popupWidth, popupY + popupHeight);
    
    // 绘制结果文本
    settextcolor(isWin ? GREEN : RED);
    settextstyle(30, 0, _T("Arial"));
    setbkmode(TRANSPARENT);
    
    TCHAR resultStr[50];
    if (isWin==1) {
        sprintf(resultStr, "胜利! 用时: %d秒", gameTime);
    } else if(isWin==-1){
        strcpy(resultStr, "你踩雷了……");
    }
    
    RECT resultRect = {popupX, popupY + 30, popupX + popupWidth, popupY + 80};
    drawtext(resultStr, &resultRect, DT_CENTER | DT_VCENTER);
    
    // 绘制按钮
    int btnWidth = 100;
    int btnHeight = 40;
    int btnY = popupY + popupHeight - 70;
    
    // 重新开始按钮
    int restartBtnX = popupX + 50;
    setfillcolor(RGB(180, 220, 180));
    solidrectangle(restartBtnX, btnY, restartBtnX + btnWidth, btnY + btnHeight);
    
    settextcolor(BLACK);
    settextstyle(20, 0, _T("Arial"));
    
    RECT restartRect = {restartBtnX, btnY, restartBtnX + btnWidth, btnY + btnHeight};
    drawtext(_T("重新开始"), &restartRect, DT_CENTER | DT_VCENTER|DT_SINGLELINE);
    
    // 退出按钮
    int quitBtnX = popupX + popupWidth - 50 - btnWidth;
    setfillcolor(RGB(220, 180, 180));
    solidrectangle(quitBtnX, btnY, quitBtnX + btnWidth, btnY + btnHeight);
    
    RECT quitRect = {quitBtnX, btnY, quitBtnX + btnWidth, btnY + btnHeight};
    drawtext(_T("退出"), &quitRect, DT_CENTER | DT_VCENTER|DT_SINGLELINE);
    
    // 检查按钮点击
    bool selected=0;
    while(!selected){
    	MOUSEMSG msg;
	    if (MouseHit()) {
	        msg = GetMouseMsg();
	        if (msg.uMsg == WM_LBUTTONDOWN) {
	            if (msg.x >= restartBtnX && msg.x <= restartBtnX + btnWidth &&
	                msg.y >= btnY && msg.y <= btnY + btnHeight) {
	                isRestart = true;
	                return;
	            } else if (msg.x >= quitBtnX && msg.x <= quitBtnX + btnWidth &&
	                       msg.y >= btnY && msg.y <= btnY + btnHeight) {
	                isQuit = true;
	                return;
	            }
	        }
	    }
	    Sleep(10);
	}
}
// 绘制难度选择界面
void drawDifficultySelect(int &selectedDifficulty) {
    initgraph(400, 200, SHOWCONSOLE);
    setbkcolor(RGB(240, 240, 240));
    cleardevice();
    
    settextcolor(BLACK);
    settextstyle(24, 0, _T("Arial"));
    setbkmode(TRANSPARENT);
    
    // 绘制标题
    RECT titleRect = {0, 30, 400, 70};
    drawtext(_T("选择游戏难度"), &titleRect, DT_CENTER | DT_VCENTER|DT_SINGLELINE);
    
    // 绘制难度按钮
    int btnWidth = 80;
    int btnHeight = 40;
    int startY = 100;
    
    // 简单难度按钮
    int easyX = 60;
    setfillcolor(selectedDifficulty == DIFFICULTY_EASY ? RGB(180, 255, 180) : RGB(220, 220, 220));
    solidrectangle(easyX, startY, easyX + btnWidth, startY + btnHeight);
    
    setlinecolor(RGB(100, 100, 100));
    rectangle(easyX, startY, easyX + btnWidth, startY + btnHeight);
    
    settextstyle(16, 0, _T("Arial"));
    RECT easyRect = {easyX, startY, easyX + btnWidth, startY + btnHeight};
    drawtext(_T("简单"), &easyRect, DT_CENTER | DT_VCENTER|DT_SINGLELINE);
    
    // 中等难度按钮
    int midX = 160;
    setfillcolor(selectedDifficulty == DIFFICULTY_MID ? RGB(255, 255, 180) : RGB(220, 220, 220));
    solidrectangle(midX, startY, midX + btnWidth, startY + btnHeight);
    
    rectangle(midX, startY, midX + btnWidth, startY + btnHeight);
    
    RECT midRect = {midX, startY, midX + btnWidth, startY + btnHeight};
    drawtext(_T("中等"), &midRect, DT_CENTER | DT_VCENTER|DT_SINGLELINE);
    
    // 困难难度按钮
    int hardX = 260;
    setfillcolor(selectedDifficulty == DIFFICULTY_HARD ? RGB(255, 180, 180) : RGB(220, 220, 220));
    solidrectangle(hardX, startY, hardX + btnWidth, startY + btnHeight);
    
    rectangle(hardX, startY, hardX + btnWidth, startY + btnHeight);
    
    RECT hardRect = {hardX, startY, hardX + btnWidth, startY + btnHeight};
    drawtext(_T("困难"), &hardRect, DT_CENTER | DT_VCENTER|DT_SINGLELINE);
    
    // 等待用户选择
    bool selected = false;
    while (!selected) {
        MOUSEMSG msg;
        if (MouseHit()) {
            msg = GetMouseMsg();
            if (msg.uMsg == WM_LBUTTONDOWN) {
                if (msg.x >= easyX && msg.x <= easyX + btnWidth &&
                    msg.y >= startY && msg.y <= startY + btnHeight) {
                    selectedDifficulty = DIFFICULTY_EASY;
                    selected = true;
                } else if (msg.x >= midX && msg.x <= midX + btnWidth &&
                          msg.y >= startY && msg.y <= startY + btnHeight) {
                    selectedDifficulty = DIFFICULTY_MID;
                    selected = true;
                } else if (msg.x >= hardX && msg.x <= hardX + btnWidth &&
                          msg.y >= startY && msg.y <= startY + btnHeight) {
                    selectedDifficulty = DIFFICULTY_HARD;
                    selected = true;
                }
            }
        }
        Sleep(10);
    }
    closegraph();
}
