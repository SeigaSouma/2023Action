//=============================================================================
// 
// ゲームマネージャ処理 [gamemanager.cpp]
//  Author : 相馬靜雅
// 
//=============================================================================
#include "gamemanager.h"
#include "game.h"
#include "debugproc.h"
#include "manager.h"
#include "renderer.h"
#include "instantfade.h"
#include "player.h"
#include "camera.h"
#include "sound.h"
#include "mapmanager.h"
#include "blackframe.h"

//==========================================================================
// マクロ定義
//==========================================================================

//==========================================================================
// 静的メンバ変数宣言
//==========================================================================

//==========================================================================
// コンストラクタ
//==========================================================================
CGameManager::CGameManager()
{
	// 値のクリア
	m_SceneType = SCENE_MAIN;	// シーンの種類
	m_bEndRush = false;			// ラッシュが終了したか
	m_bControll = false;		// 操作できるか
}

//==========================================================================
// デストラクタ
//==========================================================================
CGameManager::~CGameManager()
{

}

//==========================================================================
// 生成処理
//==========================================================================
CGameManager *CGameManager::Create(void)
{
	// 生成用のオブジェクト
	CGameManager *pManager = NULL;

	if (pManager == NULL)
	{// NULLだったら

		// メモリの確保
		pManager = DEBUG_NEW CGameManager;

		if (pManager != NULL)
		{// メモリの確保が出来ていたら

			// 初期化処理
			HRESULT hr = pManager->Init();

			if (FAILED(hr))
			{// 失敗していたら
				return NULL;
			}
		}

		return pManager;
	}

	return NULL;
}

//==========================================================================
// 初期化処理
//==========================================================================
HRESULT CGameManager::Init(void)
{
	m_bControll = true;		// 操作できるか
	return S_OK;
}

//==========================================================================
// 終了処理
//==========================================================================
void CGameManager::Uninit(void)
{
	
}

//==========================================================================
// 更新処理
//==========================================================================
void CGameManager::Update(void)
{
	if (m_SceneType == SCENE_TRANSITION)
	{// 遷移中

		// 遷移なしフェードの状態取得
		CInstantFade::STATE fadestate = CManager::GetInstance()->GetInstantFade()->GetState();

		if (fadestate == CInstantFade::STATE_FADECOMPLETION)
		{// 完了した瞬間


			// BGMストップ
			CManager::GetInstance()->GetSound()->StopSound(CSound::LABEL_BGM_GAME);

			// 追従の種類設定
			m_SceneType = SCENE_BOSS;

			// リセット処理
			CGame::Reset();

			// シーンのリセット
			CManager::GetInstance()->GetScene()->ResetScene();

			// プレイヤー情報
			CPlayer *pPlayer = CManager::GetInstance()->GetScene()->GetPlayer();
			if (pPlayer == NULL)
			{
				return;
			}
			// マップマネージャの取得
			CMapManager *pMapManager = CGame::GetMapManager();
			if (pMapManager == NULL)
			{// NULLだったら
				return;
			}

			D3DXVECTOR3 pos = pMapManager->GetTargetPosition(0, 0.0f);
			pPlayer->SetPosition(pos);
			pPlayer->SetMapIndex(0);
			pPlayer->SetMapMoveValue(0.0f);

			// カメラの情報取得
			CCamera *pCamera = CManager::GetInstance()->GetCamera();
			pCamera->ResetBoss();

			// 黒フレーム侵入
			CManager::GetInstance()->GetBlackFrame()->SetState(CBlackFrame::STATE_IN);

			// 操作不能状態にする
			m_bControll = false;
		}
	}

}

//==========================================================================
// シーンの種類設定
//==========================================================================
void CGameManager::SetType(SceneType type)
{
	m_SceneType = type;
}

//==========================================================================
// シーンの種類取得
//==========================================================================
CGameManager::SceneType CGameManager::GetType(void)
{
	return m_SceneType;
}
