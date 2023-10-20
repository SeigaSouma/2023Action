//=============================================================================
// 
//  チュートリアルプレイヤー処理 [tutorialplayer.cpp]
//  Author : 相馬靜雅
// 
//=============================================================================
#include "tutorial.h"
#include "tutorialplayer.h"
#include "tutorialstep.h"
#include "camera.h"
#include "manager.h"
#include "debugproc.h"
#include "renderer.h"
#include "input.h"
#include "motion.h"
#include "sound.h"
#include "particle.h"
#include "scene.h"
#include "calculation.h"

//==========================================================================
// マクロ定義
//==========================================================================

//==========================================================================
// 静的メンバ変数宣言
//==========================================================================

//==========================================================================
// コンストラクタ
//==========================================================================
CTutorialPlayer::CTutorialPlayer(int nPriority) : CPlayer(nPriority)
{
	// 値のクリア
	
}

//==========================================================================
// デストラクタ
//==========================================================================
CTutorialPlayer::~CTutorialPlayer()
{

}

//==========================================================================
// 初期化処理
//==========================================================================
HRESULT CTutorialPlayer::Init(void)
{
	// 親の初期化処理
	CPlayer::Init();

	return S_OK;
}

//==========================================================================
// モード別終了処理
//==========================================================================
void CTutorialPlayer::UninitByMode(void)
{

	// プレイヤー情報
	//CPlayer **ppPlayer = CTutorial::GetPlayer(0);
	CScene *pScene = CManager::GetInstance()->GetScene();
	if (pScene != NULL)
	{
		CPlayer *pPlayer = CManager::GetInstance()->GetScene()->GetPlayer();
		CPlayer **ppPlayer = &pPlayer;

		// プレイヤーをNULL
		ppPlayer = NULL;
	}
}

//==========================================================================
// 更新処理
//==========================================================================
void CTutorialPlayer::Update(void)
{
	// 死亡の判定
	if (IsDeath() == true)
	{// 死亡フラグが立っていたら
		return;
	}

	// 親の更新処理
	CPlayer::Update();

	// 死亡の判定
	if (IsDeath() == true)
	{// 死亡フラグが立っていたら
		return;
	}

	// ステップごとの更新
	UpdateByStep();
}

//==========================================================================
// ステップごとの更新処理
//==========================================================================
void CTutorialPlayer::UpdateByStep(void)
{
	// ステップの設定
	//CTutorial::GetStep()->SetStep(CTutorialStep::STEPFRAG_SPEEDUP);
}


//==========================================================================
// 操作処理
//==========================================================================
void CTutorialPlayer::Controll(void)
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
	int nType = m_pMotion->GetType();

	// 移動量取得
	float fMove = GetVelocity();

	if (m_pMotion->IsGetMove(nType) == 1)
	{// 移動可能モーションの時

#ifndef AUTO_MOVE
		// 移動中にする
		m_bMove = true;
#endif

		if (pInputKeyboard->GetPress(DIK_A) == true || pInputGamepad->GetStickMoveL(0).x < 0)
		{//←キーが押された,左移動

			if (pInputKeyboard->GetPress(DIK_W) == true || pInputGamepad->GetStickMoveL(0).y > 0)
			{//A+W,左上移動
				fRotDest = D3DX_PI * 0.75f + Camerarot.y;
			}
			else if (pInputKeyboard->GetPress(DIK_S) == true || pInputGamepad->GetStickMoveL(0).y < 0)
			{//A+S,左下移動
				fRotDest = D3DX_PI * 0.25f + Camerarot.y;
			}
			else
			{//A,左移動
				fRotDest = D3DX_PI * 0.5f + Camerarot.y;
			}
		}
		else if (pInputKeyboard->GetPress(DIK_D) == true || pInputGamepad->GetStickMoveL(0).x > 0)
		{//Dキーが押された,右移動

			if (pInputKeyboard->GetPress(DIK_W) == true || pInputGamepad->GetStickMoveL(0).y > 0)
			{//D+W,右上移動
				fRotDest = -D3DX_PI * 0.75f + Camerarot.y;
			}
			else if (pInputKeyboard->GetPress(DIK_S) == true || pInputGamepad->GetStickMoveL(0).y < 0)
			{//D+S,右下移動
				fRotDest = -D3DX_PI * 0.25f + Camerarot.y;
			}
			else
			{//D,右移動
				fRotDest = -D3DX_PI * 0.5f + Camerarot.y;
			}
		}
	}
	else
	{// 移動可能モーションじゃない場合

		if (pInputKeyboard->GetPress(DIK_A) == true || pInputGamepad->GetStickMoveL(0).x < 0)
		{//←キーが押された,左移動

			if (pInputKeyboard->GetPress(DIK_W) == true || pInputGamepad->GetStickMoveL(0).y > 0)
			{//A+W,左上移動
				fRotDest = D3DX_PI * 0.75f + Camerarot.y;
			}
			else if (pInputKeyboard->GetPress(DIK_S) == true || pInputGamepad->GetStickMoveL(0).y < 0)
			{//A+S,左下移動
				fRotDest = D3DX_PI * 0.25f + Camerarot.y;
			}
			else
			{//A,左移動
				fRotDest = D3DX_PI * 0.5f + Camerarot.y;
			}
		}
		else if (pInputKeyboard->GetPress(DIK_D) == true || pInputGamepad->GetStickMoveL(0).x > 0)
		{//Dキーが押された,右移動

			if (pInputKeyboard->GetPress(DIK_W) == true || pInputGamepad->GetStickMoveL(0).y > 0)
			{//D+W,右上移動
				fRotDest = -D3DX_PI * 0.75f + Camerarot.y;
			}
			else if (pInputKeyboard->GetPress(DIK_S) == true || pInputGamepad->GetStickMoveL(0).y < 0)
			{//D+S,右下移動
				fRotDest = -D3DX_PI * 0.25f + Camerarot.y;
			}
			else
			{//D,右移動
				fRotDest = -D3DX_PI * 0.5f + Camerarot.y;
			}
		}
	}

	//角度の正規化
	RotNormalize(fRotDest);

	//現在と目標の差分を求める
	float fRotDiff = fRotDest - rot.y;

	//角度の正規化
	RotNormalize(fRotDiff);

	//角度の補正をする
	rot.y += fRotDiff * 0.1f;

	//角度の正規化
	RotNormalize(rot.y);

#ifndef AUTO_MOVE
	move.x += sinf(D3DX_PI + rot.y) * fMove;
	move.z += cosf(D3DX_PI + rot.y) * fMove;
#endif

	// 重力処理
	move.y -= mylib_const::GRAVITY;

	// 位置更新
	pos += move;

	// 慣性補正
	move.x += (0.0f - move.x) * 0.25f;
	move.z += (0.0f - move.z) * 0.25f;

	// 位置設定
	SetPosition(pos);

	// 移動量設定
	SetMove(move);

	// 向き設定
	SetRotation(rot);

	// 目標の向き設定
	SetRotDest(fRotDest);

	//**********************************
	// 当たり判定
	//**********************************
	//Collision();

	if ((pInputKeyboard->GetTrigger(DIK_RETURN) == true || pInputGamepad->GetTrigger(CInputGamepad::BUTTON_A, 0) == true) &&
		m_bATK == false)
	{// 攻撃

		// 攻撃判定ON
		m_bATK = true;
	}
}