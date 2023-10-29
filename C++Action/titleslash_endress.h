//=============================================================================
// 
//  タイトルの斬撃ヘッダー [titleslash_endress.h]
//  Author : 相馬靜雅
// 
//=============================================================================

#ifndef _TITLESLASHENDRESS_H_
#define _TITLESLASHENDRESS_H_	// 二重インクルード防止

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
// タイトルの斬撃クラス定義
class CTitleSlashEndress : public CObject
{
public:

	CTitleSlashEndress(int nPriority = 8);
	~CTitleSlashEndress();

	static CTitleSlashEndress *Create(void);

	// オーバーライドされた関数
	HRESULT Init(void);
	void Uninit(void);
	void Update(void);
	void Draw(void);

private:
	// 列挙型定義
	enum VTX
	{
		VTX_LOGOALPHA1 = 0,		// 透明なロゴ
		VTX_LOGOALPHA2,			// 透明なロゴ
		VTX_LOGOPERFECTALPHA,	// 完成版のロゴ
		VTX_LOGO_MAX
	};

	// メンバ関数
	void UpdateLogo(int nCntSelect);	// ロゴの更新処理
	void UpdateSlash();
	void UpdateSlashAlpha(int nCntSelect);

	// メンバ変数
	int m_nLife;					// 寿命
	int m_nTexIdx[VTX_LOGO_MAX];						// テクスチャのインデックス番号
	D3DXVECTOR2 m_DestSize;		// 目標のサイズ
	CObject2D *m_pObj2D[VTX_LOGO_MAX];					// オブジェクト2Dのオブジェクト
	static const char *m_apTextureFile[VTX_LOGO_MAX];	// テクスチャのファイル
};



#endif