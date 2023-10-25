//=============================================================================
// 
//  プレイヤーヘッダー [player.h]
//  Author : 相馬靜雅
// 
//=============================================================================

#ifndef _PLAYER_H_
#define _PLAYER_H_	// 二重インクルード防止

#include "main.h"
#include "objectChara.h"

//==========================================================================
// マクロ定義
//==========================================================================
#define SLIDEMOVE		(2.05f)			// 坂の移動量

// 前方宣言
class CMotion;
class CShadow;
class CTargetPoint;
class CHP_Gauge;

//==========================================================================
// クラス定義
//==========================================================================
// プレイヤークラス定義
class CPlayer : public CObjectChara
{
public:

	// 列挙型定義
#if ACTION_MODE

#else

	// 状態定義
	typedef enum
	{
		STATE_NONE = 0,		// なにもない
		STATE_INVINCIBLE,	// 無敵
		STATE_DMG,			// ダメージ
		STATE_KNOCKBACK,	// ノックバック
		STATE_DEAD,			// 死
		STATE_ATTACK,		// 攻撃処理
		STATE_MAX
	}STATE;
#endif

	CPlayer(int nPriority = mylib_const::DEF2D_PRIORITY);
	~CPlayer();

	static CPlayer *Create();

	// オーバーライドされた関数
	virtual HRESULT Init(void);
	void Uninit(void);
	virtual void Update(void);
	void Draw(void);
	bool Hit(const int nValue);	// ヒット処理
	int GetState(void) override;

	virtual void UninitByMode(void);
	void Kill(void);	// 死亡処理

protected:
	// 列挙型定義
	enum MOTION
	{
		MOTION_DEF = 0,			// ニュートラルモーション
		MOTION_WALK,			// 移動モーション
		MOTION_ATK,				// 攻撃
		MOTION_ATK2,				// 攻撃(派生)
		MOTION_KNOCKBACK,		// やられモーション
		MOTION_MAX
	};

	enum ATKRUSH
	{
		ATKRUSH_RIGHT = 0,
		ATKRUSH_LEFT,
		ATKRUSH_MAX
	};

	bool Collision(D3DXVECTOR3 &pos, D3DXVECTOR3 &move);	// 当たり判定
	void CollisionChaseChanger(void);	// 追従の変更者との判定

	bool m_bJump;				// ジャンプ中かどうか
	bool m_bKnockback;			// ノックバック中かどうか
	bool m_bMove;				// 移動中かどうか
	bool m_bATK;				// 攻撃中かどうか
	bool m_bLandOld;			// 過去の着地情報
	bool m_bHitStage;			// ステージの当たり判定
	bool m_bLandField;			// フィールドの着地判定
	bool m_bHitWall;			// 壁の当たり判定
	int m_nCntWalk;				// 歩行カウンター
	STATE m_state;			// 状態
	CMotion *m_pMotion;		// モーションの情報
private:

	// メンバ関数
	void UpdateState(void);	// 状態更新処理
	void KnockBack(void);	// ノックバック
	void Damage(void);		// ダメージ
	void Invincible(void);	// 無敵
	virtual void Controll(void);	// 操作
	void MotionSet(void);	// モーションの設定
	void Atack(void);		// 攻撃

	STATE m_Oldstate;			// 前回の状態
	ATKRUSH m_atkRush;			// 連続アタックの種類
	D3DXCOLOR m_mMatcol;		// マテリアルの色
	D3DXVECTOR3 m_posKnokBack;	// ノックバックの位置
	D3DXVECTOR3 m_KnokBackMove;	// ノックバックの移動量
	int m_nAngle;				// 向き
	int m_nCntState;			// 状態遷移カウンター
	int m_nCntConfusion;		// 混乱カウンター
	int m_nTexIdx;				// テクスチャのインデックス番号
	int m_nIdxXFile;			// Xファイルのインデックス番号
	float m_fAtkStickRot;		// 攻撃時のスティック向き
	int m_nAtkAngle;			// 攻撃時のY軸の向き
	float m_fBodyRot;			// 攻撃時の身体向き
	float m_fTruePosY;			// 本当の補正後の位置
	bool m_bStick;				// スティック倒した判定
	ANGLE m_StickAngle;			// スティックの向き
	ANGLE m_OldStickAngle;		// 前回のスティックの向き
	CShadow *m_pShadow;			// 影の情報
	CTargetPoint *m_pTargetP;	// 目標の地点
	CHP_Gauge *m_pHPGauge;		// HPゲージの情報
};



#endif