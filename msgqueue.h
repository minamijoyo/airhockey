#ifndef _MSGQUEUE_H__
#define _MSGQUEUE_H__

/**********************************************************
//
//		Wih (Wireless Interface Hockey)
//
//		msgqueue.h (メッセージ表示クラス)
//
//		Copyright (C) 2007,
//		Masayuki Morita, Kazuki Hamada, Tatsuya Mori,
//		All Rights Reserved.
**********************************************************/

#include<windows.h>
#include<deque>
#include<utility>
#include<string>
//メッセージウィンドウ用のキュー
class CMsgQueue{
	typedef std::pair<std::string,int> msg_t;
	typedef std::deque<msg_t> msgq_t;
	msgq_t msgq_;//メッセージキュー

	HWND hwnd_;
	int timer_id_;
	int width_;
	int height_;
	std::string now_str_;

	void set_next_msg();

public:
	CMsgQueue(HWND hwnd, int timer_id, int width, int height)
		:hwnd_(hwnd),timer_id_(timer_id),width_(width),height_(height),now_str_(""){}
	~CMsgQueue();
	void push(const char* str, int msec);//メッセージ追加
	void pop();//メッセージ削除
	void clear();//キューを空にする
	void timeout();//タイマー切れ
	void draw(HDC hdc);//描画
	bool exist() const { return !msgq_.empty(); }//メッセージがあるか
};
#endif