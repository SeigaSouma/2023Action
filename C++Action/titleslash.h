//=============================================================================
// 
// タイトル斬撃処理 [titleslash.h]
// Author : 相馬靜雅
// 
//=============================================================================

#ifndef _TITLESLASH_H_
#define _TITLESLASH_H_		// 二重インクルード防止のマクロを定義する

#include "main.h"
#include "object2D.h"

//==========================================================================
// クラス定義
//==========================================================================
// タイトル斬撃クラス定義
class CTitleSlash : public CObject2D
{
public:

	CTitleSlash(int nPriority = 8);
	~CTitleSlash();

	static CTitleSlash *Create(const D3DXVECTOR3 pos);
	
	//  オーバーライドされた関数
	HRESULT Init(void);
	void Uninit(void);
	void Update(void);
	void Draw(void);
	void SetVtx(void);

private:

	float m_fMaxWidth;		// 最大の横幅
	int m_nLife;			// 寿命
	int m_nMaxLife;			// 最大寿命(固定)
	int m_nTexIdx;			// テクスチャのインデックス番号
};

#endif