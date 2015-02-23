#ifndef _RS_H_
#define _RS_H_

/**********************************************************
//
//		Wih (Wireless Interface Hockey)
//
//		rs.h (RS232Cによるシリアル通信クラス)
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
	int link_num;   //0と1がPlayer0、2と3がPlayer1
	int x;
	CPlayerData* player_data_;
	int router_num;

	int status;//内容判定の際の状態を保持する

public:
    CRs232c(int port);	//コンストラクタ
    ~CRs232c();			//デストラクタ
	bool init();	//初期化
	double get_data(int player_num);

    bool open(void);		//コネクション確立
    void close(void);	//コネクション開放

	
    int getch(void);		//データ受信
    void read(char* buf, int len);

	int getInque();
    int setTimeout(int timeval);
    BOOL clear(void);					//バッファ内の全データ削除
	void wait(void);
};

#endif
