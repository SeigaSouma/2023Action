//=============================================================================
// 
//  チュートリアルテキストヘッダー [tutorialtext.h]
//  Author : 相馬靜雅
// 
//=============================================================================

#ifndef _TUTORIALTEXT_H_
#define _TUTORIALTEXT_H_	// 二重インクルード防止

#include "main.h"
#include "object.h"

//==========================================================================
// 前方宣言
//==========================================================================
class CObject2D;

//==========================================================================
// マクロ定義
//==========================================================================

//==========================================================================
// クラス定義
//==========================================================================
// 背景クラス定義
class CTutorialText : public CObject
{
public:

	CTutorialText(int nPriority = 7);
	~CTutorialText();

	static CTutorialText *Create(void);

	// オーバーライドされた関数
	HRESULT Init(void);
	void Uninit(void);
	void Update(void);
	void Draw(void);
	void SetVtx(void);
private:
	// 列挙型定義
	enum VTX
	{
		VTX_WINDOW = 0,	// 枠
		VTX_TEXT,		// テキスト
		VTX_SKIP,		// スキップ
		VTX_MAX
	};

	enum TEX
	{
		TEX_WINDOW = 0,	// 枠
		TEX_TEXT_WAIT,		// テキスト
		TEX_TEXT_SPEEDUP,		// テキスト
		TEX_TEXT_SPEEDDOWN,		// テキスト
		TEX_TEXT_IMPACTWAVE,		// テキスト
		TEX_TEXT_DIRECTWAVE,	// テキスト
		TEX_TEXT_END,	// テキスト
		TEX_SKIP,
		TEX_MAX
	};

	// メンバ関数
	void SetVtx(int nCntSelect);

	// メンバ変数
	int m_nCntMoving;	// 移動してるカウンター
	int m_nTexIdx[TEX_MAX];							// テクスチャのインデックス番号
	CObject2D *m_pObj2D[VTX_MAX];					// オブジェクト2Dのオブジェクト
	static const char *m_apTextureFile[TEX_MAX];	// テクスチャのファイル
};



#endif