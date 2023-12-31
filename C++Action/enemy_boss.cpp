//=============================================================================
// 
//  ボス処理 [enemy_boss.cpp]
//  Author : 相馬靜雅
// 
//=============================================================================
#include "enemy_boss.h"
#include "manager.h"
#include "renderer.h"
#include "game.h"
#include "texture.h"
#include "input.h"
#include "calculation.h"
#include "bullet.h"
#include "bullet_obstacle.h"
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
#include "stage.h"
#include "objectX.h"
#include "debugproc.h"
#include "fade.h"
#include "enemybase.h"
#include "effect_enemyspawn.h"
#include "impactwave.h"
#include "blackframe.h"
#include "gamemanager.h"

//==========================================================================
// マクロ定義
//==========================================================================
#define WIDTH			(60.0f)							// 横幅
#define HEIGHT			(60.0f)							// 縦幅
#define MAX_LIFE		(5)								// 体力
#define PLAYER_SERCH	(800.0f)	// プレイヤー探索範囲
#define CHACE_DISTABCE	(50.0f)		// 追い掛ける時の間隔
#define JUMP			(18.0f)					// ジャンプ力初期値
#define BASECHANGETIME	(120)	// 拠点切り替え時間
#define PLAYERCHASETIME	(60 * 6 + 120)	// 親追い掛け時間
#define WAITTIME	(60 * 2)	// 待機時間
#define BULLETCOUNT		(3)		// 弾の回数
#define PIYOPIYOTIME	(60 * 5)	// ピヨピヨの時間
#define STUNKNOCKBACK_TIME	(90)	// 拠点切り替え時間

//==========================================================================
// 静的メンバ変数宣言
//==========================================================================

//==========================================================================
// コンストラクタ
//==========================================================================
CEnemyBoss::CEnemyBoss(int nPriority) : CEnemy(nPriority)
{
	// 値のクリア
	memset(&m_sAct, 0, sizeof(m_sAct));
	m_sAct.nKnockBackCnt = 0;
	m_sAct.nBulletCnt = 0;		// 弾のカウンター
	m_sAct.nAssultAngle = 1;	// 突進の向き
	m_sAct.AtkType = ATKTYPE_BULLET;	// 攻撃の種類
	m_sAct.StunPosDest = mylib_const::DEFAULT_VECTOR3;	// スタン時の目標の位置
	m_sAct.nKnockBackCnt = 0;
	m_BaseType = BASETYPE_ORIGIN;	// 拠点の種類
	m_BaseTypeDest = BASETYPE_ORIGIN;	// 目標の拠点種類
	m_nCntDamage = 0;			// ダメージカウンター
	m_nCntEmission = 0;			// 発生物のカウンター
}

//==========================================================================
// デストラクタ
//==========================================================================
CEnemyBoss::~CEnemyBoss()
{

}


//==========================================================================
// 初期化処理
//==========================================================================
HRESULT CEnemyBoss::Init(void)
{
	// 初期化処理
	CEnemy::Init();

	m_state = STATE_NONE;	// 親追い掛け状態
	m_Oldstate = STATE_NONE;
	m_ActType = ACTTYPE_BOSS;

	// 待機時間
	m_nCntState = WAITTIME;
	m_state = STATE_ATTACK;
	m_sMotionFrag.bATK = false;
	m_sAct.AtkType = ATKTYPE_APPEARANCE;

	m_pMotion->Set(MOTION_APPEARANCE);

	// ホバリング音
	CManager::GetInstance()->GetSound()->StopSound(CSound::LABEL_BGM_HOBARING);
	CManager::GetInstance()->GetSound()->PlaySound(CSound::LABEL_BGM_HOBARING);

	SetPosition(D3DXVECTOR3(-1500.0f, 3000.0f, 1500.0f));

	// 位置取得
	D3DXVECTOR3 pos = GetPosition();

	// 向き取得
	D3DXVECTOR3 rot = GetRotation();

	// 目標の向き取得
	float fRotDest = GetRotDest();

	// プレイヤー情報
	CPlayer *pPlayer = CManager::GetInstance()->GetScene()->GetPlayer();
	if (pPlayer == NULL)
	{
		return E_FAIL;
	}

	// マップマネージャの取得
	CMapManager *pMapManager = CGame::GetMapManager();
	if (pMapManager == NULL)
	{// NULLだったら
		return false;
	}

	D3DXVECTOR3 posTarget = pMapManager->GetTargetPosition(0, 0.0f);

	// 親の位置取得
	D3DXVECTOR3 posPlayer = pPlayer->GetPosition();

	// 目標の角度を求める
	fRotDest = atan2f((pos.x - posTarget.x), (pos.z - posTarget.z));
	rot.y = fRotDest;

	// 目標の向き設定
	SetRotDest(fRotDest);

	// 向き設定
	SetRotation(rot);

	m_posKnokBack = pos;

	return S_OK;
}

//==========================================================================
// 終了処理
//==========================================================================
void CEnemyBoss::Uninit(void)
{
	// 終了処理
	CEnemy::Uninit();

	// モード設定
	CManager::GetInstance()->GetFade()->SetFade(CScene::MODE_RESULT);
	CManager::GetInstance()->GetSound()->StopSound(CSound::LABEL_BGM_HOBARING);
}

//==========================================================================
// 死亡処理
//==========================================================================
void CEnemyBoss::Kill(void)
{
	// 死亡処理
	CEnemy::Kill();
	CManager::GetInstance()->GetSound()->StopSound(CSound::LABEL_BGM_HOBARING);
}

//==========================================================================
// ヒット処理
//==========================================================================
bool CEnemyBoss::Hit(const int nValue)
{
	// 向き取得
	D3DXVECTOR3 rot = GetRotation();
	D3DXVECTOR3 pos = GetPosition();

	// 体力取得
	int nLife = GetLife();

	if (m_nCntDamage <= 0 && m_sAct.AtkType != ATKTYPE_STUNKNOCKBACK)
	{// なにもない状態の時

		// 体力減らす
		if (nValue == mylib_const::DMG_SLASH)
		{
			nLife--;
		}
		else
		{
			nLife -= nValue;
		}

		// 体力設定
		SetLife(nLife);

		if (nLife > 0)
		{// 体力がなくなってなかったら

			// ダメージ音再生
			CManager::GetInstance()->GetSound()->PlaySound(CSound::LABEL_SE_DMG01);

			if (m_pHPGauge == NULL)
			{
				// 体力ゲージ
				m_pHPGauge = CHP_Gauge::Create(650.0f, GetLifeOrigin(), 3.0f);

				if (m_pHPGauge == NULL)
				{// NULLだったら
					m_pHPGauge = NULL;
				}
			}
		}

		if (nLife <= 0)
		{// 体力がなくなったら

			// ヒットストップ
			CManager::GetInstance()->SetEnableHitStop(5);

			// 振動
			CManager::GetInstance()->GetCamera()->SetShake(5, 10.0f, 0.0f);

			// 死亡状態にする
			m_state = STATE_DEAD;

			// 遷移カウンター設定
			m_nCntState = 40;

			// ノックバックの位置更新
			m_posKnokBack = GetPosition();

			// ノックバック判定にする
			m_sMotionFrag.bKnockback = true;

			// やられモーション
			//m_pMotion->Set(MOTION_KNOCKBACK);

			// 当たった
			return true;
		}

		// 補正
		ValueNormalize(nLife, GetLifeOrigin(), 0);

		// ノックバックの位置更新
		m_posKnokBack = GetPosition();

		// 遷移カウンター設定
		if (nValue == mylib_const::DMG_SLASH)
		{
			m_nCntDamage = 10;

			// 振動
			CManager::GetInstance()->GetCamera()->SetShake(5, 8.0f, 0.0f);

			if (m_pMotion->GetType() == MOTION_STUN)
			{
				// ヒットストップ
				CManager::GetInstance()->SetEnableHitStop(5);

				// 振動
				CManager::GetInstance()->GetCamera()->SetShake(10, 15.0f, 0.0f);
			}

		}
		else
		{// 弾撃ち返しのダメージの時
			m_nCntDamage = 20;

			if (m_sAct.AtkType == ATKTYPE_ASSULT)
			{// 突進中の時

				// スタン状態にする
				m_state = STATE_ATTACK;
				m_nCntState = PIYOPIYOTIME;
				m_sAct.AtkType = ATKTYPE_STUN;
			}
			else if(m_sAct.AtkType == ATKTYPE_BULLET || m_sAct.AtkType == ATKTYPE_ENEMYSPAWN)
			{
				// マップマネージャの取得
				CMapManager *pMapManager = CGame::GetMapManager();
				if (pMapManager == NULL)
				{// NULLだったら
					return false;
				}

				// プレイヤー情報
				CPlayer *pPlayer = CManager::GetInstance()->GetScene()->GetPlayer();
				if (pPlayer == NULL)
				{// NULLだったら
					return false;
				}

				// スタンノックバック状態にする
				m_state = STATE_ATTACK;
				m_nCntState = STUNKNOCKBACK_TIME;
				m_sAct.AtkType = ATKTYPE_STUNKNOCKBACK;
				m_BaseType = BASETYPE_MAP;
				m_sAct.nKnockBackCnt = 0;

				// スタンの目的地決定
				float fMoveValue = pPlayer->GetMapMoveValue() + 200.0f;
				float fRatio = 0.0f;
				int nMapIdx = pPlayer->GetMapIndex();
				m_sAct.StunPosDest = pMapManager->GetTargetPosition(pPlayer->GetMapIndex(), fMoveValue);
				pMapManager->UpdateNowPosition(nMapIdx, fRatio, fMoveValue, 0.0f);

				D3DXVECTOR3 move = GetMove();
				move.y = 35.0f;
				SetMove(move);

				SetMapIndex(nMapIdx);
				SetMapPointRatio(fRatio);
				SetMapMoveValue(fMoveValue);

				m_posKnokBack.y = m_sAct.StunPosDest.y;

			}

			// ヒットストップ
			CManager::GetInstance()->SetEnableHitStop(5);

			// 振動
			CManager::GetInstance()->GetCamera()->SetShake(10, 15.0f, 0.0f);
		}

		// 当たった
		return true;
	}

	// 死んでない
	return false;
}

//==========================================================================
// 更新処理
//==========================================================================
void CEnemyBoss::Update(void)
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


	// 攻撃別処理
	//UpdateByAttack();

	// 発生物のカウンター更新
	m_nCntEmission = (m_nCntEmission + 1) % 14;
	if (m_nCntEmission == 0)
	{
		// 位置取得
		D3DXVECTOR3 pos = GetPosition();

		// 衝撃波生成
		CImpactWave::Create
		(
			D3DXVECTOR3(pos.x, pos.y, pos.z),	// 位置
			D3DXVECTOR3(0.0f, 0.0f, 0.0f),		// 向き
			D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f),	// 色
			100.0f,								// 幅
			100.0f,								// 高さ
			8,									// 寿命
			40.0f,								// 幅の移動量
			CImpactWave::TYPE_SMOKE,			// テクスチャタイプ
			false								// 加算合成するか
		);
	}

	// ダメージカウンター
	m_nCntDamage--;

	if (m_nCntDamage <= 0)
	{
		m_nCntDamage = 0;
	}
	else
	{
		// 色設定
		m_mMatcol = D3DXCOLOR(1.0f, 0.0f, 0.0f, m_mMatcol.a);
	}

	// デバッグ表示
	CManager::GetInstance()->GetDebugProc()->Print(
		"------------------[ボスの情報]------------------\n"
		"体力：【%d】\n", GetLife());

}

//==========================================================================
// プレイヤーとの当たり判定
//==========================================================================
void CEnemyBoss::CollisionPlayer(void)
{
	// プレイヤーの取得
	CPlayer *pPlayer = CManager::GetInstance()->GetScene()->GetPlayer();
	if (pPlayer == NULL)
	{
		return;
	}

	if (m_state == STATE_SPAWN || m_state == STATE_DEAD || m_state == STATE_FADEOUT)
	{
		return;
	}

	// 自分の情報取得
	D3DXVECTOR3 pos = GetPosition();
	float fRadius = GetRadius();

	if (m_sAct.AtkType == ATKTYPE_STUN ||
		m_sAct.AtkType == ATKTYPE_STUNKNOCKBACK)
	{
		fRadius *= 0.5f;
	}

	// プレイヤー情報取得
	D3DXVECTOR3 PlayerPos = pPlayer->GetPosition();
	float PlayerRadius = pPlayer->GetRadius();
	CPlayer::STATE PlayerState = (CPlayer::STATE)pPlayer->GetState();

	// 球の判定
	if (SphereRange(pos, PlayerPos, fRadius, PlayerRadius) &&
		PlayerState != CPlayer::STATE_DEAD &&
		PlayerState != CPlayer::STATE_DMG &&
		PlayerState != CPlayer::STATE_KNOCKBACK &&
		PlayerState != CPlayer::STATE_INVINCIBLE)
	{
		// マップマネージャの取得
		CMapManager *pMapManager = CGame::GetMapManager();
		if (pMapManager == NULL)
		{// NULLだったら
			return;
		}

		// ヒット処理
		if (pPlayer->Hit(1) == false)
		{// 死んでなかったら
			ANGLE setAngle = pMapManager->GetTargetAngle(GetMapIndex(), pPlayer->GetMapIndex(), GetMapMoveValue(), pPlayer->GetMapMoveValue());

			int nAngle = 1;
			if (setAngle == ANGLE_LEFT)
			{
				nAngle = -1;
			}

			// 吹っ飛び移動量設定
			pPlayer->SetMove(D3DXVECTOR3(8.0f * nAngle, 0.0f, 0.0f));
		}
	}

}

//==========================================================================
// 種類別更新処理
//==========================================================================
void CEnemyBoss::UpdateByType(void)
{
	// 位置取得
	D3DXVECTOR3 pos = GetPosition();

	// 移動量取得
	D3DXVECTOR3 move = GetMove();

	// Xファイルとの判定
	CStage *pStage = CGame::GetStage();
	if (pStage == NULL)
	{// NULLだったら
		return;
	}

	bool bNowLand = false;
	for (int nCntStage = 0; nCntStage < pStage->GetNumAll(); nCntStage++)
	{
		// オブジェクト取得
		CObjectX *pObjX = pStage->GetObj(nCntStage);

		if (pObjX == NULL)
		{// NULLだったら
			continue;
		}

		// 高さ取得
		bool bLand = false;
		float fHeight = pObjX->GetHeight(pos, bLand);

		if (bLand == true && fHeight > pos.y)
		{// 地面の方が自分より高かったら

			// 地面の高さに補正
			pos.y = fHeight;

			if (bLand == true)
			{// 着地してたら

				// ジャンプ使用可能にする
				move.y = 0.0f;
				bNowLand = true;
			}
		}
	}


	// 位置設定
	SetPosition(pos);

	// 移動量設定
	SetMove(move);

	// デバッグ表示
	CManager::GetInstance()->GetDebugProc()->Print(
		"------------------[ボス情報]------------------\n"
		"行動の種類：【%d】\n"
		"目標の拠点：【%d】\n", m_ActType, m_BaseTypeDest);
}

//==========================================================================
// 攻撃別処理
//==========================================================================
void CEnemyBoss::UpdateByAttack(void)
{
	switch (m_sAct.AtkType)
	{
	case ATKTYPE_BULLET:
		UpdateAttackBullet();
		break;

	case ATKTYPE_ASSULT:
		UpdateAttackAssult();
		break;

	case ATKTYPE_ENEMYSPAWN:
		UpdateChildSpawn();
		break;

	case ATKTYPE_STUNKNOCKBACK:
		UpdateKnockBackStun();
		break;

	case ATKTYPE_STUN:
		UpdateStun();
		break;

	case ATKTYPE_APPEARANCE:
		UpdateAppearance();
		break;

	default:
		break;
	}
}

//==========================================================================
// 登場
//==========================================================================
void CEnemyBoss::UpdateAppearance(void)
{
	int nType = m_pMotion->GetType();
	if (nType == MOTION_APPEARANCE && m_pMotion->IsFinish() == true)
	{// 登場演出が終わってたら

		m_state = STATE_WAIT;	// 親追い掛け状態
		m_Oldstate = STATE_NONE;

		// 待機時間
		m_nCntState = WAITTIME;
		m_state = STATE_WAIT;
		m_sMotionFrag.bATK = false;
		m_sAct.nKnockBackCnt = 0;

		// 黒フレーム捌ける
		CManager::GetInstance()->GetBlackFrame()->SetState(CBlackFrame::STATE_OUT);
		CManager::GetInstance()->GetSound()->PlaySound(CSound::LABEL_BGM_BOSSGAME);

		// カメラ取得
		CCamera *pCamera = CManager::GetInstance()->GetCamera();
		D3DXVECTOR3 CameraRotation = pCamera->GetDestRotation();

		pCamera->SetDestRotation(D3DXVECTOR3(CameraRotation.x, CameraRotation.y, -0.25f));

		// 操作可能状態にする
		CGame::GetGameManager()->SetEnableControll(true);
		return;
	}

	// カメラ取得
	CCamera *pCamera = CManager::GetInstance()->GetCamera();
	pCamera->SetLenDest(pCamera->GetOriginDistance() - 500.0f, 10, 2.0f, 0.01f);
	D3DXVECTOR3 CameraRotation = pCamera->GetDestRotation();
	pCamera->SetDestRotation(D3DXVECTOR3(CameraRotation.x, CameraRotation.y, -0.1f));

	// 向き取得
	D3DXVECTOR3 rot = GetRotation();

	// 目標の向き取得
	float fRotDest = GetRotDest();

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

	// 目標の角度を求める
	fRotDest = atan2f((pos.x - posPlayer.x), (pos.z - posPlayer.z));
	rot.y += (fRotDest - rot.y) * 0.1f;
	SetRotation(rot);
	


	// マップマネージャの取得
	CMapManager *pMapManager = CGame::GetMapManager();
	if (pMapManager == NULL)
	{// NULLだったら
		return;
	}

	m_sAct.nKnockBackCnt++;

	if (m_sAct.nKnockBackCnt >= 100)
	{
		m_sAct.nKnockBackCnt = 100;
	}

	pos.x = Lerp(m_posKnokBack.x, 0.0f, (float)m_sAct.nKnockBackCnt / (float)100);
	pos.y = Lerp(m_posKnokBack.y, 1800.0f, (float)m_sAct.nKnockBackCnt / (float)100);
	pos.z = Lerp(m_posKnokBack.z, 0.0f, (float)m_sAct.nKnockBackCnt / (float)100);

	SetPosition(pos);


	// 登場モーション設定
	m_pMotion->Set(MOTION_APPEARANCE);
}

//==========================================================================
// 弾攻撃
//==========================================================================
void CEnemyBoss::UpdateAttackBullet(void)
{
	int nType = m_pMotion->GetType();
	if (nType == MOTION_BULLETATK && m_pMotion->IsFinish() == true)
	{// 弾攻撃が終わってたら

		// 弾のカウンター加算
		m_sAct.nBulletCnt++;

		if (BULLETCOUNT <= m_sAct.nBulletCnt)
		{//　弾の回数に満たしていたら

			// 待機時間
			m_nCntState = WAITTIME;
			m_state = STATE_WAIT;
			m_sMotionFrag.bATK = false;

			// 通常モーション設定
			m_pMotion->Set(MOTION_DEF);
		}
		else
		{// まだ弾の回数が残ってる場合
			m_state = STATE_ATTACK;

			// 通常モーション設定
			m_pMotion->Set(MOTION_DEF);
		}
		return;
	}
	
	if (nType != MOTION_BULLETATK)
	{// 弾攻撃じゃない場合

		switch (m_sAct.AtkType)
		{
		case ATKTYPE_BULLET:
			// 弾攻撃モーション設定
			m_pMotion->Set(MOTION_BULLETATK);

			// 弾発射
			CManager::GetInstance()->GetSound()->PlaySound(CSound::LABEL_SE_BOSSBULLET_CHARGE);
			break;
		}
	}

	// 攻撃中にする
	m_sMotionFrag.bATK = true;

}

//==========================================================================
// 突進攻撃
//==========================================================================
void CEnemyBoss::UpdateAttackAssult(void)
{
	if (m_nCntState <= 0)
	{// 遷移カウンターが0になったら

		// 通常モーション設定
		m_pMotion->Set(MOTION_DEF);
		return;
	}

	// モーションの情報取得
	CMotion::Info aInfo = m_pMotion->GetInfo(m_pMotion->GetType());

	// 攻撃情報の総数取得
	int nNumAttackInfo = aInfo.nNumAttackInfo;

	// 衝撃波生成
	if (m_nCntState % 15 == 0 && m_nCntState <= PLAYERCHASETIME - 120)
	{
		// 位置取得
		D3DXVECTOR3 weponpos = GetPosition();

		for (int nCntAttack = 0; nCntAttack < nNumAttackInfo; nCntAttack++)
		{
			if (aInfo.AttackInfo[nCntAttack] == NULL)
			{// NULLだったら
				continue;
			}

			// モデル情報取得
			CModel **pModel = GetObjectChara()->GetModel();
			D3DXMATRIX mtxTrans;	// 計算用マトリックス宣言

			// 武器のマトリックス取得
			D3DXMATRIX mtxWepon;
			D3DXMatrixIdentity(&mtxWepon);

			if (pModel[aInfo.AttackInfo[nCntAttack]->nCollisionNum] == NULL)
			{// NULLだったら
				return;
			}

			// 判定するパーツのマトリックス取得
			mtxWepon = pModel[aInfo.AttackInfo[nCntAttack]->nCollisionNum]->GetWorldMtx();

			// 位置を反映する
			D3DXMatrixTranslation(&mtxTrans, aInfo.AttackInfo[nCntAttack]->Offset.x, aInfo.AttackInfo[nCntAttack]->Offset.y, aInfo.AttackInfo[nCntAttack]->Offset.z);
			D3DXMatrixMultiply(&mtxWepon, &mtxTrans, &mtxWepon);

			// 武器の位置
			weponpos = D3DXVECTOR3(mtxWepon._41, mtxWepon._42, mtxWepon._43);
		}

		// 向き取得
		D3DXVECTOR3 rot = GetRotation();

		CImpactWave::Create
		(
			weponpos,	// 位置
			D3DXVECTOR3(D3DX_PI * 0.5f, D3DX_PI + rot.y, 0.0f),		// 向き
			D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.6f),	// 色
			150.0f,								// 幅
			150.0f,								// 高さ
			15,									// 寿命
			25.0f,								// 幅の移動量
			CImpactWave::TYPE_ORANGE2,			// テクスチャタイプ
			true								// 加算合成するか
		);
	}

	// 弾攻撃モーション設定
	m_pMotion->Set(MOTION_ASSULTATK);
}

//==========================================================================
// 子分出現攻撃
//==========================================================================
void CEnemyBoss::UpdateChildSpawn(void)
{
	int nType = m_pMotion->GetType();
	if (nType == MOTION_CHILDSPAWN && m_pMotion->IsFinish() == true)
	{// 出現攻撃が終わってたら

		// 突進に移る
		m_state = STATE_BASECHANGE;
		m_nCntState = BASECHANGETIME;
		m_BaseTypeDest = BASETYPE_MAP;

		// 通常モーション設定
		m_pMotion->Set(MOTION_DEF);
		return;
	}
	
	if (nType != MOTION_CHILDSPAWN)
	{// 子分出現じゃない場合

		// 子分出現モーション設定
		m_pMotion->Set(MOTION_CHILDSPAWN);
	}
}

//==========================================================================
// ピヨピヨ攻撃(?)
//==========================================================================
void CEnemyBoss::UpdateStun(void)
{
	if (m_nCntState == PIYOPIYOTIME)
	{
		// スタン音
		CManager::GetInstance()->GetSound()->StopSound(CSound::LABEL_SE_STUN);
		CManager::GetInstance()->GetSound()->PlaySound(CSound::LABEL_SE_STUN);
		CManager::GetInstance()->GetSound()->StopSound(CSound::LABEL_SE_ASSULT);
	}

	m_nCntState--;
	if (m_nCntState <= 0)
	{// 遷移カウンターが0になったら

		// 待機時間
		m_nCntState = WAITTIME;
		m_state = STATE_WAIT;
		m_sMotionFrag.bATK = false;

		// スタン音
		CManager::GetInstance()->GetSound()->StopSound(CSound::LABEL_SE_STUN);
		CManager::GetInstance()->GetSound()->PlaySound(CSound::LABEL_SE_STUNHEAL);

		// 通常モーション設定
		m_pMotion->Set(MOTION_DEF);
		return;
	}
	
	// スタンモーション設定
	m_pMotion->Set(MOTION_STUN);
}

//==========================================================================
// スタンのノックバック
//==========================================================================
void CEnemyBoss::UpdateKnockBackStun(void)
{

	// 状態遷移カウンター減算
	m_nCntState--;

	if (m_nCntState <= 0)
	{
		// スタン状態にする
		m_state = STATE_ATTACK;
		m_nCntState = PIYOPIYOTIME;
		m_sAct.AtkType = ATKTYPE_STUN;
		return;
	}

	// スタンモーション設定
	m_pMotion->Set(MOTION_STUN);

	// 位置取得
	D3DXVECTOR3 pos = GetPosition();

	// ノックバック時間加算
	m_sAct.nKnockBackCnt++;

	// 放物線移動
	pos = GetParabola(m_posKnokBack, m_sAct.StunPosDest, 600.0f, (float)m_sAct.nKnockBackCnt / (float)STUNKNOCKBACK_TIME);

	// 位置設定
	SetPosition(pos);
}

//==========================================================================
// 着地時の処理
//==========================================================================
void CEnemyBoss::ProcessLanding(void)
{
	// 着地時の処理
	CEnemy::ProcessLanding();
}

//==========================================================================
// 攻撃状態移行処理
//==========================================================================
void CEnemyBoss::ChangeToAttackState(void)
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

	float fRadius = 500.0f;

	//if (CircleRange3D(pos, posPlayer, fRadius, pPlayer->GetRadius()) == true && m_sMotionFrag.bJump == false)
	{// 一定距離間にプレイヤーが入ったら

		// 攻撃状態にする
		m_state = STATE_ATTACK;
		m_sMotionFrag.bATK = true;
	}
}

//==========================================================================
// 出現
//==========================================================================
void CEnemyBoss::Spawn(void)
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
// 待機処理
//==========================================================================
void CEnemyBoss::StateWait(void)
{

	// プレイヤー情報
	CPlayer *pPlayer = CManager::GetInstance()->GetScene()->GetPlayer();
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


	// 待機時間
	m_nCntState--;
	if (m_nCntState <= 0)
	{// ゼロになったら

		// 行動抽選
		DrawingACT();
	}
}

//==========================================================================
// プレイヤー追従
//==========================================================================
void CEnemyBoss::PlayerChase(void)
{
	// 位置取得
	D3DXVECTOR3 pos = GetPosition();

	// 向き取得
	D3DXVECTOR3 rot = GetRotation();

	// 目標の向き取得
	float fRotDest = GetRotDest();

	// 移動量取得
	float fMove = GetVelocity();

	// 現在と目標の差分
	float fRotDiff = 0.0f;

	// 距離の判定
	bool bLen = false;

	// プレイヤー情報
	CPlayer *pPlayer = CManager::GetInstance()->GetScene()->GetPlayer();

	// オブジェクト情報
	CObject *pObj = NULL;

	if (m_nCntState == PLAYERCHASETIME)
	{
		// 突進開始音
		CManager::GetInstance()->GetSound()->StopSound(CSound::LABEL_SE_ASSULTEND);
		CManager::GetInstance()->GetSound()->PlaySound(CSound::LABEL_SE_ASSULT);
	}

	// 状態遷移カウンター減算
	m_nCntState--;

	if (m_nCntState <= 0)
	{// 遷移カウンターが0になったら

		// 待機時間
		m_nCntState = WAITTIME;
		m_state = STATE_WAIT;

		// 突進終了音
		CManager::GetInstance()->GetSound()->StopSound(CSound::LABEL_SE_ASSULT);
		CManager::GetInstance()->GetSound()->PlaySound(CSound::LABEL_SE_ASSULTEND);

		// 通常モーション設定
		m_pMotion->Set(MOTION_DEF);
		return;
	}

	// プレイヤーの方を見る処理
	//RotPlayer();

	// 攻撃別処理
	UpdateByAttack();

	if (m_nCntState <= PLAYERCHASETIME - 120)
	{// 遷移カウンターが0になったら

		// 追い掛け移動処理
		ChaseMove(fMove);
	}

	// 目標の向き設定
	SetRotDest(fRotDest);
}

//==========================================================================
// 攻撃処理
//==========================================================================
void CEnemyBoss::StateAttack(void)
{
	
	// 攻撃別処理
	UpdateByAttack();

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
}

//==========================================================================
// 追い掛け移動
//==========================================================================
void CEnemyBoss::ChaseMove(float fMove)
{
	// マップマネージャの取得
	CMapManager *pMapManager = CGame::GetMapManager();
	if (pMapManager == NULL)
	{// NULLだったら
		return;
	}

	// プレイヤー情報
	CPlayer *pPlayer = CManager::GetInstance()->GetScene()->GetPlayer();
	if (pPlayer == NULL)
	{
		return;
	}

	// 親の位置取得
	D3DXVECTOR3 posPlayer = pPlayer->GetPosition();
	CObject *pMyObj = GetObject();

	// 目標が自分のどっちにいるかを求める
	ANGLE TargetAngle = pMapManager->GetTargetAngle(pPlayer, pMyObj);

	// 移動量取得
	D3DXVECTOR3 move = GetMove();

	// 位置取得
	D3DXVECTOR3 pos = GetPosition();

	// 向き取得
	D3DXVECTOR3 rot = GetRotation();

	// 目標の向き取得
	float fRotDest = GetRotDest();

	// 現在と目標の差分
	float fRotDiff = 0.0f;

	// マップ情報
	int nIdxMapPoint = GetMapIndex();
	float fPointRatio = GetMapPointRatio();
	float fMoveValue = GetMapMoveValue();

	// 移動した量加算
	//move.x += fMove * m_sAct.nAssultAngle;
	fMoveValue += fMove * m_sAct.nAssultAngle;

	// 位置更新
	pos = pMapManager->UpdateNowPosition(nIdxMapPoint, fPointRatio, fMoveValue, pos.y);

	// 目標地点
	float fDest = (fMoveValue + (fMove + GetRadius() * 0.5f) * m_sAct.nAssultAngle);
	int nDestIdx = nIdxMapPoint;
	float fDestPointRatio = fPointRatio;
	D3DXVECTOR3 sakiPos = pMapManager->UpdateNowPosition(nDestIdx, fDestPointRatio, fDest, pos.y);

	// 目標の角度を求める
	fRotDest = atan2f((pos.x - sakiPos.x), (pos.z - sakiPos.z));

	// 目標との差分
	fRotDiff = fRotDest - rot.y;

	//角度の正規化
	RotNormalize(fRotDiff);

	//角度の補正をする
	rot.y += fRotDiff * 0.025f;

	// 角度の正規化
	RotNormalize(rot.y);

	// 目標の向き設定
	SetRotDest(fRotDest);

	// 向き設定
	SetRotation(rot);

	// 移動量設定
	SetMove(move);

	// 位置設定
	SetPosition(pos);

	// マップのインデックス番号設定
	SetMapIndex(nIdxMapPoint);

	// マップポイント間の割合設定
	SetMapPointRatio(fPointRatio);

	// マップポイント間の移動量設定
	SetMapMoveValue(fMoveValue);
}

//==========================================================================
// ダメージ
//==========================================================================
void CEnemyBoss::Damage(void)
{
	// 位置取得
	D3DXVECTOR3 pos = GetPosition();

	// 移動量取得
	D3DXVECTOR3 move = GetMove();

	// 向き取得
	D3DXVECTOR3 rot = GetRotation();

	// 目標の向き取得
	float fRotDest = GetRotDest();

	// 移動量取得
	float fMove = GetVelocity();

	// 現在と目標の差分
	float fRotDiff = 0.0f;

	// 距離の判定
	bool bLen = false;

	// プレイヤー情報
	CPlayer *pPlayer = CManager::GetInstance()->GetScene()->GetPlayer();

#if _DEBUG
	// 色設定
	m_mMatcol = D3DXCOLOR(1.0f, 0.0f, 0.0f, 1.0f);
#endif

	// 状態遷移カウンター減算
	m_nCntState--;

	if (m_nCntState <= 0)
	{// 遷移カウンターが0になったら

		// 過去の状態にする
		m_state = STATE_WAIT;
		m_nCntState = 60;
	}

	// 位置設定
	SetPosition(pos);

	// 移動量設定
	SetMove(move);

	// 向き設定
	SetRotation(rot);

	// 目標の向き設定
	SetRotDest(fRotDest);
}

//==========================================================================
// 拠点切り替え
//==========================================================================
void CEnemyBoss::ChangeBase(void)
{
	// マップマネージャの取得
	CMapManager *pMapManager = CGame::GetMapManager();
	if (pMapManager == NULL)
	{// NULLだったら
		return;
	}

	// 位置取得
	D3DXVECTOR3 pos = GetPosition();

	// マップ情報
	int nIdxMapPoint = GetMapIndex();
	float fPointRatio = GetMapPointRatio();
	float fMoveValue = GetMapMoveValue();

	// 目標の位置
	D3DXVECTOR3 DestPosition = GetPosition();

	// 状態遷移カウンター減算
	m_nCntState--;

	// 目標別処理
	switch (m_BaseTypeDest)
	{
	case CEnemy::BASETYPE_MAP:
		DestPosition = pMapManager->UpdateNowPosition(nIdxMapPoint, fPointRatio, fMoveValue, pos.y);
		pos.x = Lerp(0.0f, DestPosition.x, 1.0f - ((float)m_nCntState / (float)BASECHANGETIME));
		pos.z = Lerp(0.0f, DestPosition.z, 1.0f - ((float)m_nCntState / (float)BASECHANGETIME));
		break;

	case CEnemy::BASETYPE_ORIGIN:
		DestPosition = pMapManager->UpdateNowPosition(nIdxMapPoint, fPointRatio, fMoveValue, pos.y);
		pos.x = Lerp(DestPosition.x, 0.0f, 1.0f - ((float)m_nCntState / (float)BASECHANGETIME));
		pos.z = Lerp(DestPosition.z, 0.0f, 1.0f - ((float)m_nCntState / (float)BASECHANGETIME));
		break;
	}

	// プレイヤーの方を見る処理
	RotPlayer();

	// 位置設定
	SetPosition(pos);

	if (m_nCntState <= 0)
	{// 遷移カウンターが0になったら

		switch (m_BaseTypeDest)
		{
		case CEnemy::BASETYPE_MAP:
			m_sAct.AtkType = ATKTYPE_ASSULT;
			m_state = STATE_PLAYERCHASE;
			m_nCntState = PLAYERCHASETIME;
			break;

		case CEnemy::BASETYPE_ORIGIN:
			m_sAct.AtkType = ATKTYPE_BULLET;
			m_state = STATE_ATTACK;
			break;
		}
		m_BaseType = m_BaseTypeDest;
		//m_state = STATE_WAIT;
	}
}

//==========================================================================
// 行動抽選
//==========================================================================
void CEnemyBoss::DrawingACT(void)
{
	while (1)
	{
		// 行動抽選
		m_sAct.AtkType = (ATKTYPE)(rand() % ATKTYPE_MAX);

		if (m_sAct.AtkType != ATKTYPE_ENEMYSPAWN &&
			m_sAct.AtkType != ATKTYPE_STUN)
		{// 敵出現 && スタン以外
			break;
		}
	}

	// 弾の回数リセット
	m_sAct.nBulletCnt = 0;

	// 次の行動別
	switch (m_sAct.AtkType)
	{
	case ATKTYPE_BULLET:
		if (m_BaseType == BASETYPE_MAP)
		{// マップで移動してる場合
			m_state = STATE_BASECHANGE;
			m_nCntState = BASECHANGETIME;
			m_BaseTypeDest = BASETYPE_ORIGIN;
		}
		else
		{// 中心にいる場合
			m_state = STATE_ATTACK;
		}
		break;

	case ATKTYPE_ASSULT:	// 突進

		if (m_BaseType == BASETYPE_MAP)
		{// マップで移動してる場合
			m_state = STATE_PLAYERCHASE;
			m_nCntState = PLAYERCHASETIME;
		}
		else
		{// 中心にいる場合

			m_state = STATE_ATTACK;
			m_sAct.AtkType = ATKTYPE_ENEMYSPAWN;

			/*m_state = STATE_BASECHANGE;
			m_nCntState = BASECHANGETIME;
			m_BaseTypeDest = BASETYPE_MAP;*/
		}
		m_sAct.nAssultAngle = 1;	// 突進の向き

		if (Random(0, 1) == 0)
		{
			m_sAct.nAssultAngle = -1;	// 突進の向き
		}

		break;

	case ATKTYPE_ENEMYSPAWN:
		break;

	default:
		break;
	}
}

//==========================================================================
// モーションの設定
//==========================================================================
void CEnemyBoss::MotionSet(void)
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
	}
}

//==========================================================================
// 攻撃時処理
//==========================================================================
void CEnemyBoss::AttackAction(int nModelNum, CMotion::AttackInfo ATKInfo)
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


	// 目標の角度を求める
	float fRotDest = atan2f((weponpos.x - posPlayer.x), (weponpos.z - posPlayer.z));

	// 位置取得
	D3DXVECTOR3 pos = GetPosition();

	// 位置取得
	D3DXVECTOR3 rot = GetRotation();

	int nType = m_pMotion->GetType();

	// マップマネージャの取得
	CMapManager *pMapManager = CGame::GetMapManager();
	if (pMapManager == NULL)
	{// NULLだったら
		return;
	}

	CObject *pBullet = NULL;
	CObject *pBulletObstacle = NULL;
	D3DXVECTOR3 spawnpos = D3DXVECTOR3(pos.x, pos.y + 300.0f, pos.z);
	switch (nType)
	{
	case MOTION_BULLETATK:
	{
		// 弾発射
		CManager::GetInstance()->GetSound()->PlaySound(CSound::LABEL_SE_BOSSBULLET);

		int nCircleDivision = 15;
		for (int i = 0; i < nCircleDivision + 1; i++)
		{
			float fRot = ((D3DX_PI * 2.0f) / (float)nCircleDivision) * i;

			// どの種類の弾を出すか抽選
			if (Random(0, 3) == 0)
			{
				// 弾の生成
				pBullet = CBullet::Create(
					CBullet::TYPE_ENEMY,
					CBullet::MOVETYPE_NORMAL,
					spawnpos,
					rot,
					D3DXVECTOR3(sinf(D3DX_PI + fRot) * 10.0f, 0.0f, cosf(D3DX_PI + fRot) * 10.0f),
					80.0f);

				pBullet->SetMapIndex(GetMapIndex());
				pBullet->SetMapMoveValue(GetMapMoveValue());
				pBullet->SetMapPointRatio(GetMapPointRatio());
				pBullet->SetMoveAngle(ANGLE_UP);
			}
			else
			{
				// 障害物弾の生成
				pBulletObstacle = CBulletObstacle::Create(
					spawnpos,
					rot,
					D3DXVECTOR3(sinf(D3DX_PI + fRot) * 10.0f, 0.0f, cosf(D3DX_PI + fRot) * 10.0f),
					80.0f);

				pBulletObstacle->SetMapIndex(GetMapIndex());
				pBulletObstacle->SetMapMoveValue(GetMapMoveValue());
				pBulletObstacle->SetMapPointRatio(GetMapPointRatio());
				pBulletObstacle->SetMoveAngle(ANGLE_UP);
			}
		}
	}
		break;

	case MOTION_CHILDSPAWN:

		// ボストーク
		CManager::GetInstance()->GetSound()->PlaySound(CSound::LABEL_SE_BOSSTALK);

		int nNumAll = CGame::GetEnemyBase()->GetNumAll();
		int nRandom = Random(0, nNumAll - 1);

		// 敵の拠点情報取得
		CEnemyBase::sInfo sInfo = CGame::GetEnemyBase()->GetChaseChangeInfo(nRandom);

		// 敵配置
		CGame::GetEnemyManager()->SetEnemy(
					D3DXVECTOR3(0.0f, sInfo.fSpawnPosY, 0.0f),
					sInfo.nMapIdx,
					sInfo.fMapMoveValue,
					sInfo.nPattern);

		// 敵のスポーンエフェクト生成
		CEffectEnemySpawn::Create(pMapManager->GetTargetPosition(sInfo.nMapIdx, sInfo.fMapMoveValue));
		break;
	}
	
}

//==========================================================================
// プレイヤーの方を見る
//==========================================================================
void CEnemyBoss::RotPlayer(void)
{
	// 位置取得
	D3DXVECTOR3 pos = GetPosition();

	// 向き取得
	D3DXVECTOR3 rot = GetRotation();

	// 目標の向き取得
	float fRotDest = GetRotDest();

	// 現在と目標の差分
	float fRotDiff = 0.0f;

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
	fRotDiff = fRotDest - rot.y;

	//角度の正規化
	RotNormalize(fRotDiff);

	//角度の補正をする
	rot.y += fRotDiff * 0.025f;

	// 角度の正規化
	RotNormalize(rot.y);

	// 目標の向き設定
	SetRotDest(fRotDest);

	// 向き設定
	SetRotation(rot);
}

//==========================================================================
// 描画処理
//==========================================================================
void CEnemyBoss::Draw(void)
{
	// 描画処理
	CEnemy::Draw();
}

//==========================================================================
// 敵の情報取得
//==========================================================================
CEnemyBoss *CEnemyBoss::GetEnemy(void)
{
	// 自分自身のポインタを取得
	return this;
}