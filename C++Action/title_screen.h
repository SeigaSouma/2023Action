//=============================================================================
// 
//  タイトル画面ヘッダー [title_screen.h]
//  Author : 相馬靜雅
// 
//=============================================================================

#ifndef _TITLESCREEN_H_
#define _TITLESCREEN_H_	// 二重インクルード防止

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
class CTitleScreen : public CObject
{
public:

	CTitleScreen(int nPriority = 8);
	~CTitleScreen();

	static CTitleScreen *Create(void);

	// オーバーライドされた関数
	HRESULT Init(void);
	void Uninit(void);
	void Update(void);
	void Draw(void);
	void SetVtx(void);
	void SetVtx(int nCntSelect);

private:
	// 列挙型定義
	enum VTX
	{
		VTX_LOGO = 0,		// タイトルロゴ
		VTX_SELECTSTART,	// スタート
		VTX_LOGO_MAX
	};

	// メンバ関数
	void UpdateLogo(int nCntSelect);	// ロゴの更新処理
	void UpdateSelect(int nCntSelect);	// 選択肢の更新処理

	// メンバ変数
	int m_nCntAlpha;		// 不透明度のカウンター
	int m_nCntLogoMove;		// ロゴ移動のカウント
	int m_nCntLogoRotation;	// ロゴ回転のカウント
	static const char *m_apTextureFile[VTX_LOGO_MAX];	// テクスチャのファイル
	int m_nTexIdx[VTX_LOGO_MAX];						// テクスチャのインデックス番号
	CObject2D *m_pObj2D[VTX_LOGO_MAX];					// オブジェクト2Dのオブジェクト
};



#endif