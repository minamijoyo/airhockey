
/**********************************************************
//
//		Wih (Wireless Interface Hockey)
//
//		airhockey.cpp (ゲームのメインクラス)
//
//		Copyright (C) 2007,
//		Masayuki Morita, Kazuki Hamada, Tatsuya Mori,
//		All Rights Reserved.
**********************************************************/

#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include "airhockey.h"
#include "msgqueue.h"

//前方宣言
void WaitUpdate(int msec);
HFONT SetMyFont(HDC hdc, LPCTSTR face, int h);

extern CMsgQueue* g_pMsgQueue;
extern SGameSetting g_game_setting;

//初期化
bool CAirHockey::init(){
	if(state_!=EState::init_ng) return true;
	state_=EState::init_ng;
	
	//入力デバイスの初期化
	if(!init_input_device(1,g_game_setting.input_mode1_)) return false;
	if(!init_input_device(2,g_game_setting.input_mode2_)){
		delete p_inputdevice1_;//1は成功しているので初期化を開放
		return false;
	}

	//プレイヤーの初期化
	p_player1_=new CPlayer(CPoint2(PLAYER_X1,PLAYER_Y1),
						PLAYER_WIDTH,PLAYER_HEIGHT,p_inputdevice1_);
	p_player2_=new CPlayer(CPoint2(PLAYER_X2,PLAYER_Y2),
						PLAYER_WIDTH,PLAYER_HEIGHT,p_inputdevice2_);
	
	if(p_player1_ == NULL || p_player2_ == NULL) return false;

	//テーブルの初期化
	p_table_=new CTableBMP(TABLE_WIDTH,TABLE_HEIGHT,TABLE_WALL,TABLE_GOAL);
	if(p_table_==NULL) return false;

	//乱数の初期化
	srand((unsigned int)time(NULL));

	score1_=0;
	score2_=0;
	state_=EState::init_ok;
	return true;	
}

//入力デバイスの初期化
bool CAirHockey::init_input_device(int player_id, int type){
	//RS232Cの初期化
	if( (type==INPUT_RS232C) && (p_rs232c_==NULL) ){
		p_rs232c_=new CRs232c(g_game_setting.input_com_);
		if(p_rs232c_==NULL) return false;
		if(!p_rs232c_->init()){//初期化に失敗
			delete p_rs232c_;
			p_rs232c_=NULL;
			return false;
		}
	}

	//初期化対象の設定
	typedef CInputDevice* PCInputDevice;
	PCInputDevice& p_in= ((player_id==1)? p_inputdevice1_ : p_inputdevice2_);
	CPoint2 pos=((player_id==1)? CPoint2(PLAYER_X1,PLAYER_Y1) : CPoint2(PLAYER_X2,PLAYER_Y2));

	//入力デバイスの生成
	switch(type){
		case INPUT_RS232C:
			p_in=new CInputDeviceRs232c(pos,p_rs232c_,player_id);
			break;
		case INPUT_CPU_MASTER:
			p_in=new CInputDeviceMaster(pos,this);
			break;
		case INPUT_CPU_RANDOM:
			p_in=new CInputDeviceRandom(pos,this);
			break;
		default:
			return false;
	}
	if(p_in==NULL) return false;
	else return true;
}

//ゲーム開始
bool CAirHockey::start(){
	if(state_!=EState::init_ok) return false;

	//SPACE入力待ち
	WaitUpdate(-1);
	return true;
}

//外部から強制終了または点数で終了(game_win)後
bool CAirHockey::end(){
	if(!(state_==EState::game_play || state_==EState::game_win)) return false;

	state_=EState::game_end;

	//リセットor終了確認
	g_pMsgQueue->push("続ける？",1000);
	WaitUpdate(-1);
	return true;
}

//リセット
bool CAirHockey::reset(){
	if(state_!=EState::game_end) return false;

	score1_=0;
	score2_=0;
	state_=EState::game_countdown_start;
	return true;
}

//メモリ開放
bool CAirHockey::cleanup(){
	//delete
	delete p_puck_;
	delete p_table_;
	delete p_player2_;
	delete p_player1_;
	delete p_inputdevice2_;
	delete p_inputdevice1_;
	delete p_rs232c_;

	state_=EState::init_ng;
	return true;
}



///////////////////////////////////////////////////////////
// UPDATE_TIMER_INTERVAL間隔で呼ばれるゲームメインループ
void CAirHockey::update(){
	if(!wait_flg_){
		switch(state_){
			case EState::game_play:
				update_play();
				break;
			case EState::game_win:
				update_win();
				break;
			case EState::game_countdown_start:
				update_countdown_start();
				break;
			default:
				break;
		}
	}
}

//カウントダウン
void CAirHockey::update_countdown_start(){
	
	//パック生成
	p_puck_=new CPuck(p_table_->center(),g_game_setting.puck_step_,PUCK_RADIUS,(rand()%4)+1);

	//初期位置にリセット
	p_player1_->reset();
	p_player2_->reset();


	//カウントダウン表示
	g_pMsgQueue->push("さん",1000);
	g_pMsgQueue->push("にぃー",1000);
	g_pMsgQueue->push("いち",1000);
	g_pMsgQueue->push("すたーと！",300);
	WaitUpdate(3300);
}

//状態別のupdate()内部処理
// ゲーム中
void CAirHockey::update_play(){
	int goal;
	//パックを移動
	p_puck_->move();
	
	//ゴールの判定
	if(goal=p_table_->is_goal(p_puck_)){
		//ゴールした
		goal_sub(goal);
	}else{
		//ゴールしてない
		//プレイヤーの移動
		p_player1_->move();
		p_player2_->move();

		//当たり判定
		int direction;//ヒットした場合は反射方向、ヒットしなければ0
		if(direction=p_table_->is_hit(p_puck_)){
			p_puck_->set_direction(direction);
		}else if(direction=p_player1_->is_hit(p_puck_)){
			p_puck_->set_direction(direction);
		}else if(direction=p_player2_->is_hit(p_puck_)){
			p_puck_->set_direction(direction);
		}
	}
}

//得点の更新
void CAirHockey::goal_sub(int goal){
	//パックの消去
	delete p_puck_;
	p_puck_=NULL;

	//スコアの更新
	if(goal==1){
		score1_++;//Player1が得点	
	}else{
		score2_++;//Player2が得点	
	}
	
	//得点表示
	char str[MSG_LEN_MAX];
	sprintf(str,"%d   :   %d",score1_,score2_);
	g_pMsgQueue->push("GOOOOOA L ! !",1000);
	g_pMsgQueue->push(str,1500);

	//終了判定へ
	state_=EState::game_win;
}

//終了判定
void CAirHockey::update_win(){
	//ゲーム終了判定
	if( (score1_ >= g_game_setting.game_score_) || (score2_ >= g_game_setting.game_score_) ){
		//ゲーム終了
		char str[MSG_LEN_MAX];
		sprintf(str,"Player %d WIN",( (score1_>score2_) ? 1 : 2));
		g_pMsgQueue->push(str,1000);
		end();
	}else{
		//ゲーム継続、次のパック待ちカウントダウンへ
		WaitUpdate(-1);
		state_=EState::init_ok;
	}
	
}

///////////////////////////////////////////////////////
//	描画処理
void CAirHockey::draw(HDC hdc) const{
	if(state_!=EState::init_ng){
		//初期化されていればテーブルとプレイヤーは存在
		p_table_->draw(hdc);
		p_player1_->draw(hdc);
		p_player2_->draw(hdc);
		if(p_puck_!=NULL){//ゲーム遷移中はパックがない場合がある
			p_puck_->draw(hdc);
		}
		//スコア部分の描画
		draw_score(hdc);
	}
}

//スコア部分の描画
void CAirHockey::draw_score(HDC hdc) const{
	char str[SCORE_LEN_MAX];
	strcpy(str,"Player1                              Player2");
	
	int bkOld = SetBkMode(hdc, TRANSPARENT);


	//メッセージのフォント設定
	HFONT hFont = SetMyFont(hdc, (LPCTSTR)"HGS創英角ﾎﾟｯﾌﾟ体", 30);
    HFONT hFontOld = (HFONT)SelectObject(hdc, hFont);
	SetTextColor(hdc, RGB(0, 64, 128));
	SIZE text_metric;
	GetTextExtentPoint32(hdc , str , (int)strlen(str) , &text_metric);

	//メッセージボックス描画
	HPEN hPenOld=(HPEN)SelectObject(hdc,GetStockObject(NULL_PEN));

	HBRUSH hBrush=CreateSolidBrush(RGB(189,189,223));
	HBRUSH hBrushOld=(HBRUSH)SelectObject(hdc,hBrush);
	Rectangle(hdc,0,TABLE_HEIGHT+1,TABLE_WIDTH,PARENT_HEIGHT);
	
	HBRUSH hBrush2=CreateSolidBrush(RGB(215,255,255));
	SelectObject(hdc,hBrush2);
	
	RoundRect(hdc,(TABLE_WIDTH-SCORE_WIDTH)/2,
				TABLE_HEIGHT+10,
				(TABLE_WIDTH+SCORE_WIDTH)/2,
				PARENT_HEIGHT-10,
				30,30);

	SelectObject(hdc,hPenOld);
	SelectObject(hdc,hBrushOld);
	DeleteObject(hBrush);
	DeleteObject(hBrush2);

	//メッセージ描画
	//Player1枠
	TextOut(hdc, (TABLE_WIDTH-text_metric.cx)/2, TABLE_HEIGHT+(SCORE_HEIGHT-text_metric.cy)/2+10,
			str , (int)strlen(str));
	//スコア部分
	HFONT hFont2 = SetMyFont(hdc, (LPCTSTR)"HGS創英角ﾎﾟｯﾌﾟ体", 50);
	SelectObject(hdc,hFont2);
	SetTextColor(hdc, RGB(255, 128, 0));
	sprintf(str,"%2d  :  %2d",score1_,score2_);
	GetTextExtentPoint32(hdc , str , (int)strlen(str) , &text_metric);
	TextOut(hdc, (TABLE_WIDTH-text_metric.cx)/2, TABLE_HEIGHT+(SCORE_HEIGHT-text_metric.cy)/2+10,
			str , (int)strlen(str));

	//座標の表示
	int y1=(int)(100.0*(p_player1_->pos().y_-TABLE_WALL)/(TABLE_HEIGHT-TABLE_WALL*2-PLAYER_HEIGHT));
	int y2=(int)(100.0*(p_player2_->pos().y_-TABLE_WALL)/(TABLE_HEIGHT-TABLE_WALL*2-PLAYER_HEIGHT));
	sprintf(str,"%3d                                   %3d",y1,y2);
	GetTextExtentPoint32(hdc , str , (int)strlen(str) , &text_metric);
	TextOut(hdc, (TABLE_WIDTH-text_metric.cx)/2, TABLE_HEIGHT+(SCORE_HEIGHT-text_metric.cy)/2+10,
			str , (int)strlen(str));

	//後始末
	SelectObject(hdc, hFontOld);
	DeleteObject(hFont2);
    DeleteObject(hFont);
	SetBkMode(hdc, bkOld);
}