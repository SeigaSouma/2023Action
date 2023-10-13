//=============================================================================
// 
// ステージ切り替え処理 [stageswitch.h]
// Author : 相馬靜雅
// 
//=============================================================================

#ifndef _STAGESWITCH_H_
#define _STAGESWITCH_H_		// 二重インクルード防止のマクロを定義する

#include "main.h"
#include "object2D.h"

//==========================================================================
// 前方宣言
//==========================================================================
class CNumber;

//==========================================================================
// クラス定義
//==========================================================================
// ステージ切り替えクラス
class CStageSwitch : public CObject2D
{
public:

	CStageSwitch(int nPriority = mylib_const::DEF2D_PRIORITY);
	~CStageSwitch();

	//  オーバーライドされた関数
	HRESULT Init(void);
	void Uninit(void);
	void Update(void);
	void Draw(void);
	void SetVtx(void);

	static CStageSwitch *Create(void);	// 生成処理

private:
	// 列挙型定義
	enum VTX
	{
		VTX_TEXT = 0,		// タイトルロゴ
		VTX_MAX
	};

#define NUM_STAGE	(2)	// ステージの数

	void SetValue(int nValue);	// 値の設定処理
	void Release(void);	// 開放処理
	void UpdateGame(void);	// ゲームの更新
	void UpdateTitle(void);	// タイトルの更新

	int m_nLife;	// 寿命
	CNumber *m_apNumber[NUM_STAGE];			// 数字のオブジェクト
	static const char *m_apTextureFile[];	// テクスチャのファイル
	static int m_nTexIdx[VTX_MAX];			// テクスチャのインデックス番号
	static int m_nNumberTexIdx;				// 数字テクスチャのインデックス番号
};

#endif