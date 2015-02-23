
/**********************************************************
//
//		Wih (Wireless Interface Hockey)
//
//		airhockey.cpp (�Q�[���̃��C���N���X)
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

//�O���錾
void WaitUpdate(int msec);
HFONT SetMyFont(HDC hdc, LPCTSTR face, int h);

extern CMsgQueue* g_pMsgQueue;
extern SGameSetting g_game_setting;

//������
bool CAirHockey::init(){
	if(state_!=EState::init_ng) return true;
	state_=EState::init_ng;
	
	//���̓f�o�C�X�̏�����
	if(!init_input_device(1,g_game_setting.input_mode1_)) return false;
	if(!init_input_device(2,g_game_setting.input_mode2_)){
		delete p_inputdevice1_;//1�͐������Ă���̂ŏ��������J��
		return false;
	}

	//�v���C���[�̏�����
	p_player1_=new CPlayer(CPoint2(PLAYER_X1,PLAYER_Y1),
						PLAYER_WIDTH,PLAYER_HEIGHT,p_inputdevice1_);
	p_player2_=new CPlayer(CPoint2(PLAYER_X2,PLAYER_Y2),
						PLAYER_WIDTH,PLAYER_HEIGHT,p_inputdevice2_);
	
	if(p_player1_ == NULL || p_player2_ == NULL) return false;

	//�e�[�u���̏�����
	p_table_=new CTableBMP(TABLE_WIDTH,TABLE_HEIGHT,TABLE_WALL,TABLE_GOAL);
	if(p_table_==NULL) return false;

	//�����̏�����
	srand((unsigned int)time(NULL));

	score1_=0;
	score2_=0;
	state_=EState::init_ok;
	return true;	
}

//���̓f�o�C�X�̏�����
bool CAirHockey::init_input_device(int player_id, int type){
	//RS232C�̏�����
	if( (type==INPUT_RS232C) && (p_rs232c_==NULL) ){
		p_rs232c_=new CRs232c(g_game_setting.input_com_);
		if(p_rs232c_==NULL) return false;
		if(!p_rs232c_->init()){//�������Ɏ��s
			delete p_rs232c_;
			p_rs232c_=NULL;
			return false;
		}
	}

	//�������Ώۂ̐ݒ�
	typedef CInputDevice* PCInputDevice;
	PCInputDevice& p_in= ((player_id==1)? p_inputdevice1_ : p_inputdevice2_);
	CPoint2 pos=((player_id==1)? CPoint2(PLAYER_X1,PLAYER_Y1) : CPoint2(PLAYER_X2,PLAYER_Y2));

	//���̓f�o�C�X�̐���
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

//�Q�[���J�n
bool CAirHockey::start(){
	if(state_!=EState::init_ok) return false;

	//SPACE���͑҂�
	WaitUpdate(-1);
	return true;
}

//�O�����狭���I���܂��͓_���ŏI��(game_win)��
bool CAirHockey::end(){
	if(!(state_==EState::game_play || state_==EState::game_win)) return false;

	state_=EState::game_end;

	//���Z�b�gor�I���m�F
	g_pMsgQueue->push("������H",1000);
	WaitUpdate(-1);
	return true;
}

//���Z�b�g
bool CAirHockey::reset(){
	if(state_!=EState::game_end) return false;

	score1_=0;
	score2_=0;
	state_=EState::game_countdown_start;
	return true;
}

//�������J��
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
// UPDATE_TIMER_INTERVAL�Ԋu�ŌĂ΂��Q�[�����C�����[�v
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

//�J�E���g�_�E��
void CAirHockey::update_countdown_start(){
	
	//�p�b�N����
	p_puck_=new CPuck(p_table_->center(),g_game_setting.puck_step_,PUCK_RADIUS,(rand()%4)+1);

	//�����ʒu�Ƀ��Z�b�g
	p_player1_->reset();
	p_player2_->reset();


	//�J�E���g�_�E���\��
	g_pMsgQueue->push("����",1000);
	g_pMsgQueue->push("�ɂ��[",1000);
	g_pMsgQueue->push("����",1000);
	g_pMsgQueue->push("�����[�ƁI",300);
	WaitUpdate(3300);
}

//��ԕʂ�update()��������
// �Q�[����
void CAirHockey::update_play(){
	int goal;
	//�p�b�N���ړ�
	p_puck_->move();
	
	//�S�[���̔���
	if(goal=p_table_->is_goal(p_puck_)){
		//�S�[������
		goal_sub(goal);
	}else{
		//�S�[�����ĂȂ�
		//�v���C���[�̈ړ�
		p_player1_->move();
		p_player2_->move();

		//�����蔻��
		int direction;//�q�b�g�����ꍇ�͔��˕����A�q�b�g���Ȃ����0
		if(direction=p_table_->is_hit(p_puck_)){
			p_puck_->set_direction(direction);
		}else if(direction=p_player1_->is_hit(p_puck_)){
			p_puck_->set_direction(direction);
		}else if(direction=p_player2_->is_hit(p_puck_)){
			p_puck_->set_direction(direction);
		}
	}
}

//���_�̍X�V
void CAirHockey::goal_sub(int goal){
	//�p�b�N�̏���
	delete p_puck_;
	p_puck_=NULL;

	//�X�R�A�̍X�V
	if(goal==1){
		score1_++;//Player1�����_	
	}else{
		score2_++;//Player2�����_	
	}
	
	//���_�\��
	char str[MSG_LEN_MAX];
	sprintf(str,"%d   :   %d",score1_,score2_);
	g_pMsgQueue->push("GOOOOOA L ! !",1000);
	g_pMsgQueue->push(str,1500);

	//�I�������
	state_=EState::game_win;
}

//�I������
void CAirHockey::update_win(){
	//�Q�[���I������
	if( (score1_ >= g_game_setting.game_score_) || (score2_ >= g_game_setting.game_score_) ){
		//�Q�[���I��
		char str[MSG_LEN_MAX];
		sprintf(str,"Player %d WIN",( (score1_>score2_) ? 1 : 2));
		g_pMsgQueue->push(str,1000);
		end();
	}else{
		//�Q�[���p���A���̃p�b�N�҂��J�E���g�_�E����
		WaitUpdate(-1);
		state_=EState::init_ok;
	}
	
}

///////////////////////////////////////////////////////
//	�`�揈��
void CAirHockey::draw(HDC hdc) const{
	if(state_!=EState::init_ng){
		//����������Ă���΃e�[�u���ƃv���C���[�͑���
		p_table_->draw(hdc);
		p_player1_->draw(hdc);
		p_player2_->draw(hdc);
		if(p_puck_!=NULL){//�Q�[���J�ڒ��̓p�b�N���Ȃ��ꍇ������
			p_puck_->draw(hdc);
		}
		//�X�R�A�����̕`��
		draw_score(hdc);
	}
}

//�X�R�A�����̕`��
void CAirHockey::draw_score(HDC hdc) const{
	char str[SCORE_LEN_MAX];
	strcpy(str,"Player1                              Player2");
	
	int bkOld = SetBkMode(hdc, TRANSPARENT);


	//���b�Z�[�W�̃t�H���g�ݒ�
	HFONT hFont = SetMyFont(hdc, (LPCTSTR)"HGS�n�p�p�߯�ߑ�", 30);
    HFONT hFontOld = (HFONT)SelectObject(hdc, hFont);
	SetTextColor(hdc, RGB(0, 64, 128));
	SIZE text_metric;
	GetTextExtentPoint32(hdc , str , (int)strlen(str) , &text_metric);

	//���b�Z�[�W�{�b�N�X�`��
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

	//���b�Z�[�W�`��
	//Player1�g
	TextOut(hdc, (TABLE_WIDTH-text_metric.cx)/2, TABLE_HEIGHT+(SCORE_HEIGHT-text_metric.cy)/2+10,
			str , (int)strlen(str));
	//�X�R�A����
	HFONT hFont2 = SetMyFont(hdc, (LPCTSTR)"HGS�n�p�p�߯�ߑ�", 50);
	SelectObject(hdc,hFont2);
	SetTextColor(hdc, RGB(255, 128, 0));
	sprintf(str,"%2d  :  %2d",score1_,score2_);
	GetTextExtentPoint32(hdc , str , (int)strlen(str) , &text_metric);
	TextOut(hdc, (TABLE_WIDTH-text_metric.cx)/2, TABLE_HEIGHT+(SCORE_HEIGHT-text_metric.cy)/2+10,
			str , (int)strlen(str));

	//���W�̕\��
	int y1=(int)(100.0*(p_player1_->pos().y_-TABLE_WALL)/(TABLE_HEIGHT-TABLE_WALL*2-PLAYER_HEIGHT));
	int y2=(int)(100.0*(p_player2_->pos().y_-TABLE_WALL)/(TABLE_HEIGHT-TABLE_WALL*2-PLAYER_HEIGHT));
	sprintf(str,"%3d                                   %3d",y1,y2);
	GetTextExtentPoint32(hdc , str , (int)strlen(str) , &text_metric);
	TextOut(hdc, (TABLE_WIDTH-text_metric.cx)/2, TABLE_HEIGHT+(SCORE_HEIGHT-text_metric.cy)/2+10,
			str , (int)strlen(str));

	//��n��
	SelectObject(hdc, hFontOld);
	DeleteObject(hFont2);
    DeleteObject(hFont);
	SetBkMode(hdc, bkOld);
}