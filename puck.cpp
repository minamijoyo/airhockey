
/**********************************************************
//
//		Wih (Wireless Interface Hockey)
//
//		puck.cpp (パッククラス)
//
//		Copyright (C) 2007,
//		Masayuki Morita, Kazuki Hamada, Tatsuya Mori,
//		All Rights Reserved.
**********************************************************/


#include "puck.h"

//方向から速度ベクトルを更新
void CPuck::set_direction(int direct){
	direct_=direct;
	switch(direct){
		case 0:
			break;
		case 1:
			v_.x_= step_;
			v_.y_= -step_;
			break;
		case 2:
			v_.x_= step_;
			v_.y_= step_;
			break;
		case 3:
			v_.x_= -step_;
			v_.y_= step_;
			break;
		case 4:
			v_.x_= -step_;
			v_.y_= -step_;
			break;
		default:
			v_.x_= 0;
			v_.y_= 0;
			break;
	}
}

void CPuck::draw(HDC hdc) const{
	//HBRUSH hBrush=CreateSolidBrush(RGB(225,225,255));
	//HBRUSH hBrushOld=(HBRUSH)SelectObject(hdc,hBrush);
	HBRUSH hBrushOld = (HBRUSH)SelectObject(hdc, (HBRUSH)GetStockObject(WHITE_BRUSH));
	Ellipse(hdc,pos_.x_-radius_,pos_.y_-radius_,pos_.x_+radius_,pos_.y_+radius_);
	SelectObject(hdc, hBrushOld);
	//DeleteObject(hBrush);
}