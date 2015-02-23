
/**********************************************************
//
//		Wih (Wireless Interface Hockey)
//
//		msgqueue.cpp (���b�Z�[�W�\���N���X)
//
//		Copyright (C) 2007,
//		Masayuki Morita, Kazuki Hamada, Tatsuya Mori,
//		All Rights Reserved.
**********************************************************/


#include"msgqueue.h"
#include"def_common.h"

//�O���錾
HFONT SetMyFont(HDC hdc, LPCTSTR face, int h);

CMsgQueue::~CMsgQueue(){
	clear();
}

void CMsgQueue::push(const char* str, int msec){
	msgq_.push_back(msg_t(std::string(str),msec));
	if(msgq_.size()==1){
		set_next_msg();
	}
}

void CMsgQueue::clear(){
	if(exist()){
		KillTimer(hwnd_,timer_id_);
		msgq_.clear();
	}
}
void CMsgQueue::pop(){
	msgq_.pop_front();
	set_next_msg();
}

void CMsgQueue::timeout(){
	//�^�C�}�[�폜
	KillTimer(hwnd_,timer_id_);
	//���b�Z�[�W�폜
	pop();
}

//���̃��b�Z�[�W���Z�b�g
void CMsgQueue::set_next_msg(){
	//�L���[�Ƀ��b�Z�[�W������ꍇ
	if(exist()){
		//���̃��b�Z�[�W�擾
		msg_t msg=msgq_.front();
		now_str_=msg.first;
		//�^�C�}�[�ݒ�
		if(msg.second!=-1){
			SetTimer(hwnd_,timer_id_,msg.second,0);
		}else{
			//���Ԃ�-1�̏ꍇ��pop()�����܂Ŗ�����
		}
	}
}

void CMsgQueue::draw(HDC hdc){
	int bkOld = SetBkMode(hdc, TRANSPARENT);

	//���b�Z�[�W�̃t�H���g�ݒ�
	HFONT hFont = SetMyFont(hdc, (LPCTSTR)"HGS�n�p�p�߯�ߑ�", 60);
    HFONT hFontOld = (HFONT)SelectObject(hdc, hFont);
	SetTextColor(hdc, RGB(255, 0, 0));
	SIZE text_metric;
	GetTextExtentPoint32(hdc , now_str_.c_str() , (int)now_str_.length() , &text_metric);

	//���b�Z�[�W�{�b�N�X�̃T�C�Y����
	int width = (width_ > (text_metric.cx+50) ? width_ : (text_metric.cx+50));
	int height = (height_ > (text_metric.cy+50) ? height_ : (text_metric.cy+50));

	//���b�Z�[�W�{�b�N�X�`��
	HBRUSH hBrush=CreateSolidBrush(RGB(255,215,215));
	HBRUSH hBrushOld=(HBRUSH)SelectObject(hdc,hBrush);
	HPEN hPenOld=(HPEN)SelectObject(hdc,GetStockObject(NULL_PEN));
	RoundRect(hdc,(TABLE_WIDTH-width)/2,(TABLE_HEIGHT-height)/2,
				(TABLE_WIDTH+width)/2,(TABLE_HEIGHT+height)/2,
				30,30);
	SelectObject(hdc,hPenOld);
	SelectObject(hdc,hBrushOld);
	DeleteObject(hBrush);

	//���b�Z�[�W�`��
	TextOut(hdc, (TABLE_WIDTH-text_metric.cx)/2, (TABLE_HEIGHT-text_metric.cy)/2,
			now_str_.c_str() , (int)now_str_.length());
	
	SelectObject(hdc, hFontOld);
    DeleteObject(hFont);
	SetBkMode(hdc, bkOld);
}