#include "gba1.h"
#include "8x8ascii.h"

#define CURRENT_TIME *((unsigned short *)0x0400010C)

//関数プロトタイプ宣言

int mode_StartScreen(void);//スタート画面モード
int mode_TimeAttack(volatile unsigned short);//タイムアタックモード
int mode_EndLess(volatile unsigned short);//エンドレスモード
int mode_ScoreScreen(void);//スコア画面モード

void createStartScreen(void);//スタート画面作成
void draw_Tetris(void);//「TETRiS」の表示
void draw_TimeAttack(hword);//「TIME ATTACK」の表示
void draw_Endless(hword);//「ENDLESS」の表示
void draw_SmallScore(hword);//小さな「SCORE」の表示

void createStage(void);//ステージ画面作成
void draw_NextBlock(void);//「NEXT BLOCK」の表示
void draw_Time(void);//「TIME」の表示
void draw_DeletedLine(void);//「DELETED LINE」の表示
void draw_Rule(void);//「SELECT:PAUSE」の表示
void beforeNextBlock(void);//次のブロックに行く前に
void renewStage(void);//ステージの更新
void renewBorder(void);//境界線の更新
void moveRight(void);//右移動
void moveLeft(void);//左移動
void drop(void);//落下移動
void fall(void);//一気に落下
void turn(int);//回転
void renewNextBlock(hword, hword);//次のブロックの更新
void renewDeletedLineNum(void);//消したラインの数の更新

int mode_Pause(void);//ポーズモード
void createPause(void);//PAUSE画面作成
void draw_Pause(void);//「PAUSE」の表示
void draw_Resume(hword);//「RESUME」の表示
void draw_Quit(hword);//「QUIT」の表示

void createScore(void);//スコア画面の作成
void draw_BigScore(void);//大きな「SCORE」の表示
void draw_Exit(hword);//「 EXIT」の表示
void view_TimeAttack_Score(void);//タイムアタックのスコアを表示
void view_EndLess_Score(void);//エンドレスのスコアの表示

void locate(hword, hword);
void printh(hword);
void printn(hword);
void prints(byte*);
void print_ascii(byte);
void draw_point(hword, hword, hword);
hword mod(hword dividened, hword divisor);
hword div(hword dividened, hword divisor);
void draw_block(hword);//8×8を塗りつぶす
void prints_color(byte*, hword);//文字列表示関数
void print_ascii_color(byte, hword);//locateで設定された位置にASCIIコードascii_numに相当する文字を表示する関数
void display_time(hword);

void init_timer(void);//タイマ設定
void int_handler(void);//割り込み用

//「TETRiS」&「SCORE」&「FINISH」のデータ
int T[5][5] = {{0, 1, 1, 1, 0}, {0, 0, 1, 0, 0}, {0, 0, 1, 0, 0}, {0, 0, 1, 0, 0}, {0, 0, 1, 0, 0}};
int E[5][5] = {{0, 1, 1, 1, 0}, {0, 1, 0, 0, 0}, {0, 1, 1, 0, 0}, {0, 1, 0, 0, 0}, {0, 1, 1, 1, 0}};
int R[5][5] = {{0, 1, 1, 1, 0}, {0, 1, 0, 1, 0}, {0, 1, 1, 0, 0}, {0, 1, 0, 1, 0}, {0, 1, 0, 0, 1}};
int smallI[5][5] = {{0, 0, 1, 0, 0}, {0, 0, 0, 0, 0}, {0, 0, 1, 0, 0}, {0, 0, 1, 0, 0}, {0, 0, 1, 0, 0}};
int S[5][5] = {{0, 1, 1, 1, 0}, {0, 1, 0, 0, 0}, {0, 1, 1, 1, 0}, {0, 0, 0, 1, 0}, {0, 1, 1, 1, 0}};
int C[5][5] = {{0, 1, 1, 1, 0}, {0, 1, 0, 0, 0}, {0, 1, 0, 0, 0}, {0, 1, 0, 0, 0}, {0, 1, 1, 1, 0}};
int O[5][5] = {{0, 1, 1, 1, 0}, {0, 1, 0, 1, 0}, {0, 1, 0, 1, 0}, {0, 1, 0, 1, 0}, {0, 1, 1, 1, 0}};
int F[5][5] = {{0, 1, 1, 1, 1}, {0, 1, 0, 0, 0}, {0, 1, 1, 0, 0}, {0, 1, 0, 0, 0}, {0, 1, 0, 0, 0}};
int largeI[5][5] = {{0, 1, 1, 1, 0}, {0, 0, 1, 0, 0}, {0, 0, 1, 0, 0}, {0, 0, 1, 0, 0}, {0, 1, 1, 1, 0}};
int N[5][5] = {{1, 1, 0, 0, 1}, {1, 1, 0, 0, 1}, {1, 0, 1, 0, 1}, {1, 0, 0, 1, 1}, {1, 0, 0, 1, 1}};
int H[5][5] = {{0, 1, 0, 1, 0}, {0, 1, 0, 1, 0}, {0, 1, 1, 1, 0}, {0, 1, 0, 1, 0}, {0, 1, 0, 1, 0}};

//ブロックデータ	0: 1: 2: 3: 4: 5: 6:
int blockData[7][4][4] = {{{0, 1, 0, 0}, {0, 1, 0, 0}, {0, 1, 0, 0}, {0, 1, 0, 0}},//I
						{{0, 0, 0, 0}, {0, 1, 1, 0}, {0, 1, 1, 0}, {0, 0, 0, 0}},//正方形
						{{0, 0, 0, 0}, {0, 1, 1, 0}, {1, 1, 0, 0}, {0, 0, 0, 0}},//逆乙
						{{0, 0, 0, 0}, {0, 1, 1, 0}, {0, 0, 1, 1}, {0, 0, 0, 0}},//乙
						{{0, 0, 0, 0}, {0, 1, 0, 0}, {1, 1, 1, 0}, {0, 0, 0, 0}},//T
						{{0, 0, 0, 0}, {0, 1, 0, 0}, {0, 1, 1, 1}, {0, 0, 0, 0}},//L
						{{0, 0, 0, 0}, {0, 0, 1, 0}, {1, 1, 1, 0}, {0, 0, 0, 0}}//逆L
						};

//ブロックの順序
int blockOrder[49] = {6, 2, 5, 4, 5, 1, 6, 3, 3, 2, 1, 0, 4, 1, 3, 0, 5, 3, 4, 1, 0, 6, 3, 6, 5, 4, 1, 2, 0, 1, 6, 2, 3, 4, 0, 2, 5, 5, 6, 2, 1, 4, 3, 0, 2, 6, 4, 0, 5};

//ブロックの色
hword blockColor[7] = {0x7FE0, 0x03FF, 0x03E0, 0x001F, 0x7C1F, 0x7C00, 0x019F};	//0:水色 1:黄色 2:緑 3:赤色 4:紫 5:青色 6:オレンジ

//スコアデータ
volatile unsigned short timeAttackScore[3];
int endLessScore[3];

//グローバル変数定義
int mode;//mode0:スタート画面1:ゲーム画面（タイムアタック）2:ゲーム画面（エンドレス）3:スコア画面
point p;//座標指定用構造体
point coor;//動かしているブロックの座標
int turnNum, count, lineNum, orderNum, finFlag, clearFlag, returnNum;//回転の向き	1:回転なし	2:右回転	3:左回転	4:真反対
int blockNum[3];//ブロックの番号
int blockState[4][4];//ブロックの状態
int border[10];//境界線
int stage[10][19];//ステージ上のブロックデータ
volatile unsigned short time;//時間の変数


int main(void) {
	int i, j;
	
	//画面初期化ルーチン
	*((unsigned short *)IOBASE) = 0xF03;

	mode = 0;
	
	//スコア初期化
	for(i = 0; i < 3; i++){
		timeAttackScore[i] = 0x8C96;
		endLessScore[i] = 0;
	}
	
	while(1){
		if(mode == 0){
			returnNum = mode_StartScreen();
		}else if(mode == 1){
			returnNum = mode_TimeAttack(time);
		}else if(mode == 2){
			returnNum = mode_EndLess(time);
		}else if(mode == 3){
			returnNum = mode_ScoreScreen();
		}else{
			mode = 0;
		}
	}
	return 0;
}


//スタート画面モード
int mode_StartScreen(){
	hword *ptr, tmp, i, j, push;
	int choise;//0:ゲーム画面（タイムアタック）1:ゲーム画面（エンドレス）2:スコア画面
	createStartScreen();
	init_timer();//タイマ初期化
	
	choise = 0;
	push = 0;
	time = 0x0000;
	*((unsigned short *)0x04000102) = 0x0084;	// タイマ0 制御設定（タイマON，割込みOFF，カスケードOFF，プリスケーラなし）

	while(1){
		ptr = KEY_STATUS;
		tmp = ~*ptr & KEY_ALL;

		if(tmp == KEY_UP){
			if(push == 0){
				push = 1;	
				if(choise == 1){
					choise--;
					locate(11, 12);
					draw_Endless(0x7FFF);
					locate(9, 10);
					draw_TimeAttack(0x001F);
				}else if(choise == 2){
					choise--;
					locate(12, 16);
					draw_SmallScore(0x7FFF);
					locate(11, 12);
					draw_Endless(0x001F);
				}
			}
		}else if(tmp == KEY_DOWN){
			if(push == 0){
				push = 1;
				if(choise == 0){
					choise++;
					locate(9, 10);
					draw_TimeAttack(0x7FFF);
					locate(11, 12);
					draw_Endless(0x001F);
				}else if(choise == 1){
					choise++;
					locate(11, 12);
					draw_Endless(0x7FFF);
					locate(12, 16);
					draw_SmallScore(0x001F);
				}
			}
		}else if(tmp == KEY_A){
			*((unsigned short *)0x04000102) = 0x0004;	// タイマ0 制御設定（タイマOFF，割込みOFF，カスケードOFF，プリスケーラなし）
			time = 	*((unsigned short *)0x04000100);
			for(i = 0; i < 160; i++){
				for(j = 0; j < 240; j++){
					draw_point(j, i, 0x0000);
				}
			}
		
			if(choise == 0){
				mode = 1;
				returnNum = mode_TimeAttack(time);
			}else if(choise == 1){
				mode = 2;
				returnNum = mode_EndLess(time);
			}else if(choise == 2){
				mode = 3;
				returnNum = mode_ScoreScreen();
			}
			break;
		}else{
			push = 0;
		}

	}
	init_timer();//タイマ初期化
	return 0;
}


//タイムアタックモード
int mode_TimeAttack(volatile unsigned short t){
	int i, j, button;
	hword *ptr, tmp;
	volatile unsigned short time;//時間の変数
	createStage();

	//初期化
	time = t;
	button = 0;
	
	count = 0;
	lineNum = 0;
	orderNum = mod((int)time + 3, 49);
	finFlag = 0;
	clearFlag = 0;
	
	blockNum[0] = blockOrder[mod((int)time, 49)];
	blockNum[1] = blockOrder[mod((int)time + 1, 49)];
	blockNum[2] = blockOrder[mod((int)time + 2, 49)];
	
	//ステージ初期化
	for(j = 0; j < 19; j++){
		for(i = 0; i < 10; i++){
			stage[i][j] = 7;
		}
	}
	
	//境界線初期化
	for(i = 0; i < 10; i++){
		border[i] = 18;
	}

	locate(16, 1);
	draw_NextBlock();
	locate(16, 3);
	printn(1);
	locate(23, 3);
	printn(2);
	renewNextBlock(blockNum[1], blockNum[2]);
	locate(16, 11);
	draw_Time();
	locate(22, 11);
	display_time(0x0000);
	locate(16, 14);
	draw_DeletedLine();
	locate(24, 15);
	printn(0);
	locate(16, 18);
	draw_Rule();


	init_timer();//タイマ初期化
	*((hword *)0x0400010A) = 0x00C4;// タイマ2割込み発生
	*((hword *)0x04000200) = 0x0020;//タイマ2割込み発生許可
	*((hword *)0x04000208) = 0x0001;//全割込み許可
	*((hword *)0x04000102) = 0x0080;//タイマ開始

	coor.x = 8;
	coor.y = 1;
	turnNum = 0;

	//最初のブロックを表示
	for(i = 0; i < 4; i++){
		for(j = 0; j < 4; j++){
			locate(coor.x + j - 1, coor.y + i - 1);
			if(blockData[blockNum[0]][i][j] == 1){
				draw_block(blockColor[blockNum[0]]);
			}
			blockState[i][j] = blockData[blockNum[0]][i][j];
		}
	}
	
	while(1){
		if(finFlag == 1 || clearFlag == 1){
			break;
		}
		ptr = KEY_STATUS;
		tmp = ~*ptr & KEY_ALL;
		if(tmp == KEY_RIGHT){//右移動
			if(button == 0){
				*((unsigned short *)0x04000208) = 0x0000;//全割り込み禁止
				button = 1;
				moveRight();
				*((unsigned short *)0x04000208) = 0x0001;//全割り込み許可
			}
		}else if(tmp == KEY_LEFT){//左移動
			if(button == 0){
			*((unsigned short *)0x04000208) = 0x0000;//全割り込み禁止
				button = 1;
				moveLeft();
				*((unsigned short *)0x04000208) = 0x0001;//全割り込み許可
			}
		}else if(tmp == KEY_R){//右回転
			if(button == 0){
				button = 1;
				*((unsigned short *)0x04000208) = 0x0000;//全割り込み禁止
				turnNum++;
				if(turnNum == 4){
					turnNum = 0;
				}
				turn(mod(turnNum, 4));
				*((unsigned short *)0x04000208) = 0x0001;//全割り込み許可
			}
		}else if(tmp == KEY_L){//左回転
			if(button == 0){
				*((unsigned short *)0x04000208) = 0x0000;//全割り込み禁止
				button = 1;
				turnNum--;
				if(turnNum < 0){
					turnNum = 3;
				}
				turn(mod(turnNum, 4));
				*((unsigned short *)0x04000208) = 0x0001;//全割り込み許可
			}
		}else if(tmp == KEY_UP){
			if(button == 0){
			 	*((unsigned short *)0x04000208) = 0x0000;//全割り込み禁止
				button = 1;
				fall();
				*((unsigned short *)0x04000208) = 0x0001;//全割り込み許可
			}
		}else if(tmp == KEY_DOWN){//落下
			if(button == 0){
				*((unsigned short *)0x04000208) = 0x0000;//全割り込み禁止
				button = 1;
				drop();
				*((unsigned short *)0x04000208) = 0x0001;//全割り込み許可
			}
		}else if(tmp == KEY_SELECT){//ポーズ
			*((unsigned short *)0x04000208) = 0x0000;//全割り込み禁止
			//タイマ停止
			*((hword *)0x04000102) = 0x0000;

			int number;
			number = mode_Pause();
			if(number == 1){
				finFlag = 1;
				break;
			}
			renewStage();
			for(i = 0; i < 4; i++){
				for(j = 0; j < 4; j++){
					locate(coor.x + j - 1, coor.y + i - 1);
					if(blockState[i][j] == 1){
						draw_block(blockColor[blockNum[0]]);
					}
				}
			}
			//タイマ開始
			*((hword *)0x04000102) = 0x0080;
			*((unsigned short *)0x04000208) = 0x0000;//全割り込み禁止
			drop();
			*((unsigned short *)0x04000208) = 0x0001;//全割り込み許可
		}else{
			button = 0;
		}
	}
	
	*((unsigned short *)0x04000208) = 0x0000;//全割り込み禁止
	*((hword *)0x04000102) = 0x0000;//タイマ停止
	
	//スコアに入れる
	if(lineNum >= 10 || clearFlag == 1){
		for(i = 0; i < 3; i++){
			if(timeAttackScore[i] > CURRENT_TIME){
				for(j = 2; j >= i; j--){
					if(j == i){
						timeAttackScore[j] = CURRENT_TIME;
					}else{
						timeAttackScore[j] = timeAttackScore[j - 1];
					}
				}
				break;
			}
		}
	}
	
	init_timer();//タイマ初期化
	for(i = 0; i < 160; i++){
		for(j = 0; j < 240; j++){
			draw_point(j, i, 0x0000);
		}
	}
	mode = 3;
	returnNum = mode_ScoreScreen();
	return 0;
}


//エンドレスモード   ほとんどタイムアタックモードと同じ
int mode_EndLess(volatile unsigned short t){
	int i, j, button;
	hword *ptr, tmp;
	volatile unsigned short time;
	createStage();

	//初期化
	time = t;
	count = 0;
	button = 0;
	lineNum = 0;
	orderNum = mod((int)time + 3, 49);
	finFlag = 0;
	clearFlag = 0;
	
	blockNum[0] = blockOrder[mod((int)time, 49)];
	blockNum[1] = blockOrder[mod((int)time + 1, 49)];
	blockNum[2] = blockOrder[mod((int)time + 2, 49)];
	
	for(j = 0; j < 19; j++){
		for(i = 0; i < 10; i++){
			stage[i][j] = 7;
		}
	}
	for(i = 0; i < 10; i++){
		border[i] = 18;
	}

	locate(16, 1);
	draw_NextBlock();
	locate(16, 3);
	printn(1);
	locate(23, 3);
	printn(2);
	renewNextBlock(blockNum[1], blockNum[2]);
	locate(16, 11);
	draw_Time();
	locate(22, 11);
	display_time(0x0000);
	locate(16, 14);
	draw_DeletedLine();
	locate(24, 15);
	printn(0);
	locate(16, 18);
	draw_Rule();

	
	init_timer();//タイマ初期化
	*((hword *)0x0400010A) = 0x00C4;// タイマ2割込み発生
	*((hword *)0x04000200) = 0x0020;//タイマ2割込み発生許可
	*((hword *)0x04000208) = 0x0001;//全割込み許可
	*((hword *)0x04000102) = 0x0080;//タイマ開始

	coor.x = 8;
	coor.y = 1;
	turnNum = 0;

	for(i = 0; i < 4; i++){
		for(j = 0; j < 4; j++){
			locate(coor.x + j - 1, coor.y + i - 1);
			if(blockData[blockNum[0]][i][j] == 1){
				draw_block(blockColor[blockNum[0]]);
			}
			blockState[i][j] = blockData[blockNum[0]][i][j];
		}
	}
	
	while(1){
		if(finFlag == 1){
			break;
		}
		ptr = KEY_STATUS;
		tmp = ~*ptr & KEY_ALL;
		if(tmp == KEY_RIGHT){
			if(button == 0){
			 	*((unsigned short *)0x04000208) = 0x0000;//全割り込み禁止
				button = 1;
				moveRight();
				*((unsigned short *)0x04000208) = 0x0001;//全割り込み許可
			}
		}else if(tmp == KEY_LEFT){
			if(button == 0){
			*((unsigned short *)0x04000208) = 0x0000;//全割り込み禁止
				button = 1;
				moveLeft();
				*((unsigned short *)0x04000208) = 0x0001;//全割り込み許可
			}
		}else if(tmp == KEY_R){
			if(button == 0){
				button = 1;
				*((unsigned short *)0x04000208) = 0x0000;//全割り込み禁止
				turnNum++;
				if(turnNum == 4){
					turnNum = 0;
				}
				turn(mod(turnNum, 4));
				*((unsigned short *)0x04000208) = 0x0001;//全割り込み許可
			}
		}else if(tmp == KEY_L){
			if(button == 0){
				*((unsigned short *)0x04000208) = 0x0000;//全割り込み禁止
				button = 1;
				turnNum--;
				if(turnNum < 0){
					turnNum = 3;
				}
				turn(mod(turnNum, 4));
				*((unsigned short *)0x04000208) = 0x0001;//全割り込み許可
			}
		}else if(tmp == KEY_UP){
			if(button == 0){
			 	*((unsigned short *)0x04000208) = 0x0000;//全割り込み禁止
				button = 1;
				fall();
				*((unsigned short *)0x04000208) = 0x0001;//全割り込み許可
			}
		}else if(tmp == KEY_DOWN){
			if(button == 0){
			 	*((unsigned short *)0x04000208) = 0x0000;//全割り込み禁止
				button = 1;
				drop();
				*((unsigned short *)0x04000208) = 0x0001;//全割り込み許可
			}
		}else if(tmp == KEY_SELECT){
			*((unsigned short *)0x04000208) = 0x0000;//全割り込み禁止
			//タイマ停止
			*((hword *)0x04000102) = 0x0000;

			int number;
			number = mode_Pause();
			if(number == 1){
				finFlag = 1;
				break;
			}
			renewStage();
			for(i = 0; i < 4; i++){
				for(j = 0; j < 4; j++){
					locate(coor.x + j - 1, coor.y + i - 1);
					if(blockState[i][j] == 1){
						draw_block(blockColor[blockNum[0]]);
					}
				}
			}
			//タイマ開始
			*((hword *)0x04000102) = 0x0080;
			*((unsigned short *)0x04000208) = 0x0000;//全割り込み禁止
			drop();
			*((unsigned short *)0x04000208) = 0x0001;//全割り込み許可
		}else{
			button = 0;
		}
	}
	*((unsigned short *)0x04000208) = 0x0000;//全割り込み禁止
	//タイマ停止
	*((hword *)0x04000102) = 0x0000;
	for(i = 0; i < 3; i++){
		if(endLessScore[i] < lineNum){
			for(j = 2; j >= i; j--){
				if(j == i){
					endLessScore[j] = lineNum;
				}else{
					endLessScore[j] = endLessScore[j - 1];
				}
			}
			break;
		}
	}
	init_timer();//タイマ初期化
	for(i = 0; i < 160; i++){
		for(j = 0; j < 240; j++){
			draw_point(j, i, 0x0000);
		}
	}
	mode = 3;
	returnNum = mode_ScoreScreen();
	return 0;
}


//スコア画面モード
int mode_ScoreScreen(){
	hword *ptr, tmp;
	int i, j;
	createScore();
	
	while(1){
	 	ptr = KEY_STATUS;
		tmp = ~*ptr & KEY_ALL;	
	
		if(tmp == KEY_A){//Aが押されたら戻る
			for(i = 0; i < 160; i++){
				for(j = 0; j < 240; j++){
					draw_point(j, i, 0x0000);
				}
			}
			mode = 0;
			returnNum = mode_StartScreen();
			break;
		}
	}
	return 0;
}


//スタート画面作成
void createStartScreen(){
	draw_Tetris();
	locate(9, 10);
	draw_TimeAttack(0x001F);
	locate(11, 12);
	draw_Endless(0x7FFF);
	locate(12, 16);
	draw_SmallScore(0x7FFF);
}


//「TETRiS」の表示
void draw_Tetris(){
	int x, y;
	
	//T
	for(x = 0; x < 5; x++){
		for(y = 0; y < 5; y++){
			if(T[x][y] == 1){
				locate(y, x + 2);
				draw_block(0x001F);
			}
		}
	}

	//E
	for(x = 0; x < 5; x++){
		for(y = 0; y < 5; y++){
			if(E[x][y] == 1){
				locate(y + 5, x + 2);
				draw_block(0x019F);
			}
		}
	}
	
	//T
	for(x = 0; x < 5; x++){
		for(y = 0; y < 5; y++){
			if(T[x][y] == 1){
				locate(y + 10, x + 2);
				draw_block(0x03FF);
			}
		}
	}

	//R
	for(x = 0; x < 5; x++){
		for(y = 0; y < 5; y++){
			if(R[x][y] == 1){
				locate(y + 15, x + 2);
				draw_block(0x03E0);
			}
		}
	}

	//i
	for(x = 0; x < 5; x++){
		for(y = 0; y < 5; y++){
			if(smallI[x][y] == 1){
				locate(y + 20, x + 2);
				draw_block(0x7C00);
			}
		}
	}

	//S
	for(x = 0; x < 5; x++){
		for(y = 0; y < 5; y++){
			if(S[x][y] == 1){
				locate(y + 25, x + 2);
				draw_block(0x7C1F);
			}
		}
	}
}


//「TIME ATTACK」の表示
void draw_TimeAttack(hword color){
	prints_color("TIME ATTACK", color);
}


//「ENDLESS」の表示
void draw_Endless(hword color){
	prints_color("ENDLESS", color);
}


//小さな「SCORE」の表示
void draw_SmallScore(hword color){
	prints_color("SCORE", color);
}


//ステージ画面作成
void createStage(void){
	int i, j;
	hword *ptr;

	//壁
	for(j = 0; j < 20; j++){
		for(i = 3; i < 15; i++){
			if(i == 3 || i == 14 || j == 19){
				locate(i, j);
				draw_block(0xFFFF);
			}
		}
	}
	
}


//「NEXT block」の表示
void draw_NextBlock(){
	prints("NEXT BLOCK");
}


//「TIME」の表示
void draw_Time(){
	prints("TIME");
}


//「DELETED LINE」の表示
void draw_DeletedLine(){
	prints("DELETED LINE");
}


//「SELECT:PAUSE」の表示
void draw_Rule(){
	prints("SELECT:PAUSE");
}


//次のブロックに行く前に
void beforeNextBlock(){
	int i, j, h, k, cnt;
	
	//ステージデータを上書き
	for(i = 0; i < 4; i++){
		for(j = 0; j < 4; j++){
			if(blockState[i][j] == 1){
				stage[(coor.x + j - 5)][(coor.y + i - 1)] = blockNum[0];
			}
		}
	}
	
	//視覚的に更新
	renewStage();

	for(i = 18; i >= 0; i--){
		cnt = 0;
		for(j = 0; j < 11; j++){
			if(stage[j][i] < 7){
				cnt++;
			}
		}
		if(cnt == 11){//もし一列そろっていたら
			for(j = 0; j < 11; j++){
				stage[j][i] = 7;//消して
			}
			renewStage();//ステージを視覚的にも更新して
			lineNum++;
			renewDeletedLineNum();
			for(j = 0; j < 3000; j++);//プレイヤーに消えたことを見せて
	
			//一列ずらす
			for(j = 0; j < 11; j++){
				for(h = i - 1; h >= 0; h--){
					stage[j][h + 1] = stage[j][h];
				}
				stage[j][0] = 7;
			}
			
			renewStage();//ステージを視覚的に更新
			i++;
		}
	}
	//タイムアタックモードならクリアしているか確認
	if(lineNum >= 10){
		clearFlag = 1;
	}
	
	if(mode == 1 && lineNum >= 10){
		finFlag = 1;
	}
	
	//境界線を更新
	renewBorder();

	blockNum[0] = blockNum[1];
	blockNum[1] = blockNum[2];
	blockNum[2] = blockOrder[mod(orderNum, 49)];
	orderNum++;

	renewNextBlock(blockNum[1], blockNum[2]);

	coor.x = 8;
	coor.y = 1;
	turnNum = 0;
	cnt = 0;
	
	//次のブロックを設置
	for(i = 0; i < 4; i++){
		for(j = 0; j < 4; j++){
			locate(coor.x + j - 1, coor.y + i - 1);
			if(blockData[blockNum[0]][i][j] == 1){
				draw_block(blockColor[blockNum[0]]);
				if(stage[(coor.x + j - 5)][(coor.y + i - 1)] < 7){
					cnt++;
				}
			}
			blockState[i][j] = blockData[blockNum[0]][i][j];
		}
	}
	//もし新しいブロックをおけなければ終了
	if(cnt != 0){
		finFlag = 1;
	}

}


//0:水色 1:黄色 2:緑 3:赤色 4:紫 5:青色 6:オレンジ
void renewStage(){
	int i, j;
	for(j = 0; j < 19; j++){
		for(i = 0; i < 10; i++){
			locate(i + 4, j);
			if(stage[i][j] == 0){
				draw_block(blockColor[0]);
			}else if(stage[i][j] == 1){
				draw_block(blockColor[1]);
			}else if(stage[i][j] == 2){
				draw_block(blockColor[2]);
			}else if(stage[i][j] == 3){
				draw_block(blockColor[3]);
			}else if(stage[i][j] == 4){
				draw_block(blockColor[4]);
			}else if(stage[i][j] == 5){
				draw_block(blockColor[5]);
			}else if(stage[i][j] == 6){
				draw_block(blockColor[6]);
			}else{
				draw_block(0x0000);
			}
		}
	}
}


//境界線の更新
void renewBorder(){
	int i, j;
	for(i = 0; i < 10; i++){
		for(j = 0; j < 19; j++){
			if(stage[i][j] < 7){
				border[i] = j - 1;
				break;
			}
			if(j == 18){
				border[i] = 18;
			}
		}
	}
}


//右移動 1右に移動
void moveRight(){
	int i, j, check;

	check = 0;
	
	//移動できるか確認
	for(i = 0; i < 4; i++){
		for(j = 0; j < 4; j++){
			if(blockState[i][j] == 1){
				if(stage[(coor.x + j - 5 + 1)][(coor.y + i - 1)] < 7){
					check++;
				}
				if((coor.x + j - 1) >= 13){
					check++;
				}
			}
		}
	}

	if(check == 0){//移動できたら
		for(i = 0; i < 4; i++){
			for(j = 0; j < 4; j++){
				locate(coor.x + j - 1, coor.y + i - 1);
				if(blockState[i][j] == 1){
					draw_block(0x0000);
				}
			}
		}

		coor.x++;

		for(i = 0; i < 4; i++){
			for(j = 0; j < 4; j++){
				locate(coor.x + j - 1, coor.y + i - 1);
				if(blockState[i][j] == 1){
					draw_block(blockColor[blockNum[0]]);
				}
			}
		}
	}
}


//左移動 1左に移動
void moveLeft(){
	int i, j, check;

	check = 0;

	for(i = 0; i < 4; i++){
		for(j = 0; j < 4; j++){
			if(blockState[i][j] == 1){
				if(stage[coor.x + j - 5 - 1][coor.y + i - 1] < 7){
					check++;
				}
				if((coor.x + j - 1) <= 4){
					check++;
				}
			}
		}
	}

	if(check == 0){
		for(i = 0; i < 4; i++){
			for(j = 0; j < 4; j++){
				locate(coor.x + j - 1, coor.y + i - 1);
				if(blockState[i][j] == 1){
					draw_block(0x0000);
				}
			}
		}

		coor.x--;

		for(i = 0; i < 4; i++){
			for(j = 0; j < 4; j++){
				locate(coor.x + j - 1, coor.y + i - 1);
				if(blockState[i][j] == 1){
					draw_block(blockColor[blockNum[0]]);
				}
			}
		}
	}
}


//落下移動 1落ちる
void drop(){
	int i, j, check;

	check = 0;

	for(i = 0; i < 4; i++){
		for(j = 0; j < 4; j++){
			if(blockState[i][j] == 1){
				if(stage[coor.x + j - 5][coor.y + i - 1 + 1] < 7){
					check++;
				}
				if(border[coor.x + j - 5] == coor.y + i - 1){
					check++;
				}
			}
		}
	}

	if(check == 0){
		for(i = 0; i < 4; i++){
			for(j = 0; j < 4; j++){
				locate(coor.x + j - 1, coor.y + i - 1);
				if(blockState[i][j] == 1){
					draw_block(0x0000);
				}
			}
		}

		coor.y++;

		for(i = 0; i < 4; i++){
			for(j = 0; j < 4; j++){
				locate(coor.x + j - 1, coor.y + i - 1);
				if(blockState[i][j] == 1){
					draw_block(blockColor[blockNum[0]]);
				}
			}
		}
	}else{
		beforeNextBlock();
	}
}


//一気に落ちる
void fall(){
	int i, j, check, flag;
	
	while(1){
		check = 0;

		for(i = 0; i < 4; i++){
			for(j = 0; j < 4; j++){
				if(blockState[i][j] == 1){
					if(stage[coor.x + j - 5][coor.y + i - 1 + 1] < 7){
						check++;
					}
					if(border[coor.x + j - 5] == coor.y + i - 1){
						check++;
					}
				}
			}
		}
		if(check !=0){
			break;
		}
		drop();
	}
}


//回転
void turn(int turnNum){//回転後の回転状態
	int i, j, check;
	int tmp[4][4];
	
	//とりあえず回転させてみる
	if(turnNum == 0){
		for(i = 0; i < 4; i++){
			for(j = 0; j < 4; j++){
				tmp[i][j] = blockData[blockNum[0]][i][j];
			}
		}
	}else if(turnNum == 1){
		for(i = 0; i < 4; i++){
			for(j = 0; j < 4; j++){
				tmp[i][j] = blockData[blockNum[0]][3 - j][i];
			}
		}
	}else if(turnNum == 2){
		for(i = 0; i < 4; i++){
			for(j = 0; j < 4; j++){
				tmp[i][j] = blockData[blockNum[0]][3 - i][3 - j];
			}
		}
	}else if(turnNum == 3){
		for(i = 0; i < 4; i++){
			for(j = 0; j < 4; j++){
				tmp[i][j] = blockData[blockNum[0]][j][3 - i];
			}
		}
	}

	check = 0;
	//回転したら何かと接触するか確認
	for(i = 0; i < 4; i++){
		for(j = 0; j < 4; j++){
			if(tmp[i][j] == 1){
				if(stage[coor.x + j - 5][coor.y + i - 1] < 7){
					check++;
				}
				if(border[coor.x + j - 5] == coor.y + i - 1){
					check++;
				}
				if((coor.x + j - 1) >= 13){
					check++;	
				}
				if((coor.x + j - 1) <= 4){
					check++;
				}
			}
		}
	}

	if(check == 0){//接触してなかったら
		for(i = 0; i < 4; i++){
			for(j = 0; j < 4; j++){
				locate(coor.x + j - 1, coor.y + i - 1);
				if(blockState[i][j] == 1){
					draw_block(0x0000);
				}
			}
		}

		for(i = 0; i < 4; i++){
			for(j = 0; j < 4; j++){
				blockState[i][j] = tmp[i][j];
			}
		}

		for(i = 0; i < 4; i++){
			for(j = 0; j < 4; j++){
				locate(coor.x + j - 1, coor.y + i - 1);
				if(blockState[i][j] == 1){
					draw_block(blockColor[blockNum[0]]);
				}
			}
		}
	}
}


//次のブロックの更新
void renewNextBlock(hword num1, hword num2){
	int i, j;

	for(i = 0; i < 4; i++){
		for(j = 0; j < 4; j++){
			locate(17 + j, 4 + i);
			if(blockData[num1][i][j] == 1){
				draw_block(blockColor[num1]);
			}else{
				draw_block(0x0000);
			}
		}
	}

	for(i = 0; i < 4; i++){
		for(j = 0; j < 4; j++){
			locate(24 + j, 4 + i);
			if(blockData[num2][i][j] == 1){
				draw_block(blockColor[num2]);
			}else{
				draw_block(0x0000);
			}
		}
	}
}


//消したラインの数の更新
void renewDeletedLineNum(){
	locate(24, 15);
	printn(lineNum);
}


//ポーズモード
int mode_Pause(){
	int check;
	hword *ptr, tmp;
	createPause();

	check = 0;

	while(1){
	 	ptr = KEY_STATUS;
		tmp = ~*ptr & KEY_ALL;

		if(tmp == KEY_UP){
			if(check == 1){
				check = 0;
				locate(7, 9);
				draw_Quit(0xFFFF);
				locate(6, 7);
				draw_Resume(0x001F);
			}
		}else if(tmp == KEY_DOWN){
			if(check == 0){
				check = 1;
				locate(6, 7);
				draw_Resume(0xFFFF);
				locate(7, 9);
				draw_Quit(0x001F);
			}
		}else if(tmp == KEY_A){
			return check;
		}
	}
}


//PAUSE画面作成
void createPause(){
	int i, j;
	for(j = 3; j < 12; j++){
		for(i = 5; i < 13; i++){
			locate(i, j);
			if(i == 5 || i == 12 || j == 3 || j == 11){
				draw_block(0xFFFF);
			}else{
				draw_block(0x0000);
			}
		}
	}
	locate(7, 5);
	draw_Pause();
	locate(6, 7);
	draw_Resume(0x001F);
	locate(7, 9);
	draw_Quit(0xFFFF);
}


//「PAUSE」の表示
void draw_Pause(){
	prints("PAUSE");
}


//「RESUME」の表示
void draw_Resume(hword color){
	prints_color("RESUME", color);
}


//「QUIT」の表示
void draw_Quit(hword color){
	prints_color("QUIT", color);
}


//スコア画面の作成
void createScore(){
	draw_BigScore();
	locate(5, 9);
	draw_TimeAttack(0x7FFF);
	locate(18, 9);
	draw_Endless(0x7FFF);
	locate(13, 18);
	draw_Exit(0x001F);
	view_TimeAttack_Score();
	view_EndLess_Score();	
}


//大きな「SCORE」の表示
void draw_BigScore(){
	int x, y;
	//S
	for(x = 0; x < 5; x++){
		for(y = 0; y < 5; y++){
			if(S[x][y] == 1){
				locate(y + 2, x + 2);
				draw_block(0x03FF);
			}
		}
	}

	//C
	for(x = 0; x < 5; x++){
		for(y = 0; y < 5; y++){
			if(C[x][y] == 1){
				locate(y + 7, x + 2);
				draw_block(0x03FF);
			}
		}
	}

	//O
	for(x = 0; x < 5; x++){
		for(y = 0; y < 5; y++){
			if(O[x][y] == 1){
				locate(y + 12, x + 2);
				draw_block(0x03FF);
			}
		}
	}

	//R
	for(x = 0; x < 5; x++){
		for(y = 0; y < 5; y++){
			if(R[x][y] == 1){
				locate(y + 17, x + 2);
				draw_block(0x03FF);
			}
		}
	}

	//E
	for(x = 0; x < 5; x++){
		for(y = 0; y < 5; y++){
			if(E[x][y] == 1){
				locate(y + 22, x + 2);
				draw_block(0x03FF);
			}
		}
	}
}


//「 EXIT」の表示
void draw_Exit(hword color){
	prints_color("EXIT", color);
}


//タイムアタックのスコアを表示
void view_TimeAttack_Score(){
	int i;
	for(i = 0; i < 3; i++){
		locate(6, 11 + i * 2);
		printn(i + 1);
		locate(8, 11 + i * 2);
		display_time(timeAttackScore[i]);
	}
}


//エンドレスのスコアの表示
void view_EndLess_Score(){
	int i;
	for(i = 0; i < 3; i++){
		locate(18, 11 + i * 2);
		printn(i + 1);
		locate(20, 11 + i * 2);
		printn(endLessScore[i]);
	}
}


//文字座標(0, 0)～(29,19)の位置(cx, cy)を画面座標に変換
void locate(hword cx, hword cy){
	if (cx < LCD_CHAR_WIDTH ) {
		p.x = cx << 3;				//x座標の指定
	} else {
		p.x = LCD_CHAR_WIDTH - 1;	//xの最大値を設定
	}
	if (cy < LCD_CHAR_HEIGHT) {
		p.y = cy << 3;				//y座標の指定
	} else {
		p.y = LCD_CHAR_HEIGHT - 1;	//yの最大値を設定
	}
}


//引数を16進数として表示する関数（最大値16ビット）
void printh(hword val) {

	byte char_data[] = "0123456789ABCDEF";
	byte buf[7];
	hword tmp;
	int i;

	//入力値valの桁数判定
	tmp = val;
	for (i = 0; i < 4; i++) {
		if (tmp > 0xF){
			tmp = tmp >> 4;
		} else {
			break;
		}
	}

	//文字列の最後にNULLコード挿入（"0x"の先頭2文字分の値をiに加算)
	buf[i+3] = 0;

	//最下位桁の文字を挿入し，入力値を4ビットシフト（"0x"の先頭2文字分の値をiに加算)
	for (; i >= 0; i--) {
		buf[i+2] = char_data[val & 0xF];
		val = val >> 4;
	}

	//文字列先頭に16進数の"0x"を挿入
	buf[1] = 'x';
	buf[0] = '0';

	//文字列全体を表示
	prints(buf);
	
	return;
}


//引数を10進数として表示する関数（最大値16ビット)
void printn(hword val) {

	byte char_data[] = "0123456789";
	byte buf[6];
	hword tmp;
	int i;
	
	//入力値valの桁数判定
	tmp = val;
	for (i = 0; i < 5; i++) {
		if (tmp >= 10){
			tmp = div(tmp, 10);
		} else {
			break;
		}
	}

	//文字列の最後にNULLコード挿入
	buf[i+1] = 0;
	
	//最下位桁の文字（10で割った余り）を挿入し，入力値を10で割る
	for(; i >= 0; i--) {
		buf[i] = char_data[mod(val, 10)];
		val = div(val, 10);
	}
	
	//文字列全体を表示
	prints(buf);
	
	return;
}


//割り算の商を求める関数
hword div(hword dividened, hword divisor){
	
	hword quotient = 0;			//商
	
	while(1){
		if (dividened >= divisor){
			dividened = dividened - divisor;
			quotient++;
		} else {
			break;
		}
	}
	return quotient;
}


//割り算の余りを求める関数
hword mod(hword dividened, hword divisor){
	
	hword quotient = 0;			//商
	
	while(1){
		if (dividened >= divisor){
			dividened = dividened - divisor;
			quotient++;
		} else {
			break;
		}
	}
	return dividened;
}


//文字列表示関数
void prints(byte *str) {

	while(*str) {
		print_ascii(*str++);
		p.x = p.x + CHAR_WIDTH;			//表示位置を1文字右へ
	}	
}


//locateで設定された位置にASCIIコードascii_numに相当する文字を表示する関数
void print_ascii(byte ascii_num) {
	
	hword tx, ty;							//文字パターンの座標指定
	byte cbit;								//ビット判定

	for(ty = 0; ty < 8; ty++) {				//文字パターンのy座標指定
	
		cbit = 0x80;						//判定ビットを8ビットの最上位にセット
		
		for(tx = 0; tx < 8; tx++) {							//文字パターンのx座標指定
			if((char8x8[ascii_num][ty] & cbit) == cbit){			//ビット位置の判定
				draw_point((p.x + tx), (p.y + ty), BGR(0x1F, 0x1F, 0x1F));	// 1なら白
			}else {
				draw_point((p.x + tx), (p.y + ty), BGR(0x00, 0x00, 0x00));	// 0なら黒
			}
			cbit = cbit >> 1;					//判定ビットを1ビット右へシフト
		}
	}
}


//指定した座標(x, y) に指定した色colorの点を描画する関数
void draw_point(hword x, hword y, hword color) {

	hword *ptr;
	ptr = (hword *) VRAM;
	
	ptr = ptr + x + y * LCD_WIDTH;
	*ptr = color;

}


//8×8を塗りつぶす
void draw_block(hword color){
	hword x, y;

	for(x = 0; x < 8; x++){
		for(y = 0; y < 8; y++){
			draw_point((p.x + x), (p.y + y), color);
		}
	}
}


//文字列表示関数
void prints_color(byte *str, hword color) {

	while(*str) {
		print_ascii_color(*str++, color);
		p.x = p.x + CHAR_WIDTH;			//表示位置を1文字右へ
	}	
}


//locateで設定された位置にASCIIコードascii_numに相当する文字を表示する関数
void print_ascii_color(byte ascii_num, hword color) {
	
	hword tx, ty;							//文字パターンの座標指定
	byte cbit;								//ビット判定

	for(ty = 0; ty < 8; ty++) {				//文字パターンのy座標指定
	
		cbit = 0x80;						//判定ビットを8ビットの最上位にセット
		
		for(tx = 0; tx < 8; tx++) {							//文字パターンのx座標指定
			if((char8x8[ascii_num][ty] & cbit) == cbit){			// ビット位置の判定
				draw_point((p.x + tx), (p.y + ty), color);	// 1なら白
			}else {
				draw_point((p.x + tx), (p.y + ty), BGR(0x00, 0x00, 0x00));	// 0なら黒
			}
			cbit = cbit >> 1;					//判定ビットを1ビット右へシフト
		}
	}
}


void display_time(hword val) {
	hword h, m;

	m = mod(div(val, 10), 60);//秒
	h = div(div(val, 10), 60);//分

	//引数を10進数として表示する関数（最大値16ビット)

	byte char_data[] = "0123456789";
	byte buf[6];
	hword tmp;
	int i;

	//入力値valの桁数設定
	i = 1;

	//最下位桁の文字（10で割った余り）を挿入し，入力値を10で割る
	for(; i >= 0; i--) {
		buf[i] = char_data[mod(h, 10)];
		h = div(h, 10);
	}

	buf[2] = ':';

	//入力値valの桁数設定 */
	i = 4;

	//最下位桁の文字（10で割った余り）を挿入し，入力値を10で割る
	for(; i >= 3; i--) {
		buf[i] = char_data[mod(m, 10)];
			m = div(m, 10);
		}

	//文字列の最後にNULLコード挿入
	buf[5] = ' ';

	//文字列全体を表示
	prints(buf);

	return;
}


void init_timer(void) {

	/* タイマ制御レジスタ */
	*((hword *)0x04000100) = 0xFFFF - 1678;
	*((hword *)0x04000104) = 0xFFFF - 100;
	*((hword *)0x04000108) = 0xFFFF - 10;
	*((hword *)0x0400010C) = 0x0000;

	/* タイマ設定レジスタ */
	*((hword *)0x04000102) = 0x0000;
	*((hword *)0x04000106) = 0x0000;
	*((hword *)0x0400010A) = 0x0000;
	*((hword *)0x0400010E) = 0x0000;

		/* タイマ設定レジスタ */
	*((hword *)0x04000102) = 0x0000;
	*((hword *)0x04000106) = 0x0084;
	*((hword *)0x0400010A) = 0x0084;	// タイマ2割込み発生
	*((hword *)0x0400010E) = 0x0084;

}


void int_handler(){
	*((unsigned short *)0x04000208) = 0x0000;//全割り込み禁止

	if(*((unsigned short *)0x04000202) == 0x0020){

		//time表示位置設定
		locate(22, 11);
		display_time(CURRENT_TIME);

		count++;
		if(count >= 5){
			count = 0;
			drop();
		}

		*((unsigned short *)0x04000202) = 0x0020;//タイマ2割り込みクリア

	}

	*((unsigned short *)0x04000208) = 0x0001;//全割り込み許可
}
