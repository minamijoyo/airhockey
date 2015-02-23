#ifndef _INPUTDEVICE_H__
#define _INPUTDEVICE_H__


/**********************************************************
//
//		Wih (Wireless Interface Hockey)
//
//		inputdevice.h (入力デバイスクラス)
//
//		Copyright (C) 2007,
//		Masayuki Morita, Kazuki Hamada, Tatsuya Mori,
//		All Rights Reserved.
**********************************************************/

#include "def_common.h"
#include "rs.h"

//入力デバイスのインターフェイス
class CInputDevice{
protected:
	CPoint2 pos_;//現在座標
	CPoint2 default_pos_;//初期座標
	virtual CPoint2 get_pos() =0;//座標計算用の仮想関数(ここを下位で実装)
public:
	CInputDevice(CPoint2 pos):pos_(pos){ default_pos_=pos_; }
	virtual ~CInputDevice(){}
	const CPoint2& pos() { return (pos_=get_pos()); }//座標を更新して値を返す
	const CPoint2& raw_pos() const { return pos_; }//座標の更新せずに値を返す
	void reset(){ pos_=default_pos_; }//デフォルト座標に移動
	virtual bool init()=0;
	virtual bool cleanup()=0;
};

//前方宣言
class CAirHockey;

//パックy座標に移動する最強モード(デバッグ用)
class CInputDeviceMaster: public CInputDevice{
private:
	CAirHockey* p_airhockey_;
protected:
	virtual CPoint2 get_pos();
public:
	CInputDeviceMaster(CPoint2 pos, CAirHockey* p_airhockey):CInputDevice(pos),
		p_airhockey_(p_airhockey){}
	virtual ~CInputDeviceMaster(){}

	virtual bool init(){ return true; }
	virtual bool cleanup(){ return true; }
};

//ランダム移動(CPU対戦用)
class CInputDeviceRandom: public CInputDevice{
private:
	CAirHockey* p_airhockey_;
protected:
	virtual CPoint2 get_pos();
public:
	CInputDeviceRandom(CPoint2 pos, CAirHockey* p_airhockey):CInputDevice(pos),
		p_airhockey_(p_airhockey){}
	virtual ~CInputDeviceRandom(){}

	virtual bool init(){ return true; }
	virtual bool cleanup(){ return true; }
};

//シリアルポート用
class CInputDeviceRs232c: public CInputDevice{
private:
	CRs232c* p_rs232c_;
	int player_id_;
protected:
	virtual CPoint2 get_pos();
public:
	CInputDeviceRs232c(CPoint2 pos, CRs232c* p_rs232c, int player_id):CInputDevice(pos),
		p_rs232c_(p_rs232c),player_id_(player_id){}
	virtual ~CInputDeviceRs232c(){}

	virtual bool init(){ return true; }//初期化はCRs232c生成した上位で行う
	virtual bool cleanup(){ return true; }
};

#endif