
/**********************************************************
//
//		Wih (Wireless Interface Hockey)
//
//		rs.cpp (RS232Cによるシリアル通信クラス)
//
//		Copyright (C) 2007,
//		Masayuki Morita, Kazuki Hamada, Tatsuya Mori,
//		All Rights Reserved.
**********************************************************/


#include "rs.h"
#include"player_data.h"
#include<iostream>


CRs232c::CRs232c(int port)
{
    Fhandle = 0;
    ZeroMemory(&Fcomstat, sizeof(Fcomstat));
	Fport_num_=port;
	player_data_ = new CPlayerData(); 
	status=0;
	link_num=0;
	x=0;
	router_num=2;
}

bool CRs232c::init(){
	if (!open()){
		return false;
	}
    return true;
}

CRs232c::~CRs232c()
{
    close();
	delete player_data_;
}


//-------------------------------------------------------//
int CRs232c::getDCB(DCB *pdcb)
{
    return GetCommState(Fhandle, pdcb);
}

int CRs232c::setDCB(DCB *pdcb)
{
    return SetCommState(Fhandle, pdcb);
}


int CRs232c::setTimeout(int timeval)
{
    GetCommTimeouts(Fhandle, &Fcommtimeouts);

    Fcommtimeouts.ReadIntervalTimeout = 0;
    Fcommtimeouts.ReadTotalTimeoutMultiplier = 0;
    Fcommtimeouts.ReadTotalTimeoutConstant = timeval;
    Fcommtimeouts.WriteTotalTimeoutMultiplier = 0;
    Fcommtimeouts.WriteTotalTimeoutConstant = timeval;

    if (!SetCommTimeouts(Fhandle, &Fcommtimeouts)) {
        return -1;
    }
    return timeval;
}

int CRs232c::getInque(void)
{
    DWORD errorVal;

    Fcomstat.cbInQue = 0;
    ClearCommError(Fhandle, &errorVal, &Fcomstat);	//通信エラーの情報を取得して、通信デバイスの現在の状態を通知

    return Fcomstat.cbInQue;
}

BOOL CRs232c::clear(void)
{
    return PurgeComm(Fhandle,PURGE_TXCLEAR | PURGE_RXCLEAR);
	//指定した通信資源の出力バッファまたは入力バッファにあるすべての文字を破棄
	//未処理の読み取り操作または書き込み操作を中止することもできる
}


//-------------------------------------------------------//
bool CRs232c::open(void)
{
    char portName[6] = "COM";

    if (Fhandle != 0)
        close();
	if(Fport_num_>9){				//ポート番号>10
	    portName[3] = '1';
		portName[4] = (char)(Fport_num_-10) + '0';
	    portName[5] = '\0';
	}
	else{						//ポート番号<9
		portName[3] = (char)Fport_num_ + '0';
	    portName[4] = '\0';
	}
    Fhandle = CreateFile(portName,
                         GENERIC_READ | GENERIC_WRITE, 
                         0, NULL, OPEN_ALWAYS,
                         FILE_ATTRIBUTE_NORMAL, NULL);
    if (Fhandle==(HANDLE)0xffffffff)return false;
    getDCB(&Fdcb);
    Fdcb.BaudRate = CBR_19200;
    Fdcb.ByteSize = 8;
    Fdcb.StopBits = ONESTOPBIT;
    Fdcb.Parity = NOPARITY;
    Fdcb.fParity = false;
    Fdcb.fOutxCtsFlow = true;
    Fdcb.fRtsControl = RTS_CONTROL_HANDSHAKE;
    setDCB(&Fdcb);
    setTimeout(10000);
    clear();
    return true;
}

void CRs232c::close(void)
{
    if (Fhandle == 0)
        return;
    CloseHandle(Fhandle);
}


//-------------------------------------------------------//
int CRs232c::getch(void)
{
    int ch;

    read((char *)&ch, 1);
    return ch;
}

void CRs232c::read(char* buf, int len)
{
    DWORD readsize;
    int inquesize = getInque();

    if (len > inquesize)len = inquesize;
	if(len==0) strcpy(buf,"\0");
	else ReadFile(Fhandle, buf, len, &readsize, NULL);
}

void CRs232c::wait(){
	DWORD EventMask;
	SetCommMask(Fhandle, EV_RXCHAR);
	WaitCommEvent(Fhandle, &EventMask, NULL);
}

double CRs232c::get_data(int player_num){
	while(1){
		str=CRs232c::getch();
//		std::cout<<str;
		
		if(str=='\0'){				//終端文字が来た時点で終了
  			break;
		}
		else if(str=='\n'){			//改行文字が着たら、その時点までに読み込んでる情報をリセット
			status=0;
			link_num=0;
			x=0;
			router_num=2;
		}
		switch(status){
			case 0:
				if(str=='0') status=1;
//				if(str=='0') status=1;

				break;

			case 1:
				if(str=='0'){
					router_num=0;
					status=8;
				}
				else if(str=='1'){
					router_num=1;
					status=8;
				}
				break;
			case 2:
//				if(str==':') status=3;
				if(str==':') status=10;
				break;
			case 3:
				if(str=='0') status=4;
				break;
			case 4:
				if(str>'1'&&str<'6'){
					link_num=(int)str-'0'-2;
					status=5;
				}
				break;
			case 5:
				if(str==':') status=6;
				break;
			case 6:
				if(str>'/'&& str<':'){
					status=7;
					x=str-'0';
				}
				else if(str>'@'&&str<'G'){
					status=7;
					x=str-'A'+10;
				}
				break;
			case 7:
				if(str>'/'&& str<':'){
					if((router_num==0 && (link_num==0 || link_num==1))||(router_num==1 && (link_num==2 || link_num==3))){
						player_data_->update_link_quality(link_num,x*16+str-'0');
					}
					x=0;
					link_num=0;
					status=0;
					router_num=2;
				}
				else if(str>'@'&&str<'G'){
					if((router_num==0 && (link_num==0 || link_num==1))||(router_num==1 && (link_num==2 || link_num==3))){
						player_data_->update_link_quality(link_num,x*16+str-'A'+10);
					}
					x=0;
					link_num=0;
					status=0;
					router_num=2;
				}
				break;
			case 8:
				status=9;
				break;
			case 9:
				status=2;
				break;
			case 10:
				status=11;
				break;
			case 11:
				status=3;
				break;
			default:
				break;
		}
	}
	return player_data_->get_player_position(player_num-1);
}

