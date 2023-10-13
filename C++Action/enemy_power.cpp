//=============================================================================
// 
//  プレイヤー処理 [enemy.cpp]
//  Author : 相馬靜雅
// 
//=============================================================================
#include "enemy_power.h"
#include "manager.h"
#include "renderer.h"
#include "game.h"
#include "texture.h"
#include "input.h"
#include "calculation.h"
#include "explosion.h"
#include "particle.h"
#include "motion.h"
#include "model.h"
#include "elevation.h"
#include "player.h"
#include "enemymanager.h"
#include "camera.h"
#include "ballast.h"
#include "3D_Effect.h"
#include "hp_gauge.h"
#include "shadow.h"
#include "sound.h"

//==========================================================================
// マクロ定義
//==========================================================================
#define WIDTH			(60.0f)							// 横幅
#define HEIGHT			(60.0f)							// 縦幅
#define MAX_LIFE		(5)								// 体力
#define PLAYER_SERCH	(800.0f)	// プレイヤー探索範囲
#define CHACE_DISTABCE	(50.0f)		// 追い掛ける時の間隔
#define JUMP			(18.0f)					// ジャンプ力初期値

//==========================================================================
// 静的メンバ変数宣言
//==========================================================================

//==========================================================================
// コンストラクタ
//==========================================================================
CEnemyPower::CEnemyPower(int nPriority) : CEnemy(nPriority)
{
	// 値のクリア
	m_attackType = ATTACKTYPE_IMPACTWAVE;	// 攻撃の種類

}

//==========================================================================
// デストラクタ
//==========================================================================
CEnemyPower::~CEnemyPower()
{

}


//==========================================================================
// 初期化処理
//==========================================================================
HRESULT CEnemyPower::Init(void)
{
	// 初期化処理
	CEnemy::Init();

	// 体力取得
	int nLife = GetLife();

	// 体力ゲージ
	m_pHPGauge = CHP_Gauge::Create(350.0f, nLife, 3.0f);

	if (m_pHPGauge == NULL)
	{// NULLだったら
		m_pHPGauge = NULL;
	}

	// 攻撃の種類
	m_attackType = ATTACKTYPE_IMPACTWAVE;

	return S_OK;
}

//==========================================================================
// 終了処理
//==========================================================================
void CEnemyPower::Uninit(void)
{
	// 終了処理
	CEnemy::Uninit();
}

//==========================================================================
// 死亡処理
//==========================================================================
void CEnemyPower::Kill(void)
{
	// 死亡処理
	CEnemy::Kill();
}

//==========================================================================
// 更新処理
//==========================================================================
void CEnemyPower::Update(void)
{
	// 死亡の判定
	if (IsDeath() == true)
	{// 死亡フラグが立っていたら
		return;
	}

	// 更新処理
	CEnemy::Update();

	if (IsDeath() == true)
	{// 死亡フラグが立っていたら
		return;
	}


	// 現在の種類取得
	int nType = m_pMotion->GetType();

	if (nType == MOTION_DIRECTATK)
	{
		// 位置取得
		D3DXVECTOR3 pos = GetPosition();

		// 向き取得
		D3DXVECTOR3 rot = GetRotation();

		// 目標の向き取得
		float fRotDest = GetRotDest();

		// プレイヤー情報
		CPlayer *pPlayer = CManager::GetScene()->GetPlayer();

		if (pPlayer == NULL)
		{// NULLだったら
			return;
		}

		// 親の位置取得
		D3DXVECTOR3 posPlayer = pPlayer->GetPosition();

		// 目標の角度を求める
		fRotDest = atan2f((pos.x - posPlayer.x), (pos.z - posPlayer.z));

		// 目標との差分
		float fRotDiff = fRotDest - rot.y;

		//角度の正規化
		RotNormalize(fRotDiff);

		//角度の補正をする
		rot.y += fRotDiff * 0.025f;

		// 角度の正規化
		RotNormalize(rot.y);

		// 向き設定
		SetRotation(rot);
	}
}

//==========================================================================
// 着地時の処理
//==========================================================================
void CEnemyPower::ProcessLanding(void)
{
	// 着地時の処理
	CEnemy::ProcessLanding();
}

//==========================================================================
// 攻撃状態移行処理
//==========================================================================
void CEnemyPower::ChangeToAttackState(void)
{
	// 位置取得
	D3DXVECTOR3 pos = GetPosition();

	// プレイヤー情報
	CPlayer *pPlayer = CManager::GetScene()->GetPlayer();

	if (pPlayer == NULL)
	{
		return;
	}

	// 親の位置取得
	D3DXVECTOR3 posPlayer = pPlayer->GetPosition();

	// 右前
	D3DXVECTOR3 posLeft;
	D3DXVECTOR3 posRight;

	float fSearchCircle = 400.0f;

	if (m_sMotionFrag.bATK == false &&
		CircleRange(pos, posPlayer, fSearchCircle, pPlayer->GetRadius()) == true &&
		m_sMotionFrag.bJump == false)
	{// 一定距離間にプレイヤーが入ったら

		// 攻撃状態にする
		m_state = STATE_ATTACK;
		m_sMotionFrag.bATK = true;
		m_attackType = ATTACKTYPE_IMPACTWAVE;
		return;
	}
}

//==========================================================================
// 出現
//==========================================================================
void CEnemyPower::Spawn(void)
{
	// 位置取得
	D3DXVECTOR3 pos = GetPosition();

	// 移動量取得
	D3DXVECTOR3 move = GetMove();

	// 状態遷移カウンター減算
	m_nCntState++;

	// 位置更新
	pos = (D3DXVECTOR3(0.0f, SPAWN_GRAVITY, 0.0f) * (float)(m_nCntState * m_nCntState) + move * (float)m_nCntState) + m_posOrigin;

	// 起伏との判定
	if (CGame::GetElevation()->IsHit(pos))
	{// 地面と当たっていたら
		m_state = m_Oldstate;	// 過去の情報保存
		m_nCntState = 0;	// 遷移カウンター設定
		move.y = 0.0f;			// 移動量ゼロ
	}

	// 位置設定
	SetPosition(pos);

	// 移動量設定
	SetMove(move);
}

//==========================================================================
// 追い掛け移動
//==========================================================================
void CEnemyPower::ChaseMove(float fMove)
{
	// 向き取得
	D3DXVECTOR3 rot = GetRotation();

	// 移動量取得
	D3DXVECTOR3 move = GetMove();

	// 移動量加算
	move.x += sinf(D3DX_PI + rot.y) * fMove;
	move.z += cosf(D3DX_PI + rot.y) * fMove;

	// 移動量設定
	SetMove(move);
}

//==========================================================================
// モーションの設定
//==========================================================================
void CEnemyPower::MotionSet(void)
{
	if (m_pMotion->IsFinish() == true)
	{// 終了していたら

		// 現在の種類取得
		int nType = m_pMotion->GetType();

		if (m_sMotionFrag.bMove == true && m_sMotionFrag.bKnockback == false)
		{// 移動していたら

			m_sMotionFrag.bMove = false;	// 移動判定OFF

			// 移動モーション
			m_pMotion->Set(MOTION_WALK);
		}
		else if (m_sMotionFrag.bKnockback == true)
		{// やられ中だったら

			// やられモーション
			m_pMotion->Set(MOTION_KNOCKBACK);
		}
		else if (m_sMotionFrag.bATK == true)
		{// 攻撃していたら

			m_sMotionFrag.bATK = false;		// 攻撃判定OFF

			// 攻撃モーション
			switch (m_attackType)
			{
			case CEnemyPower::ATTACKTYPE_IMPACTWAVE:
				m_pMotion->Set(MOTION_ATK);
				break;

			case CEnemyPower::ATTACKTYPE_DIRECTWAVE:
				m_pMotion->Set(MOTION_DIRECTATK);
				break;

			case CEnemyPower::ATTACKTYPE_MAX:
				break;

			default:
				break;
			}
		}
		else
		{
			// ニュートラルモーション
			m_pMotion->Set(MOTION_DEF);
		}
	}
}

//==========================================================================
// 攻撃時処理
//==========================================================================
void CEnemyPower::AttackAction(int nModelNum, CMotion::AttackInfo ATKInfo)
{
	D3DXMATRIX mtxTrans;	// 計算用マトリックス宣言

	// 攻撃時処理
	CEnemy::AttackAction(nModelNum, ATKInfo);

	if (GetObjectChara()->GetModel()[nModelNum] == NULL)
	{// NULLだったら
		return;
	}

	// 判定するパーツのマトリックス取得
	D3DXMATRIX mtxWepon = GetObjectChara()->GetModel()[nModelNum]->GetWorldMtx();

	// 位置を反映する
	D3DXMatrixTranslation(&mtxTrans, ATKInfo.Offset.x, ATKInfo.Offset.y, ATKInfo.Offset.z);
	D3DXMatrixMultiply(&mtxWepon, &mtxTrans, &mtxWepon);

	// 武器の位置
	D3DXVECTOR3 weponpos = D3DXVECTOR3(mtxWepon._41, mtxWepon._42, mtxWepon._43);

	// 位置取得
	D3DXVECTOR3 pos = GetPosition();

	// 向き取得
	D3DXVECTOR3 rot = GetRotation();

	// 種類別
	switch (m_pMotion->GetType())
	{
	case MOTION_WALK:
		break;

	case MOTION_ATK:

		// パーティクル生成
		my_particle::Create(weponpos, my_particle::TYPE_SUPERATTACK);

		// 瓦礫生成
		CBallast::Create(weponpos, DEFAULTMOVE_BALLAST, 5);

		// 振動
		CManager::GetCamera()->SetShake(20, 5.0f, 0.0f);
		break;
	}
}

//==========================================================================
// 描画処理
//==========================================================================
void CEnemyPower::Draw(void)
{
	// 描画処理
	CEnemy::Draw();
}

//==========================================================================
// 敵の情報取得
//==========================================================================
CEnemyPower *CEnemyPower::GetEnemy(void)
{
	// 自分自身のポインタを取得
	return this;
}