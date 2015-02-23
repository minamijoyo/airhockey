
/**********************************************************
//
//		Wih (Wireless Interface Hockey)
//
//		msgqueue.cpp (メッセージ表示クラス)
//
//		Copyright (C) 2007,
//		Masayuki Morita, Kazuki Hamada, Tatsuya Mori,
//		All Rights Reserved.
**********************************************************/


#include"msgqueue.h"
#include"def_common.h"

//前方宣言
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
	//タイマー削除
	KillTimer(hwnd_,timer_id_);
	//メッセージ削除
	pop();
}

//次のメッセージをセット
void CMsgQueue::set_next_msg(){
	//キューにメッセージがある場合
	if(exist()){
		//次のメッセージ取得
		msg_t msg=msgq_.front();
		now_str_=msg.first;
		//タイマー設定
		if(msg.second!=-1){
			SetTimer(hwnd_,timer_id_,msg.second,0);
		}else{
			//時間が-1の場合はpop()されるまで無期限
		}
	}
}

void CMsgQueue::draw(HDC hdc){
	int bkOld = SetBkMode(hdc, TRANSPARENT);

	//メッセージのフォント設定
	HFONT hFont = SetMyFont(hdc, (LPCTSTR)"HGS創英角ﾎﾟｯﾌﾟ体", 60);
    HFONT hFontOld = (HFONT)SelectObject(hdc, hFont);
	SetTextColor(hdc, RGB(255, 0, 0));
	SIZE text_metric;
	GetTextExtentPoint32(hdc , now_str_.c_str() , (int)now_str_.length() , &text_metric);

	//メッセージボックスのサイズ調整
	int width = (width_ > (text_metric.cx+50) ? width_ : (text_metric.cx+50));
	int height = (height_ > (text_metric.cy+50) ? height_ : (text_metric.cy+50));

	//メッセージボックス描画
	HBRUSH hBrush=CreateSolidBrush(RGB(255,215,215));
	HBRUSH hBrushOld=(HBRUSH)SelectObject(hdc,hBrush);
	HPEN hPenOld=(HPEN)SelectObject(hdc,GetStockObject(NULL_PEN));
	RoundRect(hdc,(TABLE_WIDTH-width)/2,(TABLE_HEIGHT-height)/2,
				(TABLE_WIDTH+width)/2,(TABLE_HEIGHT+height)/2,
				30,30);
	SelectObject(hdc,hPenOld);
	SelectObject(hdc,hBrushOld);
	DeleteObject(hBrush);

	//メッセージ描画
	TextOut(hdc, (TABLE_WIDTH-text_metric.cx)/2, (TABLE_HEIGHT-text_metric.cy)/2,
			now_str_.c_str() , (int)now_str_.length());
	
	SelectObject(hdc, hFontOld);
    DeleteObject(hFont);
	SetBkMode(hdc, bkOld);
}