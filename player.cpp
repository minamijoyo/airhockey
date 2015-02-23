
/**********************************************************
//
//		Wih (Wireless Interface Hockey)
//
//		player.cpp (�v���C���[�N���X)
//
//		Copyright (C) 2007,
//		Masayuki Morita, Kazuki Hamada, Tatsuya Mori,
//		All Rights Reserved.
**********************************************************/


#include "player.h"

//�v���C���[�̈ړ�
void CPlayer::move(){
	pos_= p_inputdevice_->pos();
	if(pos_.y_ < TABLE_WALL){
		pos_.y_=TABLE_WALL;
	}else if( (pos_.y_+PLAYER_HEIGHT) > (TABLE_HEIGHT-TABLE_WALL) ){
		pos_.y_=TABLE_HEIGHT-TABLE_WALL-PLAYER_HEIGHT;
	}
}
//�����蔻��
int CPlayer::is_hit(const CPuck* p_puck){
	int ret=0;// ���˕���(�������Ă��Ȃ��ꍇ��0)
	int x=p_puck->pos().x_;//�p�b�N��X���W
	int y=p_puck->pos().y_;//�p�b�N��X���W
	int r=p_puck->radius();//�p�b�N�̔��a

	//�����蔻��
	if( (pos_.x_ < x+r) && (pos_.x_+width_ > x-r) && 
		(pos_.y_ < y+r) && (pos_.y_+height_ > y-r) ){
        //��������
		int dir=p_puck->direct();
		switch(dir){
			case 1://�E��֐i�s��
				if( (pos_.y_ < y+r ) && (pos_.y_+height_ > y) ) ret=4;//���ɓ�������
				else if( (pos_.x_ < x) && (pos_.x_+width_ > x-r) ) ret=2;//���ɓ�������
				else if( (pos_.x_  > x) && (pos_.y_+height_ < y) ) ret=3;//�����ɓ�������
				break;
			case 2://�E���֐i�s��
				if( (pos_.y_ < y) && (pos_.y_+height_ > y-r) ) ret=3;//���ɓ�������
				else if( (pos_.x_ < x) && (pos_.x_+width_ > x-r) ) ret=1;//��ɓ�������
				else if( (pos_.x_ > x) && (pos_.y_ > y) ) ret=4;//����ɓ�������
				break;
			case 3://�����֐i�s��
				if( (pos_.y_ < y) && (pos_.y_+height_ > y-r) ) ret=2;//�E�ɓ�������
				else if( (pos_.x_ < x+r) && (pos_.x_+width_ > x) ) ret=4;//��ɓ�������
				else if( (pos_.x_+width_ < x) && (pos_.y_ > y) ) ret=1;//�E��ɓ�������
				break;
			case 4://����֐i�s��
				if( (pos_.y_ < y+r) && (pos_.y_+height_ > y) ) ret=1;//�E�ɓ�������
				else if( (pos_.x_ < x+r) && (pos_.x_+width_ > x) ) ret=3;//���ɓ�������
				else if( (pos_.x_+width_ < x) && (pos_.y_+height_ < y) ) ret=2;//�E���ɓ�������
				break;
		}
	}
	//�A���q�b�g�̖h�~
	if(last_hit_flg_){
		if(ret==0){
			last_hit_flg_=false;
		}else{
			ret=0;
		}
	}else if(ret!=0){
		last_hit_flg_=true;

	}
	return ret;
}

void CPlayer::draw(HDC hdc) const{
	
	//HBRUSH hOldBrush = (HBRUSH)SelectObject(hdc, (HBRUSH)GetStockObject(DKGRAY_BRUSH));
	HBRUSH hOldBrush = (HBRUSH)SelectObject(hdc, (HBRUSH)GetStockObject(WHITE_BRUSH));
	Rectangle(hdc,pos_.x_,pos_.y_,pos_.x_+width_,pos_.y_+height_);
	SelectObject(hdc, hOldBrush);
}