
/**********************************************************
//
//		Wih (Wireless Interface Hockey)
//
//		player.cpp (プレイヤークラス)
//
//		Copyright (C) 2007,
//		Masayuki Morita, Kazuki Hamada, Tatsuya Mori,
//		All Rights Reserved.
**********************************************************/


#include "player.h"

//プレイヤーの移動
void CPlayer::move(){
	pos_= p_inputdevice_->pos();
	if(pos_.y_ < TABLE_WALL){
		pos_.y_=TABLE_WALL;
	}else if( (pos_.y_+PLAYER_HEIGHT) > (TABLE_HEIGHT-TABLE_WALL) ){
		pos_.y_=TABLE_HEIGHT-TABLE_WALL-PLAYER_HEIGHT;
	}
}
//当たり判定
int CPlayer::is_hit(const CPuck* p_puck){
	int ret=0;// 反射方向(当たっていない場合は0)
	int x=p_puck->pos().x_;//パックのX座標
	int y=p_puck->pos().y_;//パックのX座標
	int r=p_puck->radius();//パックの半径

	//当たり判定
	if( (pos_.x_ < x+r) && (pos_.x_+width_ > x-r) && 
		(pos_.y_ < y+r) && (pos_.y_+height_ > y-r) ){
        //当たった
		int dir=p_puck->direct();
		switch(dir){
			case 1://右上へ進行中
				if( (pos_.y_ < y+r ) && (pos_.y_+height_ > y) ) ret=4;//左に当たった
				else if( (pos_.x_ < x) && (pos_.x_+width_ > x-r) ) ret=2;//下に当たった
				else if( (pos_.x_  > x) && (pos_.y_+height_ < y) ) ret=3;//左下に当たった
				break;
			case 2://右下へ進行中
				if( (pos_.y_ < y) && (pos_.y_+height_ > y-r) ) ret=3;//左に当たった
				else if( (pos_.x_ < x) && (pos_.x_+width_ > x-r) ) ret=1;//上に当たった
				else if( (pos_.x_ > x) && (pos_.y_ > y) ) ret=4;//左上に当たった
				break;
			case 3://左下へ進行中
				if( (pos_.y_ < y) && (pos_.y_+height_ > y-r) ) ret=2;//右に当たった
				else if( (pos_.x_ < x+r) && (pos_.x_+width_ > x) ) ret=4;//上に当たった
				else if( (pos_.x_+width_ < x) && (pos_.y_ > y) ) ret=1;//右上に当たった
				break;
			case 4://左上へ進行中
				if( (pos_.y_ < y+r) && (pos_.y_+height_ > y) ) ret=1;//右に当たった
				else if( (pos_.x_ < x+r) && (pos_.x_+width_ > x) ) ret=3;//下に当たった
				else if( (pos_.x_+width_ < x) && (pos_.y_+height_ < y) ) ret=2;//右下に当たった
				break;
		}
	}
	//連続ヒットの防止
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