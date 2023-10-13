//=============================================================================
// 
//  レンダラーヘッダー [renderer.h]
//  Author : 相馬靜雅
// 
//=============================================================================

#ifndef _RENDERER_H_
#define _RENDERER_H_	// 二重インクルード防止

#include "main.h"

//==========================================================================
// クラス定義
//==========================================================================
// レンダラークラス定義
class CRenderer
{
public:
	CRenderer();
	~CRenderer();

	HRESULT Init(HWND hWnd, BOOL bWindow);
	void Uninit(void);
	void Update(void);
	void Draw(void);
	LPDIRECT3DDEVICE9 GetDevice(void) const;

private:
	LPDIRECT3D9 m_pD3D;					// Direct3Dオブジェクトへのポインタ
	LPDIRECT3DDEVICE9 m_pD3DDevice;		// Direct3Dデバイスへのポインタ
};



#endif