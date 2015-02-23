
/**********************************************************
//
//		Wih (Wireless Interface Hockey)
//
//		main.cpp (�E�B���h�E���b�Z�[�W�̐���)
//
//		Copyright (C) 2007,
//		Masayuki Morita, Kazuki Hamada, Tatsuya Mori,
//		All Rights Reserved.
**********************************************************/

#include <string.h>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include "resource.h"
#include "airhockey.h"
#include "msgqueue.h"

//�O���錾
//�A�v���P�[�V�����֌W
BOOL InitApp(HINSTANCE hInst, WNDPROC WndProc, LPCTSTR szClassName);
BOOL InitInstance(HINSTANCE hInst, LPCSTR szClassName, int nCmdShow);

//�v���V�[�W��
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp);
static LRESULT OnCreateProc(HWND hwnd , LPCREATESTRUCT lpcs);
static LRESULT OnDestroyProc(HWND hwnd);
static LRESULT OnPaintProc(HWND hwnd);
static LRESULT OnTimerProc(HWND hwnd, WPARAM wp);
static LRESULT OnKeydownProc(HWND hwnd,WPARAM wp);

//��ƃo�b�t�@�֌W
void CreateBackBuf(HWND hwnd);
void DeleteBackBuf();
void PaintScreen(HDC hdc);

//���̑�
HFONT SetMyFont(HDC hdc, LPCTSTR face, int h);
void WaitUpdate(int msec);
bool ReadSettingFile(const char* file);

//global
static HDC		g_BackBufDC;
static HBITMAP	g_BackBufBMP;
static CAirHockey* g_pAirhockey;
HWND		g_hwnd;
CMsgQueue* g_pMsgQueue;
SGameSetting g_game_setting;

//WinMain
int WINAPI WinMain(HINSTANCE hCurInst, HINSTANCE hPrevInst,
                   LPSTR lpsCmdLine, int nCmdShow)
{
	MSG msg;
	char szClassName[] = "Airhockey";        //�E�B���h�E�N���X
    
    if (!hPrevInst) {
        if (!InitApp(hCurInst, WndProc, szClassName))
            return FALSE;
    }
    if (!InitInstance(hCurInst, szClassName, nCmdShow)) {
        return FALSE;
    }

    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return (int) msg.wParam;
}

//�E�B���h�E�N���X�o�^
BOOL InitApp(HINSTANCE hInst, WNDPROC WndProc, LPCTSTR szClassName)
{
    WNDCLASS wc;
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = WndProc;    //�v���V�[�W����
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = hInst;        //�C���X�^���X
    wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
    wc.lpszMenuName = NULL;    //���j���[��
    wc.lpszClassName = (LPCSTR)szClassName;
    return (RegisterClass(&wc));
}

//�E�B���h�E�̐���
BOOL InitInstance(HINSTANCE hInst, LPCSTR szClassName, int nCmdShow)
{
    HWND hwnd;

    hwnd = CreateWindow(szClassName,
            "Wih (Wireless Interface Hockey)",//�^�C�g���o�[
            WS_OVERLAPPEDWINDOW,    //�E�B���h�E�̎��
            CW_USEDEFAULT,    //�w���W
            CW_USEDEFAULT,    //�x���W
            PARENT_WIDTH+5,    //��
            PARENT_HEIGHT+35,    //����
            NULL,            //�e�E�B���h�E�̃n���h���A�e�����Ƃ���NULL
            NULL,            //���j���[�n���h���A�N���X���j���[���g���Ƃ���NULL
            hInst,            //�C���X�^���X�n���h��
            NULL);
    if (!hwnd)
        return FALSE;

    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);
    return TRUE;
}

/////////////////////////////////////////////////////////
//�@�E�B���h�E�v���V�[�W��
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp)
{
	g_hwnd=hwnd;
	switch (msg) {
		case WM_PAINT:
			return OnPaintProc(hwnd);
		case WM_TIMER:
            return OnTimerProc(hwnd,wp);
		case WM_KEYDOWN:
			return OnKeydownProc(hwnd,wp);
		case WM_CREATE:
			return OnCreateProc(hwnd,(LPCREATESTRUCT)lp);
        case WM_DESTROY:
			OnDestroyProc(hwnd);
        default:
            return (DefWindowProc(hwnd, msg, wp, lp));
    }
    return 0L;
}


//WM_CREATE�n���h��
//�A�v���P�[�V����������
static LRESULT OnCreateProc(HWND hwnd , LPCREATESTRUCT lpcs)
{
	//�ݒ�t�@�C���̓ǂݍ���
	if(!ReadSettingFile(SETTING_FILE)){
		char str[256];
		sprintf(str,"�ݒ�t�@�C��%s��������܂���",SETTING_FILE);
		MessageBox(hwnd,str,"�������G���[",MB_OK | MB_ICONERROR);
		SendMessage(hwnd,WM_CLOSE,0,0);
		return 2;
	}

	//��Ɨp�o�b�t�@�̐���
	CreateBackBuf(hwnd);

	//���b�Z�[�W�{�b�N�X�̍쐬
	g_pMsgQueue=new CMsgQueue(hwnd,ID_TIMER_MSG,MSG_WIDTH,MSG_HEIGHT);

	//�Q�[�������̏�����
	g_pAirhockey=new CAirHockey();
	if(g_pAirhockey==NULL) return 1;
	if(!g_pAirhockey->init()){
		MessageBox(hwnd,"�������Ɏ��s���܂����B���̓f�o�C�X���m�F���Ă��������B","�������G���[",MB_OK | MB_ICONERROR);
				SendMessage(hwnd,WM_CLOSE,0,0);
		return 2;
	}

	//�^�C�}�[�쐬
	SetTimer(hwnd,ID_TIMER_UPDATE,g_game_setting.update_time_interval_,NULL);
	//�Q�[���J�n
	g_pAirhockey->start();

	return 0;
}


//WM_DESTROY�n���h��
//�A�v���P�[�V�����I��
static LRESULT OnDestroyProc(HWND hwnd)
{
	//�Q�[�������̌�n��
	if(g_pAirhockey!=NULL){
		g_pAirhockey->cleanup();
		delete g_pAirhockey;
	}
	//�`��֌W�̌�n��
	DeleteBackBuf();
	
	//�^�C�}�[
	KillTimer(hwnd,ID_TIMER_UPDATE);
	KillTimer(g_hwnd,ID_TIMER_WAIT);
	KillTimer(g_hwnd,ID_TIMER_MSG);
	delete g_pMsgQueue;
	PostQuitMessage(0);
	return 0;
}

//WM_PAINT�n���h��
//��ʕ`��
static LRESULT OnPaintProc(HWND hwnd)
{
	PAINTSTRUCT ps;
	HDC  wndDC;

	wndDC = BeginPaint(hwnd,&ps);
	if(g_pAirhockey!=NULL){
		//�`�揈��
		g_pAirhockey->draw(g_BackBufDC);
		//���b�Z�[�W������ꍇ�͕\��
		if(g_pMsgQueue->exist()){
			g_pMsgQueue->draw(g_BackBufDC);
		}
		
		//�o�b�t�@����ʂɏo��
		PaintScreen(wndDC);
	}
	EndPaint(hwnd,&ps);
	return 0;
}

//WM_TIMER�n���h��
//�^�C�}�[�Ăяo���ɂ����Ԋu���ƂɎ��s
static LRESULT OnTimerProc(HWND hwnd,WPARAM wp)
{	
	if(g_pAirhockey==NULL) return 0;
	switch(wp){
		case ID_TIMER_UPDATE:
			//�Q�[���̃��C�����[�v�p�^�C�}�[
			g_pAirhockey->update();
			break;
		case ID_TIMER_WAIT:
			//�Q�[������莞�Ԓ��f�p
			KillTimer(g_hwnd,ID_TIMER_WAIT);
			g_pAirhockey->set_wait_flg(false);
			if(g_pAirhockey->state()==CAirHockey::EState::game_countdown_start){
				g_pAirhockey->set_state(CAirHockey::EState::game_play);
			}
			break;
		case ID_TIMER_MSG:
			//���b�Z�[�W�p�^�C�}�[
			g_pMsgQueue->timeout();
			break;
		default:
			break;
	}
	InvalidateRect(hwnd,NULL,false);
	return 0;
}

//WM_KEYDOWN�n���h��
//�X�y�[�X�ɂ��Q�[���J�n
//�Q�[�����̓X�y�[�X�ňꎞ���f
static LRESULT OnKeydownProc(HWND hwnd,WPARAM wp){
	switch(wp){
		case VK_SPACE:
			if((g_pAirhockey!=NULL) && (g_pAirhockey->state()!=CAirHockey::EState::init_ng)){
				//�֌W�Ȃ���Ԃ̂Ƃ��̃X�y�[�X�͖���
				if(	!(g_pAirhockey->state()==CAirHockey::EState::game_play ||
					g_pAirhockey->state()==CAirHockey::EState::game_end ||
					g_pAirhockey->state()==CAirHockey::EState::init_ok)
					)
					break;

				if(g_pAirhockey->wait_flg()){
					//wait��
					g_pMsgQueue->clear();
					g_pAirhockey->set_wait_flg(false);
					if(g_pAirhockey->state()==CAirHockey::EState::game_end){
						//�Q�[���I����̏ꍇ�̓��Z�b�g
						g_pAirhockey->reset();
					}else if(g_pAirhockey->state()==CAirHockey::EState::init_ok){
						//�Q�[���J�nor���_��̍ĊJ�̏ꍇ�̓J�E���g�_�E����
						g_pAirhockey->set_state(CAirHockey::EState::game_countdown_start);
					}
				}else{
					//game���̃E�F�C�g�v��
					WaitUpdate(-1);
				}
			}
			break;
	}
	InvalidateRect(hwnd,NULL,false);
	return 0;
}

////////////////////////////////////////////////////////////
// ��Ɨp�o�b�t�@�֌W
// ������̗}���̂��ߍ�ƃo�b�t�@��ŕ`�悵�A��ʂɓ]��

//����
void CreateBackBuf(HWND hwnd)
{
	HDC WndDC = GetWindowDC(hwnd);
	g_BackBufDC = CreateCompatibleDC(WndDC);	//��ƗpDC���擾
	g_BackBufBMP= CreateCompatibleBitmap(WndDC,PARENT_WIDTH,PARENT_HEIGHT);
	ReleaseDC(hwnd,WndDC);

	SelectObject(g_BackBufDC,g_BackBufBMP);
}

//�폜
void DeleteBackBuf()
{
	DeleteDC(g_BackBufDC);			//��ƗpDC������
	DeleteObject(g_BackBufBMP);		//�ǂݍ��񂾉摜������
}


//�]��
void PaintScreen(HDC hdc)
{
	BitBlt(hdc,0,0,PARENT_WIDTH,PARENT_HEIGHT,
				g_BackBufDC,0,0,SRCCOPY); //�]��
}

//�t�H���g�쐬�p
HFONT SetMyFont(HDC hdc, LPCTSTR face, int h)
{
    HFONT hFont;
    hFont = CreateFont(h,    //�t�H���g����
        0,                    //������
        0,                    //�e�L�X�g�̊p�x
        0,                    //�x�[�X���C���Ƃ����Ƃ̊p�x
        FW_REGULAR,            //�t�H���g�̏d���i�����j
        FALSE,                //�C�^���b�N��
        FALSE,                //�A���_�[���C��
        FALSE,                //�ł�������
        SHIFTJIS_CHARSET,    //�����Z�b�g
        OUT_DEFAULT_PRECIS,    //�o�͐��x
        CLIP_DEFAULT_PRECIS,//�N���b�s���O���x
        PROOF_QUALITY,        //�o�͕i��
        FIXED_PITCH | FF_MODERN,//�s�b�`�ƃt�@�~���[
        face);    //���̖�
    return hFont;
}

//��莞�ԃE�F�C�g����
//���Ԃ�-1�̏ꍇ�̓X�y�[�X�����܂Ŗ������E�F�C�g
void WaitUpdate(int msec){
	g_pAirhockey->set_wait_flg(true);
	if(msec!=-1){
		SetTimer(g_hwnd,ID_TIMER_WAIT,msec,0);
	}else{
		//Space�����܂Ŗ������E�F�C�g
		g_pMsgQueue->push("Press SPACE",-1);
	}
}

/////////////////////////////////////////////////////////////
// �ݒ�t�@�C���̓ǂݍ���

//�s�ǂݔ�΂��}�N��
#define MACRO_SKEP_LINE() std::getline(ifs,line,'\n')

#define MACRO_SKEP_COMMENT() do{std::getline(ifs,line,'\n');}while((line.c_str()[0])=='#')

//�ݒ�t�@�C���̓ǂݍ���
//�ݒ�t�@�C���̓J�����g�f�B���N�g���ɒu������
bool ReadSettingFile(const char* file){
	std::ifstream ifs(file);
	if(!ifs.is_open()){//�t�@�C���I�[�v�����s
		return false;
	}else{//�t�@�C���I�[�v������
		std::string line;
		//�ݒ�f�[�^�̓ǂݍ���
		MACRO_SKEP_COMMENT();
		g_game_setting.update_time_interval_=atoi(line.c_str());
		MACRO_SKEP_COMMENT();
		g_game_setting.puck_step_=atoi(line.c_str());
		MACRO_SKEP_COMMENT();
		g_game_setting.game_score_=atoi(line.c_str());
		MACRO_SKEP_COMMENT();
		g_game_setting.input_mode1_=atoi(line.c_str());
		MACRO_SKEP_COMMENT();
		g_game_setting.input_mode2_=atoi(line.c_str());
		MACRO_SKEP_COMMENT();
		g_game_setting.input_com_=atoi(line.c_str());
		MACRO_SKEP_COMMENT();
		g_game_setting.input_kando_=atof(line.c_str());
		//�t�@�C�������
		ifs.close();
	}
	return true;
}