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
	// スコアの更新処理
	m_pScore->Update();

	if (CManager::GetInstance()->GetEdit() == NULL &&
		m_pEditControlPoint == NULL &&
		m_pEditCameraAxis == NULL &&
		GetEnemyManager()->GetState() != CEnemyManager::STATE_COMBOANIM)
	{
		// タイマーの更新処理
		m_pTimer->Update();
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

#if _DEBUG

	if (pInputKeyboard->GetTrigger(DIK_F))
	{
		// モード設定
		CManager::GetInstance()->GetFade()->SetFade(CScene::MODE_RESULT);
	}
#endif

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

	// ステージ
	m_pStage = CStage::Create("data\\TEXT\\bossstage_info.txt");
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
