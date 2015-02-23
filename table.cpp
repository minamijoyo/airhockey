
/**********************************************************
//
//		Wih (Wireless Interface Hockey)
//
//		table.cpp (テーブルクラス)
//
//		Copyright (C) 2007,
//		Masayuki Morita, Kazuki Hamada, Tatsuya Mori,
//		All Rights Reserved.
**********************************************************/



#include "table.h"
#include "resource.h"

extern HWND g_hwnd;
//ゴール判定
int CTable::is_goal(const CPuck* p_puck) const{
	int ret=0;// not goal
	int x=p_puck->pos().x_;
	int y=p_puck->pos().y_;
	int r=p_puck->radius();

	int goal_y1= (height_-goal_)/2;
	int goal_y2= (height_+goal_)/2;
	
	if( (x-r < wall_) && (y-r > goal_y1) && (y+r < goal_y2) ){
		ret=2;//左のゴール(Player2が得点)
	}else if( (x+r > width_-wall_) && (y-r > goal_y1) && (y+r < goal_y2) ){
		ret=1;//右のゴール(Player1が得点)
	}

	return ret;
}

//壁の当たり判定
int CTable::is_hit(const CPuck* p_puck) const{
	int ret=0;// not hit
	int x=p_puck->pos().x_;
	int y=p_puck->pos().y_;
	int r=p_puck->radius();

	if( (y-r < wall_) || (y+r > height_-wall_) || (x-r < wall_) || (x+r > width_-wall_) ){
        //hit
		int dir=p_puck->direct();
		switch(dir){
			case 1://右上へ進行中
				if( (y-r < wall_) && (x+r > width_-wall_) ) ret=3;//右上に当たった
				else if(y-r < wall_) ret=2;//上に当たった
				else ret=4;//右に当たった
				break;
			case 2://右下へ進行中
				if( (y+r > height_-wall_) && (x+r > width_-wall_) ) ret=4;//右下に当たった
				else if(y+r > height_-wall_) ret=1;//下に当たった
				else ret=3;//右に当たった
				break;
			case 3://左下へ進行中
				if( (y+r > height_-wall_) && (x-r < wall_) ) ret=1;//左下に当たった
				else if(y+r > height_-wall_) ret=4;//下に当たった
				else ret=2;//左に当たった
				break;
			case 4://左上へ進行中
				if( (y-r < wall_) && (x-r < wall_) ) ret=2;//左上に当たった
				else if(y-r < wall_) ret=3;//上に当たった
				else ret=1;//左に当たった
				break;
		}
	}
	return ret;
}


void CTable::draw(HDC hdc) const{

	int goal_y1= (height_-goal_)/2;
	int goal_y2= (height_+goal_)/2;

	HBRUSH hOldBrush = (HBRUSH)SelectObject(hdc, (HBRUSH)GetStockObject(BLACK_BRUSH));
	Rectangle(hdc,0,0,width_,height_);//壁
	SelectObject(hdc, (HBRUSH)GetStockObject(WHITE_BRUSH));
	Rectangle(hdc,wall_,wall_,width_-wall_,height_-wall_);//フィールド
	Rectangle(hdc,0,goal_y1,wall_,goal_y2);//ゴール1
	Rectangle(hdc,width_-wall_,goal_y1,width_,goal_y2);//ゴール2
	SelectObject(hdc, hOldBrush);

}


CTableBMP::CTableBMP(int width, int height, int wall, int goal):CTable(width,height,wall,goal){
	BITMAP bmp;

	//bmpの読み込み
	hInst_ = (HINSTANCE)GetWindowLong(g_hwnd, GWL_HINSTANCE);
	hBitmap_ = LoadBitmap(hInst_, MAKEINTRESOURCE(IDB_BITMAP1));
    GetObject(hBitmap_, sizeof(BITMAP), &bmp);
    BMP_W_ = (int)bmp.bmWidth;
    BMP_H_ = (int)bmp.bmHeight;
}

CTableBMP::~CTableBMP(){
	//bmpの開放
	DeleteObject(hBitmap_);
}

void CTableBMP::draw(HDC hdc) const{
	HDC hmdc;

	//bmp貼り付け
	hmdc = CreateCompatibleDC(hdc);
    SelectObject(hmdc, hBitmap_);
    BitBlt(hdc, 0, 0, BMP_W_, BMP_H_, hmdc, 0, 0, SRCCOPY);
	DeleteDC(hmdc);
}