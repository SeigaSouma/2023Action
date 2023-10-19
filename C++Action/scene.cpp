//=============================================================================
// 
//  シーン処理 [scene.cpp]
//  Author : 相馬靜雅
// 
//=============================================================================
#include "scene.h"
#include "renderer.h"
#include "calculation.h"
#include "Xload.h"
#include "map.h"
#include "fade.h"
#include "elevation.h"
#include "enemymanager.h"
#include "mapmanager.h"
#include "player.h"
#include "cameraaxis.h"
#include "camerachasechanger.h"

// 遷移先
#include "game.h"
#include "title.h"
#include "tutorial.h"
#include "result.h"
#include "ranking.h"

//==========================================================================
// 静的メンバ変数宣言
//==========================================================================
CXLoad *CScene::m_pXLoad = NULL;				// Xファイルのオブジェクト
CElevation *CScene::m_pObject3DMesh = NULL;		// オブジェクト3Dメッシュのオブジェクト
CEnemyManager *CScene::m_pEnemyManager = NULL;	// 敵マネージャのオブジェクト
CMapManager *CScene::m_pMapManager = NULL;		// マップマネージャのオブジェクト
CCameraAxis *CScene::m_pCameraAxis = NULL;		// カメラの軸のオブジェクト
CCameraChaseChanger *CScene::m_pCameraChaseChanger = NULL;	// カメラ追従変更者のオブジェクト

//==========================================================================
// コンストラクタ
//==========================================================================
CScene::CScene()
{
	// 変数のクリア
	m_mode = MODE_TITLE;
	m_pPlayer = NULL;
}

//==========================================================================
// デストラクタ
//==========================================================================
CScene::~CScene()
{

}

//==========================================================================
// 生成処理
//==========================================================================
CScene *CScene::Create(CScene::MODE mode)
{
	// 生成用のオブジェクト
	CScene *pScene = NULL;

	if (pScene == NULL)
	{// NULLだったら

		// 生成処理
		switch (mode)
		{
		case CScene::MODE_TITLE:
			pScene = DEBUG_NEW CTitle;
			break;

		case CScene::MODE_TUTORIAL:
			pScene = DEBUG_NEW CTutorial;
			break;

		case CScene::MODE_GAME:
			pScene = DEBUG_NEW CGame;
			break;

		case CScene::MODE_RESULT:
			pScene = DEBUG_NEW CResult;
			break;

		case CScene::MODE_RANKING:
			pScene = DEBUG_NEW CRanking;
			break;
		}

		if (pScene != NULL)
		{// メモリの確保が出来ていたら

			// モードの設定
			pScene->m_mode = mode;
		}

		return pScene;
	}

	return NULL;
}

//==========================================================================
// 初期化処理
//==========================================================================
HRESULT CScene::Init(void)
{
	HRESULT hr;

	//**********************************
	// Xファイル
	//**********************************
	if (m_pXLoad != NULL)
	{// 確保されていたら
		return E_FAIL;
	}

	// メモリ確保
	m_pXLoad = DEBUG_NEW CXLoad;

	if (m_pXLoad == NULL)
	{// メモリの確保が出来ていなかったら
		return E_FAIL;
	}
	m_pXLoad->Init();

	//**********************************
	// マップマネージャ
	//**********************************
	m_pMapManager = CMapManager::Create("data\\BIN\\maptarget.bin");

	if (m_pMapManager == NULL)
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
	// 敵マネージャ
	//**********************************
	m_pEnemyManager = CEnemyManager::Create("data\\TEXT\\enemy_manager.txt");

	if (m_pEnemyManager == NULL)
	{// NULLだったら
		return E_FAIL;
	}

	//**********************************
	// マップの生成
	//**********************************
	if (FAILED(map::Create()))
	{// 失敗した場合
		return E_FAIL;
	}

	//**********************************
	// 起伏の地面
	//**********************************
	m_pObject3DMesh = CElevation::Create("data\\TEXT\\elevation_field.txt");

	// 初期化処理
	hr = m_pXLoad->Init();

	if (FAILED(hr))
	{// 初期化処理が失敗した場合
		return E_FAIL;
	}

	// メモリ確保
	m_pPlayer = CPlayer::Create();

	return S_OK;
}

//==========================================================================
// 終了処理
//==========================================================================
void CScene::Uninit(void)
{
	// Xファイルの破棄
	if (m_pXLoad != NULL)
	{// メモリの確保が出来ていたら

		// 終了処理
		m_pXLoad->Uninit();

		// メモリの開放
		delete m_pXLoad;
		m_pXLoad = NULL;
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

	// 敵マネージャ
	if (m_pEnemyManager != NULL)
	{
		m_pEnemyManager->Uninit();
		delete m_pEnemyManager;
		m_pEnemyManager = NULL;
	}

	// プレイヤーの破棄
	if (m_pPlayer != NULL)
	{// メモリの確保が出来ていたら

		m_pPlayer->Uninit();
		m_pPlayer = NULL;
	}

}

//==========================================================================
// 更新処理
//==========================================================================
void CScene::Update(void)
{
	// マップマネージャの更新処理
	m_pMapManager->Update();

	// カメラの軸の更新処理
	m_pCameraAxis->Update();

	// カメラ追従変更者
	m_pCameraChaseChanger->Update();
}

//==========================================================================
// 描画処理
//==========================================================================
void CScene::Draw(void)
{
	
}

//==========================================================================
// 現在のモード取得
//==========================================================================
CScene::MODE CScene::GetMode(void)
{
	return m_mode;
}

//==========================================================================
// Xファイルの取得
//==========================================================================
CXLoad *CScene::GetXLoad(void)
{
	return m_pXLoad;
}

//==========================================================================
// 3Dオブジェクトの取得
//==========================================================================
CElevation *CScene::GetElevation(void)
{
	return m_pObject3DMesh;
}

//==========================================================================
// 敵マネージャの取得
//==========================================================================
CEnemyManager *CScene::GetEnemyManager(void)
{
	return m_pEnemyManager;
}

//==========================================================================
// マップマネージャの取得
//==========================================================================
CMapManager *CScene::GetMapManager(void)
{
	return m_pMapManager;
}

//==========================================================================
// カメラの軸取得
//==========================================================================
CCameraAxis *CScene::GetCameraAxis(void)
{
	return m_pCameraAxis;
}

//==========================================================================
// カメラ追従変更者の取得
//==========================================================================
CCameraChaseChanger *CScene::GetCameraChaseChanger(void)
{
	return m_pCameraChaseChanger;
}

//==========================================================================
// プレイヤーの取得
//==========================================================================
CPlayer *CScene::GetPlayer(void)
{
	return m_pPlayer;
}

//==========================================================================
// プレイヤーの終了
//==========================================================================
void CScene::UninitPlayer()
{
	m_pPlayer = NULL;
}
