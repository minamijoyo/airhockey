#ifndef _INPUTDEVICE_H__
#define _INPUTDEVICE_H__


/**********************************************************
//
//		Wih (Wireless Interface Hockey)
//
//		inputdevice.h (���̓f�o�C�X�N���X)
//
//		Copyright (C) 2007,
//		Masayuki Morita, Kazuki Hamada, Tatsuya Mori,
//		All Rights Reserved.
**********************************************************/

#include "def_common.h"
#include "rs.h"

//���̓f�o�C�X�̃C���^�[�t�F�C�X
class CInputDevice{
protected:
	CPoint2 pos_;//���ݍ��W
	CPoint2 default_pos_;//�������W
	virtual CPoint2 get_pos() =0;//���W�v�Z�p�̉��z�֐�(���������ʂŎ���)
public:
	CInputDevice(CPoint2 pos):pos_(pos){ default_pos_=pos_; }
	virtual ~CInputDevice(){}
	const CPoint2& pos() { return (pos_=get_pos()); }//���W���X�V���Ēl��Ԃ�
	const CPoint2& raw_pos() const { return pos_; }//���W�̍X�V�����ɒl��Ԃ�
	void reset(){ pos_=default_pos_; }//�f�t�H���g���W�Ɉړ�
	virtual bool init()=0;
	virtual bool cleanup()=0;
};

//�O���錾
class CAirHockey;

//�p�b�Ny���W�Ɉړ�����ŋ����[�h(�f�o�b�O�p)
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

//�����_���ړ�(CPU�ΐ�p)
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

//�V���A���|�[�g�p
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

	virtual bool init(){ return true; }//��������CRs232c����������ʂōs��
	virtual bool cleanup(){ return true; }
};

#endif