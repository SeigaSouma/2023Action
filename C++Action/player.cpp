//=============================================================================
// 
//  プレイヤー処理 [player.cpp]
//  Author : 相馬靜雅
// 
//=============================================================================
#include "game.h"
#include "player.h"
#include "camera.h"
#include "manager.h"
#include "debugproc.h"
#include "renderer.h"
#include "input.h"
#include "enemy.h"
#include "calculation.h"
#include "score.h"
#include "texture.h"
#include "Xload.h"
#include "model.h"
#include "motion.h"
#include "hp_gauge.h"
#include "objectChara.h"
#include "elevation.h"
#include "shadow.h"
#include "particle.h"
#include "3D_Effect.h"
#include "ballast.h"
#include "impactwave.h"
#include "sound.h"
#include "enemymanager.h"
#include "mapmanager.h"
#include "edit_controlpoint.h"
#include "bullet.h"
#include "slash.h"
#include "camerachasechanger.h"
#include "cameraaxis.h"
#include "stage.h"
#include "objectX.h"
#include "gamemanager.h"
#include "instantfade.h"

// 派生先
#include "tutorialplayer.h"

//==========================================================================
// マクロ定義
//==========================================================================
#define CHARAFILE		"data\\TEXT\\motion_set_player.txt"
#define JUMP			(20.0f * 1.5f)	// ジャンプ力初期値
#define MAX_LIFE		(100)			// 体力
#define TARGET_LEN		(400.0f)		// 目標までの距離
#define WALK_INT		(30)			// 歩行の時間
#define INVINCIBLE_INT	(2)				// 無敵の間隔
#define INVINCIBLE_TIME	(90)			// 無敵の時間
#define CONFUSIONTIME	(60 * 2)		// 混乱時間

//==========================================================================
// 静的メンバ変数宣言
//==========================================================================

//==========================================================================
// コンストラクタ
//==========================================================================
CPlayer::CPlayer(int nPriority) : CObjectChara(nPriority)
{
	// 値のクリア
	m_state = STATE_NONE;	// 状態
	m_Oldstate = m_state;	// 前回の状態
	m_atkRush = ATKRUSH_RIGHT;	// 連続アタックの種類
	m_mMatcol = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);	// マテリアルの色
	m_posKnokBack = mylib_const::DEFAULT_VECTOR3;	// ノックバックの位置
	m_KnokBackMove = mylib_const::DEFAULT_VECTOR3;	// ノックバックの移動量
	m_bLandOld = false;				// 過去の着地情報
	m_bJump = false;				// ジャンプ中かどうか
	m_sMotionFrag.bATK = false;		// モーションのフラグ
	m_sMotionFrag.bJump = false;		// モーションのフラグ
	m_sMotionFrag.bKnockBack = false;	// モーションのフラグ
	m_sMotionFrag.bMove = false;		// モーションのフラグ
	m_nAngle = 1;					// 向き
	m_nCntState = 0;				// 状態遷移カウンター
	m_nCntWalk = 0;					// 歩行カウンター
	m_nCntConfusion = 0;			// 混乱カウンター
	m_nTexIdx = 0;					// テクスチャのインデックス番号
	m_nIdxXFile = 0;				// Xファイルのインデックス番号
	m_fAtkStickRot = 0.0f;			// 攻撃時のスティック向き
	m_fBodyRot = 0.0f;				// 攻撃時の身体向き
	m_fTruePosY = 0.0f;				// 本当の補正後の位置
	m_bStick = false;				// スティック倒した判定
	m_StickAngle = ANGLE_RIGHT;		// スティックの向き
	m_OldStickAngle = ANGLE_RIGHT;	// 前回のスティックの向き
	m_pMotion = NULL;				// モーションの情報
	m_pShadow = NULL;				// 影の情報
	m_pTargetP = NULL;				// 目標の地点
	m_pHPGauge = NULL;				// HPゲージの情報
}

//==========================================================================
// デストラクタ
//==========================================================================
CPlayer::~CPlayer()
{

}

//==========================================================================
// 生成処理
//==========================================================================
CPlayer *CPlayer::Create(void)
{
	// 生成用のオブジェクト
	CPlayer *pPlayer = NULL;

	if (pPlayer == NULL)
	{// NULLだったら

		// メモリの確保
		switch (CManager::GetInstance()->GetMode())
		{
		case CScene::MODE_GAME:
			pPlayer = DEBUG_NEW CPlayer;
			break;

		case CScene::MODE_TUTORIAL:
			pPlayer = DEBUG_NEW CTutorialPlayer;
			break;

		default:
			return NULL;
			break;
		}

		if (pPlayer != NULL)
		{// メモリの確保が出来ていたら

			// 初期化処理
			pPlayer->Init();
		}

		return pPlayer;
	}

	return NULL;
}

//==========================================================================
// 初期化処理
//==========================================================================
HRESULT CPlayer::Init(void)
{
	// 種類の設定
	SetType(TYPE_PLAYER);

	m_state = STATE_NONE;	// 状態
	m_nCntState = 0;		// 状態遷移カウンター
	m_bLandOld = true;

	// キャラ作成
	HRESULT hr = SetCharacter(CHARAFILE);

	if (FAILED(hr))
	{// 失敗していたら
		return E_FAIL;
	}

	// モーションの生成処理
	m_pMotion = CMotion::Create(CHARAFILE);

	// オブジェクトキャラクターの情報取得
	CObjectChara *pObjChar = GetObjectChara();

	// モーションの設定
	m_pMotion->SetModel(pObjChar->GetModel(), pObjChar->GetNumModel(), CObjectChara::GetObjectChara());

	// 体力ゲージ
	//m_pHPGauge = CHP_Gauge::Create(120.0f, m_nLife);

	// 位置取得
	D3DXVECTOR3 pos = GetPosition();

	// 影の生成
	m_pShadow = CShadow::Create(pos, 50.0f);

	// ポーズのリセット
	m_pMotion->ResetPose(MOTION_DEF);
	//m_atkRush = ATKRUSH_LEFT;	// 連続アタックの種類
	//SetMapIndex(35);

	return S_OK;
}

//==========================================================================
// 終了処理
//==========================================================================
void CPlayer::Uninit(void)
{
	if (m_pMotion != NULL)
	{
		m_pMotion->Uninit();
		delete m_pMotion;
		m_pMotion = NULL;
	}

	// HPゲージを消す
	if (m_pHPGauge != NULL)
	{
		//m_pHPGauge->Uninit();
		m_pHPGauge = NULL;
	}

	// 影を消す
	if (m_pShadow != NULL)
	{
		//m_pShadow->Uninit();
		m_pShadow = NULL;
	}

	// 終了処理
	CObjectChara::Uninit();

	// モード別終了処理
	UninitByMode();
}

//==========================================================================
// モード別終了処理
//==========================================================================
void  CPlayer::UninitByMode(void)
{
	CScene *pScene = CManager::GetInstance()->GetScene();
	if (pScene != NULL)
	{
		// プレイヤー情報
		CPlayer *pPlayer = CManager::GetInstance()->GetScene()->GetPlayer();
		CPlayer **ppPlayer = &pPlayer;

		// プレイヤーをNULL
		*ppPlayer = NULL;
		int n = 0;
		CManager::GetInstance()->GetScene()->UninitPlayer();
	}
}

//==========================================================================
// 終了処理
//==========================================================================
void CPlayer::Kill(void)
{

	my_particle::Create(GetPosition(), my_particle::TYPE_OFFSETTING);

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
void CPlayer::Update(void)
{
	// キーボード情報取得
	CInputKeyboard *pInputKeyboard = CManager::GetInstance()->GetInputKeyboard();
	if (pInputKeyboard->GetTrigger(DIK_F5) == true)
	{// F5でリセット
		SetPosition(D3DXVECTOR3(0.0f, 0.0f, -1000.0f));
		SetMove(D3DXVECTOR3(0.0f, 0.0f, 0.0f));
	}

	// エディット中は抜ける
	if (CGame::GetElevation()->IsEdit())
	{
		return;
	}

	// エディット中は抜ける
	if (CGame::GetEditType() != CGame::EDITTYPE_OFF)
	{
		return;
	}

	// コンボ演出中は抜ける
	if (CGame::GetEnemyManager()->GetState() == CEnemyManager::STATE_COMBOANIM)
	{
		return;
	}

	// 過去の位置保存
	SetOldPosition(GetPosition());

	// 過去の移動量保存
	SetOldMapMoveValue(GetMapMoveValue());

	// 操作
	Controll();
	CollisionChaseChanger();

	// モーションの設定処理
	MotionSet();

	// モーションの更新処理
	if (m_pMotion != NULL)
	{
		m_pMotion->Update();

		if (m_pMotion->GetType() == MOTION_ATK || m_pMotion->GetType() == MOTION_ATK2)
		{
#if 0
			// モーションの情報取得
			CMotion::Info aInfo = m_pMotion->GetInfo(m_pMotion->GetType());

			CModel **ppModel = GetObjectChara()->GetModel();
			D3DXVECTOR3 bodyrot = mylib_const::DEFAULT_VECTOR3;
			D3DXVECTOR3 playerRot = mylib_const::DEFAULT_VECTOR3;

			// ゲームパッド情報取得
			CInputGamepad *pInputGamepad = CManager::GetInstance()->GetInputGamepad();

			// 移動方向取得
			CObject::ANGLE MoveAngle = GetMoveAngle();

			if (pInputGamepad->GetStickPositionRatioR(0).y >= 0.5f || pInputGamepad->GetStickPositionRatioR(0).y <= -0.5f ||
				pInputGamepad->GetStickPositionRatioR(0).x >= 0.5f || pInputGamepad->GetStickPositionRatioR(0).x <= -0.5f)
			{
#if 0
				switch (MoveAngle)
				{
				case CPlayer::ANGLE_UP:
					bodyrot.x = pInputGamepad->GetStickRotR(0);

					if (bodyrot.x > D3DX_PI * 0.15f)
					{
						MoveAngle = ANGLE_RIGHT;
						m_nAngle = 1;	// 向き
					}
					else if (bodyrot.x < D3DX_PI * -0.15f)
					{
						MoveAngle = ANGLE_LEFT;
						m_nAngle = -1;	// 向き
					}
					if (bodyrot.x > D3DX_PI * 0.85f || bodyrot.x < D3DX_PI * -0.85f)
					{
						MoveAngle = ANGLE_DOWN;
						bodyrot.x = 0.0f;
					}
					bodyrot.x = 0.0f;
					break;

				case CPlayer::ANGLE_RIGHT:
					bodyrot.x = -pInputGamepad->GetStickRotR(0);

					if (bodyrot.x <= D3DX_PI * -0.85f)
					{
						MoveAngle = ANGLE_DOWN;
					}
					else if (bodyrot.x > D3DX_PI * 0.15f && bodyrot.x < D3DX_PI * 0.85f)
					{
						MoveAngle = ANGLE_LEFT;
						m_nAngle = -1;	// 向き
					}
					else if (bodyrot.x <= D3DX_PI * -0.75f)
					{
						bodyrot.x = D3DX_PI * -0.75f;
					}
					else if (bodyrot.x >= D3DX_PI * -0.15f)
					{
						MoveAngle = ANGLE_UP;
						bodyrot.x = D3DX_PI * -0.15f;
					}

					bodyrot.x += D3DX_PI;
					bodyrot.x -= (D3DX_PI * 0.5f);
					break;

				case CPlayer::ANGLE_DOWN:

					bodyrot.x = pInputGamepad->GetStickRotR(0);

					if (bodyrot.x >= 0.0f && bodyrot.x <= D3DX_PI * 0.85f)
					{
						MoveAngle = ANGLE_RIGHT;
						m_nAngle = 1;	// 向き
					}
					else if (bodyrot.x < D3DX_PI * -0.85f)
					{
						MoveAngle = ANGLE_LEFT;
						m_nAngle = -1;	// 向き
					}
					if (bodyrot.x < D3DX_PI * 0.15f && bodyrot.x > D3DX_PI * -0.15f)
					{
						MoveAngle = ANGLE_UP;
						bodyrot.x = 0.0f;
					}
					bodyrot.x = 0.0f;

					break;

				case CPlayer::ANGLE_LEFT:
					bodyrot.x = pInputGamepad->GetStickRotR(0);

					if (bodyrot.x >= D3DX_PI * 0.85f)
					{
						MoveAngle = ANGLE_DOWN;
						bodyrot.x = D3DX_PI * -0.85f;
					}
					else if (bodyrot.x < D3DX_PI * 0.85f && bodyrot.x > 0.0f)
					{
						bodyrot.x = D3DX_PI * -0.75f;
						MoveAngle = ANGLE_RIGHT;
						m_nAngle = 1;	// 向き
					}
					else if (bodyrot.x <= D3DX_PI * -0.75f)
					{
						bodyrot.x = D3DX_PI * -0.75f;
					}
					else if (bodyrot.x >= D3DX_PI * -0.15f)
					{
						MoveAngle = ANGLE_UP;
						bodyrot.x = D3DX_PI * -0.15f;
					}

					bodyrot.x += D3DX_PI;
					bodyrot.x -= (D3DX_PI * 0.5f);
					break;

				default:
					break;
		}
#else
				bodyrot.x = m_fBodyRot;
#endif

				RotNormalize(bodyrot.x);
				ppModel[1]->SetRotation(ppModel[1]->GetRotation() + bodyrot);
	}

			// 移動方向設定
			SetMoveAngle(MoveAngle);
#endif
}
	}

	// 頂点情報設定
	SetVtx();

	// 攻撃処理
	Atack();

	// 状態更新
	UpdateState();

	// 位置取得
	D3DXVECTOR3 pos = GetPosition();

	// 移動量取得
	D3DXVECTOR3 move = GetMove();

	// 向き取得
	D3DXVECTOR3 rot = GetRotation();

	// 影の位置更新
	if (m_pShadow != NULL)
	{
		m_pShadow->SetPosition(D3DXVECTOR3(pos.x, m_pShadow->GetPosition().y, pos.z));
	}

	// HPゲージの位置更新
	if (m_pHPGauge != NULL)
	{
		m_pHPGauge->UpdatePosition(GetPosition(), GetLife());
	}

	// デバッグ表示
	CManager::GetInstance()->GetDebugProc()->Print(
		"------------------[プレイヤーの操作]------------------\n"
		"位置：【X：%f, Y：%f, Z：%f】 【W / A / S / D】\n"
		"向き：【X：%f, Y：%f, Z：%f】 【Z / C】\n"
		"移動量：【X：%f, Y：%f, Z：%f】\n", pos.x, pos.y, pos.z, rot.x, rot.y, rot.y, move.x, move.y, move.z);
}

//==========================================================================
// 操作処理
//==========================================================================
void CPlayer::Controll(void)
{
	// キーボード情報取得
	CInputKeyboard *pInputKeyboard = CManager::GetInstance()->GetInputKeyboard();

	// ゲームパッド情報取得
	CInputGamepad *pInputGamepad = CManager::GetInstance()->GetInputGamepad();

	// カメラの情報取得
	CCamera *pCamera = CManager::GetInstance()->GetCamera();

	// カメラの向き取得
	D3DXVECTOR3 Camerarot = pCamera->GetRotation();

	// 位置取得
	D3DXVECTOR3 pos = GetPosition();

	// 移動量取得
	D3DXVECTOR3 move = GetMove();

	// 向き取得
	D3DXVECTOR3 rot = GetRotation();

	// 目標の向き取得
	float fRotDest = GetRotDest();

	// 現在の種類取得
	int nMotionType = m_pMotion->GetType();

	// 移動量取得
	float fMove = GetVelocity();

	// 移動方向取得
	CObject::ANGLE MoveAngle = GetMoveAngle();
	SetOldMoveAngle(MoveAngle);

	if (m_pMotion->IsGetMove(nMotionType) == 1)
	{// 移動可能モーションの時

		// 移動中にする
		m_sMotionFrag.bMove = true;

		// スティックの向き
		float fStickRot = pInputGamepad->GetStickRotL(0);

		if (pInputKeyboard->GetPress(DIK_A) == true || pInputGamepad->GetStickMoveL(0).x < 0)
		{//←キーが押された,左移動

			// 移動中にする
			m_sMotionFrag.bMove = true;

			move.x -= fMove;

			// 左向き
			MoveAngle = ANGLE_LEFT;
			m_nAngle = -1;	// 向き
		}
		else if (pInputKeyboard->GetPress(DIK_D) == true || pInputGamepad->GetStickMoveL(0).x > 0)
		{//Dキーが押された,右移動

			// 移動中にする
			m_sMotionFrag.bMove = true;

			move.x += fMove;

			// 右向き
			MoveAngle = ANGLE_RIGHT;
			m_nAngle = 1;	// 向き
		}
		else if (pInputKeyboard->GetPress(DIK_W) == true || pInputGamepad->GetStickMoveL(0).y > 0)
		{//Wが押された、上移動

			// 移動中にする
			m_sMotionFrag.bMove = true;

			// 上向き
			MoveAngle = ANGLE_UP;
		}
		else if (pInputKeyboard->GetPress(DIK_S) == true || pInputGamepad->GetStickMoveL(0).y < 0)
		{//Sが押された、下移動

			// 移動中にする
			m_sMotionFrag.bMove = true;

			// 上向き
			MoveAngle = ANGLE_DOWN;
		}
		else
		{
			// 移動中かどうか
			m_sMotionFrag.bMove = false;
		}


		if (pInputGamepad->GetStickPositionRatioL(0).y >= 0.5f || pInputGamepad->GetStickPositionRatioL(0).y <= -0.5f ||
			pInputGamepad->GetStickPositionRatioL(0).x >= 0.5f || pInputGamepad->GetStickPositionRatioL(0).x <= -0.5f)
		{// 攻撃

			if ((fStickRot >= 0.0f && fStickRot < D3DX_PI * 0.25f) ||
				(fStickRot <= 0.0f && fStickRot > -D3DX_PI * 0.25f))
			{// 上方向
				MoveAngle = ANGLE_UP;
				m_nAngle = 1;	// 向き
			}
			else if (fStickRot >= D3DX_PI * 0.25f && fStickRot < D3DX_PI * 0.75f)
			{// 右方向
				MoveAngle = ANGLE_RIGHT;
				m_nAngle = 1;	// 向き
			}
			else if (
				fStickRot >= -D3DX_PI * 0.75f &&
				fStickRot < -D3DX_PI * 0.25f)
			{// 左方向
				MoveAngle = ANGLE_LEFT;
				m_nAngle = -1;	// 向き
			}
			else if (
				(fStickRot > D3DX_PI * 0.75f && fStickRot <= D3DX_PI) ||
				(fStickRot < -D3DX_PI * 0.75f && fStickRot >= -D3DX_PI))
			{// 下方向
				MoveAngle = ANGLE_DOWN;
			}
		}

		if (m_bJump == false &&
			(pInputKeyboard->GetTrigger(DIK_SPACE) == true || pInputGamepad->GetTrigger(CInputGamepad::BUTTON_LB, 0)))
		{//SPACEが押された,ジャンプ

			m_bJump = true;
			m_sMotionFrag.bJump = true;
			move.y += 17.0f;
		}
	}
	else
	{// 移動可能モーションじゃない場合

		if (pInputKeyboard->GetPress(DIK_A) == true || pInputGamepad->GetStickMoveL(0).x < 0)
		{//←キーが押された,左移動

			// 左向き
			MoveAngle = ANGLE_LEFT;
			m_nAngle = -1;	// 向き
		}
		else if (pInputKeyboard->GetPress(DIK_D) == true || pInputGamepad->GetStickMoveL(0).x > 0)
		{//Dキーが押された,右移動

			// 右向き
			MoveAngle = ANGLE_RIGHT;
			m_nAngle = 1;	// 向き
		}
		else if (pInputKeyboard->GetPress(DIK_W) == true || pInputGamepad->GetStickMoveL(0).y > 0)
		{//Wが押された、上移動

			// 上向き
			MoveAngle = ANGLE_UP;
		}
		else if (pInputKeyboard->GetPress(DIK_S) == true || pInputGamepad->GetStickMoveL(0).y < 0)
		{//Sが押された、下移動

			// 下向き
			MoveAngle = ANGLE_DOWN;
		}
	}

	// マップマネージャの取得
	CMapManager *pMapManager = CGame::GetMapManager();
	if (pMapManager == NULL)
	{// NULLだったら
		return;
	}

	float fTimeDest = 0.0f;
	D3DXVECTOR3 sakiPos;
	float fMoveValue = GetMapMoveValue();

	// 移動した量加算
	fMoveValue += move.x;

	// 曲線作る為の4点
	int nIdxMapPoint = GetMapIndex();

	// 位置の割合取得
	float fPointRatio = GetMapPointRatio();

	// 位置更新
	D3DXVECTOR3 newPosition = pos;
	newPosition = pMapManager->UpdateNowPosition(nIdxMapPoint, fPointRatio, fMoveValue, pos.y);

	// 目標地点
	float fDest = (fMoveValue + (fMove + GetRadius() * 0.5f) * m_nAngle);
	int nDestIdx = nIdxMapPoint;
	float fDestPointRatio = fPointRatio;
	sakiPos = pMapManager->UpdateNowPosition(nDestIdx, fDestPointRatio, fDest, pos.y);


	// 目標の角度を求める
	fRotDest = atan2f((newPosition.x - sakiPos.x), (newPosition.z - sakiPos.z));

	if (MoveAngle == ANGLE_DOWN)
	{
		fRotDest = atan2f((pos.x - pCamera->GetPositionV().x), (pos.z - pCamera->GetPositionV().z));
	}
	else if (MoveAngle == ANGLE_UP)
	{
		// カメラの情報取得
		CCamera *pCamera = CManager::GetInstance()->GetCamera();

		// カメラの追従種類取得
		CCamera::CHASETYPE CameraChaseType = pCamera->GetChaseType();

		if (CameraChaseType == CCamera::CHASETYPE_NORMAL)
		{
			fRotDest = atan2f((pos.x - pCamera->GetTargetPosition().x), (pos.z - pCamera->GetTargetPosition().z));
		}
		else
		{
			fRotDest = fRotDest - D3DX_PI * 0.5f;
		}

	}

	// 角度の正規化
	RotNormalize(fRotDest);

	// 現在と目標の差分を求める
	float fRotDiff = fRotDest - rot.y;

	// 角度の正規化
	RotNormalize(fRotDiff);

	// 角度の補正をする
	rot.y += fRotDiff * 0.15f;

	// 角度の正規化
	RotNormalize(rot.y);

	// 重力処理
	if (m_state != STATE_KNOCKBACK && m_state != STATE_DMG)
	{
		move.y -= mylib_const::GRAVITY;

		// 位置更新
		newPosition.y += move.y;
		sakiPos.y = newPosition.y;
	}

	//**********************************
	// 当たり判定
	//**********************************
	bool bLandStage = Collision(sakiPos, move);

	m_bHitStage;

	bool bMove = false;
	if (m_bLandOld == false && bLandStage == true)
	{// 前回は着地していなくて、今回は着地している場合

		bMove = false;
	}

	if (m_bLandOld == true && bLandStage == true)
	{// 前回も今回も着地している場合
		bMove = true;
	}

	// 過去の着地判定保存
	//m_bLandOld = bLandStage;

	if (m_bHitWall == false && 
		(bLandStage == false || bMove == true || m_bLandField == true || m_bJump == true || m_sMotionFrag.bKnockBack == true))
	{
		pos.x = newPosition.x;
		pos.z = newPosition.z;
		pos.y = sakiPos.y;
		Collision(pos, move);

		//if (m_bHitStage == true)
		{// ステージで移動する場合

			// 前回は乗ってたことにする
			m_bLandOld = true;
		}
	}
	else
	{
		D3DXVECTOR3 posOld = GetOldPosition();
		pos.x = posOld.x;
		pos.z = posOld.z;
		pos = posOld;
		move.x = 0.0f;
		fMoveValue = GetOldMapMoveValue();

		// 前回は乗ってなかったってことにする
		//m_bLandOld = false;
	}

	if (CGame::GetGameManager()->GetType() == CGameManager::SCENE_RUSH)
	{// 敵のラッシュ中

		// 情報取得
		D3DXVECTOR3 posMapIndex = pMapManager->GetTargetPosition(37, 0.0f);

		if (CircleRange(pos, posMapIndex, GetRadius(), 900.0f) == false)
		{// 円から外れたら

			D3DXVECTOR3 posOld = GetOldPosition();
			pos.x = posOld.x;
			pos.z = posOld.z;
			pos = posOld;
			move.x = 0.0f;
			fMoveValue = GetOldMapMoveValue();
		}
	}

	// 位置更新
	//pos.y += move.y;

	// 慣性補正
	if (m_state != STATE_KNOCKBACK && m_state != STATE_DMG)
	{
		move.x += (0.0f - move.x) * 0.25f;
		move.z += (0.0f - move.z) * 0.25f;
	}

	// 位置設定
	SetPosition(pos);

	// 移動量設定
	SetMove(move);

	// 向き設定
	SetRotation(rot);

	// 目標の向き設定
	SetRotDest(fRotDest);

	// マップのインデックス番号設定
	SetMapIndex(nIdxMapPoint);

	// マップポイント間の割合設定
	SetMapPointRatio(fPointRatio);

	// マップポイント間の移動量設定
	SetMapMoveValue(fMoveValue);

	// 移動方向設定
	SetMoveAngle(MoveAngle);


	if (pInputGamepad->GetStickPositionRatioR(0).y <= 0.5f && pInputGamepad->GetStickPositionRatioR(0).y >= -0.5f &&
		pInputGamepad->GetStickPositionRatioR(0).x <= 0.5f && pInputGamepad->GetStickPositionRatioR(0).x >= -0.5f)
	{

		// スティック倒した判定
		m_bStick = false;
	}
	
	if (/*m_sMotionFrag.bATK == false &&*/
		(
			pInputGamepad->GetStickPositionRatioR(0).y >= 0.5f || pInputGamepad->GetStickPositionRatioR(0).y <= -0.5f ||
			pInputGamepad->GetStickPositionRatioR(0).x >= 0.5f || pInputGamepad->GetStickPositionRatioR(0).x <= -0.5f))
	{// 攻撃

		if (pInputGamepad->GetStickPositionRatioR(0).x >= 0.2f)
		{
			m_StickAngle = ANGLE_RIGHT;		// スティックの向き
		}
		else if (pInputGamepad->GetStickPositionRatioR(0).x <= -0.2f)
		{
			m_StickAngle = ANGLE_LEFT;		// スティックの向き
		}
		else if (pInputGamepad->GetStickPositionRatioR(0).y >= 0.2f)
		{
			m_StickAngle = ANGLE_UP;		// スティックの向き
		}
		else if (pInputGamepad->GetStickPositionRatioR(0).y <= -0.2f)
		{
			m_StickAngle = ANGLE_DOWN;		// スティックの向き
		}

		// 攻撃判定ON
		if (m_StickAngle != m_OldStickAngle)
		{
			// スティックの向き
			m_fAtkStickRot = pInputGamepad->GetStickRotR(0);

			switch (MoveAngle)
			{
			case CPlayer::ANGLE_UP:

				if (m_fAtkStickRot <= 0.0f)
				{// 左半分
					m_fAtkStickRot *= -1;
				}
				m_fAtkStickRot -= D3DX_PI * 0.5f;	// 半周分
				break;

			case CPlayer::ANGLE_RIGHT:
				m_fAtkStickRot -= D3DX_PI * 0.5f;	// 半周分
				break;

			case CPlayer::ANGLE_DOWN:

				if (m_fAtkStickRot >= 0.0f)
				{// 右半分
					m_fAtkStickRot *= -1;
					m_fAtkStickRot += D3DX_PI * 0.5f;
				}
				else
				{
					m_fAtkStickRot -= D3DX_PI * 0.5f;	// 半周分
				}
				break;

			case CPlayer::ANGLE_LEFT:
				m_fAtkStickRot -= D3DX_PI * 0.5f;	// 半周分
				break;

			default:
				break;
			}
			RotNormalize(m_fAtkStickRot);

			int nType = m_pMotion->GetType();
			if (nType == MOTION_JUMP || nType == MOTION_FALL)
			{
				m_pMotion->ToggleFinish(true);
			}

			m_sMotionFrag.bJump = false;
			m_sMotionFrag.bATK = true;

		}

		// スティック倒した判定
		m_bStick = true;

		m_OldStickAngle = m_StickAngle;	// 前回のスティックの向き
	}
	else
	{
		//m_atkRush = ATKRUSH_LEFT;
		m_OldStickAngle = ANGLE_MAX;

		// スティック倒した判定
		m_bStick = false;
	}
}

//==========================================================================
// モーションの設定
//==========================================================================
void CPlayer::MotionSet(void)
{
	if (m_pMotion == NULL)
	{// モーションがNULLだったら
		return;
	}

	if (m_pMotion->IsFinish() == true)
	{// 終了していたら

		// 現在の種類取得
		int nType = m_pMotion->GetType();
		int nOldType = m_pMotion->GetOldType();

		if (m_sMotionFrag.bMove == true && m_sMotionFrag.bATK == false && m_sMotionFrag.bKnockBack == false && m_bJump == false)
		{// 移動していたら

			m_sMotionFrag.bMove = false;	// 移動判定OFF

			// 移動モーション
			m_pMotion->Set(MOTION_WALK);
		}
		else if (m_sMotionFrag.bJump == true && m_sMotionFrag.bATK == false && m_sMotionFrag.bKnockBack == false)
		{// ジャンプ中

			// ジャンプのフラグOFF
			m_sMotionFrag.bJump = false;

			// ジャンプモーション
			m_pMotion->Set(MOTION_JUMP);
		}
		else if (m_bJump == true && m_sMotionFrag.bJump == false && m_sMotionFrag.bATK == false && m_sMotionFrag.bKnockBack == false)
		{// ジャンプ中&&ジャンプモーションが終わってる時

			// 落下モーション
			m_pMotion->Set(MOTION_FALL);
		}
		else if (m_sMotionFrag.bKnockBack == true)
		{// やられ中だったら

			// やられモーション
			m_pMotion->Set(MOTION_KNOCKBACK);
		}
		else if (m_sMotionFrag.bATK == true)
		{// 攻撃していたら

			m_sMotionFrag.bATK = false;		// 攻撃判定OFF

			// 連続アタックの種類
			m_atkRush = (ATKRUSH)(((int)m_atkRush + 1) % (int)ATKRUSH_MAX);

			int nType = MOTION_ATK + m_atkRush;
			m_pMotion->Set(nType, false);
		}
		else
		{
			// ニュートラルモーション
			m_pMotion->Set(MOTION_DEF);
		}
	}
}

//==========================================================================
// 攻撃
//==========================================================================
void CPlayer::Atack(void)
{
	if (m_pMotion == NULL)
	{// モーションがNULLだったら
		return;
	}

	// 位置取得
	D3DXVECTOR3 pos = GetPosition();

	// 向き取得
	D3DXVECTOR3 rot = GetRotation();

	// カメラの情報取得
	CCamera *pCamera = CManager::GetInstance()->GetCamera();

	// カメラの向き取得
	D3DXVECTOR3 Camerarot = pCamera->GetRotation();

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

		if (m_pMotion->GetAllCount() == aInfo.AttackInfo[nCntAttack]->nInpactCnt)
		{// 衝撃のカウントと同じとき

			D3DXMATRIX mtxTrans;	// 計算用マトリックス宣言

			if (GetObjectChara()->GetModel()[aInfo.AttackInfo[nCntAttack]->nCollisionNum] == NULL)
			{// NULLだったら
				return;
			}

			// 判定するパーツのマトリックス取得
			D3DXMATRIX mtxWepon = GetObjectChara()->GetModel()[aInfo.AttackInfo[nCntAttack]->nCollisionNum]->GetWorldMtx();

			// 位置を反映する
			D3DXMatrixTranslation(&mtxTrans, aInfo.AttackInfo[nCntAttack]->Offset.x, aInfo.AttackInfo[nCntAttack]->Offset.y, aInfo.AttackInfo[nCntAttack]->Offset.z);
			D3DXMatrixMultiply(&mtxWepon, &mtxTrans, &mtxWepon);

			// 武器の位置
			D3DXVECTOR3 weponpos = D3DXVECTOR3(mtxWepon._41, mtxWepon._42, mtxWepon._43);

			// 種類別
			switch (m_pMotion->GetType())
			{
			case MOTION_ATK:
			case MOTION_ATK2:
				//// パーティクル生成
				//my_particle::Create(weponpos, my_particle::TYPE_SUPERATTACK);

				//// チャージカウントリセット
				////CGame::GetPowerGauge()->SetChargeCount(0);

				//// 衝撃波生成
				//CImpactWave::Create
				//(
				//	D3DXVECTOR3(pos.x, pos.y + 80.0f, pos.z),	// 位置
				//	D3DXVECTOR3(0.0f, 0.0f, 0.0f),				// 向き
				//	D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.5f),			// 色
				//	100.0f,										// 幅
				//	20.0f,										// 高さ
				//	20,											// 寿命
				//	28.0f,										// 幅の移動量
				//	CImpactWave::TYPE_BLACK2,					// テクスチャタイプ
				//	true										// 加算合成するか
				//);

				//CImpactWave::Create
				//(
				//	D3DXVECTOR3(pos.x, pos.y + 150.0f, pos.z),	// 位置
				//	D3DXVECTOR3(0.0f, 0.0f, D3DX_PI),				// 向き
				//	D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.4f),			// 色
				//	180.0f,										// 幅
				//	150.0f,										// 高さ
				//	14,											// 寿命
				//	4.0f,										// 幅の移動量
				//	CImpactWave::TYPE_GIZAWHITE,				// テクスチャタイプ
				//	false										// 加算合成するか
				//);

				// 振動
				//CManager::GetInstance()->GetCamera()->SetShake(20, 10.0f, 0.0f);

				float fRotY = rot.y;
				int nA = 1;
				if (GetMoveAngle() == ANGLE_LEFT)
				{
					nA = -1;
					fRotY += D3DX_PI;
				}

				// 斬撃生成
				//CSlash::Create
				//(
				//	D3DXVECTOR3(pos.x, pos.y + 50.0f, pos.z),	// 位置
				//	D3DXVECTOR3(0.0f, 0.0f, 0.0f),		// 向き
				//	D3DXVECTOR3(m_fAtkStickRot, D3DX_PI + fRotY, 0.0f),		// 向き
				//	D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f),	// 色
				//	200.0f,								// 幅
				//	50.0f,								// 中心からの間隔
				//	10,									// 寿命
				//	40.0f,								// 幅の移動量
				//	CImpactWave::TYPE_PURPLE4,			// テクスチャの種類
				//	true,								// 加算合成するかどうか
				//	GetMoveAngle()
				//);
				CSlash *pSlash = CSlash::Create
				(
					D3DXVECTOR3(pos.x, pos.y + 50.0f, pos.z),	// 位置
					D3DXVECTOR3(0.0f, 0.0f, 0.0f),		// 向き
					D3DXVECTOR3(m_fAtkStickRot, D3DX_PI + fRotY, 0.0f),		// 向き
					D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f),	// 色
					200.0f,								// 幅
					50.0f,								// 中心からの間隔
					10,									// 寿命
					40.0f,								// 幅の移動量
					CImpactWave::TYPE_PURPLE4,			// テクスチャの種類
					true,								// 加算合成するかどうか
					GetMoveAngle()
				);
				pSlash->SetMapIndex(GetMapIndex());
				pSlash->SetMapMoveValue(GetMapMoveValue());

				// 歩行音再生
				CManager::GetInstance()->GetSound()->PlaySound(CSound::LABEL_SE_IMPACT01);
				break;
			}
		}

		// モーションカウンター取得
		if (m_pMotion->GetAllCount() > aInfo.AttackInfo[nCntAttack]->nMinCnt && m_pMotion->GetAllCount() < aInfo.AttackInfo[nCntAttack]->nMaxCnt)
		{// 攻撃判定中

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

			CEffect3D::Create(weponpos, D3DXVECTOR3(0.0f, 0.0f, 0.0f), D3DXCOLOR(1.0f, 0.0f, 0.0f, 1.0f), aInfo.AttackInfo[nCntAttack]->fRangeSize * 0.5f, 10, CEffect3D::MOVEEFFECT_NONE, CEffect3D::TYPE_NORMAL);

#if TOPCUR

			// 先頭を保存
			CObject *pObj = CObject::GetTop(mylib_const::ENEMY_PRIORITY);

			while (pObj != NULL)
			{// NULLが来るまで無限ループ

				// 次のオブジェクトを一時保存
				CObject *pObjNext = pObj->GetNext();

				// 死亡の判定
				if (pObj->IsDeath() == true)
				{// 死亡フラグが立っていたら

					// 次のオブジェクトを代入
					pObj = pObjNext;
					continue;
				}

				// 種類の取得
				if (pObj->GetType() != CObject::TYPE_ENEMY)
				{// 敵じゃなかったら

					// 次のオブジェクトを代入
					pObj = pObjNext;
					continue;
				}

				// 敵の位置取得
				D3DXMATRIX mtxOrigin = pObj->GetObjectChara()->GetModel()[0]->GetWorldMtx();
				D3DXVECTOR3 TargetPos = D3DXVECTOR3(mtxOrigin._41, mtxOrigin._42, mtxOrigin._43);

				// 判定サイズ取得
				float fRadius = pObj->GetObjectChara()->GetRadius();

				if (SphereRange(weponpos, TargetPos, aInfo.AttackInfo[nCntAttack]->fRangeSize, fRadius))
				{// 球の判定

					if (pObj->Hit(aInfo.AttackInfo[nCntAttack]->nDamage) == true)
					{// 死んでたら

						my_particle::Create(TargetPos, my_particle::TYPE_OFFSETTING);
					}
				}

				// 次のオブジェクトを代入
				pObj = pObjNext;
			}
#endif
		}
	}

	CManager::GetInstance()->GetDebugProc()->Print(
		"モーションカウンター：%d\n", m_pMotion->GetAllCount());
}

//==========================================================================
// 当たり判定
//==========================================================================
bool CPlayer::Collision(D3DXVECTOR3 &pos, D3DXVECTOR3 &move)
{

	// 向き取得
	D3DXVECTOR3 rot = GetRotation();

	// 着地したかどうか
	bool bLand = false;
	float fHeight = 0.0f;
	m_bLandField = false;
	m_bHitWall = false;			// 壁の当たり判定

	// 高さ取得
	if (m_state != STATE_KNOCKBACK && m_state != STATE_DMG)
	{
		fHeight = CManager::GetInstance()->GetScene()->GetElevation()->GetHeight(pos, bLand);
	}
	else
	{
		fHeight = pos.y;
	}

	if (fHeight > pos.y)
	{// 地面の方が自分より高かったら

		// 地面の高さに補正
		pos.y = fHeight;
		m_bLandField = true;

		if (bLand == true)
		{// 着地してたら

			// ジャンプ使用可能にする
			m_bJump = false;
			move.y = 0.0f;
			m_bLandOld = false;
		}
	}


	// Xファイルとの判定
	CStage *pStage = CGame::GetStage();
	if (pStage == NULL)
	{// NULLだったら
		return false;
	}

	bool bNowLand = false;

	// ステージに当たった判定
	m_bHitStage = false;
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
		fHeight = pObjX->GetHeight(pos, bLand);

		if (bLand == true && fHeight > pos.y)
		{// 地面の方が自分より高かったら

			// 地面の高さに補正
			if (pos.y + 50.0f <= fHeight)
			{// 自分より壁が高すぎる
				m_bHitWall = true;
			}
			else
			{
				pos.y = fHeight;
			}

			m_bHitStage = true;
			m_bLandField = false;

			if (bLand == true)
			{// 着地してたら

				if ((m_sMotionFrag.bKnockBack || m_bJump == true) && GetPosition().y  >= fHeight)
				{
					m_bLandOld = true;
				}

				if (m_bJump == true)
				{// ジャンプ中だったら
					m_pMotion->ToggleFinish(true);
				}

				// ジャンプ使用可能にする
				m_bJump = false;
				move.y = 0.0f;
				bNowLand = true;
				m_sMotionFrag.bJump = false;
			}
		}
	}

	// 過去の着地判定保存
	//m_bLandOld = bNowLand;



	// 位置取得
	D3DXVECTOR3 posOld = GetPosition();

	// 箱
	float fLen = CGame::GetElevation()->GetWidthLen();
	int nBlock = CGame::GetElevation()->GetWidthBlock();
	nBlock /= 2;
	if (pos.x + GetRadius() >= fLen * nBlock) { pos.x = fLen * nBlock - GetRadius(); }
	if (pos.x - GetRadius() <= -fLen * nBlock) { pos.x = -fLen * nBlock + GetRadius(); }
	if (pos.z + GetRadius() >= fLen * nBlock) { pos.z = fLen * nBlock - GetRadius(); }
	if (pos.z - GetRadius() <= -fLen * nBlock) { pos.z = -fLen * nBlock + GetRadius(); }

	// 向き設定
	SetRotation(rot);

	// 位置設定
	//SetPosition(pos);

	return bNowLand;
}

//==========================================================================
// 追従の変更者との判定
//==========================================================================
void CPlayer::CollisionChaseChanger(void)
{
	// 追従の変更者取得
	CCameraChaseChanger *pCameraChaseChanger = CGame::GetCameraChaseChanger();
	if (pCameraChaseChanger == NULL)
	{// NULLだったら
		return;
	}

	// マップマネージャの取得
	CMapManager *pMapManager = CGame::GetMapManager();
	if (pMapManager == NULL)
	{// NULLだったら
		return;
	}

	// カメラの情報取得
	CCamera *pCamera = CManager::GetInstance()->GetCamera();

	if (CGame::GetGameManager()->GetType() == CGameManager::SCENE_BOSS)
	{// ボス中
		pCamera->SetChaseType(CCamera::CHASETYPE_NORMAL);
		pCamera->SetTargetPosition(D3DXVECTOR3(0.0f, 0.0f, 0.0f));
		return;
	}

	// 情報取得
	CCameraChaseChanger::sChaseChangeInfo ChaseChangerInfo;
	CCamera::CHASETYPE CameraChaseType = CCamera::CHASETYPE_MAP;

	// 位置取得
	D3DXVECTOR3 pos = GetPosition();
	int nMapIdx = GetMapIndex();
	int nChangeNumAll = pCameraChaseChanger->GetNumAll();

	if (nMapIdx >= 32 && CGame::GetGameManager()->IsRushEnd() == false)
	{// 敵ラッシュの時

		// 情報取得
		D3DXVECTOR3 posMapIndex = pMapManager->GetTargetPosition(37, 0.0f);

		// 円の判定
		if (CircleRange(pos, posMapIndex, GetRadius(), 900.0f))
		{
			// 追従の種類設定
			CGame::GetGameManager()->SetType(CGameManager::SCENE_RUSH);
			pCamera->SetChaseType(CCamera::CHASETYPE_NONE);
			pCamera->SetTargetPosition(D3DXVECTOR3(posMapIndex.x, posMapIndex.y + 150.0f, posMapIndex.z));
			pCamera->SetLenDest(pCamera->GetOriginDistance() + 500.0f);
			return;
		}
	}

	if (nMapIdx >= 39 && CGame::GetGameManager()->GetType() != CGameManager::SCENE_TRANSITION)
	{// 最後の洞穴の近く

		// 情報取得
		D3DXVECTOR3 posMapIndex = pMapManager->GetTargetPosition(40, 0.0f);

		// 円の判定
		if (CircleRange(pos, posMapIndex, GetRadius(), 400.0f))
		{
			// 追従の種類設定
			CGame::GetGameManager()->SetType(CGameManager::SCENE_TRANSITIONWAIT);
			pCamera->SetChaseType(CCamera::CHASETYPE_NONE);
			pCamera->SetTargetPosition(D3DXVECTOR3(posMapIndex.x, posMapIndex.y + 150.0f, posMapIndex.z));
			pCamera->SetLenDest(pCamera->GetOriginDistance() - 200.0f);

			if (CircleRange(pos, posMapIndex, GetRadius(), 300.0f))
			{// 更に中央

				// ゲームパッド情報取得
				CInputGamepad *pInputGamepad = CManager::GetInstance()->GetInputGamepad();

				if (pInputGamepad->GetStickPositionRatioL(0).y >= 0.5f)
				{// 奥に進む
					CGame::GetGameManager()->SetType(CGameManager::SCENE_TRANSITION);
					CManager::GetInstance()->GetInstantFade()->SetFade();
				}
			}
			return;
		}
	}

	for (int i = 0; i < nChangeNumAll + 1; i++)
	{
		// 情報取得
		ChaseChangerInfo = pCameraChaseChanger->GetChaseChangeInfo(i);

		if (nMapIdx == ChaseChangerInfo.nMapIdx && i != nChangeNumAll)
		{// 一緒な場合
			if (pMapManager->GetTargetAngle(nMapIdx, ChaseChangerInfo.nMapIdx, GetMapMoveValue(), ChaseChangerInfo.fMapMoveValue) == CObject::ANGLE_LEFT)
			{// 左にいたら
				continue;
			}
		}
		else if (nMapIdx > ChaseChangerInfo.nMapIdx && i != nChangeNumAll)
		{// 自分のマップインデックスより小さい場合 && 終端じゃない
			continue;
		}
		else if (i == nChangeNumAll)
		{// 最後
			ChaseChangerInfo.chaseType = CCamera::CHASETYPE_NORMAL;
		}

		// 種類別処理
		switch (ChaseChangerInfo.chaseType)
		{
		case CCamera::CHASETYPE_NORMAL:
		
			CameraChaseType = CCamera::CHASETYPE_MAP;
			pCamera->SetChaseType(CCamera::CHASETYPE_MAP);
		
			break;

		case CCamera::CHASETYPE_MAP:
			pCamera->SetChaseType(CCamera::CHASETYPE_NORMAL);
			D3DXVECTOR3 AxisPos = CGame::GetCameraAxis()->GetAxis(ChaseChangerInfo.nByTypeIdx);
			pCamera->SetTargetPosition(AxisPos);
			CameraChaseType = CCamera::CHASETYPE_NORMAL;
			break;
		}

		break;
	}

	// デバッグ表示
	CManager::GetInstance()->GetDebugProc()->Print(
		"------------------[マップの追従]------------------\n"
		"種類：【%d】\n", CameraChaseType);

}

//==========================================================================
// ヒット処理
//==========================================================================
bool CPlayer::Hit(const int nValue)
{
	// 体力取得
	int nLife = GetLife();

	if (m_state != STATE_DEAD && m_state != STATE_DMG && m_state != STATE_KNOCKBACK && m_state != STATE_INVINCIBLE)
	{// ダメージ受付状態の時

		// 体力減らす
		nLife -= nValue;

		m_KnokBackMove.y += 18.0f;
		m_bHitStage = false;

		// 体力設定
		SetLife(nLife);

		if (nLife <= 0)
		{// 体力がなくなったら

			// 死状態
			m_state = STATE_DEAD;

			// 体力設定
			SetLife(0);

			// 死亡処理
			Kill();

			// 終了処理
			Uninit();

			// 死んだ
			return true;
		}

		// 過去の状態保存
		m_Oldstate = m_state;

		// ダメージ状態にする
		m_state = STATE_DMG;

		// 遷移カウンター設定
		m_nCntState = 0;

		// ノックバックの位置更新
		D3DXVECTOR3 pos = GetPosition();
		D3DXVECTOR3 rot = GetRotation();
		m_posKnokBack = pos;

		// ノックバック判定にする
		m_sMotionFrag.bKnockBack = true;

		// やられモーション
		m_pMotion->Set(MOTION_KNOCKBACK);

		// 衝撃波生成
		CImpactWave::Create
		(
			D3DXVECTOR3(pos.x, pos.y + 80.0f, pos.z),	// 位置
			D3DXVECTOR3(D3DX_PI * 0.5f, D3DX_PI + rot.y, D3DX_PI),				// 向き
			D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.3f),			// 色
			80.0f,										// 幅
			80.0f,										// 高さ
			20,											// 寿命
			10.0f,										// 幅の移動量
			CImpactWave::TYPE_GIZAWHITE,				// テクスチャタイプ
			false										// 加算合成するか
		);

		CManager::GetInstance()->SetEnableHitStop(12);

		// 振動
		CManager::GetInstance()->GetCamera()->SetShake(12, 25.0f, 0.0f);

		// サウンド再生
		CManager::GetInstance()->GetSound()->PlaySound(CSound::LABEL_SE_PLAYERDMG);
	}

	// 死んでない
	return false;
}

//==========================================================================
// 状態更新処理
//==========================================================================
void CPlayer::UpdateState(void)
{
	switch (m_state)
	{
	case STATE_NONE:
		// 色設定
		m_mMatcol = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
		break;

	case STATE_INVINCIBLE:
		Invincible();
		break;

	case STATE_DMG:
		Damage();
		break;

	case STATE_KNOCKBACK:
		KnockBack();
		break;
	}
}

//==========================================================================
// 無敵
//==========================================================================
void CPlayer::Invincible(void)
{
	// 状態遷移カウンター減算
	m_nCntState--;

	if (m_nCntState % INVINCIBLE_INT == 0)
	{// 規定間隔
		
		// 色設定
		if (m_mMatcol.a == 1.0f)
		{
			m_mMatcol.a = 0.4f;
		}
		else
		{
			m_mMatcol.a = 1.0f;
		}
	}

	if (m_nCntState <= 0)
	{// 遷移カウンターが0になったら

		// なにもない状態にする
		m_state = STATE_NONE;
		m_nCntState = 0;

		// 色設定
		m_mMatcol = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	}

}

//==========================================================================
// ダメージ
//==========================================================================
void CPlayer::Damage(void)
{
	// 位置取得
	D3DXVECTOR3 pos = GetPosition();

	// 移動量取得
	D3DXVECTOR3 move = GetMove();

	// 向き取得
	D3DXVECTOR3 rot = GetRotation();

	// 状態遷移カウンター減算
	m_nCntState++;

	// 色設定
	m_mMatcol = D3DXCOLOR(1.0f, 0.0f, 0.0f, 1.0f);

	// 位置更新
	pos.y = (-0.4f * (float)(m_nCntState * m_nCntState) + m_KnokBackMove.y * (float)m_nCntState) + m_posKnokBack.y;
	/*pos.x += move.x;
	pos.z += move.z;*/

	// 起伏との判定
	if (m_bHitStage && m_nCntState >= 10)
	{// 地面と当たっていたら
		m_state = STATE_INVINCIBLE;
		m_nCntState = INVINCIBLE_TIME;
		m_KnokBackMove.y = 0.0f;	// 移動量ゼロ
		m_bLandOld = true;

		// 色設定
		m_mMatcol = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);

		// ノックバック判定消す
		m_sMotionFrag.bKnockBack = false;
		m_pMotion->ToggleFinish(true);


		// Xファイルとの判定
		CStage *pStage = CGame::GetStage();
		if (pStage == NULL)
		{// NULLだったら
			return;
		}

		// ステージに当たった判定
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
			pos.y = pObjX->GetHeight(pos, bLand);
		}


	}
	else if (m_nCntState >= 30)
	{// 遷移カウンターが30になったら

		// ノックバック状態にする
		m_state = STATE_KNOCKBACK;
	}


	// 位置設定
	SetPosition(pos);

	// 移動量設定
	SetMove(move);

	// 向き設定
	SetRotation(rot);
}

//==========================================================================
// ノックバック
//==========================================================================
void CPlayer::KnockBack(void)
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
	m_nCntState++;

	// 位置更新
	pos.y = (-0.4f * (float)(m_nCntState * m_nCntState) + m_KnokBackMove.y * (float)m_nCntState) + m_posKnokBack.y;
	/*pos.x += move.x;
	pos.z += move.z;*/

	// 起伏との判定
	if (m_bHitStage)
	{// 地面と当たっていたら
		m_state = STATE_INVINCIBLE;
		m_nCntState = INVINCIBLE_TIME;
		m_KnokBackMove.y = 0.0f;	// 移動量ゼロ
		m_bLandOld = true;

		// 色設定
		m_mMatcol = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
		
		// ノックバック判定消す
		m_sMotionFrag.bKnockBack = false;
		m_pMotion->ToggleFinish(true);

		// Xファイルとの判定
		CStage *pStage = CGame::GetStage();
		if (pStage == NULL)
		{// NULLだったら
			return;
		}

		// ステージに当たった判定
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
			pos.y = pObjX->GetHeight(pos, bLand);
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
}

//==========================================================================
// 描画処理
//==========================================================================
void CPlayer::Draw(void)
{
#if _DEBUG
	CObjectChara::Draw();
#else
	CObjectChara::Draw();
#endif
}

//==========================================================================
// 状態取得
//==========================================================================
int CPlayer::GetState(void)
{
	return m_state;
}