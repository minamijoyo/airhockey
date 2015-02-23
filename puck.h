#ifndef _PUCK_H__
#define _PUCK_H__

/**********************************************************
//
//		Wih (Wireless Interface Hockey)
//
//		puck.h (パッククラス)
//
//		Copyright (C) 2007,
//		Masayuki Morita, Kazuki Hamada, Tatsuya Mori,
//		All Rights Reserved.
**********************************************************/


#include "def_common.h"

//パッククラス
class CPuck{
	CPoint2 pos_;//現在座標
	CPoint2 v_;//移動速度
	int step_;//1ステップあたりの距離
	int radius_;//半径
	int direct_;//移動方向
public:
	CPuck(CPoint2 pos, int step, int radius, int direct)
		:pos_(pos),step_(step),radius_(radius){ set_direction(direct); }
	CPoint2 pos() const{ return pos_; }
	int step() const{ return step_; }
	int radius() const{ return radius_; }
	int direct() const{ return direct_; }
	void set_direction(int direct);

	void draw(HDC hdc) const;
	void move() { pos_.x_+= v_.x_; pos_.y_+= v_.y_; }
};
#endif