
/**********************************************************
//
//		Wih (Wireless Interface Hockey)
//
//		inputdevice.cpp (入力デバイスクラス)
//
//		Copyright (C) 2007,
//		Masayuki Morita, Kazuki Hamada, Tatsuya Mori,
//		All Rights Reserved.
**********************************************************/

#include <stdlib.h>
#include "inputdevice.h"
#include "airhockey.h"

//パック座標に移動
CPoint2 CInputDeviceMaster::get_pos(){
	return CPoint2 (pos_.x_,
					(p_airhockey_->p_puck()->pos().y_) - PLAYER_HEIGHT/2);
}

//ランダム移動
CPoint2 CInputDeviceRandom::get_pos(){
	//何回かに一回だけ実行
	if(rand()%5==0){
		//パックのY座標の取得
		int py=p_airhockey_->p_puck()->pos().y_;
		int ny;
		//パックの方向へ移動
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

//シリアル通信データから得た位置座標に移動
//0～1の正規化座標を画面座標に変換
CPoint2 CInputDeviceRs232c::get_pos(){
	double y=p_rs232c_->get_data(player_id_);
	return CPoint2(pos_.x_,(int)(y*(TABLE_HEIGHT-TABLE_WALL*2-PLAYER_HEIGHT)+TABLE_WALL));
}