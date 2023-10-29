//=============================================================================
// 
//  ハチ敵ヘッダー [enemy_bee.h]
//  Author : 相馬靜雅
// 
//=============================================================================

#ifndef _ENEMYBEE_H_
#define _ENEMYBEE_H_	// 二重インクルード防止

#include "main.h"
#include "enemy.h"

// 前方宣言

//==========================================================================
// クラス定義
//==========================================================================
// ハチ敵クラス定義
class CEnemyBee : public CEnemy
{
public:

	CEnemyBee(int nPriority = mylib_const::ENEMY_PRIORITY);
	~CEnemyBee();

	// オーバーライドされた関数
	HRESULT Init(void) override;
	void Uninit(void) override;
	void Update(void) override;
	void Draw(void) override;
	void Kill(void) override;

	CEnemyBee *GetEnemy(void);
protected:

private:
	typedef enum
	{
		MOTION_DEF = 0,	// ニュートラルモーション
		MOTION_WALK,	// 移動モーション
		MOTION_ATK,		// 攻撃
		MOTION_KNOCKBACK,		// やられモーション
		MOTION_FADEOUT,		// 帰還モーション
		MOTION_MAX
	}MOTION;

	void ProcessLanding(void) override;	// 着地時処理
	void AttackAction(int nModelNum, CMotion::AttackInfo ATKInfo) override;	// 攻撃時処理
	void ChaseMove(float fMove);	// 追い掛け移動
	void ChangeToAttackState(void);			// 攻撃状態移行処理
	void StateAttack(void) override;		// 攻撃処理
	void MotionSet(void) override;		// モーションの設定

	// 状態更新系
	void Spawn(void) override;		// スポーン
};



#endif