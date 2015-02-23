#ifndef _AIRHOCKEY_H__
#define _AIRHOCKEY_H__

/**********************************************************
//
//		Wih (Wireless Interface Hockey)
//
//		airhockey.h (�Q�[���̃��C���N���X)
//
//		Copyright (C) 2007,
//		Masayuki Morita, Kazuki Hamada, Tatsuya Mori,
//		All Rights Reserved.
**********************************************************/

//VC++2005
#pragma warning(disable:4482)//enum�̏璷�Ȍ���
#pragma warning(disable:4996)//sprintf�񐄏�

#include "def_common.h"
#include "table.h"
#include "player.h"
#include "puck.h"
#include "inputdevice.h"

//AirHockey�N���X
//�Q�[���̃��C��
class CAirHockey{
public:
	//�Q�[���̏�ԃt���O�penum
	enum EState{
		init_ng, init_ok,
		game_play, game_end, game_win,
		game_countdown_start
	};
private:
	CTable* p_table_;//�e�[�u��
	CInputDevice* p_inputdevice1_;//���̓f�o�C�X
	CInputDevice* p_inputdevice2_;
	CPlayer* p_player1_;//�v���C���[
	CPlayer* p_player2_;
	CPuck* p_puck_;//�p�b�N
	CRs232c* p_rs232c_;//�V���A���ʐM

	//�Q�[���X�R�A
	int score1_;
	int score2_;

	EState state_;//�Q�[���t���O
	bool wait_flg_;//�E�F�C�g�t���O


	//���̓f�o�C�X�̏�����
	bool init_input_device(int player_id, int type);
	//��ԕʂ�update()��������
	void update_play();
	void update_win();
	void update_countdown_start();
	
	//goal���̏���
	void goal_sub(int goal);

	//score�`�揈��
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
	void update();//���Ԋu���ƂɌĂ΂�郁�C�����[�v
	
	//�A�N�Z�b�T
	void set_state(EState state){ state_=state; }
	EState state() const { return state_; }
	void set_wait_flg(bool flg){ wait_flg_=flg; }
	bool wait_flg() const { return wait_flg_; }
	const CPuck* p_puck() const{ return p_puck_; }
	
};
#endif