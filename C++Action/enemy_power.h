//=============================================================================
// 
//  力の敵ヘッダー [enemy_power.h]
//  Author : 相馬靜雅
// 
//=============================================================================

#ifndef _ENEMYPOWER_H_
#define _ENEMYPOWER_H_	// 二重インクルード防止

#include "main.h"
#include "enemy.h"

// 前方宣言

//==========================================================================
// クラス定義
//==========================================================================
// プレイヤークラス定義
class CEnemyPower : public CEnemy
{
public:

	CEnemyPower(int nPriority = mylib_const::ENEMY_PRIORITY);
	~CEnemyPower();

	// オーバーライドされた関数
	HRESULT Init(void) override;
	void Uninit(void) override;
	void Update(void) override;
	void Draw(void) override;
	void Kill(void) override;

	CEnemyPower *GetEnemy(void);
protected:

private:
	virtual void ProcessLanding(void) override;	// 着地時処理
	virtual void AttackAction(int nModelNum, CMotion::AttackInfo ATKInfo) override;	// 攻撃時処理
	virtual void ChaseMove(float fMove);	// 追い掛け移動
	void ChangeToAttackState(void);			// 攻撃状態移行処理
	void MotionSet(void) override;		// モーションの設定
	
	// 状態更新系
	void Spawn(void) override;		// スポーン

	enum MOTION
	{
		MOTION_DEF = 0,	// ニュートラルモーション
		MOTION_WALK,	// 移動モーション
		MOTION_ATK,		// 攻撃
		MOTION_KNOCKBACK,		// やられモーション
		MOTION_DIRECTATK,		// 攻撃
		MOTION_MAX
	};

	// 攻撃の種類
	enum ATTACKTYPE
	{
		ATTACKTYPE_IMPACTWAVE = 0,	// 円形波
		ATTACKTYPE_DIRECTWAVE,		// 直線波
		ATTACKTYPE_MAX
	};

	// メンバ変数
	ATTACKTYPE m_attackType;	// 攻撃の種類

};



#endif