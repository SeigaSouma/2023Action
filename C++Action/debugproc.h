//=============================================================================
//
// デバッグ表示処理 [debugproc.h]
// Author Ibuki Okusada
//
//=============================================================================

#ifndef _DEBUGPROC_H_
#define _DEBUGPROC_H_	// 二重インクルード防止

#include "main.h"

//==========================================================================
// マクロ定義
//==========================================================================
#define MAX_DEBUGSTRING	(2048)	// 最大文字数

//==========================================================================
// クラス定義
//==========================================================================
class CDebugProc
{
public:

	CDebugProc();
	~CDebugProc();

	HRESULT Init(HINSTANCE hInstance, HWND hWnd);
	void Uninit(void);
	void Update(void);
	void Draw(void);
	void Print(const char *fmt, ...);
	static void SetText(void);		// 設定処理

private:

	char m_aStr[MAX_DEBUGSTRING];	// デバッグ表示用
	bool m_bDisp;					// デバッグ表示するか
	static LPD3DXFONT m_pFont;		// フォントへのポインタ
};

#endif