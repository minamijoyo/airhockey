#ifndef _DEF_COMMON_H__
#define _DEF_COMMON_H__

/**********************************************************
//
//		Wih (Wireless Interface Hockey)
//
//		def_common.h (�Q�[���̋��ʒ萔�ݒ�)
//
//		Copyright (C) 2007,
//		Masayuki Morita, Kazuki Hamada, Tatsuya Mori,
//		All Rights Reserved.
**********************************************************/

//���ʃw�b�_
#include <windows.h>
#include "point2.h"

#define SETTING_FILE "setting.txt"
/////////////////////////////////////////////////////
// �e����W��T�C�Y
//table
//�����̃e�[�u���T�C�Y�͉�80cm�A�c�͓K��
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

//�e�E�B���h�E�T�C�Y
#define PARENT_WIDTH (TABLE_WIDTH )
#define PARENT_HEIGHT (TABLE_HEIGHT + 100)

//���b�Z�[�W�e�L�X�g�\���̈�T�C�Y
#define MSG_WIDTH 450
#define MSG_HEIGHT 200
#define MSG_LEN_MAX 32

//�X�R�A�\���̈�T�C�Y
#define SCORE_WIDTH (TABLE_WIDTH-100)
#define SCORE_HEIGHT 80
#define SCORE_LEN_MAX 128

//���\�[�XID
#define	ID_TIMER_UPDATE		8001
#define ID_TIMER_MSG		8002
#define ID_TIMER_WAIT		8003

//���̓f�o�C�X�̒�`�p�萔
#define INPUT_RS232C 1
#define INPUT_CPU_MASTER 2
#define INPUT_CPU_RANDOM 3

//�Q�[���ݒ�\����
typedef struct tag_SGameSetting{
	int update_time_interval_;//�X�V�Ԋumsec(�Q�[���X�s�[�h�̋t��)
	int puck_step_;//�p�b�N�̐i�ދ���
	int game_score_;//���_��1�Q�[���Ƃ��邩
	int input_mode1_;//���̓f�o�C�X�̎��
	int input_mode2_;//���̓f�o�C�X�̎��
	int input_com_;//COM�|�[�g�̔ԍ�
	double input_kando_;//�Z���T���͂̊��x
}SGameSetting;


#endif