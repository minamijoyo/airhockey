
/**********************************************************
//
//		Wih (Wireless Interface Hockey)
//
//		inputdevice.cpp (���̓f�o�C�X�N���X)
//
//		Copyright (C) 2007,
//		Masayuki Morita, Kazuki Hamada, Tatsuya Mori,
//		All Rights Reserved.
**********************************************************/

#include <stdlib.h>
#include "inputdevice.h"
#include "airhockey.h"

//�p�b�N���W�Ɉړ�
CPoint2 CInputDeviceMaster::get_pos(){
	return CPoint2 (pos_.x_,
					(p_airhockey_->p_puck()->pos().y_) - PLAYER_HEIGHT/2);
}

//�����_���ړ�
CPoint2 CInputDeviceRandom::get_pos(){
	//���񂩂Ɉ�񂾂����s
	if(rand()%5==0){
		//�p�b�N��Y���W�̎擾
		int py=p_airhockey_->p_puck()->pos().y_;
		int ny;
		//�p�b�N�̕����ֈړ�
		if(py>pos_.y_){
			ny=pos_.y_+PLAYER_HEIGHT/2;
		}else{
			ny=pos_.y_-PLAYER_HEIGHT/2;
		}
		return CPoint2( pos_.x_,ny);
	}else{
		return pos_;
	}
}

//�V���A���ʐM�f�[�^���瓾���ʒu���W�Ɉړ�
//0�`1�̐��K�����W����ʍ��W�ɕϊ�
CPoint2 CInputDeviceRs232c::get_pos(){
	double y=p_rs232c_->get_data(player_id_);
	return CPoint2(pos_.x_,(int)(y*(TABLE_HEIGHT-TABLE_WALL*2-PLAYER_HEIGHT)+TABLE_WALL));
}