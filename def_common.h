#ifndef _DEF_COMMON_H__
#define _DEF_COMMON_H__

/**********************************************************
//
//		Wih (Wireless Interface Hockey)
//
//		def_common.h (ゲームの共通定数設定)
//
//		Copyright (C) 2007,
//		Masayuki Morita, Kazuki Hamada, Tatsuya Mori,
//		All Rights Reserved.
**********************************************************/

//共通ヘッダ
#include <windows.h>
#include "point2.h"

#define SETTING_FILE "setting.txt"
/////////////////////////////////////////////////////
// 各種座標やサイズ
//table
//実物のテーブルサイズは横80cm、縦は適当
#define TABLE_WIDTH 900
#define TABLE_HEIGHT 600
#define TABLE_WALL 30
#define TABLE_GOAL 300

//player
#define PLAYER_WIDTH (TABLE_WALL+20)
#define PLAYER_HEIGHT (120)
#define PLAYER_X1 (TABLE_WALL*2+40)
#define PLAYER_Y1 ((TABLE_HEIGHT - PLAYER_HEIGHT)/2)
#define PLAYER_X2 (TABLE_WIDTH - TABLE_WALL*2 - PLAYER_WIDTH -40)
#define PLAYER_Y2 ((TABLE_HEIGHT - PLAYER_HEIGHT)/2)

//puck
#define PUCK_RADIUS (TABLE_WALL/2)

//親ウィンドウサイズ
#define PARENT_WIDTH (TABLE_WIDTH )
#define PARENT_HEIGHT (TABLE_HEIGHT + 100)

//メッセージテキスト表示領域サイズ
#define MSG_WIDTH 450
#define MSG_HEIGHT 200
#define MSG_LEN_MAX 32

//スコア表示領域サイズ
#define SCORE_WIDTH (TABLE_WIDTH-100)
#define SCORE_HEIGHT 80
#define SCORE_LEN_MAX 128

//リソースID
#define	ID_TIMER_UPDATE		8001
#define ID_TIMER_MSG		8002
#define ID_TIMER_WAIT		8003

//入力デバイスの定義用定数
#define INPUT_RS232C 1
#define INPUT_CPU_MASTER 2
#define INPUT_CPU_RANDOM 3

//ゲーム設定構造体
typedef struct tag_SGameSetting{
	int update_time_interval_;//更新間隔msec(ゲームスピードの逆数)
	int puck_step_;//パックの進む距離
	int game_score_;//何点で1ゲームとするか
	int input_mode1_;//入力デバイスの種類
	int input_mode2_;//入力デバイスの種類
	int input_com_;//COMポートの番号
	double input_kando_;//センサ入力の感度
}SGameSetting;


#endif