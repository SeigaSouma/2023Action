//=============================================================================
// 
//  プレイヤー処理 [enemy.cpp]
//  Author : 相馬靜雅
// 
//=============================================================================
#include "enemy.h"
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
#include "camera.h"
#include "ballast.h"
#include "3D_Effect.h"
#include "hp_gauge.h"
#include "shadow.h"
#include "sound.h"
#include "impactwave.h"
#include "score.h"
#include "bullet.h"
#include "mapmanager.h"
#include "enemyfixedmove_manager.h"

// 子クラス
#include "enemy_boss.h"
#include "enemy_crowd.h"
#include "enemy_fly.h"
#include "enemy_papion.h"

//==========================================================================
// マクロ定義
//==========================================================================
#define WIDTH			(60.0f)		// 横幅
#define HEIGHT			(60.0f)		// 縦幅
#define MAX_LIFE		(5)			// 体力
#define PLAYER_SERCH	(800.0f)	// プレイヤー探索範囲
#define CHACE_DISTABCE	(50.0f)		// 追い掛ける時の間隔
#define JUMP			(12.0f)		// ジャンプ力初期値
#define SURVIVALLIFE	(60 * 20)	// 生存時間
#define FADEOUT_TIME	(40)		// フェードアウト時間

//==========================================================================
// 静的メンバ変数宣言
//==========================================================================


//==========================================================================
// コンストラクタ
//==========================================================================
CEnemy::CEnemy(int nPriority) : CObjectChara(nPriority)
{
	// 値のクリア
	m_posOrigin = D3DXVECTOR3(0.0f, 0.0f, 0.0f);	// 最初の位置
	memset(&m_sFormationInfo, NULL, sizeof(m_sFormationInfo));	// 隊列の情報
	m_posKnokBack = D3DXVECTOR3(0.0f, 0.0f, 0.0f);	// ノックバックの位置
	m_rotOrigin = D3DXVECTOR3(0.0f, 0.0f, 0.0f);	// 最初の向き
	m_type = TYPE_BOSS;	// 種類
	m_state = STATE_NONE;	// 状態
	m_Oldstate = m_state;	// 前回の状態
	m_mMatcol = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);	// マテリアルの色
	m_nCntState = 0;		// 状態遷移カウンター
	m_nMapIdxOrigin = 0;		// 元のマップインデックス
	m_fMoveValueOrigin = 0.0f;	// 元の移動量
	m_nTexIdx = 0;			// テクスチャのインデックス番号
	m_nNumChild = 0;		// この数
	m_nIdxManager = 0;		// マネージャのインデックス番号
	m_nSurvivalLife = 0;	// 生存時間
	m_nSurvivalLifeOrigin = 0;	// 生存時間
	m_bAddScore = false;	// スコア加算するかの判定
	m_AddType = CResultManager::ADDTYPE_NONE;	// スコアの加算量
	m_nBallastEmission = 0;	// 瓦礫の発生カウンター
	m_sMotionFrag.bJump = false;		// ジャンプ中かどうか
	m_sMotionFrag.bKnockback = false;	// ノックバック中かどうか
	m_sMotionFrag.bMove = false;		// 移動中かどうか
	m_sMotionFrag.bATK = false;			// 攻撃中かどうか
	m_pParent = NULL;					// 親のポインタ
	m_pHPGauge = NULL;					// HPゲージの情報
	m_pMotion = NULL;					// モーションの情報
	m_pFixedMoveManager = NULL;			// 一定の動きマネージャ
	m_colorType = COLORTYPE_NORMAL;		// 色ごとの種類
	m_ActType = ACTTYPE_FIXED;			// 行動の種類

	memset(&m_pChild[0], NULL, sizeof(m_pChild));	// 子のポインタ
}

//==========================================================================
// デストラクタ
//==========================================================================
CEnemy::~CEnemy()
{

}

//==========================================================================
// 生成処理
//==========================================================================
CEnemy *CEnemy::Create(int nIdx, const char *pFileName, D3DXVECTOR3 pos, TYPE type)
{
	// 生成用のオブジェクト
	CEnemy *pEnemy = NULL;

	if (pEnemy == NULL)
	{// NULLだったら

		// メモリの確保
		switch (type)
		{
		case TYPE_BOSS:
			pEnemy = DEBUG_NEW CEnemyBoss;
			break;

		case TYPE_CROUWD:
			pEnemy = DEBUG_NEW CEnemyCrowd;
			break;

		case TYPE_FLY:
			pEnemy = DEBUG_NEW CEnemyFly;
			break;

		case TYPE_PAPION:
			pEnemy = DEBUG_NEW CEnemyPapion;
			break;

		default:
			return NULL;
			break;
		}

		if (pEnemy != NULL)
		{// メモリの確保が出来ていたら

			// インデックス番号
			pEnemy->m_nIdxManager = nIdx;

			// 種類
			pEnemy->m_type = type;

			// 最初の位置設定
			pEnemy->m_posOrigin = pos;

			// 位置設定
			pEnemy->SetPosition(pos);

			// テキスト読み込み
			HRESULT hr = pEnemy->RoadText(pFileName);
			if (FAILED(hr))
			{// 失敗していたら
				return NULL;
			}

			// 初期化処理
			pEnemy->Init();
		}

		return pEnemy;
	}

	return NULL;
}

//==========================================================================
// 初期化処理
//==========================================================================
HRESULT CEnemy::Init(void)
{
	// 各種変数の初期化
	m_state = STATE_SPAWN;	// 状態
	m_Oldstate = STATE_PLAYERCHASE;
	m_nCntState = 0;		// 状態遷移カウンター
	m_nSurvivalLife = SURVIVALLIFE;	// 生存時間
	m_posKnokBack = m_posOrigin;	// ノックバックの位置
	m_bAddScore = true;	// スコア加算するかの判定

	// 種類の設定
	SetType(TYPE_ENEMY);

	// 体力取得
	int nLife = GetLife();

	//if (m_pHPGauge == NULL)
	//{// NULLだったら
	//	m_pHPGauge = NULL;
	//}

	// 向き設定
	SetRotation(D3DXVECTOR3(0.0f, 0.0f, 0.0f));

	// 影の生成
	m_pShadow = CShadow::Create(GetPosition(), GetRadius() * 0.5f);

	// 一定の動き生成
	m_pFixedMoveManager = CEnemyFixedMoveManager::Create();
	if (m_pFixedMoveManager == NULL)
	{// NULLだったら
		return E_FAIL;
	}

	// ポーズのリセット
	m_pMotion->ResetPose(MOTION_DEF);

	// 生存時間
	m_nSurvivalLifeOrigin = m_nSurvivalLife;


	return S_OK;
}

//==========================================================================
// テキスト読み込み
//==========================================================================
HRESULT CEnemy::RoadText(const char *pFileName)
{
	// キャラ作成
	HRESULT hr = SetCharacter(pFileName);

	if (FAILED(hr))
	{// 失敗していたら
		return E_FAIL;
	}

	// モーションの生成処理
	m_pMotion = CMotion::Create(pFileName);
	if (m_pMotion == NULL)
	{
		return E_FAIL;
	}

	// オブジェクトキャラクターの情報取得
	CObjectChara *pObjChar = GetObjectChara();

	// モーションの設定
	m_pMotion->SetModel(pObjChar->GetModel(), pObjChar->GetNumModel(), pObjChar);

	return S_OK;
}

//==========================================================================
// 子登録
//==========================================================================
void CEnemy::RegistrChild(CEnemy *pChild)
{
	if (m_nNumChild >= mylib_const::MAX_ENEMY)
	{
		return;
	}

	if (m_pChild[m_nNumChild] == NULL)
	{
		m_pChild[m_nNumChild] = pChild;
		m_nNumChild++;	// 子の数加算
	}
	else
	{
		int n = 0;
	}
}

//==========================================================================
// 子のリセット
//==========================================================================
void CEnemy::ResetChild(CEnemy *pChild)
{
	int nMax = m_nNumChild;

	for (int nCntChild = 0; nCntChild < nMax; nCntChild++)
	{
		if (m_pChild[nCntChild] == NULL)
		{
			continue;
		}

		if (m_pChild[nCntChild] == pChild)
		{
			m_pChild[nCntChild] = NULL;
		}
	}
}


//==========================================================================
// 親のポインタ設定
//==========================================================================
void CEnemy::SetParent(CEnemy *pParent)
{
	if (this != nullptr)
	{
		// 子の割り当て
		if (pParent != NULL)
		{
			pParent->RegistrChild(this);
		}

		// 親のポインタ渡す
		m_pParent = pParent;
		
	}
	else
	{
		int n = 0;
	}
}

//==========================================================================
// 終了処理
//==========================================================================
void CEnemy::Uninit(void)
{
	int nIdx = m_nIdxManager;

	// 終了処理
	if (m_pMotion != NULL)
	{
		m_pMotion->Uninit();
		delete m_pMotion;
		m_pMotion = NULL;
	}

	// 終了処理
	if (m_pFixedMoveManager != NULL)
	{
		m_pFixedMoveManager->Uninit();
		delete m_pFixedMoveManager;
		m_pFixedMoveManager = NULL;
	}

	// 影を消す
	if (m_pShadow != NULL)
	{
		m_pShadow = NULL;
	}

	// 終了処理
	CObjectChara::Uninit();

	// マネージャの終了
	CEnemyManager::Release(nIdx);
}

//==========================================================================
// 死亡処理
//==========================================================================
void CEnemy::Kill(void)
{

	// スコア加算
	if (m_AddType != CResultManager::ADDTYPE_NONE && CManager::GetInstance()->GetMode() == CScene::MODE_GAME)
	{
		CGame::GetScore()->Add(m_AddType);

		if (m_bAddScore == true)
		{// スコア加算する判定だったら
			CGame::GetScore()->Add(GetAddScoreValue());
		}
	}

	for (int nCntEnemy = 0; nCntEnemy < mylib_const::MAX_ENEMY; nCntEnemy++)
	{// 子の数分回す

		if (m_pChild[nCntEnemy] != NULL)
		{// 子がいる場合

			// 自分の子の親をNULLにする
			m_pChild[nCntEnemy]->m_pParent = NULL;
		}
	}

	if (m_pParent != NULL)
	{// 自分に親がいる場合

		// 親の子をNULLにする
		m_pParent->ResetChild(this);
	}

	// HPゲージを消す
	if (m_pHPGauge != NULL)
	{
		m_pHPGauge->Uninit();
		m_pHPGauge = NULL;
	}

	// 影を消す
	if (m_pShadow != NULL)
	{
		m_pShadow->Uninit();
		m_pShadow = NULL;
	}
}

//==========================================================================
// 更新処理
//==========================================================================
void CEnemy::Update(void)
{
	// 死亡の判定
	if (IsDeath() == true)
	{// 死亡フラグが立っていたら
		return;
	}

	// エディット中は抜ける
	if (CGame::GetElevation()->IsEdit())
	{
		return;
	}

	// コンボ演出中は抜ける
	if (CGame::GetEnemyManager()->GetState() == CEnemyManager::STATE_COMBOANIM)
	{
		return;
	}

	// 過去の位置設定
	SetOldPosition(GetPosition());

	// 当たり判定
	Collision();

	if (m_ActType == CEnemy::ACTTYPE_FIXED)
	{
		// 一定の動きで移動
		FixedMove();
	}

	// 死亡の判定
	if (IsDeath() == true)
	{// 死亡フラグが立っていたら
		return;
	}

	// モーションの設定処理
	MotionSet();

	// 状態更新
	UpdateState();

	// 死亡の判定
	if (IsDeath() == true)
	{// 死亡フラグが立っていたら
		return;
	}

	// 攻撃処理
	Atack();

	// モーションの更新
	m_pMotion->Update();

	// 種類別処理
	UpdateByType();

	// HPゲージの位置更新
	if (m_pHPGauge != NULL)
	{
		// 位置取得
		D3DXVECTOR3 pos = GetPosition();

		// 体力取得
		int nLife = GetLife();

		m_pHPGauge->UpdatePosition(pos, nLife);
	}

	// 影の位置更新
	if (m_pShadow != NULL)
	{
		// 位置取得
		D3DXVECTOR3 pos = GetPosition();
		m_pShadow->SetPosition(D3DXVECTOR3(pos.x, m_pShadow->GetPosition().y, pos.z));
	}

	if (GetPosition().y <= mylib_const::KILL_Y)
	{
		// 敵の終了処理
		Kill();
		Uninit();
		return;
	}
}

//==========================================================================
// 当たり判定
//==========================================================================
void CEnemy::Collision(void)
{
	// 位置取得
	D3DXVECTOR3 pos = GetPosition();

	// 移動量取得
	D3DXVECTOR3 move = GetMove();

	// 向き取得
	D3DXVECTOR3 rot = GetRotation();

	// 重力処理
	if (m_type != TYPE_FLY)
	{
		move.y -= mylib_const::GRAVITY;
	}

	// 位置更新
	pos += move;

	// マップマネージャの取得
	CMapManager *pMapManager = CGame::GetMapManager();
	if (pMapManager == NULL)
	{// NULLだったら
		return;
	}

	// 慣性補正
	if (m_state != STATE_SPAWN && m_state != STATE_DMG && m_state != STATE_DEAD)
	{
		move.x += (0.0f - move.x) * 0.25f;
		move.z += (0.0f - move.z) * 0.25f;
	}

	if (move.x >= 0.1f || move.x <= -0.1f ||
		move.z >= 0.1f || move.z <= -0.1f)
	{// 移動中
		m_sMotionFrag.bMove = true;
	}
	else
	{
		m_sMotionFrag.bMove = false;
	}

	// 高さ取得
	bool bLand = false;
	float fHeight = CGame::GetElevation()->GetHeight(pos, bLand);

	if (fHeight > pos.y)
	{// 地面の方が自分より高かったら

		// 地面の高さに補正
		if (m_state != STATE_DEAD && m_state != STATE_FADEOUT)
		{
			pos.y = fHeight;
		}

		if (bLand == true)
		{// 着地してたら

			// 着地時処理
			ProcessLanding();

			// ジャンプ使用可能にする
			if (m_state != STATE_DMG && m_state != STATE_DEAD)
			{
				move.y = 0.0f;
			}
			m_sMotionFrag.bJump = false;
		}
	}


	// 箱
	float fLen = CGame::GetElevation()->GetWidthLen();
	int nBlock = CGame::GetElevation()->GetWidthBlock();
	nBlock /= 2;
	if (pos.x + GetRadius() > fLen * nBlock) { pos.x = fLen * nBlock - GetRadius();}
	if (pos.x - GetRadius() < -fLen * nBlock) { pos.x = -fLen * nBlock + GetRadius(); }
	if (pos.z + GetRadius() > fLen * nBlock) { pos.z = fLen * nBlock - GetRadius(); }
	if (pos.z - GetRadius() < -fLen * nBlock) { pos.z = -fLen * nBlock + GetRadius(); }


	if (pos.y <= -800.0f)
	{
		pos.y = -800.0f;
	}

	// 位置設定
	SetPosition(pos);

	// 移動量設定
	SetMove(move);
}

//==========================================================================
// 着地時の処理
//==========================================================================
void CEnemy::ProcessLanding(void)
{
	// 移動量取得
	D3DXVECTOR3 move = GetMove();

	// ジャンプ使用可能にする
	if (m_state != STATE_DMG && m_state != STATE_DEAD)
	{
		move.y = 0.0f;
	}
	m_sMotionFrag.bJump = false;

	// 移動量設定
	SetMove(move);
}

//==========================================================================
// ヒット処理
//==========================================================================
bool CEnemy::Hit(const int nValue)
{
	// 向き取得
	D3DXVECTOR3 rot = GetRotation();
	D3DXVECTOR3 pos = GetPosition();

	// 体力取得
	int nLife = GetLife();


	if ((nValue == mylib_const::DMG_BOUNCE && m_state != STATE_DEAD) || (m_state != STATE_DMG && m_state != STATE_DEAD && m_state != STATE_SPAWN))
	{// なにもない状態の時

		// 体力減らす
		nLife -= nValue;

		// 体力設定
		SetLife(nLife);

		if (nLife > 0)
		{// 体力がなくなってなかったら

			// ダメージ音再生
			CManager::GetInstance()->GetSound()->PlaySound(CSound::LABEL_SE_DMG01);

			if (m_pHPGauge == NULL)
			{
				// 体力ゲージ
				m_pHPGauge = CHP_Gauge::Create(50.0f, GetLifeOrigin(), 3.0f);

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

			// 死んだ
			return true;
		}

		// 補正
		ValueNormalize(nLife, GetLifeOrigin(), 0);

		// 過去の状態保存
		m_Oldstate = m_state;

		// ダメージ状態にする
		m_state = STATE_DMG;

		// 遷移カウンター設定
		if (nValue == mylib_const::DMG_SLASH)
		{
			m_nCntState = 10;

			// ヒットストップ
			//CManager::GetInstance()->SetEnableHitStop(2);

			// 振動
			CManager::GetInstance()->GetCamera()->SetShake(5, 8.0f, 0.0f);
		}
		else
		{
			m_nCntState = 30;

			// ヒットストップ
			CManager::GetInstance()->SetEnableHitStop(5);

			// 振動
			CManager::GetInstance()->GetCamera()->SetShake(10, 15.0f, 0.0f);
		}

		// ノックバックの位置更新
		m_posKnokBack = GetPosition();
	}

	// 死んでない
	return false;
}

//==========================================================================
// 種類別更新処理
//==========================================================================
void CEnemy::UpdateByType(void)
{

}

//==========================================================================
// 状態更新処理
//==========================================================================
void CEnemy::UpdateState(void)
{
	//return;
#if TOPCUR
	// 位置取得
	D3DXVECTOR3 pos = GetPosition();
	D3DXVECTOR3 pos11 = D3DXVECTOR3(GetObjectChara()->GetModel()[0]->GetWorldMtx()._41, GetObjectChara()->GetModel()[0]->GetWorldMtx()._42, GetObjectChara()->GetModel()[0]->GetWorldMtx()._43);

	//CEffect3D::Create(pos11, D3DXVECTOR3(0.0f, 0.0f, 0.0f), D3DXCOLOR(1.0f, 0.0f, 0.0f, 1.0f), GetRadius(), 10, CEffect3D::MOVEEFFECT_NONE, CEffect3D::TYPE_NORMAL);

	// 移動量取得
	D3DXVECTOR3 move = GetMove();

	// 色設定
	m_mMatcol = D3DXCOLOR(1.0f, 1.0f, 1.0f, m_mMatcol.a);

	switch (m_state)
	{
	case STATE_NONE:
		StateNone();
		break;

	case STATE_SPAWN:
		Spawn();
		break;

	case STATE_DMG:
		Damage();
		break;

	case STATE_DEAD:
		Dead();
		break;

	case STATE_FADEOUT:
		FadeOut();
		break;

	case STATE_PLAYERCHASE:
		PlayerChase();
		break;

	case STATE_PARENTCHASE:
		ParentChase();
		break;

	case STATE_ATTACK:
		StateAttack();
		break;

	case STATE_WAIT:
		StateWait();
		break;

	case STATE_BASECHANGE:
		ChangeBase();
		break;

	}
#endif
}

//==========================================================================
// 種類別状態更新処理
//==========================================================================
void CEnemy::UpdateStateByType(void)
{
	return;
}

//==========================================================================
// 何もない状態
//==========================================================================
void CEnemy::StateNone(void)
{
	
	// 色設定
	m_mMatcol = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);

	// プレイヤー情報
	CPlayer *pPlayer = CManager::GetInstance()->GetScene()->GetPlayer();

	if (pPlayer == NULL)
	{// NULLのとき
		return;
	}

	// 親の位置取得
	D3DXVECTOR3 PlayerPos = pPlayer->GetPosition();

	// 状態遷移カウンター減算
	m_nCntState--;

	switch (m_ActType)
	{
	case CEnemy::ACTTYPE_FIXED:
		break;

	case CEnemy::ACTTYPE_CHASE:

		m_state = STATE_PLAYERCHASE;

		//// 位置取得
		//D3DXVECTOR3 pos = GetPosition();

		//if (CircleRange(pos, PlayerPos, 200.0f, PLAYER_SERCH) == true)
		//{// プレイヤーが視界に入った
		//	m_state = STATE_PLAYERCHASE;
		//}

		//// 位置設定
		//SetPosition(pos);
		break;

	case CEnemy::ACTTYPE_TURRET:
		break;

	case CEnemy::ACTTYPE_MAX:
		break;

	default:
		break;
	}

	if (m_nCntState <= 0)
	{// 遷移カウンターが0になったら
		m_nCntState = 0;
	}
}

//==========================================================================
// 一定の動き
//==========================================================================
void CEnemy::FixedMove(void)
{
	// 位置取得
	D3DXVECTOR3 pos = GetPosition();

	// 一定の動きの位置取得
	pos = m_pFixedMoveManager->UpdatePosition(this);

	// 位置設定
	SetPosition(pos);

	// 攻撃状態移行処理
	ChangeToAttackState();
}

//==========================================================================
// 出現
//==========================================================================
void CEnemy::Spawn(void)
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
		m_nCntState = 0;		// 遷移カウンター設定
		move.y = 0.0f;			// 移動量ゼロ
	}

	// 位置設定
	SetPosition(pos);

	// 移動量設定
	SetMove(move);
}

//==========================================================================
// ダメージ
//==========================================================================
void CEnemy::Damage(void)
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
		m_state = m_Oldstate;
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
// 死亡
//==========================================================================
void CEnemy::Dead(void)
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

	// 状態遷移カウンター減算
	m_nCntState--;

	// 色設定
	m_mMatcol = D3DXCOLOR(1.0f, 1.0f, 1.0f, m_mMatcol.a);
	m_mMatcol.a -= 1.0f / 80.0f;

	if (m_nCntState <= 0)
	{// 遷移カウンターが0になったら or 地面に接触

		// パーティクル生成
		my_particle::Create(pos, my_particle::TYPE_ENEMY_FADE);

		// 敵の終了処理
		Kill();
		Uninit();
		return;
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
// フェードアウト
//==========================================================================
void CEnemy::FadeOut(void)
{
	// 移動量取得
	float fMove = GetVelocity();

	// 位置取得
	D3DXVECTOR3 pos = GetPosition();

	// フェードアウトモーション設定
	m_pMotion->Set(MOTION_FADEOUT);

	// フェードアウトのフレーム数
	int nAllFrame = m_pMotion->GetMaxAllCount(MOTION_FADEOUT);

	// モーションの情報取得
	CMotion::Info aInfo = m_pMotion->GetInfo(m_pMotion->GetType());

	// 攻撃情報の総数取得
	int nNumAttackInfo = aInfo.nNumAttackInfo;

	// 高さ取得
	bool bLand = false;
	float fHeight = CGame::GetElevation()->GetHeight(pos, bLand);

	m_sMotionFrag.bMove = false;	// 移動判定OFF
	m_sMotionFrag.bATK = false;		// 攻撃判定OFF

	// 遷移カウンター加算
	m_nCntState++;

	if (m_nCntState >= nAllFrame)
	{// 遷移カウンターがモーションを超えたら

		// スコア加算の判定オフ
		m_bAddScore = false;

		// 敵の終了処理
		Kill();
		Uninit();
		return;
	}
}

//==========================================================================
// プレイヤー追従
//==========================================================================
void CEnemy::PlayerChase(void)
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

	// マップマネージャの取得
	CMapManager *pMapManager = CGame::GetMapManager();
	if (pMapManager == NULL)
	{// NULLだったら
		return;
	}


	// 状態遷移カウンター減算
	m_nCntState--;

	if (m_nCntState <= 0)
	{// 遷移カウンターが0になったら
		m_nCntState = 0;
	}

	if (pPlayer != NULL)
	{// NULLじゃないとき

		// 親の位置取得
		D3DXVECTOR3 posPlayer = pPlayer->GetPosition();
		CObject *pMyObj = GetObject();

		// 目標が自分のどっちにいるかを求める
		ANGLE TargetAngle = pMapManager->GetTargetAngle(pPlayer, pMyObj);

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

		// 向き設定
		SetRotation(rot);

		// 攻撃状態移行処理
		ChangeToAttackState();

		// 向いてる方向にダッシュ
		if (bLen == false)
		{// 距離が保たれていたら

			// 追い掛け移動処理
			ChaseMove(fMove);
		}

		if (CircleRange(pos, pPlayer->GetPosition(), 200.0f, PLAYER_SERCH) == false)
		{// プレイヤーが視界から消えたら
			m_state = STATE_NONE;
		}
	}


	// 位置設定
	SetPosition(pos);

	// 目標の向き設定
	SetRotDest(fRotDest);
}

//==========================================================================
// 親追従
//==========================================================================
void CEnemy::ParentChase(void)
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

	// かなり離れてるかの判定
	bool bLongDistance = true;

	// プレイヤー情報
	CPlayer *pPlayer = CManager::GetInstance()->GetScene()->GetPlayer();

	// オブジェクト情報
	CObject *pObj = NULL;


	// 状態遷移カウンター減算
	//m_nCntState++;

	if (m_nCntState <= 0)
	{// 遷移カウンターが0になったら
		m_nCntState = 0;
	}

	if (m_pParent != NULL)
	{// 親がいる場合

		// 親の移動量取得
		D3DXVECTOR3 moveParent = m_pParent->GetMove();

		// 親の位置取得
		D3DXVECTOR3 posParent = m_pParent->GetPosition();

		// 親の向き取得
		D3DXVECTOR3 rotParent = m_pParent->GetRotation();

		// 目標の位置
		D3DXVECTOR3 posDest;
		posDest = posParent;

		// 目標の角度を求める
		fRotDest = atan2f((pos.x - posDest.x), (pos.z - posDest.z));

		// 目標との差分
		fRotDiff = fRotDest - rot.y;

		//角度の正規化
		RotNormalize(fRotDiff);

		//角度の補正をする
		rot.y += fRotDiff * 0.025f;

		// 角度の正規化
		RotNormalize(rot.y);

		// 向き設定
		SetRotation(rot);

		if (CircleRange(pos, posDest, 25.0f, CHACE_DISTABCE) == true)
			{// 一定距離間に親が入ったら
				bLen = true;	// 長さ判定
			}

			// 向いてる方向にダッシュ
			if (bLen == false)
			{// 距離が保たれていたら

				// 追い掛け移動処理
				ChaseMove(fMove * 1.4f);
			}

	}
	else
	{// 自分自身が親の時

		float fRotDiff = 0.0f;	// 現在と目標の差分

		// 状態遷移カウンター更新
		m_nCntState = (m_nCntState + 1) % 120;

		// 目標の角度を求める
		if (m_nCntState == 0)
		{
			fRotDest = Random(-31, 31) * 0.1f;
		}

		// 目標との差分
		fRotDiff = fRotDest - rot.y;

		//角度の正規化
		RotNormalize(fRotDiff);

		//角度の補正をする
		rot.y += fRotDiff * 0.025f;

		// 角度の正規化
		RotNormalize(rot.y);

		// 向き設定
		SetRotation(rot);

		// 追い掛け移動処理
		ChaseMove(fMove);

#if _DEBUG
		// 色設定
		m_mMatcol = D3DXCOLOR(1.0f, 0.5f, 1.0f, 1.0f);
#endif
	}

	if (m_colorType == COLORTYPE_POISON ||
		m_colorType == COLORTYPE_CONFUSION)
	{// 毒キノコは攻撃的

		// プレイヤー追従の判定
		TriggerChasePlayer();
	}

	// 目標の向き設定
	SetRotDest(fRotDest);
}

//==========================================================================
// 攻撃処理
//==========================================================================
void CEnemy::StateAttack(void)
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

	// オブジェクト情報
	CObject *pObj = NULL;


	// 現在の種類取得
	int nType = m_pMotion->GetType();

	if (m_pParent != NULL)
	{// 親がいる場合

		// 親も追い掛け状態にする
		if (m_pParent->m_state != STATE_ATTACK)
		{
			if (m_pParent->m_state != STATE_DMG && m_pParent->m_state != STATE_DEAD)
			{
				m_pParent->m_state = STATE_PLAYERCHASE;
				m_pParent->m_nCntState = 60;
			}

			for (int nCntEnemy = 0; nCntEnemy < m_pParent->m_nNumChild; nCntEnemy++)
			{// 子の数分回す
				if (m_pParent->m_pChild[nCntEnemy] == NULL)
				{
					continue;
				}

				if (m_pParent->m_pChild[nCntEnemy]->m_state == STATE_DMG || m_pParent->m_pChild[nCntEnemy]->m_state == STATE_DEAD)
				{
					continue;
				}

				m_pParent->m_pChild[nCntEnemy]->m_state = STATE_PLAYERCHASE;
				m_pParent->m_pChild[nCntEnemy]->m_nCntState = 60;
			}
		}
	}
	else
	{// 自分が親な場合

		for (int nCntEnemy = 0; nCntEnemy < m_nNumChild; nCntEnemy++)
		{// 子の数分回す

			if (m_pChild[nCntEnemy] == NULL)
			{// NULLだったら
				continue;
			}

			if (m_pChild[nCntEnemy]->m_state != STATE_ATTACK)
			{// 攻撃状態の時

				if (m_pChild[nCntEnemy]->m_state != STATE_DMG && m_pChild[nCntEnemy]->m_state != STATE_DEAD)
				{
					m_pChild[nCntEnemy]->m_state = STATE_PLAYERCHASE;
					m_pChild[nCntEnemy]->m_nCntState = 60;
				}
			}
		}
	}

	if (nType == MOTION_DEF && pPlayer != NULL)
	{// ニュートラルに戻れば

		if (CircleRange(pos, pPlayer->GetPosition(), 400.0f, PLAYER_SERCH) == false)
		{// プレイヤーと離れすぎていたら

			// 間隔をあける状態にする
			m_state = STATE_NONE;
		}
		else
		{// まだ追える時

			m_state = STATE_PLAYERCHASE;

			if (m_pParent != NULL)
			{// 親がいる場合

				// 親も追い掛け状態にする
				if (m_pParent->m_state != STATE_DMG && m_pParent->m_state != STATE_DEAD)
				{
					m_pParent->m_state = STATE_PLAYERCHASE;
				}

				for (int nCntEnemy = 0; nCntEnemy < m_pParent->m_nNumChild; nCntEnemy++)
				{// 子の数分回す

					if (m_pParent->m_pChild[nCntEnemy] == NULL)
					{
						continue;
					}

					if (m_pParent->m_pChild[nCntEnemy]->m_state == STATE_DMG || m_pParent->m_pChild[nCntEnemy]->m_state == STATE_DEAD)
					{
						continue;
					}

					m_pParent->m_pChild[nCntEnemy]->m_state = STATE_PLAYERCHASE;
				}
			}
			else
			{// 自分が親な場合

				for (int nCntEnemy = 0; nCntEnemy < m_nNumChild; nCntEnemy++)
				{// 子の数分回す

					if (m_pChild[nCntEnemy] == NULL)
					{
						continue;
					}

					if (m_pChild[nCntEnemy]->m_state == STATE_DMG || m_pChild[nCntEnemy]->m_state == STATE_DEAD)
					{
						continue;
					}

					m_pChild[nCntEnemy]->m_state = STATE_PLAYERCHASE;
				}
			}
		}
	}

	// 位置設定
	SetPosition(pos);

	// 移動量設定
	SetMove(move);

	// 向き設定
	SetRotation(rot);

	// 目標の向き設定
	SetRotDest(fRotDest);


	// 色設定
#if _DEBUG
	m_mMatcol = D3DXCOLOR(0.0f, 1.0f, 0.0f, 1.0f);
#endif
}

//==========================================================================
// 種類別状態更新処理
//==========================================================================
void CEnemy::StateWait(void)
{
	return;
}

//==========================================================================
// 拠点切り替え
//==========================================================================
void CEnemy::ChangeBase(void)
{
	return;
}


//==========================================================================
// プレイヤー追従ONにするトリガー
//==========================================================================
void CEnemy::TriggerChasePlayer(void)
{

	// プレイヤー情報
	CPlayer *pPlayer = CManager::GetInstance()->GetScene()->GetPlayer();

	// 位置取得
	D3DXVECTOR3 pos = GetPosition();

	if (pPlayer != NULL)
	{// NULLじゃないとき

		float fRadius = PLAYER_SERCH;

		if (CircleRange(pos, pPlayer->GetPosition(), 200.0f, fRadius) == true)
		{// プレイヤーが範囲に入れば
			m_state = STATE_PLAYERCHASE;

			if (m_pParent != NULL)
			{// 親がいる場合

				// 親も追い掛け状態にする
				if (m_pParent->m_state != STATE_DMG && m_pParent->m_state != STATE_DEAD)
				{
					m_pParent->m_state = STATE_PLAYERCHASE;
				}

				for (int nCntEnemy = 0; nCntEnemy < m_pParent->m_nNumChild; nCntEnemy++)
				{// 子の数分回す

					if (m_pParent->m_pChild[nCntEnemy] == NULL)
					{
						continue;
					}

					if (m_pParent->m_pChild[nCntEnemy]->m_state == STATE_DMG || m_pParent->m_pChild[nCntEnemy]->m_state == STATE_DEAD)
					{
						continue;
					}

					// 自分の親の子もプレイヤー追い掛け状態
					m_pParent->m_pChild[nCntEnemy]->m_state = STATE_PLAYERCHASE;
				}
			}
			else
			{// 自分が親な場合

				for (int nCntEnemy = 0; nCntEnemy < m_nNumChild; nCntEnemy++)
				{// 子の数分回す

					if (m_pChild[nCntEnemy] == NULL)
					{
						continue;
					}

					if (m_pChild[nCntEnemy]->m_state == STATE_DMG || m_pChild[nCntEnemy]->m_state == STATE_DEAD)
					{
						continue;
					}

					m_pChild[nCntEnemy]->m_state = STATE_PLAYERCHASE;
				}
			}
		}
	}
}

//==========================================================================
// 攻撃状態移行処理
//==========================================================================
void CEnemy::ChangeToAttackState(void)
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

	if (CircleRange(pos, posPlayer, 400.0f, pPlayer->GetRadius()) == true && m_sMotionFrag.bJump == false)
	{// 一定距離間にプレイヤーが入ったら

		// 攻撃状態にする
		m_state = STATE_ATTACK;
		m_sMotionFrag.bATK = true;
	}
}

//==========================================================================
// 追い掛け移動
//==========================================================================
void CEnemy::ChaseMove(float fMove)
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
// 攻撃
//==========================================================================
void CEnemy::Atack(void)
{

	// 位置取得
	D3DXVECTOR3 pos = GetPosition();

	// 向き取得
	D3DXVECTOR3 rot = GetRotation();

	// 移動量取得
	D3DXVECTOR3 move = GetMove();

	// モーションの情報取得
	CMotion::Info aInfo = m_pMotion->GetInfo(m_pMotion->GetType());

	// 攻撃情報の総数取得
	int nNumAttackInfo = aInfo.nNumAttackInfo;

	for (int nCntAttack = 0; nCntAttack < nNumAttackInfo; nCntAttack++)
	{
		if (aInfo.AttackInfo[nCntAttack] == NULL)
		{// NULLだったら
			continue;
		}

		// 攻撃情報保存
		CMotion::AttackInfo atkInfo = *aInfo.AttackInfo[nCntAttack];

		if (m_pMotion->GetAllCount() == aInfo.AttackInfo[nCntAttack]->nInpactCnt)
		{// 衝撃のカウントと同じとき

			// 攻撃時処理
			AttackAction(atkInfo.nCollisionNum, atkInfo);
		}

		// モーションカウンター取得
		if (m_pMotion->GetAllCount() > aInfo.AttackInfo[nCntAttack]->nMinCnt && m_pMotion->GetAllCount() < aInfo.AttackInfo[nCntAttack]->nMaxCnt)
		{// 攻撃判定中

			// プレイヤー情報
			CPlayer *pPlayer = CManager::GetInstance()->GetScene()->GetPlayer();

			if (pPlayer == NULL)
			{// NULLだったら
				return;
			}

			// 敵の位置取得
			D3DXVECTOR3 TargetPos = pPlayer->GetPosition();

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
			D3DXVECTOR3 weponpos = D3DXVECTOR3(mtxWepon._41, mtxWepon._42, mtxWepon._43);
#if _DEBUG
			CEffect3D::Create(weponpos, D3DXVECTOR3(0.0f, 0.0f, 0.0f), D3DXCOLOR(1.0f, 0.0f, 0.0f, 1.0f), aInfo.AttackInfo[nCntAttack]->fRangeSize * 0.5f, 10, CEffect3D::MOVEEFFECT_NONE, CEffect3D::TYPE_NORMAL);
#endif
			// 判定サイズ取得
			float fRadius = pPlayer->GetObjectChara()->GetRadius();

			if (aInfo.AttackInfo[nCntAttack]->fRangeSize == 0.0f)
			{
				continue;
			}

			if (SphereRange(weponpos, TargetPos, aInfo.AttackInfo[nCntAttack]->fRangeSize, fRadius))
			{// 球の判定

#if 0
				int playerState = pPlayer->GetState();
				if (playerState != CPlayer::STATE_DMG &&
					playerState != CPlayer::STATE_KNOCKBACK &&
					playerState != CPlayer::STATE_DEAD &&
					playerState != CPlayer::STATE_INVINCIBLE)
				{
					D3DXVECTOR3 PlayerRot = pPlayer->GetRotation();

					// ターゲットと敵との向き
					float fRot = atan2f((pos.x - TargetPos.x), (pos.z - TargetPos.z));

					// 向きを正面にする
					fRot = D3DX_PI + fRot;

					// 角度の正規化
					RotNormalize(fRot);

					pPlayer->SetRotation(D3DXVECTOR3(PlayerRot.x, fRot, PlayerRot.z));
					fRot = pPlayer->GetRotation().y;

					// 吹き飛ばし
					pPlayer->SetMove(D3DXVECTOR3(
						sinf(fRot) * 4.0f,
						12.0f,
						cosf(fRot) * 4.0f));
				}
				if (pPlayer->Hit(aInfo.AttackInfo[nCntAttack]->nDamage) == true)
				{// 死んでたら

					my_particle::Create(TargetPos, my_particle::TYPE_OFFSETTING);

					// プレイヤー情報
					CPlayer *pPlayer = CManager::GetInstance()->GetScene()->GetPlayer();
					CPlayer **ppPlayer = &pPlayer;

					// プレイヤーをNULL
					ppPlayer = NULL;
					int m = 0;
				}
#endif
			}
		}
	}

	// 向き設定
	SetRotation(rot);
}

//==========================================================================
// 攻撃時処理
//==========================================================================
void CEnemy::AttackAction(int nModelNum, CMotion::AttackInfo ATKInfo)
{
	return;
}

//==========================================================================
// 描画処理
//==========================================================================
void CEnemy::Draw(void)
{
#if _DEBUG
	if (m_mMatcol != D3DXCOLOR(1.0f, 1.0f, 1.0f, m_mMatcol.a))
	{
		// オブジェクトキャラの描画
		CObjectChara::Draw(m_mMatcol);
	}
	else
	{
		// オブジェクトキャラの描画
		CObjectChara::Draw();
	}
#else
	//CObjectChara::Draw();

	if (m_mMatcol != D3DXCOLOR(1.0f, 1.0f, 1.0f, m_mMatcol.a))
	{
		// オブジェクトキャラの描画
		CObjectChara::Draw(m_mMatcol);
	}
	else
	{
		// オブジェクトキャラの描画
		CObjectChara::Draw();
	}
#endif
}


//==========================================================================
// マップインデックス番号の設定
//==========================================================================
void CEnemy::SetMapIndexOrigin(int nIdx)
{
	m_nMapIdxOrigin = nIdx;
}

//==========================================================================
// マップインデックス番号の取得
//==========================================================================
int CEnemy::GetMapIndexOrigin(void)
{
	return m_nMapIdxOrigin;
}

//==========================================================================
// マップポイント間の移動量設定
//==========================================================================
void CEnemy::SetMapMoveValueOrigin(float fValue)
{
	m_fMoveValueOrigin = fValue;
}

//==========================================================================
// マップポイント間の移動量設定
//==========================================================================
float CEnemy::GetMapMoveValueOrigin(void)
{
	return m_fMoveValueOrigin;
}

//==========================================================================
// 状態取得
//==========================================================================
int CEnemy::GetState(void)
{
	return m_state;
}

//==========================================================================
// 元の向き
//==========================================================================
void CEnemy::SetOriginRotation(D3DXVECTOR3 rot)
{
	m_rotOrigin = rot;
}

// スポーン地点設定
void CEnemy::SetSpawnPosition(D3DXVECTOR3 pos)
{
	m_posOrigin = pos;
}

//==========================================================================
// スポーン地点取得
//==========================================================================
D3DXVECTOR3 CEnemy::GetSpawnPosition(void)
{
	return m_posOrigin;
}

//==========================================================================
// 状態設定
//==========================================================================
void CEnemy::SetState(int state)
{
	m_state = (STATE)state;
}

//==========================================================================
// 状態設定
//==========================================================================
void CEnemy::SetState(int state, int nCntState)
{
	m_state = (STATE)state;
	m_nCntState = nCntState;
}

//==========================================================================
// 種類取得
//==========================================================================
int CEnemy::GetCharaType(void)
{
	return m_type;
}

//==========================================================================
// 敵の情報取得
//==========================================================================
CEnemy *CEnemy::GetEnemy(void)
{
	// 自分自身のポインタを取得
	return this;
}

//==========================================================================
// 一定の動きポインタ取得
//==========================================================================
CEnemyFixedMoveManager *CEnemy::GetFixedManager(void)
{
	return m_pFixedMoveManager;
}
