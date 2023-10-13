//=============================================================================
// 
//  群体敵処理 [enemy_crowd.cpp]
//  Author : 相馬靜雅
// 
//=============================================================================
#include "enemy_crowd.h"
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
CEnemyCrowd::CEnemyCrowd(int nPriority) : CEnemy(nPriority)
{
	// 値のクリア
	m_nCntEmission = 0;	// 発生物のカウンター
	m_nIntEmission = 0;	// 発生物の間隔
}

//==========================================================================
// デストラクタ
//==========================================================================
CEnemyCrowd::~CEnemyCrowd()
{

}


//==========================================================================
// 初期化処理
//==========================================================================
HRESULT CEnemyCrowd::Init(void)
{
	// 初期化処理
	CEnemy::Init();

	// 体力取得
	int nLife = GetLife();

	//// 体力ゲージ
	//m_pHPGauge = CHP_Gauge::Create(80.0f, nLife, 0.8f);
	m_state = STATE_SPAWN;	// 親追い掛け状態
	m_Oldstate = STATE_PLAYERCHASE;

	//if (m_pHPGauge == NULL)
	//{// NULLだったら
	//	m_pHPGauge = NULL;
	//}

	m_nIntEmission = 3;	// 発生物の間隔
	switch (m_colorType)
	{
	case COLORTYPE_NORMAL:
		break;

	case COLORTYPE_POISON:

		m_nIntEmission = 3;	// 発生物の間隔
		break;

	case COLORTYPE_LUCKY:
		m_nIntEmission = 50;	// 発生物の間隔
		m_nSurvivalLife = 60 * 15;	// 生存時間
		break;

	case COLORTYPE_CONFUSION:
		m_nIntEmission = 100;	// 発生物の間隔
		break;

	case COLORTYPE_SWIFTFOOT:
		m_nIntEmission = 2;	// 発生物の間隔
		break;

	case COLORTYPE_TUTORIAL:
		m_Oldstate = STATE_WAIT;
		break;

	default:
		break;
	}
	
	// 生存時間
	m_nSurvivalLifeOrigin = m_nSurvivalLife;
	return S_OK;
}

//==========================================================================
// 終了処理
//==========================================================================
void CEnemyCrowd::Uninit(void)
{
	// 終了処理
	CEnemy::Uninit();
}

//==========================================================================
// 死亡処理
//==========================================================================
void CEnemyCrowd::Kill(void)
{
	// 死亡処理
	CEnemy::Kill();
}

//==========================================================================
// 更新処理
//==========================================================================
void CEnemyCrowd::Update(void)
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

	// 発生物のカウンター加算
	m_nCntEmission = (m_nCntEmission + 1) % m_nIntEmission;

	if (m_nCntEmission == 0)
	{
		CEffect3D *pEffect = NULL;
		D3DXVECTOR3 spawnmove = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		float fRadius = 0.0f;
		int nCntEffect = 0;

		switch (m_colorType)
		{
		case COLORTYPE_NORMAL:
			break;

		case COLORTYPE_POISON:

			// 出現移動量
			spawnmove.x = sinf((float)Random(-314, 314) * 0.01f) * ((float)Random(0, 90) * 0.01f);
			spawnmove.z = cosf((float)Random(-314, 314) * 0.01f) * ((float)Random(0, 90) * 0.01f);
			spawnmove.y = (float)Random(20, 40) * 0.1f;

			fRadius = (float)Random(600, 800) * 0.1f;

			// エフェクト生成
			pEffect = CEffect3D::Create(
				pos,
				spawnmove,
				D3DXCOLOR(0.6f, 0.2f, 0.6f, 0.35f),
				fRadius,
				60,
				CEffect3D::MOVEEFFECT_SUB,
				CEffect3D::TYPE_SMOKE);

			if (pEffect != NULL)
			{
				// セットアップ位置設定
				pEffect->SetUp(D3DXVECTOR3(0.0f, 20.0f, 0.0f), CObject::GetObject(), SetEffectParent(pEffect));
			}

			// 半径ランダム決め
			fRadius = (float)Random(200, 400) * 0.1f;

			// エフェクト生成
			pEffect = CEffect3D::Create(
				pos,
				spawnmove,
				D3DXCOLOR(0.8f, 0.8f, 0.2f, 0.2f),
				fRadius,
				60,
				CEffect3D::MOVEEFFECT_SUB,
				CEffect3D::TYPE_SMOKE);

			if (pEffect != NULL)
			{
				// セットアップ位置設定
				pEffect->SetUp(D3DXVECTOR3(0.0f, 20.0f, 0.0f), CObject::GetObject(), SetEffectParent(pEffect));
			}
			break;

		case COLORTYPE_LUCKY:

			// 出現移動量
			spawnmove.x = sinf((float)Random(-314, 314) * 0.01f) * ((float)Random(0, 100) * 0.01f);
			spawnmove.z = cosf((float)Random(-314, 314) * 0.01f) * ((float)Random(0, 100) * 0.01f);
			spawnmove.y = (float)Random(20, 100) * 0.01f;

			fRadius = (float)Random(100, 200) * 0.1f;

			pos.x += ((float)Random(-100, 100) * 0.1f) + sinf(D3DX_PI + rot.y) * 30.0f;
			pos.z += ((float)Random(-100, 100) * 0.1f) + cosf(D3DX_PI + rot.y) * 30.0f;
			pos.y += (float)Random(30, 50);

			// 煙
			CEffect3D::Create(
				pos,
				spawnmove,
				D3DXCOLOR(1.0f, 0.8f, 0.1f, 1.0f),
				fRadius,
				90,
				CEffect3D::MOVEEFFECT_ADD,
				CEffect3D::TYPE_JUJI,
				fRadius * 0.05f);
			break;

		case COLORTYPE_CONFUSION:
			// 出現移動量
			spawnmove.x = sinf((float)Random(-314, 314) * 0.01f) * ((float)Random(0, 80) * 0.01f);
			spawnmove.z = cosf((float)Random(-314, 314) * 0.01f) * ((float)Random(0, 80) * 0.01f);
			spawnmove.y = (float)Random(20, 100) * 0.01f;

			fRadius = (float)Random(300, 500) * 0.1f;

			pos.x += ((float)Random(-100, 100) * 0.1f) + sinf(D3DX_PI + rot.y) * 20.0f;
			pos.z += ((float)Random(-100, 100) * 0.1f) + cosf(D3DX_PI + rot.y) * 20.0f;
			pos.y += (float)Random(40, 60);

			// 煙
			pEffect = CEffect3D::Create(
				pos,
				spawnmove,
				D3DXCOLOR(0.7f, 0.2f, 0.7f, 0.8f),
				fRadius,
				90,
				CEffect3D::MOVEEFFECT_ADD,
				CEffect3D::TYPE_SMOKEBLACK,
				fRadius * 0.025f);

			if (pEffect != NULL)
			{
				// セットアップ位置設定
				pEffect->SetUp(D3DXVECTOR3(0.0f, 20.0f, 0.0f), CObject::GetObject(), SetEffectParent(pEffect));
			}


			// 出現移動量
			fRadius = (float)Random(200, 400) * 0.1f;

			// 煙
			pEffect = CEffect3D::Create(
				pos,
				spawnmove,
				D3DXCOLOR(0.2f, 0.9f, 0.9f, 0.3f),
				fRadius,
				90,
				CEffect3D::MOVEEFFECT_ADD,
				CEffect3D::TYPE_SMOKE,
				fRadius * 0.025f);

			if (pEffect != NULL)
			{
				// セットアップ位置設定
				pEffect->SetUp(D3DXVECTOR3(0.0f, 20.0f, 0.0f), CObject::GetObject(), SetEffectParent(pEffect));
			}
			break;

		case COLORTYPE_SWIFTFOOT:
			// 出現移動量
			spawnmove.x = sinf(D3DX_PI + rot.y) * -((float)Random(20, 40) * 0.1f);
			spawnmove.z = cosf(D3DX_PI + rot.y) * -((float)Random(20, 40) * 0.1f);
			spawnmove.y = 0.0f;

			fRadius = (float)Random(300, 350) * 0.1f;

			// 煙
			CEffect3D::Create(
				D3DXVECTOR3(pos.x, pos.y + 20.0f, pos.z),
				spawnmove,
				D3DXCOLOR(0.3f, 0.3f, 0.8f, 0.3f),
				fRadius,
				25,
				CEffect3D::MOVEEFFECT_SUB,
				CEffect3D::TYPE_NORMAL,
				0.0f);
			break;

		default:
			break;
		}
	}

}

//==========================================================================
// 着地時の処理
//==========================================================================
void CEnemyCrowd::ProcessLanding(void)
{
	// 着地時の処理
	CEnemy::ProcessLanding();
}

//==========================================================================
// 攻撃状態移行処理
//==========================================================================
void CEnemyCrowd::ChangeToAttackState(void)
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

	float fRadius = 500.0f;

	if (CircleRange(pos, posPlayer, fRadius, pPlayer->GetRadius()) == true && m_sMotionFrag.bJump == false)
	{// 一定距離間にプレイヤーが入ったら

		// 攻撃状態にする
		m_state = STATE_ATTACK;
		m_sMotionFrag.bATK = true;
	}
}

//==========================================================================
// 出現
//==========================================================================
void CEnemyCrowd::Spawn(void)
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
	if (CManager::GetScene()->GetElevation()->IsHit(pos))
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
void CEnemyCrowd::StateAttack(void)
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


	// プレイヤー情報
	CPlayer *pPlayer = CManager::GetScene()->GetPlayer();
	if (pPlayer == NULL)
	{// NULLだったら
		return;
	}

	// 親の位置取得
	D3DXVECTOR3 posPlayer = pPlayer->GetPosition();

	// 位置取得
	D3DXVECTOR3 pos = GetPosition();

	// 向き取得
	D3DXVECTOR3 rot = GetRotation();

	// 目標の向き取得
	float fRotDest = GetRotDest();

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

//==========================================================================
// 追い掛け移動
//==========================================================================
void CEnemyCrowd::ChaseMove(float fMove)
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
void CEnemyCrowd::MotionSet(void)
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
void CEnemyCrowd::AttackAction(int nModelNum, CMotion::AttackInfo ATKInfo)
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

	CPlayer *pPlayer = CManager::GetScene()->GetPlayer();

	if (pPlayer == NULL)
	{// NULLだったら
		return;
	}

	// プレイヤーの位置
	D3DXVECTOR3 posPlayer = pPlayer->GetPosition();


	// 目標の角度を求める
	float fRotDest = atan2f((weponpos.x - posPlayer.x), (weponpos.z - posPlayer.z));

	// 位置取得
	D3DXVECTOR3 pos = GetPosition();

	// 位置取得
	D3DXVECTOR3 rot = GetRotation();

	// 弾の生成
	CObject *pBullet = CBullet::Create(
		CBullet::TYPE_ENEMY,
		CBullet::MOVETYPE_NORMAL,
		D3DXVECTOR3(pos.x, pos.y + 50.0f, pos.z),
		rot,
		D3DXVECTOR3(3.0f, 0.0f, 0.0f),
		40.0f);

	// マップマネージャの取得
	CMapManager *pMapManager = CManager::GetScene()->GetMapManager();
	if (pMapManager == NULL)
	{// NULLだったら
		return;
	}
	ANGLE setAngle = pMapManager->GetTargetAngle(GetMapIndex(), pPlayer->GetMapIndex(), GetMapMoveValue(), pPlayer->GetMapMoveValue());

	pBullet->SetMapIndex(GetMapIndex());
	pBullet->SetMapMoveValue(GetMapMoveValue());
	pBullet->SetMapPointRatio(GetMapPointRatio());
	pBullet->SetMoveAngle(setAngle);
}

//==========================================================================
// 描画処理
//==========================================================================
void CEnemyCrowd::Draw(void)
{
	// 描画処理
	CEnemy::Draw();
}

//==========================================================================
// 敵の情報取得
//==========================================================================
CEnemyCrowd *CEnemyCrowd::GetEnemy(void)
{
	// 自分自身のポインタを取得
	return this;
}