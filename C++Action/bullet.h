//=============================================================================
// 
//  弾ヘッダー [bullet.h]
//  Author : 相馬靜雅
// 
//=============================================================================

#ifndef _BULLET_H_
#define _BULLET_H_	// 二重インクルード防止

#include "main.h"
#include "meshsphere.h"

//==========================================================================
// クラス定義
//==========================================================================
// 弾クラス定義
class CBullet : public CMeshSphere
{
public:

	// 列挙型定義
	enum TYPE
	{
		TYPE_PLAYER = 0,	// プレイヤーの弾
		TYPE_ENEMY,		// 敵の弾
		TYPE_MAX
	};

	enum MOVETYPE
	{
		MOVETYPE_NORMAL = 0,	// 通常
		MOVETYPE_CHASE,			// 追従
		MOVETYPE_PARABOLA,		// 放物線
		MOVETYPE_MAX
	};

	enum STATE
	{
		STATE_NONE = 0,	// なにもなし
		STATE_DMG,		// 跳ね返し
		STATE_MAX
	};

	CBullet(int nPriority = 6);
	~CBullet();


	// オーバーライドされた関数
	virtual HRESULT Init(void);
	virtual void Uninit(void);
	virtual void Update(void);
	virtual void Draw(void);
	void SetVtx(void);

	float GetRadius(void);	// 半径取得
	void SetType(TYPE type);	// 種類設定
	TYPE GetType(void);			// 種類取得
	void SetState(STATE state, int nCntState);	// 状態設定
	STATE GetState(void);	// 状態取得

	static int GetNumAll(void);
	static CBullet *Create(TYPE type, MOVETYPE movetype, const D3DXVECTOR3 pos, const D3DXVECTOR3 rot, const D3DXVECTOR3 move, const float fSize);

private:
	void UpdatePos(void);		// 移動
	void UpdateTypePlayer(void);	// プレイヤー弾の更新
	void CollisionPlayer(void);		// プレイヤーとの判定
	void CollisionEnemy(void);		// 敵との判定
	
	void StateNone(void);		// 何もない状態
	void StateDamage(void);		// ダメージ状態処理


	TYPE m_type;							// 弾の種類
	STATE m_state;				// 状態
	MOVETYPE m_MoveType;					// 移動の種類
	int m_nCntState;			// 状態遷移カウンター
	int m_nIdxBulletManager;			// 弾マネージャのインデックス番号
	int m_nLife;							// 寿命
	int m_nLifeMax;					// 寿命の最大値
	float m_fRadius;				// 半径
	static int m_nNumAll;					// 弾の総数
	static const char *m_apTextureFile[TYPE_MAX];	// テクスチャのファイル
	int m_nTexIdx;				// テクスチャのインデックス番号

	//using STATE_FUNC = void(CBullet::*)();
	typedef void(CBullet::*STATE_FUNC)(void);
	typedef void(CBullet::*COLLISION_FUNC)(void);
	static STATE_FUNC m_FuncList[];
	static COLLISION_FUNC m_CollisionFuncList[];	// 当たり判定のリスト
};


#endif