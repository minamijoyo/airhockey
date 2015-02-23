#ifndef _AIRHOCKEY_H__
#define _AIRHOCKEY_H__

/**********************************************************
//
//		Wih (Wireless Interface Hockey)
//
//		airhockey.h (ゲームのメインクラス)
//
//		Copyright (C) 2007,
//		Masayuki Morita, Kazuki Hamada, Tatsuya Mori,
//		All Rights Reserved.
**********************************************************/

//VC++2005
#pragma warning(disable:4482)//enumの冗長な限定
#pragma warning(disable:4996)//sprintf非推奨

#include "def_common.h"
#include "table.h"
#include "player.h"
#include "puck.h"
#include "inputdevice.h"

//AirHockeyクラス
//ゲームのメイン
class CAirHockey{
public:
	//ゲームの状態フラグ用enum
	enum EState{
		init_ng, init_ok,
		game_play, game_end, game_win,
		game_countdown_start
	};
private:
	CTable* p_table_;//テーブル
	CInputDevice* p_inputdevice1_;//入力デバイス
	CInputDevice* p_inputdevice2_;
	CPlayer* p_player1_;//プレイヤー
	CPlayer* p_player2_;
	CPuck* p_puck_;//パック
	CRs232c* p_rs232c_;//シリアル通信

	//ゲームスコア
	int score1_;
	int score2_;

	EState state_;//ゲームフラグ
	bool wait_flg_;//ウェイトフラグ


	//入力デバイスの初期化
	bool init_input_device(int player_id, int type);
	//状態別のupdate()内部処理
	void update_play();
	void update_win();
	void update_countdown_start();
	
	//goal時の処理
	void goal_sub(int goal);

	//score描画処理
	void draw_score(HDC hdc) const;

public:
	CAirHockey():p_table_(NULL),p_inputdevice1_(NULL),p_inputdevice2_(NULL),
		p_player1_(NULL),p_player2_(NULL),p_puck_(NULL),p_rs232c_(NULL),score1_(0),score2_(0),
		state_(EState::init_ng),wait_flg_(false){}
	~CAirHockey(){ if(state_!=EState::init_ng) cleanup(); }

	bool init();
	bool start();
	bool end();
	bool reset();
	bool cleanup();
	void draw(HDC hdc) const;
	void update();//一定間隔ごとに呼ばれるメインループ
	
	//アクセッサ
	void set_state(EState state){ state_=state; }
	EState state() const { return state_; }
	void set_wait_flg(bool flg){ wait_flg_=flg; }
	bool wait_flg() const { return wait_flg_; }
	const CPuck* p_puck() const{ return p_puck_; }
	
};
#endif