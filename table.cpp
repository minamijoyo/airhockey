
/**********************************************************
//
//		Wih (Wireless Interface Hockey)
//
//		table.cpp (�e�[�u���N���X)
//
//		Copyright (C) 2007,
//		Masayuki Morita, Kazuki Hamada, Tatsuya Mori,
//		All Rights Reserved.
**********************************************************/



#include "table.h"
#include "resource.h"

extern HWND g_hwnd;
//�S�[������
int CTable::is_goal(const CPuck* p_puck) const{
	int ret=0;// not goal
	int x=p_puck->pos().x_;
	int y=p_puck->pos().y_;
	int r=p_puck->radius();

	int goal_y1= (height_-goal_)/2;
	int goal_y2= (height_+goal_)/2;
	
	if( (x-r < wall_) && (y-r > goal_y1) && (y+r < goal_y2) ){
		ret=2;//���̃S�[��(Player2�����_)
	}else if( (x+r > width_-wall_) && (y-r > goal_y1) && (y+r < goal_y2) ){
		ret=1;//�E�̃S�[��(Player1�����_)
	}

	return ret;
}

//�ǂ̓����蔻��
int CTable::is_hit(const CPuck* p_puck) const{
	int ret=0;// not hit
	int x=p_puck->pos().x_;
	int y=p_puck->pos().y_;
	int r=p_puck->radius();

	if( (y-r < wall_) || (y+r > height_-wall_) || (x-r < wall_) || (x+r > width_-wall_) ){
        //hit
		int dir=p_puck->direct();
		switch(dir){
			case 1://�E��֐i�s��
				if( (y-r < wall_) && (x+r > width_-wall_) ) ret=3;//�E��ɓ�������
				else if(y-r < wall_) ret=2;//��ɓ�������
				else ret=4;//�E�ɓ�������
				break;
			case 2://�E���֐i�s��
				if( (y+r > height_-wall_) && (x+r > width_-wall_) ) ret=4;//�E���ɓ�������
				else if(y+r > height_-wall_) ret=1;//���ɓ�������
				else ret=3;//�E�ɓ�������
				break;
			case 3://�����֐i�s��
				if( (y+r > height_-wall_) && (x-r < wall_) ) ret=1;//�����ɓ�������
				else if(y+r > height_-wall_) ret=4;//���ɓ�������
				else ret=2;//���ɓ�������
				break;
			case 4://����֐i�s��
				if( (y-r < wall_) && (x-r < wall_) ) ret=2;//����ɓ�������
				else if(y-r < wall_) ret=3;//��ɓ�������
				else ret=1;//���ɓ�������
				break;
		}
	}
	return ret;
}


void CTable::draw(HDC hdc) const{

	int goal_y1= (height_-goal_)/2;
	int goal_y2= (height_+goal_)/2;

	HBRUSH hOldBrush = (HBRUSH)SelectObject(hdc, (HBRUSH)GetStockObject(BLACK_BRUSH));
	Rectangle(hdc,0,0,width_,height_);//��
	SelectObject(hdc, (HBRUSH)GetStockObject(WHITE_BRUSH));
	Rectangle(hdc,wall_,wall_,width_-wall_,height_-wall_);//�t�B�[���h
	Rectangle(hdc,0,goal_y1,wall_,goal_y2);//�S�[��1
	Rectangle(hdc,width_-wall_,goal_y1,width_,goal_y2);//�S�[��2
	SelectObject(hdc, hOldBrush);

}


CTableBMP::CTableBMP(int width, int height, int wall, int goal):CTable(width,height,wall,goal){
	BITMAP bmp;

	//bmp�̓ǂݍ���
	hInst_ = (HINSTANCE)GetWindowLong(g_hwnd, GWL_HINSTANCE);
	hBitmap_ = LoadBitmap(hInst_, MAKEINTRESOURCE(IDB_BITMAP1));
    GetObject(hBitmap_, sizeof(BITMAP), &bmp);
    BMP_W_ = (int)bmp.bmWidth;
    BMP_H_ = (int)bmp.bmHeight;
}

CTableBMP::~CTableBMP(){
	//bmp�̊J��
	DeleteObject(hBitmap_);
}

void CTableBMP::draw(HDC hdc) const{
	HDC hmdc;

	//bmp�\��t��
	hmdc = CreateCompatibleDC(hdc);
    SelectObject(hmdc, hBitmap_);
    BitBlt(hdc, 0, 0, BMP_W_, BMP_H_, hmdc, 0, 0, SRCCOPY);
	DeleteDC(hmdc);
}