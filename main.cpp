#include "MineSweeper.h"

int map[2][16][30]={0};
//第一维是0：格子有雷(9)和周围有多少(0-8) 
//第一维是1：无标记(0)和红旗(1)疑问旗(2)翻开(3) 
bool isGameOver=0; //结束：踩雷或胜利 S
int isGameWin=0;//改成1胜利-1输0还在继续 
bool isFirstClick=1;//第一次点击时要生成地雷 
bool isShowPopup=0;//显示结束弹窗 
bool isTimerRunning=0;//计时
int selectedDifficulty=DIFFICULTY_EASY;//默认简单模式
int flagCount=0;//插红旗数
int gameTime=0;
clock_t startTime=0;//计时类型 
int maxRow,maxCol,winWidth,winHeight;
bool isRestart,isQuit;
int c,r;//窗口大小col*raw 
void draw_Win_and_Buffer(int map[2][16][30],int selectedDifficulty,int gametime,int flagCount,bool isShowPopup){
	drawGameInfo(//信息栏：难度，重新开始，计时，旗数/总雷数 
			selectedDifficulty,
			gameTime,
			flagCount,
			getMineCountByDifficulty(selectedDifficulty)
		);
	drawMineMap(map,selectedDifficulty,isShowPopup);	 	
}
void initAll(){
	int i,j,k;
	for(k=0;k<2;++k){
		for(i=0;i<16;++i){
			for(j=0;j<30;++j){
				map[k][i][j]=0;
			}
		}	
	}
	isGameOver=0;
	isGameWin=0;//0还在进行，1赢，-1输 
	flagCount=0;
	gameTime=0;
	isFirstClick=1;//还没翻雷 
	isShowPopup=0;
	isTimerRunning=0;
	startTime=0;
	isRestart=0;
	isQuit=0;
	selectedDifficulty=DIFFICULTY_EASY;//默认简单模式 
	srand((unsigned int)time(NULL));//随机种子 
	drawDifficultySelect(selectedDifficulty);//绘制难度选择界面 

	switch(selectedDifficulty){
		case DIFFICULTY_EASY:
			c=EASY_WIN_C;
			r=EASY_WIN_R;
			break;
		case DIFFICULTY_MID:
			c=MID_WIN_C;
			r=MID_WIN_R;
			break;
		case DIFFICULTY_HARD:
			c=HARD_WIN_C;	
			r=HARD_WIN_R;
			break;
	}
	initGraphicsWindow(selectedDifficulty);//根据选择难度画窗口
	draw_Win_and_Buffer(map,selectedDifficulty,gameTime,flagCount,isShowPopup);
}
int main(){
	initAll();
	while(1){
		//检测一次鼠标事件
		handleMouseEvent( 
			map,selectedDifficulty,isGameOver,isGameWin,flagCount,
			isFirstClick,isShowPopup,isRestart,startTime,isTimerRunning
		); 
		if(isTimerRunning){//更新计时 
			gameTime=calcGameTime(startTime,isTimerRunning);
		} 
		cleardevice();//清空画布，避免残影
		//画游戏页面 
		draw_Win_and_Buffer(map,selectedDifficulty,gameTime,flagCount,isShowPopup);
		
		if(isRestart){
			closegraph();
			initAll();
		}
		if(isShowPopup){//游戏结束弹窗 
			isRestart=0;
			isQuit=0;
			drawGameOverUI(isGameWin,gameTime,c,r,isRestart,isQuit);
			if(isRestart){
				closegraph();
				initAll();
			}		
			else if(isQuit){
				closegraph();
				return 0;
			}
		}
		Sleep(16);//休息16ms约60帧/秒，避免CPU占用过高 
	} 
	closegraph();
	return 0; 
} 
