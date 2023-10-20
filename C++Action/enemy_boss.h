//=============================================================================
// 
//  ボスヘッダー [enemy_boss.h]
//  Author : 相馬靜雅
// 
//=============================================================================

#ifndef _ENEMYBOSS_H_
#define _ENEMYBOSS_H_	// 二重インクルード防止

#include "main.h"
#include "enemy.h"

// 前方宣言

//==========================================================================
// クラス定義
//==========================================================================
// ボスクラス定義
class CEnemyBoss : public CEnemy
{
public:

	CEnemyBoss(int nPriority = mylib_const::ENEMY_PRIORITY);
	~CEnemyBoss();

	// オーバーライドされた関数
	HRESULT Init(void) override;
	void Uninit(void) override;
	void Update(void) override;
	void Draw(void) override;
	void Kill(void) override;

	CEnemyBoss *GetEnemy(void);
protected:

private:
	typedef enum
	{
		MOTION_DEF = 0,		// ニュートラルモーション
		MOTION_WALK,		// 移動モーション
		MOTION_BULLETATK,			// 攻撃
		MOTION_KNOCKBACK,	// やられモーション
		MOTION_FADEOUT,		// 帰還モーション
		MOTION_MAX
	}MOTION;

	enum ATKTYPE
	{
		ATKTYPE_BULLET = 0,	// 弾発射攻撃
		ATKTYPE_MAX
	};

	// 行動の構造体定義
	struct BOSSACT
	{
		int nCntWait;		// 待機カウンター
		ATKTYPE AtkType;	// 攻撃の種類
	};

	// オーバーライドされた関数
	void UpdateByType(void) override;	// 種類別更新処理
	void ProcessLanding(void) override;	// 着地時処理
	void AttackAction(int nModelNum, CMotion::AttackInfo ATKInfo) override;	// 攻撃時処理
	void StateAttack(void) override;	// 攻撃処理
	void MotionSet(void) override;		// モーションの設定
	void StateWait(void) override;		// 待機処理

	// 状態更新系
	void Spawn(void) override;		// スポーン

	// メンバ関数
	void ChaseMove(float fMove);		// 追い掛け移動
	void ChangeToAttackState(void);		// 攻撃状態移行処理
	void UpdateByAttack(void);			// 攻撃別処理
	void UpdateAttackBullet(void);		// 弾攻撃

	// メンバ変数
	BOSSACT m_sAct;	// ボスの行動
};



#endif