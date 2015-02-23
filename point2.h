#ifndef _POINT2_H__
#define _POINT2_H__

/**********************************************************
//
//		Wih (Wireless Interface Hockey)
//
//		point2.h (2次元座標クラス)
//
//		Copyright (C) 2007,
//		Masayuki Morita, Kazuki Hamada, Tatsuya Mori,
//		All Rights Reserved.
**********************************************************/

//2次元座標
class CPoint2{
public:
	int x_;
	int y_;
	CPoint2(){}
	CPoint2(const CPoint2& rhs){ x_=rhs.x_ ; y_=rhs.y_; }
	CPoint2(int x, int y):x_(x),y_(y){}
};

#endif