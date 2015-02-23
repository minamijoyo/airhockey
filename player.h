#ifndef _PLAYER_H__
#define _PLAYER_H__

/**********************************************************
//
//		Wih (Wireless Interface Hockey)
//
//		player.h (�v���C���[�N���X)
//
//		Copyright (C) 2007,
//		Masayuki Morita, Kazuki Hamada, Tatsuya Mori,
//		All Rights Reserved.
**********************************************************/


#include "def_common.h"
#include "inputdevice.h"
#include "puck.h"

//�v���C���[�N���X
class CPlayer{
	CPoint2 pos_;//���ݍ��W
	CInputDevice* p_inputdevice_;//���̓f�o�C�X
	int width_;
	int height_;
	bool last_hit_flg_;//�A���q�b�g�̖h�~
public:
	CPlayer(CPoint2 default_pos, int width, int height, CInputDevice* p_inputdevice)
		:pos_(default_pos),width_(width), height_(height),p_inputdevice_(p_inputdevice),
	last_hit_flg_(false){}
	CPoint2 pos() const{ return pos_; }
	void reset(){ p_inputdevice_->reset(); pos_=p_inputdevice_->raw_pos(); }
	void draw(HDC hdc) const;
	void move();
	int is_hit(const CPuck* p_puck);//�����蔻��
	
};
#endif