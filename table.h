#ifndef _TABLE_H__
#define _TABLE_H__

/**********************************************************
//
//		Wih (Wireless Interface Hockey)
//
//		table.h (テーブルクラス)
//
//		Copyright (C) 2007,
//		Masayuki Morita, Kazuki Hamada, Tatsuya Mori,
//		All Rights Reserved.
**********************************************************/

#include "def_common.h"
#include "puck.h"

//テーブルクラス
class CTable{
	int width_;
	int height_;
	int wall_;
	int goal_;
public:
	CTable(int width, int height, int wall, int goal):
		width_(width),height_(height),wall_(wall),goal_(goal){}
	virtual ~CTable(){}
	virtual void draw(HDC hdc) const;
	int is_goal(const CPuck* p_puck) const;
	int CTable::is_hit(const CPuck* p_puck) const;
	CPoint2 center() const{ return CPoint2(width_/2,height_/2); }
};

//背景にbmp埋め込み
class CTableBMP: public CTable{
    HBITMAP hBitmap_;
    HINSTANCE hInst_;
    int BMP_W_, BMP_H_;
public:
	CTableBMP(int width, int height, int wall, int goal);
	virtual ~CTableBMP();

	virtual void draw(HDC hdc) const;
};

#endif