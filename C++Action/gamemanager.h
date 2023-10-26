//=============================================================================
// 
//  ゲームマネージャヘッダー [gamemanager.h]
//  Author : 相馬靜雅
// 
//=============================================================================

#ifndef _GAMEMANAGER_H_
#define _GAMEMANAGER_H_	// 二重インクルード防止

#include "main.h"
#include "constans.h"

//==========================================================================
// 前方宣言
//==========================================================================

//==========================================================================
// クラス定義
//==========================================================================
// ゲームマネージャクラス定義
class CGameManager
{
public:

	// シーンの種類
	enum SceneType
	{
		SCENE_MAIN = 0,			// メイン
		SCENE_RUSH,				// ラッシュ
		SCENE_BOSS,				// ボス
		SCENE_TRANSITIONWAIT,	// 遷移待機
		SCENE_TRANSITION,		// 遷移
		SCENE_MAX
	};

	CGameManager();
	~CGameManager();

	HRESULT Init(void);
	void Uninit(void);
	void Update(void);

	void SetType(SceneType type);	// シーンの種類設定
	SceneType GetType(void);		// シーンの種類取得
	void SetEnableRush(void) { m_bEndRush = true; }		// ラッシュの終了フラグを有効にする
	bool IsRushEnd(void) { return m_bEndRush; }			// 終了のフラグ取得
	static CGameManager *Create(void);

private:

	// メンバ関数

	// メンバ変数
	SceneType m_SceneType;	// シーンの種類
	bool m_bEndRush;		// ラッシュが終了したか
};



#endif