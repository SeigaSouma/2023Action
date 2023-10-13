//=============================================================================
// 
//  背景ヘッダー [bg.h]
//  Author : 相馬靜雅
// 
//=============================================================================

#ifndef _BG_H_
#define _BG_H_	// 二重インクルード防止

#include "main.h"
#include "object.h"

//==========================================================================
// 前方宣言
//==========================================================================
class CObject2D;

//==========================================================================
// マクロ定義
//==========================================================================
#define NUM_BG	(3)	// 背景の数

//==========================================================================
// クラス定義
//==========================================================================
// 背景クラス定義
class CBG : public CObject
{
private:

	// 構造体定義
	struct SBgInfo
	{
		float fMoveU;				// Uスクロール移動量
		float fMoveV;				// Vスクロール移動量
		float fTexU;				// Uスクロール用
		float fTexV;				// Vスクロール用
	};

	struct SBg
	{
		CObject2D *pObject2D;		// 背景のオブジェクト
		SBgInfo Info;				// 情報
	};

public:

	CBG(int nPriority = 0);
	~CBG();

	static HRESULT Load(void);
	static void Unload(void);
	static CBG *Create(void);

	// オーバーライドされた関数
	HRESULT Init(void);
	void Uninit(void);
	void Update(void);
	void Draw(void);
	void SetVtx(void);
	void SetVtx(int nCntBg);

	void SetMoveTex(const D3DXVECTOR3 move);

private:
	static const char *m_apTextureFile[];			// テクスチャのファイル
	SBg m_aBg[NUM_BG];								// 背景の情報
	static const SBgInfo m_aBgInfo[];				// 背景の情報の情報
	int m_nTexIdx[NUM_BG];				// テクスチャのインデックス番号
};



#endif