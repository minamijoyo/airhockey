
/**********************************************************
//
//		Wih (Wireless Interface Hockey)
//
//		main.cpp (ウィンドウメッセージの制御)
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

//前方宣言
//アプリケーション関係
BOOL InitApp(HINSTANCE hInst, WNDPROC WndProc, LPCTSTR szClassName);
BOOL InitInstance(HINSTANCE hInst, LPCSTR szClassName, int nCmdShow);

//プロシージャ
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp);
static LRESULT OnCreateProc(HWND hwnd , LPCREATESTRUCT lpcs);
static LRESULT OnDestroyProc(HWND hwnd);
static LRESULT OnPaintProc(HWND hwnd);
static LRESULT OnTimerProc(HWND hwnd, WPARAM wp);
static LRESULT OnKeydownProc(HWND hwnd,WPARAM wp);

//作業バッファ関係
void CreateBackBuf(HWND hwnd);
void DeleteBackBuf();
void PaintScreen(HDC hdc);

//その他
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
	char szClassName[] = "Airhockey";        //ウィンドウクラス
    
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

//ウィンドウクラス登録
BOOL InitApp(HINSTANCE hInst, WNDPROC WndProc, LPCTSTR szClassName)
{
    WNDCLASS wc;
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = WndProc;    //プロシージャ名
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = hInst;        //インスタンス
    wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
    wc.lpszMenuName = NULL;    //メニュー名
    wc.lpszClassName = (LPCSTR)szClassName;
    return (RegisterClass(&wc));
}

//ウィンドウの生成
BOOL InitInstance(HINSTANCE hInst, LPCSTR szClassName, int nCmdShow)
{
    HWND hwnd;

    hwnd = CreateWindow(szClassName,
            "Wih (Wireless Interface Hockey)",//タイトルバー
            WS_OVERLAPPEDWINDOW,    //ウィンドウの種類
            CW_USEDEFAULT,    //Ｘ座標
            CW_USEDEFAULT,    //Ｙ座標
            PARENT_WIDTH+5,    //幅
            PARENT_HEIGHT+35,    //高さ
            NULL,            //親ウィンドウのハンドル、親を作るときはNULL
            NULL,            //メニューハンドル、クラスメニューを使うときはNULL
            hInst,            //インスタンスハンドル
            NULL);
    if (!hwnd)
        return FALSE;

    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);
    return TRUE;
}

/////////////////////////////////////////////////////////
//　ウィンドウプロシージャ
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


//WM_CREATEハンドラ
//アプリケーション初期化
static LRESULT OnCreateProc(HWND hwnd , LPCREATESTRUCT lpcs)
{
	//設定ファイルの読み込み
	if(!ReadSettingFile(SETTING_FILE)){
		char str[256];
		sprintf(str,"設定ファイル%sが見つかりません",SETTING_FILE);
		MessageBox(hwnd,str,"初期化エラー",MB_OK | MB_ICONERROR);
		SendMessage(hwnd,WM_CLOSE,0,0);
		return 2;
	}

	//作業用バッファの生成
	CreateBackBuf(hwnd);

	//メッセージボックスの作成
	g_pMsgQueue=new CMsgQueue(hwnd,ID_TIMER_MSG,MSG_WIDTH,MSG_HEIGHT);

	//ゲーム内部の初期化
	g_pAirhockey=new CAirHockey();
	if(g_pAirhockey==NULL) return 1;
	if(!g_pAirhockey->init()){
		MessageBox(hwnd,"初期化に失敗しました。入力デバイスを確認してください。","初期化エラー",MB_OK | MB_ICONERROR);
				SendMessage(hwnd,WM_CLOSE,0,0);
		return 2;
	}

	//タイマー作成
	SetTimer(hwnd,ID_TIMER_UPDATE,g_game_setting.update_time_interval_,NULL);
	//ゲーム開始
	g_pAirhockey->start();

	return 0;
}


//WM_DESTROYハンドラ
//アプリケーション終了
static LRESULT OnDestroyProc(HWND hwnd)
{
	//ゲーム内部の後始末
	if(g_pAirhockey!=NULL){
		g_pAirhockey->cleanup();
		delete g_pAirhockey;
	}
	//描画関係の後始末
	DeleteBackBuf();
	
	//タイマー
	KillTimer(hwnd,ID_TIMER_UPDATE);
	KillTimer(g_hwnd,ID_TIMER_WAIT);
	KillTimer(g_hwnd,ID_TIMER_MSG);
	delete g_pMsgQueue;
	PostQuitMessage(0);
	return 0;
}

//WM_PAINTハンドラ
//画面描画
static LRESULT OnPaintProc(HWND hwnd)
{
	PAINTSTRUCT ps;
	HDC  wndDC;

	wndDC = BeginPaint(hwnd,&ps);
	if(g_pAirhockey!=NULL){
		//描画処理
		g_pAirhockey->draw(g_BackBufDC);
		//メッセージがある場合は表示
		if(g_pMsgQueue->exist()){
			g_pMsgQueue->draw(g_BackBufDC);
		}
		
		//バッファを画面に出力
		PaintScreen(wndDC);
	}
	EndPaint(hwnd,&ps);
	return 0;
}

//WM_TIMERハンドラ
//タイマー呼び出しにより一定間隔ごとに実行
static LRESULT OnTimerProc(HWND hwnd,WPARAM wp)
{	
	if(g_pAirhockey==NULL) return 0;
	switch(wp){
		case ID_TIMER_UPDATE:
			//ゲームのメインループ用タイマー
			g_pAirhockey->update();
			break;
		case ID_TIMER_WAIT:
			//ゲームを一定時間中断用
			KillTimer(g_hwnd,ID_TIMER_WAIT);
			g_pAirhockey->set_wait_flg(false);
			if(g_pAirhockey->state()==CAirHockey::EState::game_countdown_start){
				g_pAirhockey->set_state(CAirHockey::EState::game_play);
			}
			break;
		case ID_TIMER_MSG:
			//メッセージ用タイマー
			g_pMsgQueue->timeout();
			break;
		default:
			break;
	}
	InvalidateRect(hwnd,NULL,false);
	return 0;
}

//WM_KEYDOWNハンドラ
//スペースによりゲーム開始
//ゲーム中はスペースで一時中断
static LRESULT OnKeydownProc(HWND hwnd,WPARAM wp){
	switch(wp){
		case VK_SPACE:
			if((g_pAirhockey!=NULL) && (g_pAirhockey->state()!=CAirHockey::EState::init_ng)){
				//関係ない状態のときのスペースは無視
				if(	!(g_pAirhockey->state()==CAirHockey::EState::game_play ||
					g_pAirhockey->state()==CAirHockey::EState::game_end ||
					g_pAirhockey->state()==CAirHockey::EState::init_ok)
					)
					break;

				if(g_pAirhockey->wait_flg()){
					//wait中
					g_pMsgQueue->clear();
					g_pAirhockey->set_wait_flg(false);
					if(g_pAirhockey->state()==CAirHockey::EState::game_end){
						//ゲーム終了後の場合はリセット
						g_pAirhockey->reset();
					}else if(g_pAirhockey->state()==CAirHockey::EState::init_ok){
						//ゲーム開始or得点後の再開の場合はカウントダウンへ
						g_pAirhockey->set_state(CAirHockey::EState::game_countdown_start);
					}
				}else{
					//game中のウェイト要求
					WaitUpdate(-1);
				}
			}
			break;
	}
	InvalidateRect(hwnd,NULL,false);
	return 0;
}

////////////////////////////////////////////////////////////
// 作業用バッファ関係
// ちらつきの抑制のため作業バッファ上で描画し、画面に転送

//生成
void CreateBackBuf(HWND hwnd)
{
	HDC WndDC = GetWindowDC(hwnd);
	g_BackBufDC = CreateCompatibleDC(WndDC);	//作業用DCを取得
	g_BackBufBMP= CreateCompatibleBitmap(WndDC,PARENT_WIDTH,PARENT_HEIGHT);
	ReleaseDC(hwnd,WndDC);

	SelectObject(g_BackBufDC,g_BackBufBMP);
}

//削除
void DeleteBackBuf()
{
	DeleteDC(g_BackBufDC);			//作業用DCを消去
	DeleteObject(g_BackBufBMP);		//読み込んだ画像も消去
}


//転送
void PaintScreen(HDC hdc)
{
	BitBlt(hdc,0,0,PARENT_WIDTH,PARENT_HEIGHT,
				g_BackBufDC,0,0,SRCCOPY); //転送
}

//フォント作成用
HFONT SetMyFont(HDC hdc, LPCTSTR face, int h)
{
    HFONT hFont;
    hFont = CreateFont(h,    //フォント高さ
        0,                    //文字幅
        0,                    //テキストの角度
        0,                    //ベースラインとｘ軸との角度
        FW_REGULAR,            //フォントの重さ（太さ）
        FALSE,                //イタリック体
        FALSE,                //アンダーライン
        FALSE,                //打ち消し線
        SHIFTJIS_CHARSET,    //文字セット
        OUT_DEFAULT_PRECIS,    //出力精度
        CLIP_DEFAULT_PRECIS,//クリッピング精度
        PROOF_QUALITY,        //出力品質
        FIXED_PITCH | FF_MODERN,//ピッチとファミリー
        face);    //書体名
    return hFont;
}

//一定時間ウェイトする
//時間が-1の場合はスペース押すまで無期限ウェイト
void WaitUpdate(int msec){
	g_pAirhockey->set_wait_flg(true);
	if(msec!=-1){
		SetTimer(g_hwnd,ID_TIMER_WAIT,msec,0);
	}else{
		//Space押すまで無期限ウェイト
		g_pMsgQueue->push("Press SPACE",-1);
	}
}

/////////////////////////////////////////////////////////////
// 設定ファイルの読み込み

//行読み飛ばしマクロ
#define MACRO_SKEP_LINE() std::getline(ifs,line,'\n')

#define MACRO_SKEP_COMMENT() do{std::getline(ifs,line,'\n');}while((line.c_str()[0])=='#')

//設定ファイルの読み込み
//設定ファイルはカレントディレクトリに置くこと
bool ReadSettingFile(const char* file){
	std::ifstream ifs(file);
	if(!ifs.is_open()){//ファイルオープン失敗
		return false;
	}else{//ファイルオープン成功
		std::string line;
		//設定データの読み込み
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
		//ファイルを閉じる
		ifs.close();
	}
	return true;
}