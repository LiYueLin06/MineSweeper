//MineData.cpp
#include "MineSweeper.h"
int dx[8]={0,0,1,1,1,-1,-1,-1};
int dy[8]={1,-1,0,1,-1,0,1,-1};

int getMineCountByDifficulty(int difficulty){
	//根据难度算出地雷数 
	switch(difficulty){
		case DIFFICULTY_EASY:
			return EASY_MINE;
		case DIFFICULTY_MID:
			return MID_MINE;
		default:
			return HARD_MINE;
	} 
}

void getMapSizeByDifficulty(int difficulty, int &maxRow, 
	int &maxCol){
	//根据难度设置地图大小 
	switch(difficulty){
		case DIFFICULTY_EASY:
			maxRow=EASY_ROW;
			maxCol=EASY_COL;
			break;
		case DIFFICULTY_MID:
			maxRow=MID_ROW;
			maxCol=MID_COL;
			break;
		default:
			maxRow=HARD_ROW;
			maxCol=HARD_COL;
			break; 
	}
}

void initMineMap(int map[2][16][30], int difficulty, int firstClickRow, int firstClickCol){
	//根据难度生成地雷，点击位置周围3*3安全区，不要有雷 
	int maxRow,maxCol,i,j,k,rand_idx,x,y,nr,nc;
	int mineCount=getMineCountByDifficulty(difficulty);
	getMapSizeByDifficulty(difficulty,maxRow,maxCol);
	
	int valid_adr[30*16][2]={0},cnt=0,around=0;
	bool flag=0; 
	//存可以放雷的坐标,总共多少位置 
	for(i=0;i<maxRow;++i){
		for(j=0;j<maxCol;++j){
			if(abs(i-firstClickRow)<=1 && abs(j-firstClickCol)<=1){
	            continue; // 跳过安全区
	        }
	        valid_adr[cnt][0]=i;
	        valid_adr[cnt][1]=j;//统计可以布雷的区域 
	        cnt++;
		}
	}
	int res=mineCount;//还有多少雷没放
	while(res){
		rand_idx=rand()%cnt;
		x=valid_adr[rand_idx][0];
		y=valid_adr[rand_idx][1];
		around=0;
		for(k=0;k<8;++k){
			nr=x+dx[k];
			nc=y+dy[k];
			if(nr>=0&&nc>=0&&nr<maxRow&&nc<maxCol&&map[0][nr][nc]==9){
				around++;
			}
		}//每次新雷周围不要太密，避免无解 
		if(around<4&&map[0][x][y]==0){
			map[0][x][y]=9;
			valid_adr[rand_idx][0]=valid_adr[cnt-1][0];//删掉用过的位置 
			valid_adr[rand_idx][1]=valid_adr[cnt-1][1];
			res--;
			cnt--; 
		}
	} 
	//统计周围雷数
	for(i=0;i<maxRow;++i){
		for(j=0;j<maxCol;++j){
			if(map[0][i][j]==0){
				for(k=0;k<8;++k){
					x=i+dx[k];
					y=j+dy[k];
					if(x<0||y<0||x>=maxRow||y>=maxCol) continue;
					if(map[0][x][y]==9){
						map[0][i][j]++;
					}
				}
			}
		}
	} 
}
bool updateBlockState(int map[2][16][30], int row, int col, 
	int newState, int maxRow, int maxCol){
	//改变当前位置状态 newState为目标改成什么状态 
	//返回1修改成功，更新页面
	//返回0修改失败，页面不能变化 
	if(row<0||row>=maxRow||col<0||col>=maxCol){
		return 0;
	}
	map[1][row][col]=newState;	
	return 1;
}

bool checkGameWin(int map[2][16][30], int maxRow, int maxCol, 
	int mineCount){
	//检查是否游戏胜利 ，mineCount为该难度雷数应有多少 
	int cnt=0,i,j;
	bool opened=1;//要除了旗子的全部翻开 
	for(i=0;i<maxRow;++i){
		for(j=0;j<maxCol;++j){
			if(map[1][i][j]==FLAGGED){
				cnt++;//雷全被标记即成功 
			}
			else if(map[1][i][j]!=OPENED){
				opened=0;
				return 0;
			} 
		}
	}
	return cnt==mineCount;
}
