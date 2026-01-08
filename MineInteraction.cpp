// MineInteraction.cpp
#include "MineSweeper.h"
extern int dx[8];//={0,0,1,1,1,-1,-1,-1};
extern int dy[8];//={1,-1,0,1,-1,0,1,-1};
bool book[16][30]; 

void clear(){//清楚标记 
	for(int i=0;i<16;++i){
		for(int j=0;j<30;++j){
			book[i][j]=0;
		}
	}
}
// 扩展空白区域（递归翻开周围格子）
void expandBlankArea(int map[2][16][30],int row,int col,int maxRow,int maxCol){
    //越界 
	if(row<0||row>=maxRow||col<0||col>=maxCol) return;
    // 如果已经是雷或者是旗、问号，不翻了 
	//用book标记不要走回头路 
    if (book[row][col]||map[0][row][col]==MINE||map[1][row][col]==QUESTION||
		map[1][row][col]==FLAGGED){
        return;
    }
    //翻开当前格子
    book[row][col]=1;
    updateBlockState(map,row,col,OPENED,maxRow,maxCol);
    //递归翻开周围8个格子,把连起来的空白格都翻开 
    if(map[0][row][col]==0){
	   	int i,nr,nc; 
	    for (i=0;i<8;i++) {
	        nr=row+dx[i];
	        nc=col+dy[i];
	        expandBlankArea(map,nr,nc,maxRow,maxCol);
	    }
	}
}

// 开始/重置游戏计时器
void startGameTimer(clock_t &startTime,bool &isTimerRunning,bool isReset) {
    if (isReset) {
        startTime=clock();
        isTimerRunning=1;
    } 
}

// 计算游戏时间
int calcGameTime(clock_t startTime,bool isTimerRunning) {
    if (!isTimerRunning) return 0;
    clock_t currentTime = clock();
    return (int)((currentTime-startTime)/CLOCKS_PER_SEC);
    //计算秒数 
}

// 处理标记操作（右键点击）
void handleFlagOperation(int map[2][16][30],int row,int col,
	int &flagCount) {
    switch(map[1][row][col]){
        case EMPTY:
            // 空白或地雷 -> 标记为红旗
            map[1][row][col]=FLAGGED;
            flagCount++;
            break;
        case FLAGGED:
            // 红旗 -> 标记为问号
            map[1][row][col]=QUESTION;
            flagCount--;
            break;
        case QUESTION:
            // 问号 -> 恢复为原始状态
            map[1][row][col]=EMPTY;
            break;
        default:
            // 已翻开的格子不能标记
            break;
    }
}

// 处理鼠标事件
void handleMouseEvent(int map[2][16][30], int difficulty, bool &isGameOver, int &isGameWin, 
	int &flagCount, bool &isFirstClick, bool &isShowPopup,bool &isRestart,
	clock_t &startTime,bool &isTimerRunning) {
    // 如果游戏结束或显示弹窗，不处理地图点击
    if (isGameOver || isShowPopup) return;
    bool selected=0;
	//本来用于判断是否监听到有效鼠标信息，不过后面直接写return了就没必要了，while(1)就行 
    while(!selected){
    	 MOUSEMSG msg;
		//EasyX库里的封装鼠标信息的结构体
		//.x是光标在窗口的x坐标，横向，.y是y坐标，和数组的标法不一样 
		if (MouseHit()) {
	    	//要按了左右键才能退！只要鼠标在图框内就会算MouseHit一次了 
	        msg=GetMouseMsg();
	        if(!(msg.uMsg == WM_LBUTTONDOWN||msg.uMsg==WM_RBUTTONDOWN)){
	        	Sleep(10);
	        	continue;
			}
			//并且判断要按到了能反应的位置 
	        int maxRow, maxCol;//其实写extern成全局变量更方便 
	        getMapSizeByDifficulty(difficulty, maxRow, maxCol);
	        int mineCount=getMineCountByDifficulty(difficulty);
	
	        int restartBtnWidth=150;//重新开始键 
	        int restartBtnHeight=30;
	        int restartBtnX=30;//注意MineGraphics里改了 
	        int restartBtnY=20;
	        // 检查是否点击重新开始按钮
	        if (msg.uMsg==WM_LBUTTONDOWN&&msg.x>=restartBtnX&&msg.x<=restartBtnX+restartBtnWidth&&
	            msg.y>=restartBtnY&&msg.y<=restartBtnY+restartBtnHeight){
	            isRestart=1;
	            selected=1;
	            return;
	        }
	        // 检查是否点击游戏区域
	        if(msg.x>=0&&msg.x<maxCol*BLOCK_SIZE&&msg.y>=INFO_AREA_HEIGHT&&
				msg.y<INFO_AREA_HEIGHT+maxRow*BLOCK_SIZE) {
	            int row=(msg.y-INFO_AREA_HEIGHT)/BLOCK_SIZE;
	            int col=msg.x/BLOCK_SIZE;
	            if(row>=0&&row<maxRow&&col>=0&&col<maxCol) {
	                if(msg.uMsg == WM_LBUTTONDOWN) {// 左键点击
	                    selected=1;
	                    if(isFirstClick) {
	                    	// 如果是第一次点击，生成地雷
	                        initMineMap(map,difficulty,row,col);
	                        //翻一下 
	                        clear();
	                        expandBlankArea(map,row,col,maxRow,maxCol);
	                        //开始计时 
	                        startGameTimer(startTime,isTimerRunning,true);
	                        isFirstClick=0;
	                        isTimerRunning=1; 
	                    	return;
						}
	                    //非雷 //0的直接expand，非0的判断周围能否翻 
						if(map[0][row][col]==0){
							clear();
	                        expandBlankArea(map,row,col,maxRow,maxCol);
						} 
	                    if(map[0][row][col]>=1&&map[0][row][col]<9){
	                        //如果数字等于周围旗数，翻开周围，否则只翻自己 
	                        //旗子插错了翻数字格也会炸掉 
	                        int flagsAround=0,i,nr,nc;
	                        clear();
	                    	expandBlankArea(map,row,col,maxRow,maxCol);
	                        for(i=0;i<8;i++){
	                            nr=row+dx[i];
	                            nc=col+dy[i];
	                            if(nr>=0&&nr<maxRow&&nc>=0&&nc<maxCol){
	                            	//是雷且插旗，计数 
	                                if(map[1][nr][nc]==FLAGGED&&map[0][nr][nc]==MINE){
	                                    flagsAround++;
	                                }
	                                //旗插错了，执意要通过周围的翻 
	                                else if(map[1][nr][nc]==FLAGGED&&map[0][nr][nc]<9){
	                                	isGameOver=1;
								        isShowPopup=1;
								        isTimerRunning=0;
								        isGameWin=-1;
								        return; 
									} 
	                            }
	                        }
	                        if(map[0][row][col]==flagsAround){
	                        	for(i=0;i<8;++i){
	                        		nr=row+dx[i];
		                            nc=col+dy[i];
		                            if(nr>=0&&nr<maxRow&&nc>=0&&nc<maxCol){
		                                if(map[1][nr][nc]==EMPTY&&map[0][nr][nc]!=MINE){	
		                                	clear();
	                        				expandBlankArea(map,nr,nc,maxRow,maxCol);
		                                }
		                            }
								}
	                        }
	                    }
						else if(map[0][row][col]==MINE){
	                        // 点击地雷
	                        isGameOver=1;
					        isShowPopup=1;
					        isTimerRunning=0;
					        isGameWin=-1;//挂了 
					        return; 
	                    }
	                }
	                // 右键点击
	                else if(msg.uMsg==WM_RBUTTONDOWN) {
	                    handleFlagOperation(map,row,col,flagCount);
	                    selected=1; //别急着退，判断是否胜利 
	                }
	                // 检查游戏是否胜利
	                isGameWin=checkGameWin(map,maxRow,maxCol,mineCount);
	                if (isGameWin) {
	                    isGameOver=1;
	                    isShowPopup=1;
	                    isTimerRunning=0;
	                    return;
	                }
	            }
	        }
	    }
    	Sleep(10);
	}
}



