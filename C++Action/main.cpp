//==========================================================================
// 
// C++DirectX[main.cpp]
// Author:相馬靜雅
// 
//==========================================================================
#include <stdio.h>
#include "main.h"
#include "manager.h"

//==========================================================================
// マクロ定義
//==========================================================================
#define CLASS_NAME		"WindowsClass"	// ウインドウクラスの名前
#define WINDOW_NAME		"EcoRumble"	// ウインドウの名前(キャプション)

//==========================================================================
// プロトタイプ宣言
//==========================================================================
LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

//==========================================================================
// グローバル変数宣言
//==========================================================================
int g_nCountFPS;

//==========================================================================
// メイン関数
//==========================================================================
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hInstancePrev, LPSTR lpCmbLine, int nCmbShow)
{
	// メモリリークを出力
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

	WNDCLASSEX wcex =
	{
		sizeof(WNDCLASSEX),					// WNDCLASSのメモリサイズ
		CS_CLASSDC,							// ウインドウのスタイル
		WindowProc,							// ウインドウプロシージャ
		0,									// 0にする(通常は使用しない)
		0,									// 0にする(通常は使用しない)
		hInstance,							// インスタンスハンドル
		LoadIcon(NULL, IDI_APPLICATION),	// タスクバーのアイコン
		LoadCursor(NULL, IDC_ARROW),		// マウスカーソル
		(HBRUSH)(COLOR_WINDOW + 1),			// クライアント領域の背景色
		NULL,								// メニューバー
		CLASS_NAME,							// ウインドウクラスの名前
		LoadIcon(NULL, IDI_APPLICATION)		// ファイルのアイコン
	};

	HWND hWnd;	// ウインドウハンドル(識別子)
	MSG msg;	// メッセージを格納する変数

	RECT rect = { 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT };	// 画面サイズの構造体

	// ウインドウクラスの登録
	RegisterClassEx(&wcex);

	// クライアント領域を指定のサイズに調整
	AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW, FALSE);

	// ウインドウの生成
	hWnd = CreateWindowEx(
		0,									// 拡張ウインドウスタイル
		CLASS_NAME,							// ウインドウクラスの名前
		WINDOW_NAME,						// ウインドウの名前
		WS_OVERLAPPEDWINDOW,				// ウインドウスタイル
		CW_USEDEFAULT,						// ウインドウの左上X座標
		CW_USEDEFAULT,						// ウインドウの左上Y座標
		(rect.right - rect.left),			// ウインドウの幅
		(rect.bottom - rect.top),			// ウインドウの高さ
		NULL,								// 親ウインドウのハンドル
		NULL,								// メニューハンドルまたは子ウインドウID
		hInstance,							// インスタンスハンドル
		NULL								// ウインドウ作成データ
	);

	DWORD dwCurrentTime;					// 現在時刻
	DWORD dwExecLastTime;					// 最後に処理した時刻
	DWORD dwFrameCount;						// フレームカウント
	DWORD dwFPSLastTime;					// 最後にFPSを計測した時刻


	// マネージャの生成
	CManager *pManager = NULL;

	// マネージャの生成
	if (pManager == NULL)
	{// NULLだったら

		// メモリ確保
		pManager = DEBUG_NEW CManager;
	}

	if (pManager != NULL)
	{// メモリの確保が出来ていたら

		// 初期化処理
		if (FAILED(pManager->Init(hInstance, hWnd, TRUE)))
		{// 初期化処理が失敗した場合

			return -1;
		}
	}

	g_nCountFPS = 0;

	// 分解能を設定
	timeBeginPeriod(1);
	dwCurrentTime = 0;				// 初期化する
	dwExecLastTime = timeGetTime();	// 現在時刻を取得(保存)

	// ウインドウの表示
	ShowWindow(hWnd, nCmbShow);		// ウインドウの表示状態を設定
	UpdateWindow(hWnd);				// クライアント領域を更新

	dwFrameCount = 0;
	dwFPSLastTime = timeGetTime();

	// メッセージループ
	while(1)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE) != 0)
		{// Windowの処理
			if (msg.message == WM_QUIT)
			{// WM_QUITメッセージを受け取ったらメッセージループを抜ける
				break;
			}
			else
			{
				// メッセージの設定
				TranslateMessage(&msg);		// 仮想キーメッセージを文字メッセージへ変換
				DispatchMessage(&msg);		// ウインドウプロシージャへメッセージを送出
			}
		}
		else
		{// DirectXの処理

			dwCurrentTime = timeGetTime();	// 現在時刻を取得

			if ((dwCurrentTime - dwFPSLastTime) >= 500)
			{// 0.5秒経過
				g_nCountFPS = (dwFrameCount * 1000) / (dwCurrentTime - dwFPSLastTime);
				dwFPSLastTime = dwCurrentTime;		// FPSを測定した時刻を保存
				dwFrameCount = 0;					// フレームカウントをクリア
			}

			if ((dwCurrentTime - dwExecLastTime) >= (1000 / 60))
			{// 60分の1秒経過

				// 処理開始の時刻[現在時刻]を保存
				dwExecLastTime = dwCurrentTime;

				if (pManager != NULL)
				{// メモリの確保が出来ていたら

					// 更新処理
					pManager->Update();

					// 描画処理
					pManager->Draw();
				}

				// フレームカウントを加算
				dwFrameCount++;
			}
		}
	}

	if (pManager != NULL)
	{// メモリの確保が出来ていたら

		// 終了処理
		pManager->Uninit();

		// メモリの開放
		delete pManager;
		pManager = NULL;
	}
	

	// 分解能を戻す
	timeEndPeriod(1);

	// ウインドウクラスの登録を解除
	UnregisterClass(CLASS_NAME, wcex.hInstance);
		
	return (int)msg.wParam;
}

//==========================================================================
// ウインドウプロシージャ
//==========================================================================
LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	int nID;	// 返り値を格納

	switch (uMsg)
	{
	case WM_DESTROY:				// ウインドウ破棄のメッセージ
		// WM_QUITメッセージを送る
		PostQuitMessage(0);
		break;

	case WM_KEYDOWN:				// キー押下のメッセージ
		switch (wParam)
		{
		case VK_ESCAPE:				// ESCキー入力

			// メッセージボックスを表示
			nID = MessageBox(hWnd, "終了しますか？", "終了メッセージ", MB_YESNO);

			if (nID == IDYES)
			{
				// ウインドウを破棄する(VK_DESTROYメッセージを送る)
				DestroyWindow(hWnd);
			}
			break;
		}
		break;

	case WM_CLOSE:				// バツ閉じ
		// メッセージボックスを表示
		nID = MessageBox(hWnd, "終了しますか？", "終了メッセージ", MB_YESNO);

		if (nID == IDYES)
		{
			// ウインドウを破棄する(VK_DESTROYメッセージを送る)
			DestroyWindow(hWnd);
		}
		else
		{
			return 0;			// 0を返して終了を防ぐ
		}
		break;

	case WM_LBUTTONDOWN:		// マウス左クリック
		// 対象ウインドウにフォーカスを合わせる
		SetFocus(hWnd);
		break;

	case WM_TIMER:				// タイマー発行のメッセージ
		break;
	}

	return DefWindowProc(hWnd, uMsg, wParam, lParam);	// 既定の処理を返す
}

//==========================================================================
// FPS取得
//==========================================================================
int GetFPS(void)
{
	return g_nCountFPS;
}
