#ifndef _RS_H_
#define _RS_H_

/**********************************************************
//
//		Wih (Wireless Interface Hockey)
//
//		rs.h (RS232C�ɂ��V���A���ʐM�N���X)
//
//		Copyright (C) 2007,
//		Masayuki Morita, Kazuki Hamada, Tatsuya Mori,
//		All Rights Reserved.
**********************************************************/

#include <windows.h>
#include"player_data.h"

class CRs232c {
private:
    HANDLE Fhandle;
    DCB Fdcb;
    COMMTIMEOUTS Fcommtimeouts;
    COMSTAT Fcomstat;

    int Fport_num_;
    int getDCB(DCB *pdcb);
    int setDCB(DCB *pdcb);
	char str;
	int link_num;   //0��1��Player0�A2��3��Player1
	int x;
	CPlayerData* player_data_;
	int router_num;

	int status;//���e����̍ۂ̏�Ԃ�ێ�����

public:
    CRs232c(int port);	//�R���X�g���N�^
    ~CRs232c();			//�f�X�g���N�^
	bool init();	//������
	double get_data(int player_num);

    bool open(void);		//�R�l�N�V�����m��
    void close(void);	//�R�l�N�V�����J��

	
    int getch(void);		//�f�[�^��M
    void read(char* buf, int len);

	int getInque();
    int setTimeout(int timeval);
    BOOL clear(void);					//�o�b�t�@���̑S�f�[�^�폜
	void wait(void);
};

#endif
