//=============================================================================
// 
//  ゲーム処理 [game.cpp]
//  Author : 相馬靜雅
// 
//=============================================================================
#include "manager.h"
#include "game.h"
#include "renderer.h"
#include "calculation.h"
#include "debugproc.h"
#include "fade.h"
#include "camera.h"

#include "input.h"
#include "player.h"
#include "enemy.h"
#include "score.h"
#include "timer.h"
#include "map.h"
#include "elevation.h"
#include "sound.h"
#include "edit_controlpoint.h"
#include "edit_cameraaxis.h"
#include "edit_camerachasechanger.h"
#include "edit_enemybase.h"
#include "bulletmanager.h"
#include "stage.h"
#include "gamemanager.h"
#include "hitscore.h"

#include "enemymanager.h"
#include "mapmanager.h"
#include "player.h"
#include "cameraaxis.h"
#include "camerachasechanger.h"
#include "enemybase.h"

//==========================================================================
// 静的メンバ変数宣言
//==========================================================================
CScore *CGame::m_pScore = NULL;					// スコアのオブジェクト
CTimer *CGame::m_pTimer = NULL;					// タイマーのオブジェクト
CPowerGauge *CGame::m_pPowerGauge = NULL;		// パワーゲージのオブジェクト
CEditControlPoint *CGame::m_pEditControlPoint = NULL;	// 制御点エディターのオブジェクト
CBulletManager *CGame::m_pBulletManager = NULL;		// 弾マネージャのオブジェクト
CEditCameraAxis *CGame::m_pEditCameraAxis = NULL;		// カメラ軸エディターのオブジェクト
CEditCameraChaseChanger *CGame::m_pEditCmaeraChaseChanger = NULL;	// カメラ追従変更者エディターのオブジェクト
CEditEnemyBase *CGame::m_pEditEnemyBase = NULL;		// 敵の拠点エディター
CStage *CGame::m_pStage = NULL;						// ステージのオブジェクト
CGameManager *CGame::m_pGameManager = NULL;			// ゲームマネージャのオブジェクト
CGame::EEditType CGame::m_EditType = EDITTYPE_OFF;		// エディットの種類

CEnemyManager *CGame::m_pEnemyManager = NULL;	// 敵マネージャのオブジェクト
CMapManager *CGame::m_pMapManager = NULL;		// マップマネージャのオブジェクト
CCameraAxis *CGame::m_pCameraAxis = NULL;		// カメラの軸のオブジェクト
CCameraChaseChanger *CGame::m_pCameraChaseChanger = NULL;	// カメラ追従変更者のオブジェクト
CEnemyBase *CGame::m_pEnemyBase = NULL;			// 敵の拠点
CHitScore *CGame::m_pHitScore = NULL;			// ヒットスコア

//==========================================================================
// コンストラクタ
//==========================================================================
CGame::CGame()
{
	// 値のクリア
}

//==========================================================================
// デストラクタ
//==========================================================================
CGame::~CGame()
{

}

//==========================================================================
// 初期化処理
//==========================================================================
HRESULT CGame::Init(void)
{

	// 初期化処理
	if (FAILED(CScene::Init()))
	{// 失敗した場合
		return E_FAIL;
	}

	//**********************************
	// ゲームマネージャ
	//**********************************
	m_pGameManager = CGameManager::Create();

	//**********************************
	// マップマネージャ
	//**********************************
	m_pMapManager = CMapManager::Create("data\\BIN\\maptarget.bin");

	if (m_pMapManager == NULL)
	{// NULLだったら
		return E_FAIL;
	}

	//**********************************
	// 敵マネージャ
	//**********************************
	m_pEnemyManager = CEnemyManager::Create("data\\TEXT\\enemy_manager.txt");

	if (m_pEnemyManager == NULL)
	{// NULLだったら
		return E_FAIL;
	}

	//**********************************
	// カメラの軸
	//**********************************
	m_pCameraAxis = CCameraAxis::Create("data\\BIN\\cameraaxis.bin");
	if (m_pCameraAxis == NULL)
	{// NULLだったら
		return E_FAIL;
	}

	//**********************************
	// カメラ追従変更者
	//**********************************
	m_pCameraChaseChanger = CCameraChaseChanger::Create("data\\BIN\\camerachanger.bin");
	if (m_pCameraChaseChanger == NULL)
	{// NULLだったら
		return E_FAIL;
	}

	//**********************************
	// 敵の拠点
	//**********************************
	m_pEnemyBase = CEnemyBase::Create();
	if (m_pEnemyBase == NULL)
	{// NULLだったら
		return E_FAIL;
	}

	// BGM再生
	CManager::GetInstance()->GetSound()->PlaySound(CSound::LABEL_BGM_GAME);

	//**********************************
	// プレイヤー
	//**********************************
	if (CManager::GetInstance()->GetScene()->GetPlayer() != NULL)
	{
		CManager::GetInstance()->GetScene()->GetPlayer()->SetPosition(D3DXVECTOR3(0.0f, 1000.0f, -1000.0f));
		CManager::GetInstance()->GetScene()->GetPlayer()->SetRotation(D3DXVECTOR3(0.0f, 0.0f, 0.0f));
	}

	//**********************************
	// 弾マネージャ
	//**********************************
	m_pBulletManager = CBulletManager::Create();

	// ステージ
	m_pStage = CStage::Create("data\\TEXT\\stage_info.txt");

	// スコアの生成処理
	m_pScore = CScore::Create(D3DXVECTOR3(1000.0f, 50.0f, 0.0f));

	// タイマーの生成処理
	m_pTimer = CTimer::Create(D3DXVECTOR3(200.0f, 50.0f, 0.0f));

	// ヒットスコア
	m_pHitScore = CHitScore::Create(D3DXVECTOR3(1300.0f, 600.0f, 0.0f));

	CManager::GetInstance()->GetCamera()->Reset(CScene::MODE_GAME);

	// 成功
	return S_OK;
}

//==========================================================================
// 終了処理
//==========================================================================
void CGame::Uninit(void)
{
	if (m_pGameManager != NULL)
	{
		// 終了処理
		m_pGameManager->Uninit();
		delete m_pGameManager;
		m_pGameManager = NULL;
	}

	// スコアの破棄
	if (m_pScore != NULL)
	{// メモリの確保が出来ていたら

		// 終了処理
		m_pScore->Uninit();

		// メモリの開放
		delete m_pScore;
		m_pScore = NULL;
	}

	// タイマーの破棄
	if (m_pTimer != NULL)
	{// メモリの確保が出来ていたら

		// 終了処理
		m_pTimer->Uninit();

		// メモリの開放
		delete m_pTimer;
		m_pTimer = NULL;
	}

	// ヒットスコアの破棄
	if (m_pHitScore != NULL)
	{
		// 終了処理
		m_pHitScore->Uninit();
		delete m_pHitScore;
		m_pHitScore = NULL;
	}

	// ステージの破棄
	if (m_pStage != NULL)
	{// メモリの確保が出来ていたら

		// 終了処理
		m_pStage->Uninit();
		delete m_pStage;
		m_pStage = NULL;
	}

	if (m_pEditControlPoint != NULL)
	{
		// 終了させる
		m_pEditControlPoint->Uninit();
		delete m_pEditControlPoint;
		m_pEditControlPoint = NULL;
	}
	
	if (m_pEditCameraAxis != NULL)
	{
		// 終了させる
		m_pEditCameraAxis->Uninit();
		delete m_pEditCameraAxis;
		m_pEditCameraAxis = NULL;
	}

	if (m_pEditCmaeraChaseChanger != NULL)
	{
		// 終了させる
		m_pEditCmaeraChaseChanger->Uninit();
		delete m_pEditCmaeraChaseChanger;
		m_pEditCmaeraChaseChanger = NULL;
	}

	if (m_pEditEnemyBase != NULL)
	{
		// 終了させる
		m_pEditEnemyBase->Uninit();
		delete m_pEditEnemyBase;
		m_pEditEnemyBase = NULL;
	}

	if (m_pBulletManager != NULL)
	{
		// 終了させる
		m_pBulletManager->Uninit();
		delete m_pBulletManager;
		m_pBulletManager = NULL;
	}

	// マップマネージャ
	if (m_pMapManager != NULL)
	{
		m_pMapManager->Uninit();
		delete m_pMapManager;
		m_pMapManager = NULL;
	}

	// カメラの軸
	if (m_pCameraAxis != NULL)
	{
		m_pCameraAxis->Uninit();
		delete m_pCameraAxis;
		m_pCameraAxis = NULL;
	}

	// カメラ追従変更者
	if (m_pCameraChaseChanger != NULL)
	{
		m_pCameraChaseChanger->Uninit();
		delete m_pCameraChaseChanger;
		m_pCameraChaseChanger = NULL;
	}

	// 敵の拠点
	if (m_pEnemyBase != NULL)
	{
		m_pEnemyBase->Uninit();
		delete m_pEnemyBase;
		m_pEnemyBase = NULL;
	}

	// 敵マネージャ
	if (m_pEnemyManager != NULL)
	{
		m_pEnemyManager->Uninit();
		delete m_pEnemyManager;
		m_pEnemyManager = NULL;
	}

	// 終了処理
	CScene::Uninit();
}

//==========================================================================
// 更新処理
//==========================================================================
void CGame::Update(void)
{
	// ゲームマネージャ
	if (m_pGameManager != NULL)
	{
		// 更新処理
		m_pGameManager->Update();
	}

	CManager::GetInstance()->GetDebugProc()->Print(
		"現在のモード：【ゲーム】\n"
		"切り替え：【 F 】\n\n");

	// キーボード情報取得
	CInputKeyboard *pInputKeyboard = CManager::GetInstance()->GetInputKeyboard();

	// ゲームパッド情報取得
	CInputGamepad *pInputGamepad = CManager::GetInstance()->GetInputGamepad();

#if 1
	if (CManager::GetInstance()->GetEdit() == NULL &&
		m_pEditControlPoint == NULL &&
		m_pEditCameraAxis == NULL &&
		GetEnemyManager()->GetState() != CEnemyManager::STATE_COMBOANIM)
	{
		// タイマーの更新処理
		m_pTimer->Update();

		// スコアの更新処理
		m_pScore->Update();

		// ヒットスコアの更新処理
		if (m_pHitScore != NULL)
		{
			// 更新処理
			m_pHitScore->Update();
		}
	}
#endif

	if (pInputKeyboard->GetTrigger(DIK_F4))
	{// F4でエディット切り替え

		// 切り替え
		m_EditType = (EEditType)(((int)m_EditType + 1) % (int)EDITTYPE_MAX);	// 追従の種類

		EditReset();

		switch (m_EditType)
		{
		case CGame::EDITTYPE_OFF:	// 全部オフ

			break;

		case CGame::EDITTYPE_MAPCONTROL:

			if (m_pEditControlPoint == NULL)
			{// NULLだったら

				// エディットの生成処理
				m_pEditControlPoint = CEditControlPoint::Create();
			}
			break;

		case CGame::EDITTYPE_CAMERACHASE:

			if (m_pEditCmaeraChaseChanger == NULL)
			{// NULLだったら

				// エディットの生成処理
				m_pEditCmaeraChaseChanger = CEditCameraChaseChanger::Create();
			}
			break;

		case CGame::EDITTYPE_CAMERAAXIS:

			if (m_pEditCameraAxis == NULL)
			{// NULLだったら

				// エディットの生成処理
				m_pEditCameraAxis = CEditCameraAxis::Create();
			}
			break;

		case EDITTYPE_ENEMYBASE:
			if (m_pEditEnemyBase == NULL)
			{// NULLだったら

				// エディットの生成処理
				m_pEditEnemyBase = CEditEnemyBase::Create();
			}
			break;

		}
	}

	if (GetEnemyManager() != NULL)
	{// 敵マネージャの更新処理
		GetEnemyManager()->Update();
	}

	if (m_pEditControlPoint != NULL)
	{// 制御点マネージャの更新処理
		m_pEditControlPoint->Update();
	}

	if (m_pEditCmaeraChaseChanger != NULL)
	{// カメラ追従変更者マネージャの更新処理
		m_pEditCmaeraChaseChanger->Update();
	}

	if (m_pEditCameraAxis != NULL)
	{// カメラ軸エディターの更新処理
		m_pEditCameraAxis->Update();
	}

	if (m_pEditEnemyBase != NULL)
	{// 敵の拠点エディターの更新処理
		m_pEditEnemyBase->Update();
	}

	// マップマネージャの更新処理
	m_pMapManager->Update();

	// カメラの軸の更新処理
	m_pCameraAxis->Update();

	// カメラ追従変更者
	m_pCameraChaseChanger->Update();

	// 敵の拠点
	m_pEnemyBase->Update();

#if _DEBUG

	if (pInputKeyboard->GetTrigger(DIK_F))
	{
		// モード設定
		CManager::GetInstance()->GetFade()->SetFade(CScene::MODE_RESULT);
	}
#endif

	// シーンの更新
	CScene::Update();

}

//==========================================================================
// 描画処理
//==========================================================================
void CGame::Draw(void)
{

}

//==========================================================================
// スコアの取得
//==========================================================================
CScore *CGame::GetScore(void)
{
	return m_pScore;
}

//==========================================================================
// タイマーの取得
//==========================================================================
CTimer *CGame::GetTimer(void)
{
	return m_pTimer;
}

//==========================================================================
// ヒットスコアの取得
//==========================================================================
CHitScore *CGame::GetHitScore(void)
{
	return m_pHitScore;
}

//==========================================================================
// パワーゲージの取得
//==========================================================================
CPowerGauge *CGame::GetPowerGauge(void)
{
	return m_pPowerGauge;
}

//==========================================================================
// 制御点エディターの取得
//==========================================================================
CEditControlPoint *CGame::GetEditControlPoint(void)
{
	return m_pEditControlPoint;
}

//==========================================================================
// 弾マネージャの取得
//==========================================================================
CBulletManager *CGame::GetBulletManager(void)
{
	return m_pBulletManager;
}

//==========================================================================
// ステージの取得
//==========================================================================
CStage *CGame::GetStage(void)
{
	return m_pStage;
}

//==========================================================================
// ゲームマネージャの取得
//==========================================================================
CGameManager *CGame::GetGameManager(void)
{
	return m_pGameManager;
}


//==========================================================================
// 敵マネージャの取得
//==========================================================================
CEnemyManager *CGame::GetEnemyManager(void)
{
	return m_pEnemyManager;
}

//==========================================================================
// マップマネージャの取得
//==========================================================================
CMapManager *CGame::GetMapManager(void)
{
	return m_pMapManager;
}

//==========================================================================
// カメラの軸取得
//==========================================================================
CCameraAxis *CGame::GetCameraAxis(void)
{
	return m_pCameraAxis;
}

//==========================================================================
// カメラ追従変更者の取得
//==========================================================================
CCameraChaseChanger *CGame::GetCameraChaseChanger(void)
{
	return m_pCameraChaseChanger;
}

//==========================================================================
// 敵の拠点
//==========================================================================
CEnemyBase *CGame::GetEnemyBase(void)
{
	return m_pEnemyBase;
}

//==========================================================================
// リセット処理
//==========================================================================
void CGame::Reset(void)
{
	// ステージの破棄
	if (m_pStage != NULL)
	{// メモリの確保が出来ていたら

		// 終了処理
		m_pStage->Release();
		delete m_pStage;
		m_pStage = NULL;
	}

	// マップマネージャ
	if (m_pMapManager != NULL)
	{
		m_pMapManager->Release();
		delete m_pMapManager;
		m_pMapManager = NULL;
	}

	// カメラの軸
	if (m_pCameraAxis != NULL)
	{
		m_pCameraAxis->Uninit();
		delete m_pCameraAxis;
		m_pCameraAxis = NULL;
	}

	// カメラ追従変更者
	if (m_pCameraChaseChanger != NULL)
	{
		m_pCameraChaseChanger->Uninit();
		delete m_pCameraChaseChanger;
		m_pCameraChaseChanger = NULL;
	}

	// 敵の拠点
	if (m_pEnemyBase != NULL)
	{
		m_pEnemyBase->Uninit();
		delete m_pEnemyBase;
		m_pEnemyBase = NULL;
	}

	// 敵マネージャ
	if (m_pEnemyManager != NULL)
	{
		m_pEnemyManager->Kill();
	}

	// ステージ
	m_pStage = CStage::Create("data\\TEXT\\bossstage_info.txt");

	//**********************************
	// マップマネージャ
	//**********************************
	m_pMapManager = CMapManager::Create("data\\BIN\\maptarget_boss.bin");

	if (m_pMapManager == NULL)
	{// NULLだったら
		return;
	}
	m_pMapManager->SetEnableCrawl();

	//**********************************
	// カメラの軸
	//**********************************
	m_pCameraAxis = CCameraAxis::Create("data\\BIN\\cameraaxis_boss.bin");
	if (m_pCameraAxis == NULL)
	{// NULLだったら
		return;
	}

	//**********************************
	// カメラ追従変更者
	//**********************************
	m_pCameraChaseChanger = CCameraChaseChanger::Create("data\\BIN\\camerachanger_boss.bin");
	if (m_pCameraChaseChanger == NULL)
	{// NULLだったら
		return;
	}

	//**********************************
	// 敵の拠点
	//**********************************
	m_pEnemyBase = CEnemyBase::Create();
	if (m_pEnemyBase == NULL)
	{// NULLだったら
		return;
	}
}

//==========================================================================
// エディターリセット処理
//==========================================================================
void CGame::EditReset(void)
{
	if (m_pEditControlPoint != NULL)
	{
		// 終了させる
		m_pEditControlPoint->Uninit();
		delete m_pEditControlPoint;
		m_pEditControlPoint = NULL;
	}

	if (m_pEditCameraAxis != NULL)
	{
		// 終了させる
		m_pEditCameraAxis->Uninit();
		delete m_pEditCameraAxis;
		m_pEditCameraAxis = NULL;
	}

	if (m_pEditCmaeraChaseChanger != NULL)
	{
		// 終了させる
		m_pEditCmaeraChaseChanger->Uninit();
		delete m_pEditCmaeraChaseChanger;
		m_pEditCmaeraChaseChanger = NULL;
	}

	if (m_pEditEnemyBase != NULL)
	{
		// 終了させる
		m_pEditEnemyBase->Release();
		m_pEditEnemyBase->Uninit();
		delete m_pEditEnemyBase;
		m_pEditEnemyBase = NULL;
	}

}
