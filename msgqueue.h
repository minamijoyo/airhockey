#ifndef _MSGQUEUE_H__
#define _MSGQUEUE_H__

/**********************************************************
//
//		Wih (Wireless Interface Hockey)
//
//		msgqueue.h (���b�Z�[�W�\���N���X)
//
//		Copyright (C) 2007,
//		Masayuki Morita, Kazuki Hamada, Tatsuya Mori,
//		All Rights Reserved.
**********************************************************/

#include<windows.h>
#include<deque>
#include<utility>
#include<string>
//���b�Z�[�W�E�B���h�E�p�̃L���[
class CMsgQueue{
	typedef std::pair<std::string,int> msg_t;
	typedef std::deque<msg_t> msgq_t;
	msgq_t msgq_;//���b�Z�[�W�L���[

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
	void push(const char* str, int msec);//���b�Z�[�W�ǉ�
	void pop();//���b�Z�[�W�폜
	void clear();//�L���[����ɂ���
	void timeout();//�^�C�}�[�؂�
	void draw(HDC hdc);//�`��
	bool exist() const { return !msgq_.empty(); }//���b�Z�[�W�����邩
};
#endif