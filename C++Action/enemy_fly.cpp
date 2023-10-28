//=============================================================================
// 
//  飛行敵処理 [enemy_fly.cpp]
//  Author : 相馬靜雅
// 
//=============================================================================
#include "enemy_fly.h"
#include "manager.h"
#include "renderer.h"
#include "game.h"
#include "texture.h"
#include "input.h"
#include "calculation.h"
#include "bullet.h"
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
#include "mapmanager.h"

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
CEnemyFly::CEnemyFly(int nPriority) : CEnemy(nPriority)
{
	// 値のクリア
}

//==========================================================================
// デストラクタ
//==========================================================================
CEnemyFly::~CEnemyFly()
{

}


//==========================================================================
// 初期化処理
//==========================================================================
HRESULT CEnemyFly::Init(void)
{
	// 初期化処理
	CEnemy::Init();

	m_state = STATE_NONE;	// 親追い掛け状態
	m_Oldstate = STATE_NONE;

	switch (m_colorType)
	{
	case COLORTYPE_NORMAL:
		break;

	case COLORTYPE_POISON:
		break;

	case COLORTYPE_LUCKY:
		break;

	case COLORTYPE_CONFUSION:
		break;

	case COLORTYPE_SWIFTFOOT:
		break;

	case COLORTYPE_TUTORIAL:
		m_Oldstate = STATE_WAIT;
		break;

	default:
		break;
	}
	return S_OK;
}

//==========================================================================
// 終了処理
//==========================================================================
void CEnemyFly::Uninit(void)
{
	// 終了処理
	CEnemy::Uninit();
}

//==========================================================================
// 死亡処理
//==========================================================================
void CEnemyFly::Kill(void)
{
	// 死亡処理
	CEnemy::Kill();
}

//==========================================================================
// 更新処理
//==========================================================================
void CEnemyFly::Update(void)
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

	// 位置取得
	D3DXVECTOR3 pos = GetPosition();
	D3DXVECTOR3 rot = GetRotation();

	// 目標の向き取得
	float fRotDest = GetRotDest();

	// プレイヤー情報
	CPlayer *pPlayer = CManager::GetInstance()->GetScene()->GetPlayer();
	if (pPlayer == NULL)
	{
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

	// 目標の向き設定
	SetRotDest(fRotDest);

}

//==========================================================================
// 着地時の処理
//==========================================================================
void CEnemyFly::ProcessLanding(void)
{
	// 着地時の処理
	CEnemy::ProcessLanding();
}

//==========================================================================
// 攻撃状態移行処理
//==========================================================================
void CEnemyFly::ChangeToAttackState(void)
{
	// 位置取得
	D3DXVECTOR3 pos = GetPosition();

	// プレイヤー情報
	CPlayer *pPlayer = CManager::GetInstance()->GetScene()->GetPlayer();

	if (pPlayer == NULL)
	{
		return;
	}

	// 親の位置取得
	D3DXVECTOR3 posPlayer = pPlayer->GetPosition();

	float fRadius = 700.0f;

	if (m_pMotion->GetType() != MOTION_ATK && CircleRange(pos, posPlayer, fRadius, pPlayer->GetRadius()) == true)
	{// 一定距離間にプレイヤーが入ったら

		// 攻撃状態にする
		m_sMotionFrag.bATK = true;
	}
}

//==========================================================================
// 出現
//==========================================================================
void CEnemyFly::Spawn(void)
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
	if (CManager::GetInstance()->GetScene()->GetElevation()->IsHit(pos))
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
// 攻撃処理
//==========================================================================
void CEnemyFly::StateAttack(void)
{
	// 攻撃処理
	CEnemy::StateAttack();

	// モーションの情報取得
	CMotion::Info aInfo = m_pMotion->GetInfo(m_pMotion->GetType());

	// 攻撃情報の総数取得
	int nNumAttackInfo = aInfo.nNumAttackInfo;

	bool bAtkWait = true;	// 攻撃待機中
	for (int nCntAttack = 0; nCntAttack < nNumAttackInfo; nCntAttack++)
	{
		if (aInfo.AttackInfo[nCntAttack] == NULL)
		{// NULLだったら
			continue;
		}

		// モーションカウンター取得
		if (m_pMotion->GetAllCount() > aInfo.AttackInfo[nCntAttack]->nMinCnt)
		{// 攻撃判定中

			// 攻撃判定中にする
			bAtkWait = false;
		}
	}

	if (bAtkWait == false)
	{// 判定中の時
		return;
	}


	//// プレイヤー情報
	//CPlayer *pPlayer = CManager::GetInstance()->GetScene()->GetPlayer();
	//if (pPlayer == NULL)
	//{// NULLだったら
	//	return;
	//}

	//// 親の位置取得
	//D3DXVECTOR3 posPlayer = pPlayer->GetPosition();

	//// 位置取得
	//D3DXVECTOR3 pos = GetPosition();

	//// 向き取得
	//D3DXVECTOR3 rot = GetRotation();

	//// 目標の向き取得
	//float fRotDest = GetRotDest();

	//// 目標の角度を求める
	//fRotDest = atan2f((pos.x - posPlayer.x), (pos.z - posPlayer.z));

	//// 目標との差分
	//float fRotDiff = fRotDest - rot.y;

	////角度の正規化
	//RotNormalize(fRotDiff);

	////角度の補正をする
	//rot.y += fRotDiff * 0.025f;

	//// 角度の正規化
	//RotNormalize(rot.y);

	//// 向き設定
	//SetRotation(rot);
}

//==========================================================================
// 追い掛け移動
//==========================================================================
void CEnemyFly::ChaseMove(float fMove)
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
void CEnemyFly::MotionSet(void)
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
			m_pMotion->Set(MOTION_ATK);
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
void CEnemyFly::AttackAction(int nModelNum, CMotion::AttackInfo ATKInfo)
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

	CPlayer *pPlayer = CManager::GetInstance()->GetScene()->GetPlayer();

	if (pPlayer == NULL)
	{// NULLだったら
		return;
	}

	// プレイヤーの位置
	D3DXVECTOR3 posPlayer = pPlayer->GetPosition();

	// プレイヤーと敵のベクトル
	D3DXVECTOR3 vec = weponpos - posPlayer;

	// 目標の角度を求める
	float fRotDest = atan2f(vec.x, vec.z);
	float fRotHeight = atan2f((weponpos.y - posPlayer.y - 100.0f), D3DXVec3Length(&vec));

	// 位置取得
	D3DXVECTOR3 pos = GetPosition();

	// 位置取得
	D3DXVECTOR3 rot = GetRotation();

	// マップマネージャの取得
	CMapManager *pMapManager = CGame::GetMapManager();
	if (pMapManager == NULL)
	{// NULLだったら
		return;
	}
	ANGLE setAngle = pMapManager->GetTargetAngle(GetMapIndex(), pPlayer->GetMapIndex(), GetMapMoveValue(), pPlayer->GetMapMoveValue());

	// 弾の生成
	CObject *pBullet = CBullet::Create(
		CBullet::TYPE_ENEMY,
		CBullet::MOVETYPE_NORMAL,
		D3DXVECTOR3(pos.x, pos.y, pos.z),
		rot,
		D3DXVECTOR3(fabsf(sinf(D3DX_PI * 0.5f + fRotHeight) * 5.0f), cosf(D3DX_PI * 0.5f + fRotHeight) * 5.0f, 0.0f),
		40.0f);
	//SlashMove = D3DXVECTOR3(cosf(fRotX) * fFabsMove, sinf(D3DX_PI + fRotY) * fFabsMove, 0.0f);

	pBullet->SetMapIndex(GetMapIndex());
	pBullet->SetMapMoveValue(GetMapMoveValue());
	pBullet->SetMapPointRatio(GetMapPointRatio());
	pBullet->SetMoveAngle(setAngle);
}

//==========================================================================
// 描画処理
//==========================================================================
void CEnemyFly::Draw(void)
{
	// 描画処理
	CEnemy::Draw();
}

//==========================================================================
// 敵の情報取得
//==========================================================================
CEnemyFly *CEnemyFly::GetEnemy(void)
{
	// 自分自身のポインタを取得
	return this;
}