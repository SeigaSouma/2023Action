//=============================================================================
// 
//  敵の一定の動きマネージャ [enemyfixedmove_manager.h]
//  Author : 相馬靜雅
// 
//=============================================================================

#ifndef _ENEMYFIXEDMOVE_MANAGER_H_
#define _ENEMYFIXEDMOVE_MANAGER_H_	// 二重インクルード防止

#include "main.h"
#include "constans.h"
#include "object.h"

// 前方宣言
class CEnemy;

//==========================================================================
// クラス定義
//==========================================================================
// 敵の一定の動きマネージャクラス定義
class CEnemyFixedMoveManager
{
protected:

	// 構造体定義
	struct sFixedInfo
	{
		int nFrame;			// フレーム数
		D3DXVECTOR3 move;	// 移動量
	};

	// 一定行動
	struct sFixedMove
	{
		sFixedInfo info[mylib_const::MAX_FIXEDMOVE_INFO];	// 行動情報
		int nNumKey;		// キーの数
	};

public:

	CEnemyFixedMoveManager();
	~CEnemyFixedMoveManager();

	HRESULT Init(void);
	void Uninit(void);
	void Update(void);

	void Set(int nType);	// 設定処理
	void StartSet(int nKey, int nFrame);	// スタート時の設定
	D3DXVECTOR3 UpdatePosition(CEnemy *pEnemy);	// 位置更新
	static CEnemyFixedMoveManager *Create(void);
	static HRESULT ReadText(const std::string pTextFile);	// 外部ファイル読み込み処理

private:

	// メンバ関数
	void UpdateKeyFrame(void);	// キーフレーム更新

	// メンバ変数
	static sFixedMove m_aFxedMove[mylib_const::MAX_FIXEDMOVE];	// 一定の動きの情報
	int m_nType;				// 現在のモーションの種類
	int m_nPatternKey;			// 何個目のキーか
	int m_nCntFrame;			// フレームのカウント
	int m_nCntAllFrame;			// 全てのカウント
	int m_nMaxAllFrame;			// 全てのカウントの最大値
	static int m_nNumAll;				// モーションの総数
};



#endif