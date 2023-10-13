//=============================================================================
// 
//  群体敵ヘッダー [enemy_crowd.h]
//  Author : 相馬靜雅
// 
//=============================================================================

#ifndef _ENEMYCROWD_H_
#define _ENEMYCROWD_H_	// 二重インクルード防止

#include "main.h"
#include "enemy.h"

// 前方宣言

//==========================================================================
// クラス定義
//==========================================================================
// 群体敵クラス定義
class CEnemyCrowd : public CEnemy
{
public:

	CEnemyCrowd(int nPriority = mylib_const::ENEMY_PRIORITY);
	~CEnemyCrowd();

	// オーバーライドされた関数
	HRESULT Init(void) override;
	void Uninit(void) override;
	void Update(void) override;
	void Draw(void) override;
	void Kill(void) override;

	CEnemyCrowd *GetEnemy(void);
protected:

private:
	virtual void ProcessLanding(void) override;	// 着地時処理
	virtual void AttackAction(int nModelNum, CMotion::AttackInfo ATKInfo) override;	// 攻撃時処理
	virtual void ChaseMove(float fMove);	// 追い掛け移動
	void ChangeToAttackState(void);			// 攻撃状態移行処理
	void StateAttack(void) override;		// 攻撃処理
	void MotionSet(void) override;		// モーションの設定

	// 状態更新系
	void Spawn(void) override;		// スポーン

	typedef enum
	{
		MOTION_DEF = 0,	// ニュートラルモーション
		MOTION_WALK,	// 移動モーション
		MOTION_ATK,		// 攻撃
		MOTION_KNOCKBACK,		// やられモーション
		MOTION_FADEOUT,		// 帰還モーション
		MOTION_MAX
	}MOTION;


	// メンバ変数
	int m_nCntEmission;	// 発生物のカウンター
	int m_nIntEmission;	// 発生物の間隔
};



#endif