//=============================================================================
// 
//  敵のマネージャヘッダー [enemymanager.h]
//  Author : 相馬靜雅
// 
//=============================================================================

#ifndef _ENEMYMANAGER_H_
#define _ENEMYMANAGER_H_	// 二重インクルード防止

#include "main.h"
#include "constans.h"

// 前方宣言
class CEnemy;

//==========================================================================
// クラス定義
//==========================================================================
// 敵のマネージャクラス定義
class CEnemyManager
{
public:

	// 構造体定義
	struct EnemyData
	{
		int nType;	// キャラクター種類
		int nStartFrame;	// 初期フレーム
		int nStartKey;		// 初期キー
		float fStartMoveValue;	// 初期マップ移動量
	};

	struct Pattern
	{
		int nNumEnemy;	// 敵の数
		int nFixedType;	// 一定の動きの種類
		EnemyData EnemyData[mylib_const::MAX_PATTEN_ENEMY];
	};

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

	// 列挙型定義
	enum STATE
	{
		STATE_NONE = 0,		// 何もない状態
		STATE_COMBOANIM,	// コンボ時のアニメーション
		STATE_MAX
	};

	CEnemyManager();
	~CEnemyManager();

	HRESULT Init(void);
	void Uninit(void);
	virtual void Update(void);

	static CEnemyManager *Create(const std::string pTextFile);
	HRESULT ReadText(const std::string pTextFile);	// 外部ファイル読み込み処理
	CEnemy **SetEnemy(D3DXVECTOR3 pos, D3DXVECTOR3 rot, int nPattern);	// 敵配置
	CEnemy **SetEnemy(D3DXVECTOR3 pos, int nMapIndex, float fMapMoveValue, int nPattern);	// 敵配置
	static int GetPatternNum(void);
	static void Release(int nIdx);		// 破棄
	STATE GetState(void) { return m_state; }	// 状態取得
	int GetNumAll(void);		// 敵の総数取得
	Pattern GetPattern(int nPattern);	// パターン取得
	CEnemy **GetEnemy(void);	// 敵取得
	static const char *GetMotionFilename(int nType);
protected:


private:

	static int m_nNumAll;		// 敵の総数
	static CEnemy *m_pEnemy[mylib_const::MAX_OBJ];						// 敵へのポインタ
	static Pattern m_aPattern[mylib_const::MAX_PATTEN_ENEMY];			// 配置の種類
	static std::string sMotionFileName[mylib_const::MAX_PATTEN_ENEMY];	// モーションファイル名
	static bool m_bHitStop;		// ヒットストップの判定
	static STATE m_state;		// 状態
	static int m_nCntSpawn;		// 出現カウント
	static int m_nPatternNum;	// 出現パターン数
	static int m_nNumChara;		// 敵の種類の総数
	static bool m_bLoadPattern;	// パターン読み込み判定
};



#endif