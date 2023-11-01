//=============================================================================
// 
// カメラ処理 [camera.cpp]
// Author : 相馬靜雅
// 
//=============================================================================
#include "debugproc.h"
#include "manager.h"
#include "renderer.h"
#include "game.h"
#include "gamemanager.h"
#include "camera.h"
#include "input.h"
#include "calculation.h"
#include "player.h"
#include "elevation.h"
#include "enemymanager.h"
#include "title.h"
#include "instantfade.h"
#include "mapmanager.h"
#include "calculation.h"
#include "light.h"

//==========================================================================
// マクロ定義
//==========================================================================
#define MOVE			(2.5f)				// 移動量
#define MAX_LENGTH		(20000.0f)			// 最大距離
#define MIN_LENGTH		(10.0f)				// 最少距離
//#define START_CAMERALEN	(5100.0f)			// 元の距離
#define START_CAMERALEN	(1200.0f)			// 元の距離
#define ROT_MOVE_MOUSE	(0.01f)				// 回転移動量
#define ROT_MOVE_STICK	(0.0015f)			// 回転移動量
#define ROT_MOVE		(0.025f)			// 回転移動量
#define MIN_ROT			(-D3DX_PI * 0.49f)	// カメラ固定用
#define MAX_ROT			(D3DX_PI * 0.49f)	// カメラ固定用
#define BACKFOLLOW_TIME	(20)				// 背面補正までの時間
#define TITLE_POSR_DEST	(D3DXVECTOR3(0.0f, 0.0f, 0.0f))
#define TITLE_LEN_DEST	(6700.0f)
#define TITLECAMERAROT_NONE		(D3DXVECTOR3(0.0f, -0.79f, -0.25f))
#define TITLECAMERAROT_ENEMY	(D3DXVECTOR3(0.0f, -0.79f, -0.12f))
#define TITLESTATE_CHANGE	(60 * 14)
#define TITLESTATE_CHASE	(60 * 20)
#define RESULT_LEN	(6700.0f)
//#define RESULT_LEN	(1000.0f)
#define RANKINGROT_NONE		(D3DXVECTOR3(0.0f, -0.79f + D3DX_PI, -0.30f))
#define ROTHOSEI	(0.01f)	// 向きの補正係数
#define POSV_HOSEI	(0.12f * 0.8f)
#define POSR_HOSEI	(0.08f * 0.8f)
#define CHASEDISTANCE_DEST	(250.0f)
#define ROTCHANGEHOSEI		(0.3f)
#define RANKNIG_LEN_DEST	(7500.0f)

//==========================================================================
// コンストラクタ
//==========================================================================
CCamera::CCamera()
{
	m_viewport.X = 0;							// 描画する画面の左上X座標
	m_viewport.Y = 0;							// 描画する画面の左上Y座標
	m_viewport.Width = 0;						// 描画する画面の幅
	m_viewport.Height = 0;						// 描画する画面の高さ
	m_viewport.Width = 0;						// 描画する画面の幅
	m_viewport.Height = 0;						// 描画する画面の高さ
	m_viewport.MinZ = 0.0f;
	m_viewport.MaxZ = 0.0f;
	m_posR = D3DXVECTOR3(0.0f, 0.0f, 0.0f);		// 注視点(見たい場所)
	m_posV = D3DXVECTOR3(0.0f, 0.0f, 0.0f);		// 視点(カメラの位置)
	m_posVDest = D3DXVECTOR3(0.0f, 0.0f, 0.0f);	// 目標の視点
	m_posRDest = D3DXVECTOR3(0.0f, 0.0f, 0.0f);	// 目標の注視点
	m_vecU = D3DXVECTOR3(0.0f, 1.0f, 0.0f);		// 上方向ベクトル
	m_move = D3DXVECTOR3(0.0f, 0.0f, 0.0f);		// 移動量
	m_rot = D3DXVECTOR3(0.0f, 0.0f, 0.0f);		// 向き
	m_rotVDest = D3DXVECTOR3(0.0f, 0.0f, 0.0f);	// 目標の視点の向き
	m_TargetPos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);// 目標の位置
	m_fDistance = 0.0f;							// 距離
	m_fDestDistance = 0.0f;						// 目標の距離
	m_fOriginDistance = 0.0f;					// 元の距離
	m_nShakeLength = 0.0f;						// 揺れの長さ
	m_nShakeLengthY = 0.0f;						// Yの揺れの長さ
	m_fMoveShake = 0.0f;						// 揺れの移動量
	m_fMoveShakeY = 0.0f;						// Yの揺れの移動量
	m_fHeightMaxDest = 0.0f;					// カメラの最大高さの目標
	m_fHeightMax = 0.0f;			// カメラの最大高さ
	m_fDiffHeight = 0.0f;						// 高さの差分
	m_fDiffHeightSave = 0.0f;					// 高さの差分保存用
	m_fDiffHeightDest = 0.0f;					// 目標の高さの差分
	m_fChaseLerpStart = 0.0f;					// 追従補正の初期値
	m_fChaseDistance = 0.0f;					// 追従の間隔
	m_fDestChaseDistance = 0.0f;				// 目標の追従の間隔
	m_bFollow = false;							// 追従するかどうか
	m_state = CAMERASTATE_NONE;					// 状態
	m_nCntState = 0;							// 状態カウンター
	m_nCntDistance = 0;							// 距離カウンター
	m_nOriginCntDistance = 0;					// 元の距離カウンター
	m_fDistanceCorrection = 0.0f;				// 距離の慣性補正係数
	m_fDistanceDecrementValue = 0.0f;			// 距離の減少係数
	m_ChaseType = CHASETYPE_NORMAL;				// 追従の種類
	m_OldChaseType = CHASETYPE_NORMAL;			// 前回の追従の種類
	m_nCntChaseType = 0;			// 追従のカウンター
}

//==========================================================================
// デストラクタ
//==========================================================================
CCamera::~CCamera()
{

}

//==================================================================================
// カメラの初期化処理
//==================================================================================
HRESULT CCamera::Init(void)
{
	m_viewport.X = 0;										// 描画する画面の左上X座標
	m_viewport.Y = 0;										// 描画する画面の左上Y座標
	m_viewport.Width = SCREEN_WIDTH;						// 描画する画面の幅
	m_viewport.Height = SCREEN_HEIGHT;						// 描画する画面の高さ
	m_viewport.MinZ = 0.0f;
	m_viewport.MaxZ = 1.0f;
	m_posR = D3DXVECTOR3(0.0f, 200.0f, 0.0f);				// 注視点(見たい場所)
	m_posV = D3DXVECTOR3(0.0f, 300.0f, m_posR.z + -400.0f);	// 視点(カメラの位置)
	m_posVDest = m_posV;									// 目標の視点
	m_posRDest = m_posR;									// 目標の注視点
	m_vecU = D3DXVECTOR3(0.0f, 1.0f, 0.0f);					// 上方向ベクトル
	m_move = D3DXVECTOR3(0.0f, 0.0f, 0.0f);					// 移動量
	m_rot = D3DXVECTOR3(0.0f, 0.0f, -0.20f);					// 向き
	m_rotVDest = m_rot;										// 目標の視点の向き
	m_TargetPos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);			// 目標の位置
	m_fDistance = START_CAMERALEN;							// 距離
	m_fDestDistance = START_CAMERALEN;						// 目標の距離
	m_fOriginDistance = START_CAMERALEN;					// 元の距離
	m_fDiffHeight = 0.0f;									// 高さの差分
	m_fDiffHeightSave = 0.0f;								// 高さの差分保存用
	m_fDiffHeightDest = 0.0f;								// 目標の高さの差分
	m_fChaseLerpStart = 0.0f;								// 追従補正の初期値
	m_fChaseDistance = 0.0f;								// 追従の間隔
	m_fDestChaseDistance = 0.0f;							// 目標の追従の間隔
	m_bFollow = true;										// 追従するかどうか
	m_state = CAMERASTATE_NONE;								// 状態
	m_nCntState = 0;							// 状態カウンター
	m_nCntDistance = 0;							// 距離カウンター
	m_nOriginCntDistance = 0;					// 元の距離カウンター
	m_fDistanceCorrection = 0;					// 距離の慣性補正係数
	m_fDistanceDecrementValue = 0.0f;			// 距離の減少係数
	m_fHeightMaxDest = 0.0f;					// カメラの最大高さの目標
	m_ChaseType = CHASETYPE_MAP;				// 追従の種類
	m_OldChaseType = m_ChaseType;			// 前回の追従の種類
	m_nCntChaseType = 0;			// 追従のカウンター

	// 視点の代入処理
	SetCameraV();

	return S_OK;
}

//==================================================================================
// カメラの終了処理
//==================================================================================
void CCamera::Uninit(void)
{

}

//==================================================================================
// カメラの更新処理
//==================================================================================
void CCamera::Update(void)
{
	// キーボード情報取得
	CInputKeyboard *pInputKeyboard = CManager::GetInstance()->GetInputKeyboard();

	//if (m_state == CAMERASTATE_NONE)
	{
		// 高さの差分リセット
		m_fDiffHeightSave = 0.0f;

		if ((m_OldChaseType != CHASETYPE_NONE && m_ChaseType == CHASETYPE_NONE) ||
			(m_OldChaseType == CHASETYPE_NONE && m_ChaseType != CHASETYPE_NONE))
		{// 前回が追従してて今回が追従してない
			m_nCntChaseType = 150;
		}

		m_nCntChaseType--;
		if (m_nCntChaseType <= 0)
		{
			m_nCntChaseType = -1;
		}

		// 視点・注視点移動
		MoveCameraFollow();
		MoveCameraInput();
		MoveCameraVR();
		MoveCameraR();
		MoveCameraV();
		MoveCameraDistance();
		MoveCameraDistance();
		UpdateByMode();
		UpdateSpotLightVec();

		if (m_state == CAMERASTATE_SHAKE)
		{
			UpdateState();
		}
	}

//#ifdef _DEBUG
	
	if (pInputKeyboard->GetTrigger(DIK_F7) == true)
	{// F7が押された,追従切り替え

		m_bFollow = m_bFollow ? false : true;
	}

//#endif

	// 前回の追従の種類
	m_OldChaseType = m_ChaseType;

	// テキストの描画
	CManager::GetInstance()->GetDebugProc()->Print(
		"---------------- カメラ情報 ----------------\n"
		"【向き】[X：%f Y：%f Z：%f]\n"
		"【距離】[%f]\n"
		"【視点】[X：%f Y：%f Z：%f]\n"
		"【注視点】[X：%f Y：%f Z：%f]\n",
		m_rot.x, m_rot.y, m_rot.z,
		m_fDistance,
		m_posV.x, m_posV.y, m_posV.z,
		m_posR.x, m_posR.y, m_posR.z);
}

//==================================================================================
// モード別更新処理
//==================================================================================
void CCamera::UpdateByMode(void)
{
	switch (CManager::GetInstance()->GetMode())
	{
	case CScene::MODE_TITLE:
		
		// 回転させる
		m_rot.y += D3DX_PI * 0.001f;
		RotNormalize(m_rot.y);
		break;

	case CScene::MODE_GAME:
		break;

	default:
		break;
	}
}

//==================================================================================
// 入力機器のカメラ移動
//==================================================================================
void CCamera::MoveCameraInput(void)
{
#ifdef _DEBUG
	// マウスでの移動処理
	MoveCameraMouse();
#endif

	// スティックでの移動処理
	MoveCameraStick();
}

//==================================================================================
// コントローラーでの移動処理
//==================================================================================
void CCamera::MoveCameraStick(void)
{
	// ゲームパッド情報取得
	CInputGamepad *pInputGamepad = CManager::GetInstance()->GetInputGamepad();

#if 0
	m_rot.y += pInputGamepad->GetStickMoveR(0).x * ROT_MOVE_STICK;
	m_rot.z += pInputGamepad->GetStickMoveR(0).y * ROT_MOVE_STICK;
#endif
	// 角度の正規化
	RotNormalize(m_rot.y);
	RotNormalize(m_rot.z);

	// 視点の代入処理
	SetCameraV();
}

//==================================================================================
// マウスでの移動処理
//==================================================================================
void CCamera::MoveCameraMouse(void)
{
	// キーボード情報取得
	CInputKeyboard *pInputKeyboard = CManager::GetInstance()->GetInputKeyboard();

	// キーボード情報取得
	CInputMouse *pInputMouse = CManager::GetInstance()->GetInputMouse();

	if (pInputMouse->GetPress(CInputMouse::BUTTON_LEFT) == true &&
		pInputMouse->GetPress(CInputMouse::BUTTON_RIGHT) == true)
	{// 左右同時押し

//#if _DEBUG
		m_move.x += (pInputMouse->GetMouseMove().x * sinf(-D3DX_PI * MOVE_LR + m_rot.y) * MOVE) -
			(pInputMouse->GetMouseMove().y * cosf(-D3DX_PI * MOVE_LR + m_rot.y) * MOVE);

		m_move.z += (pInputMouse->GetMouseMove().x * cosf(-D3DX_PI * MOVE_LR + m_rot.y) * MOVE) +
			(pInputMouse->GetMouseMove().y * sinf(-D3DX_PI * MOVE_LR + m_rot.y) * MOVE);
//#endif

		// 移動量補正
		MoveCameraVR();

		// 角度の正規化
		RotNormalize(m_rot.y);
		RotNormalize(m_rot.z);

		// 注視点設定
		SetCameraR();
	}
	else if (pInputMouse->GetPress(CInputMouse::BUTTON_LEFT) == true)
	{// 左クリックしてるとき,視点回転

		m_rot.y += pInputMouse->GetMouseMove().x * ROT_MOVE_MOUSE;

//#if _DEBUG

		m_rot.z += pInputMouse->GetMouseMove().y * ROT_MOVE_MOUSE;	
//#endif

		// 角度の正規化
		RotNormalize(m_rot.y);
		RotNormalize(m_rot.z);

		// 値の正規化
		ValueNormalize(m_rot.z, MAX_ROT, MIN_ROT);

		// 視点の代入処理
		SetCameraV();

	}
	else if (pInputMouse->GetPress(CInputMouse::BUTTON_RIGHT) == true)
	{// 右クリックしてるとき,注視点回転

		m_rot.y += pInputMouse->GetMouseMove().x * ROT_MOVE_MOUSE;

//#if _DEBUG
		m_rot.z += pInputMouse->GetMouseMove().y * ROT_MOVE_MOUSE;
//#endif

		// 角度の正規化
		RotNormalize(m_rot.y);
		RotNormalize(m_rot.z);

		// 値の正規化
		ValueNormalize(m_rot.z, MAX_ROT, MIN_ROT);

		// 注視点の位置更新
		SetCameraR();
	}

	// マウスホイールで距離調整
	m_fDistance += pInputMouse->GetMouseMove().z * (MOVE * 0.3f);
	m_fDestDistance += pInputMouse->GetMouseMove().z * (MOVE * 0.3f);
	m_fOriginDistance += pInputMouse->GetMouseMove().z * (MOVE * 0.3f);

	// 視点の代入処理
	SetCameraV();
}

//==================================================================================
// カメラの視点移動
//==================================================================================
void CCamera::MoveCameraV(void)
{
	// キーボード情報取得
	CInputKeyboard *pInputKeyboard = CManager::GetInstance()->GetInputKeyboard();

#ifdef _DEBUG
	// 視点移動
	if (pInputKeyboard->GetPress(DIK_Y) == true)
	{// Yキーが押された,視点上移動

		m_rot.z -= ROT_MOVE;
	}
	else if (pInputKeyboard->GetPress(DIK_N) == true)
	{// Nキーが押された,視点下移動

		m_rot.z += ROT_MOVE;
	}

	if (pInputKeyboard->GetPress(DIK_Z) == true)
	{// Zキーが押された

		m_rot.y += ROT_MOVE;
	}
	if (pInputKeyboard->GetPress(DIK_C) == true)
	{// Cキーが押された

		m_rot.y -= ROT_MOVE;
	}
#endif

#if 0
	if (pInputKeyboard->GetPress(DIK_D) == true)
	{// Zキーが押された

		m_rot.y += ROT_MOVE * 1.8f;
	}
	if (pInputKeyboard->GetPress(DIK_A) == true)
	{// Cキーが押された

		m_rot.y -= ROT_MOVE * 1.8f;
	}
#endif


	// 角度の正規化
	RotNormalize(m_rot.y);
	RotNormalize(m_rot.z);

	// 値の正規化
	ValueNormalize(m_rot.z, MAX_ROT, MIN_ROT);

	// 視点の代入処理
	SetCameraV();

}

//==================================================================================
// カメラの注視点移動
//==================================================================================
void CCamera::MoveCameraR(void)
{
	// キーボード情報取得
	CInputKeyboard *pInputKeyboard = CManager::GetInstance()->GetInputKeyboard();

#ifdef _DEBUG
	// 旋回
	if (pInputKeyboard->GetPress(DIK_Q) == true)
	{// Qキーが押された,左旋回

		m_rot.y -= ROT_MOVE;
	}
	else if (pInputKeyboard->GetPress(DIK_E) == true)
	{// Eキーが押された,左旋回

		m_rot.y += ROT_MOVE;
	}

	// 上昇下降
	if (pInputKeyboard->GetPress(DIK_T) == true)
	{// Tキーが押された

		m_rot.z += ROT_MOVE;
	}
	else if (pInputKeyboard->GetPress(DIK_B) == true)
	{// Bキーが押された

		m_rot.z -= ROT_MOVE;
	}
#endif

	// 角度の正規化
	RotNormalize(m_rot.y);
	RotNormalize(m_rot.z);

	// 値の正規化
	ValueNormalize(m_rot.z, MAX_ROT, MIN_ROT);

	// 注視点の位置更新
	SetCameraR();
}

//==================================================================================
// カメラの視点・注視点移動
//==================================================================================
void CCamera::MoveCameraVR(void)
{

	// 移動量分を加算
	m_posV.x += m_move.x;
	m_posV.z += m_move.z;

	// 移動量をリセット
	m_move.x = 0.0f;
	m_move.z = 0.0f;

	// 注視点の位置更新
	SetCameraR();
}

//==================================================================================
//  視点・注視点間距離
//==================================================================================
void CCamera::MoveCameraDistance(void)
{
	// 距離カウンター減算
	m_nCntDistance--;

	if (m_nCntDistance <= 0)
	{
		m_nCntDistance = 0;

		if (m_fDestDistance >= m_fOriginDistance)
		{// カメラを近づけていく
			m_fDestDistance -= m_fDistanceDecrementValue;
		}

		if (m_fDestDistance <= m_fOriginDistance)
		{// 補正しすぎたら戻す
			m_fDestDistance = m_fOriginDistance;
		}
	}

	// 限界値補正
	if (m_fDistance >= MAX_LENGTH)
	{
		m_fDistance = MAX_LENGTH;
	}
	else if (m_fDistance <= MIN_LENGTH)
	{
		m_fDistance = MIN_LENGTH;
	}

	// 目標の距離へ補正
	InertiaCorrection(m_fDistance, m_fDestDistance, m_fDistanceCorrection);

}

//==================================================================================
// カメラの視点代入処理
//==================================================================================
void CCamera::SetCameraV(void)
{

	switch (CManager::GetInstance()->GetMode())
	{
	case CScene::MODE_TITLE:
		SetCameraVTitle();
		break;

	case CScene::MODE_TUTORIAL:
		SetCameraVGame();
		break;

	case CScene::MODE_GAME:
		SetCameraVGame();
		break;

	case CScene::MODE_RESULT:
		SetCameraVResult();
		break;

	default:
		break;
	}

}

//==================================================================================
// カメラの視点代入処理(タイトル)
//==================================================================================
void CCamera::SetCameraVTitle(void)
{
	if (m_bFollow == false)
	{// 追従しないとき

		// 視点の代入処理
		m_posV.x = m_posR.x + cosf(m_rot.z) * sinf(m_rot.y) * -m_fDistance;
		m_posV.z = m_posR.z + cosf(m_rot.z) * cosf(m_rot.y) * -m_fDistance;
		m_posV.y = m_posR.y + sinf(m_rot.z) * -m_fDistance;
	}
	else if (m_bFollow == true)
	{// 追従ON

		// 視点の代入処理
		m_posVDest.x = m_posR.x + cosf(m_rot.z) * sinf(m_rot.y) * -m_fDistance;
		m_posVDest.z = m_posR.z + cosf(m_rot.z) * cosf(m_rot.y) * -m_fDistance;
		m_posVDest.y = m_posR.y + sinf(m_rot.z) * -m_fDistance;

		float fDistance = 0.0f;
		m_fHeightMaxDest = m_posVDest.y;
		// 目標の角度を求める
		float fRotDest = atan2f((m_posVDest.x - m_posR.x), (m_posVDest.z - m_posR.z));
		while (1)
		{
			
			// 仮想の弾の位置
			float fPosBulletX = m_TargetPos.x + cosf(m_rot.z) * sinf(m_rot.y) * -fDistance;
			float fPosBulletZ = m_TargetPos.z + cosf(m_rot.z) * cosf(m_rot.y) * -fDistance;

			// 高さ取得
			bool bLand = false;
			float fHeight = CGame::GetElevation()->GetHeight(D3DXVECTOR3(fPosBulletX, 0.0f, fPosBulletZ), bLand);

			if (m_fHeightMaxDest <= fHeight)
			{// 最大の高さを更新したら

				// 距離の応じた割合保存
				float fDistanceRatio = fDistance / (m_fDistance);

				// 目標の最大高さ保存
				m_fHeightMaxDest = fHeight * (fDistanceRatio + 1.0f);
			}

			// 長さ加算
			fDistance += 10.0f;

			if (fDistance >= m_fDistance)
			{// 長さを超えたら終わり
				break;
			}
		}

		// 目標の視点更新
		if (m_fHeightMaxDest > m_posVDest.y)
		{
			// 最大の高さ補正
			m_fHeightMax += (m_fHeightMaxDest - m_fHeightMax) * 0.08f;

			m_posVDest.y = m_fHeightMax;

			// 高さの差分
			m_fDiffHeightSave += m_fHeightMax - m_posV.y;
		}

		// 補正する
		m_posV += (m_posVDest - m_posV) * 0.12f;
	}
}

//==================================================================================
// カメラの視点代入処理(ゲーム)
//==================================================================================
void CCamera::SetCameraVGame(void)
{
	if (m_bFollow == false)
	{// 追従しないとき

		// 視点の代入処理
		m_posV.x = m_posR.x + cosf(m_rot.z) * sinf(m_rot.y) * -m_fDistance;
		m_posV.z = m_posR.z + cosf(m_rot.z) * cosf(m_rot.y) * -m_fDistance;
		m_posV.y = m_posR.y + sinf(m_rot.z) * -m_fDistance;
	}
	else if (m_bFollow == true)
	{// 追従ON

		//現在と目標の差分を求める
		float fRotDiff = m_rotVDest.z - m_rot.z;
		RotNormalize(fRotDiff);

		m_rot.z += fRotDiff * 0.1f;
		RotNormalize(m_rot.z);

		switch (m_ChaseType)
		{
		case CCamera::CHASETYPE_NORMAL:
			ChaseNormal();
			break;

		case CCamera::CHASETYPE_MAP:
			ChaseMap();
			break;

		default:
			ChaseNone();
			break;
		}
	}
}

//==================================================================================
// カメラの視点代入処理(リザルト)
//==================================================================================
void CCamera::SetCameraVResult(void)
{
	// 視点の代入処理
	m_posV.x = m_posR.x + cosf(m_rot.z) * sinf(m_rot.y) * -m_fDistance;
	m_posV.z = m_posR.z + cosf(m_rot.z) * cosf(m_rot.y) * -m_fDistance;
	m_posV.y = m_posR.y + sinf(m_rot.z) * -m_fDistance;
}

//==================================================================================
// カメラの注視点代入処理
//==================================================================================
void CCamera::SetCameraR(void)
{

	switch (CManager::GetInstance()->GetMode())
	{
	case CScene::MODE_TITLE:
		SetCameraRTitle();
		break;

	case CScene::MODE_TUTORIAL:
		SetCameraRGame();
		break;

	case CScene::MODE_GAME:
		SetCameraRGame();
		break;

	case CScene::MODE_RESULT:
		SetCameraRResult();
		break;

	default:
		break;
	}

}

//==================================================================================
// カメラの注視点代入処理(タイトル)
//==================================================================================
void CCamera::SetCameraRTitle(void)
{
	if (m_bFollow == false)
	{// 追従しないとき

		// 注視点の代入処理
		m_posR.x = m_posV.x + cosf(m_rot.z) * sinf(m_rot.y) * m_fDistance;
		m_posR.z = m_posV.z + cosf(m_rot.z) * cosf(m_rot.y) * m_fDistance;
		m_posR.y = m_posV.y + sinf(m_rot.z) * m_fDistance;
	}
	else
	{// 追従ON

		float fYcamera = 100.0f;

		if (m_TargetPos.y >= 50.0f)
		{
			fYcamera = (m_TargetPos.y - 50.0f) + 100.0f;
		}
		else if (m_TargetPos.y <= -50.0f)
		{
			fYcamera = (m_TargetPos.y + 50.0f) + 100.0f;
		}

		fYcamera = m_TargetPos.y + 100.0f;

		if (fYcamera <= 0.0f)
		{
			m_fDiffHeightSave += -fYcamera;
		}

		// 目標の高さの差分を代入
		m_fDiffHeightDest = m_fDiffHeightSave;

		// 高さの差分を補正する
		m_fDiffHeight += (m_fDiffHeightDest - m_fDiffHeight) * 0.001f;


		// 注視点の代入処理
		m_posRDest.x = m_TargetPos.x;
		m_posRDest.z = m_TargetPos.z;
		m_posRDest.y = fYcamera - m_fDiffHeight;

		// 補正する
		m_posR += (m_posRDest - m_posR) * POSR_HOSEI;
	}
}

//==================================================================================
// カメラの注視点代入処理(ゲーム)
//==================================================================================
void CCamera::SetCameraRGame(void)
{
	if (m_bFollow == false)
	{// 追従しないとき

		// 注視点の代入処理
		m_posR.x = m_posV.x + cosf(m_rot.z) * sinf(m_rot.y) * m_fDistance;
		m_posR.z = m_posV.z + cosf(m_rot.z) * cosf(m_rot.y) * m_fDistance;
		m_posR.y = m_posV.y + sinf(m_rot.z) * m_fDistance;
	}
	else
	{// 追従ON

		if (m_ChaseType == CHASETYPE_NONE)
		{
			ChaseNone();
			return;
		}

		// プレイヤーの情報取得
		CPlayer *pPlayer = CManager::GetInstance()->GetScene()->GetPlayer();
		if (pPlayer == NULL)
		{// NULLだったら
			return;
		}

		D3DXVECTOR3 PlayerPos = pPlayer->GetPosition();
		D3DXVECTOR3 PlayerRot = pPlayer->GetRotation();
		float fYcamera = 100.0f;

		if (PlayerPos.y >= 50.0f)
		{
			fYcamera = (PlayerPos.y - 50.0f) + 100.0f;
		}
		else if (PlayerPos.y <= -50.0f)
		{
			fYcamera = (PlayerPos.y + 50.0f) + 100.0f;
		}

		fYcamera = PlayerPos.y + 100.0f;

		if (fYcamera <= 0.0f)
		{
			m_fDiffHeightSave += -fYcamera;
		}

		// 目標の高さの差分を代入
		m_fDiffHeightDest = m_fDiffHeightSave;

		// 高さの差分を補正する
		m_fDiffHeight += (m_fDiffHeightDest - m_fDiffHeight) * 0.001f;



		// 注視点
		D3DXVECTOR3 DestPoint = mylib_const::DEFAULT_VECTOR3;
		if (CGame::GetGameManager()->GetType() != CGameManager::SCENE_BOSS)
		{// ボスじゃないとき


			// マップマネージャの取得
			CMapManager *pMapManager = CGame::GetMapManager();
			if (pMapManager == NULL)
			{// NULLだったら
				return;
			}

			// マップ情報取得
			int nMapIdx = pPlayer->GetMapIndex();
			float fMapRatio = pPlayer->GetMapPointRatio();
			float fMapMoveValue = pPlayer->GetMapMoveValue();

			// 曲線作る為の4点
			int nP0 = nMapIdx;
			int nP1 = nMapIdx + 1;
			int nP2 = nMapIdx + 2;

			// 目標地点
			D3DXVECTOR3 TargetPoint0 = pMapManager->GetControlPoint(nP0);
			D3DXVECTOR3 TargetPoint1 = pMapManager->GetControlPoint(nP1);
			D3DXVECTOR3 TargetPoint2 = pMapManager->GetControlPoint(nP2);

			int nAngle = 1;
			if (pPlayer->GetMoveAngle() == CObject::ANGLE_LEFT)
			{
				nAngle = -1;
			}

			static float time = 0.0f;
			if (pPlayer->GetMoveAngle() == CObject::ANGLE_RIGHT && pPlayer->GetOldMoveAngle() != CObject::ANGLE_RIGHT)
			{// 目標が右
				time = 0.0f;
				m_fChaseLerpStart = m_fChaseDistance;	// 追従補正の初期値
				m_fDestChaseDistance = CHASEDISTANCE_DEST;
			}
			else if (pPlayer->GetMoveAngle() == CObject::ANGLE_LEFT && pPlayer->GetOldMoveAngle() != CObject::ANGLE_LEFT)
			{// 目標が右
				time = 0.0f;
				m_fChaseLerpStart = m_fChaseDistance;	// 追従補正の初期値
				m_fDestChaseDistance = -CHASEDISTANCE_DEST;
			}
			else if (
				(pPlayer->GetMoveAngle() == CObject::ANGLE_UP && pPlayer->GetOldMoveAngle() != CObject::ANGLE_UP) ||
				(pPlayer->GetMoveAngle() == CObject::ANGLE_DOWN && pPlayer->GetOldMoveAngle() != CObject::ANGLE_DOWN))
			{// 目標が右
				time = 0.0f;
				m_fChaseLerpStart = m_fChaseDistance;	// 追従補正の初期値
				m_fDestChaseDistance = 0.0f;
			}
			time += CManager::GetInstance()->DeltaTime() * 0.5f;
#if 0
			m_fChaseDistance = Lerp(m_fChaseLerpStart, m_fDestChaseDistance, time);
#else
			m_fChaseDistance += (m_fDestChaseDistance - m_fChaseDistance) * 0.02f;
#endif
			// 少し先の地点取得
			float fMoveValue = fMapMoveValue + m_fChaseDistance;
			DestPoint = pMapManager->GetTargetPosition(nMapIdx, fMoveValue);
		}
		else
		{
			DestPoint = PlayerPos;
		}

		// 注視点の代入処理
		m_posRDest.x = DestPoint.x;
		m_posRDest.z = DestPoint.z;
		m_posRDest.y = fYcamera - m_fDiffHeight;

		// 補正する
		float fHosei = POSR_HOSEI;
		if (m_nCntChaseType != -1)
		{
			float fBairitu = 1.0f - ((float)m_nCntChaseType / 150.0f);
			fHosei *= fBairitu;
		}

		m_posR += (m_posRDest - m_posR) * fHosei;

	}
}

//==================================================================================
// カメラの注視点代入処理(リザルト)
//==================================================================================
void CCamera::SetCameraRResult(void)
{
	// 注視点の代入処理
	m_posR.x = m_posV.x + cosf(m_rot.z) * sinf(m_rot.y) * m_fDistance;
	m_posR.z = m_posV.z + cosf(m_rot.z) * cosf(m_rot.y) * m_fDistance;
	m_posR.y = m_posV.y + sinf(m_rot.z) * m_fDistance;
}

//==================================================================================
// スポットライトのベクトル更新
//==================================================================================
void CCamera::UpdateSpotLightVec(void)
{
	// 方向ベクトル
	D3DXVECTOR3 vec = mylib_const::DEFAULT_VECTOR3;

	// 視点から注視点への向き
	vec = m_posR - m_posV;

	// 正規化
	D3DXVec3Normalize(&vec, &vec);

	// スポットライトの方向設定
	CManager::GetInstance()->GetLight()->UpdateSpotLightDirection(vec);

}

//==================================================================================
// 通常の追従
//==================================================================================
void CCamera::ChaseNormal(void)
{
	// プレイヤーの情報取得
	CPlayer *pPlayer = CManager::GetInstance()->GetScene()->GetPlayer();

	if (pPlayer == NULL)
	{
		return;
	}

	D3DXVECTOR3 PlayerPos = pPlayer->GetPosition();
	D3DXVECTOR3 PlayerRot = pPlayer->GetRotation();

	float fYcamera = 100.0f;

	// 目標の視点の向き
	m_rotVDest.y = atan2f((m_TargetPos.x - PlayerPos.x), (m_TargetPos.z - PlayerPos.z));

	//現在と目標の差分を求める
	float fRotDiff = m_rotVDest.y - m_rot.y;
	RotNormalize(fRotDiff);

	float fHosei = ROTHOSEI;
	if (m_nCntChaseType != -1)
	{
		float fBairitu = 1.0f - ((float)m_nCntChaseType / 150.0f);
		fHosei *= fBairitu;
	}

	m_rot.y += fRotDiff * fHosei;
	RotNormalize(m_rot.y);

	// 視点の代入処理
	m_posVDest.x = m_posR.x + cosf(m_rot.z) * sinf(m_rot.y) * -m_fDistance;
	m_posVDest.z = m_posR.z + cosf(m_rot.z) * cosf(m_rot.y) * -m_fDistance;
	m_posVDest.y = m_posR.y + sinf(m_rot.z) * -m_fDistance;

	float fDistance = 0.0f;
	m_fHeightMaxDest = m_posVDest.y;
	// 目標の角度を求める
	float fRotDest = atan2f((m_posVDest.x - m_posR.x), (m_posVDest.z - m_posR.z));
	while (1)
	{
		// プレイヤーの情報取得
		CPlayer *pPlayer = CManager::GetInstance()->GetScene()->GetPlayer();

		if (pPlayer == NULL)
		{
			return;
		}

		D3DXVECTOR3 PlayerPos = pPlayer->GetPosition();

		// 仮想の弾の位置
		float fPosBulletX = PlayerPos.x + cosf(m_rot.z) * sinf(m_rot.y) * -fDistance;
		float fPosBulletZ = PlayerPos.z + cosf(m_rot.z) * cosf(m_rot.y) * -fDistance;

		// 高さ取得
		bool bLand = false;
		float fHeight = CGame::GetElevation()->GetHeight(D3DXVECTOR3(fPosBulletX, 0.0f, fPosBulletZ), bLand);

		if (m_fHeightMaxDest <= fHeight)
		{// 最大の高さを更新したら

			// 距離の応じた割合保存
			float fDistanceRatio = fDistance / (m_fDistance);

			// 目標の最大高さ保存
			m_fHeightMaxDest = fHeight * (fDistanceRatio + 1.0f);
		}

		// 長さ加算
		fDistance += 10.0f;

		if (fDistance >= m_fDistance)
		{// 長さを超えたら終わり
			break;
		}
	}

	// 目標の視点更新
	if (m_fHeightMaxDest > m_posVDest.y)
	{
		// 最大の高さ補正
		m_fHeightMax += (m_fHeightMaxDest - m_fHeightMax) * 0.08f;

		m_posVDest.y = m_fHeightMax;

		// 高さの差分
		m_fDiffHeightSave += m_fHeightMax - m_posV.y;
	}

	// 補正する
	fHosei = POSV_HOSEI;
	if (m_nCntChaseType != -1)
	{
		float fBairitu = 1.0f - ((float)m_nCntChaseType / 150.0f);
		fHosei *= fBairitu;
	}
	m_posV += (m_posVDest - m_posV) * fHosei;
}

//==================================================================================
// マップに沿った追従
//==================================================================================
void CCamera::ChaseMap(void)
{
	// プレイヤーの情報取得
	CPlayer *pPlayer = CManager::GetInstance()->GetScene()->GetPlayer();
	if (pPlayer == NULL)
	{// NULLだったら
		return;
	}

	D3DXVECTOR3 PlayerPos = pPlayer->GetPosition();
	// マップマネージャの取得
	CMapManager *pMapManager = CGame::GetMapManager();
	if (pMapManager == NULL)
	{// NULLだったら
		return;
	}

	// マップ情報取得
	int nMapIdx = pPlayer->GetMapIndex();
	float fMapRatio = pPlayer->GetMapPointRatio();
	float fMapMoveValue = pPlayer->GetMapMoveValue();

	// 曲線作る為の4点
	int nP0 = nMapIdx;
	int nP1 = nMapIdx + 1;
	int nP2 = nMapIdx + 2;

	// 目標地点
	D3DXVECTOR3 TargetPoint0 = pMapManager->GetControlPoint(nP0);
	D3DXVECTOR3 TargetPoint1 = pMapManager->GetControlPoint(nP1);
	D3DXVECTOR3 TargetPoint2 = pMapManager->GetControlPoint(nP2);

	int nAngle = 1;
	if (pPlayer->GetMoveAngle() == CObject::ANGLE_LEFT)
	{
		nAngle = -1;
	}

	// 少し先の地点取得
	float fMoveValue = fMapMoveValue + 50.0f * nAngle;
	D3DXVECTOR3 DestPoint = pMapManager->GetTargetPosition(nMapIdx, fMoveValue);

	// ベクトル
	D3DXVECTOR3 vec = mylib_const::DEFAULT_VECTOR3;
	D3DXVECTOR3 newvec = mylib_const::DEFAULT_VECTOR3;

	// 2点間の距離取得
	float fPosLength = GetPosLength(TargetPoint1, TargetPoint2);

	// ベクトル
	if (fMapMoveValue < fMoveValue)
	{
		vec = DestPoint - PlayerPos;
	}
	else
	{
		vec = PlayerPos - DestPoint;
	}
	D3DXVec3Normalize(&vec, &vec);

	// 90度傾ける
	newvec.x = vec.z;
	newvec.z = -vec.x;

	// 目標の視点の向き
	m_rotVDest.y = atan2f((0.0f - newvec.x), (0.0f - newvec.z));

	//現在と目標の差分を求める
	float fRotDiff = m_rotVDest.y - m_rot.y;
	RotNormalize(fRotDiff);

	float fHosei = ROTHOSEI;
	if (m_nCntChaseType != -1)
	{
		float fBairitu = 1.0f - ((float)m_nCntChaseType / 150.0f);
		fHosei *= fBairitu;
	}

	m_rot.y += fRotDiff * fHosei;
	RotNormalize(m_rot.y);

	// 視点の代入処理
	m_posVDest.x = m_posR.x + cosf(m_rot.z) * sinf(m_rot.y) * -m_fDistance;
	m_posVDest.z = m_posR.z + cosf(m_rot.z) * cosf(m_rot.y) * -m_fDistance;
	m_posVDest.y = m_posR.y + sinf(m_rot.z) * -m_fDistance;


	float fDistance = 0.0f;
	m_fHeightMaxDest = m_posVDest.y;
	// 目標の角度を求める
	float fRotDest = atan2f((m_posVDest.x - m_posR.x), (m_posVDest.z - m_posR.z));

	// 地面との接触判定
	while (1)
	{
		// プレイヤーの情報取得
		CPlayer *pPlayer = CManager::GetInstance()->GetScene()->GetPlayer();

		if (pPlayer == NULL)
		{
			return;
		}

		D3DXVECTOR3 PlayerPos = pPlayer->GetPosition();

		// 仮想の弾の位置
		float fPosBulletX = PlayerPos.x + cosf(m_rot.z) * sinf(m_rot.y) * -fDistance;
		float fPosBulletZ = PlayerPos.z + cosf(m_rot.z) * cosf(m_rot.y) * -fDistance;

		// 高さ取得
		bool bLand = false;
		float fHeight = CGame::GetElevation()->GetHeight(D3DXVECTOR3(fPosBulletX, 0.0f, fPosBulletZ), bLand);

		if (m_fHeightMaxDest <= fHeight)
		{// 最大の高さを更新したら

			// 距離の応じた割合保存
			float fDistanceRatio = fDistance / (m_fDistance);

			// 目標の最大高さ保存
			m_fHeightMaxDest = fHeight * (fDistanceRatio + 1.0f);
		}

		// 長さ加算
		fDistance += 10.0f;

		if (fDistance >= m_fDistance)
		{// 長さを超えたら終わり
			break;
		}
	}

	// 目標の視点更新
	if (m_fHeightMaxDest > m_posVDest.y)
	{
		// 最大の高さ補正
		m_fHeightMax += (m_fHeightMaxDest - m_fHeightMax) * 0.08f;

		m_posVDest.y = m_fHeightMax;

		// 高さの差分
		m_fDiffHeightSave += m_fHeightMax - m_posV.y;
	}

	fHosei = POSV_HOSEI;
	if (m_nCntChaseType != -1)
	{
		float fBairitu = 1.0f - ((float)m_nCntChaseType / 150.0f);
		fHosei *= fBairitu;
	}
	// 補正する
	m_posV += (m_posVDest - m_posV) * fHosei;

}

//==================================================================================
// 追従なし
//==================================================================================
void CCamera::ChaseNone(void)
{


	// マップマネージャの取得
	CMapManager *pMapManager = CGame::GetMapManager();
	if (pMapManager == NULL)
	{// NULLだったら
		return;
	}

	// マップ情報取得
	int nMapIdx = 37;
	float fMapRatio = 0.0f;
	float fMapMoveValue = 0.0f;

	// 曲線作る為の4点
	int nP0 = nMapIdx;
	int nP1 = nMapIdx + 1;
	int nP2 = nMapIdx + 2;

	// 目標地点
	D3DXVECTOR3 TargetPoint0 = pMapManager->GetControlPoint(nP0);
	D3DXVECTOR3 TargetPoint1 = pMapManager->GetControlPoint(nP1);
	D3DXVECTOR3 TargetPoint2 = pMapManager->GetControlPoint(nP2);

	int nAngle = 1;

	// 少し先の地点取得
	float fMoveValue = fMapMoveValue + 50.0f * nAngle;
	D3DXVECTOR3 DestPoint = pMapManager->GetTargetPosition(nMapIdx, fMoveValue);

	// ベクトル
	D3DXVECTOR3 vec = mylib_const::DEFAULT_VECTOR3;
	D3DXVECTOR3 newvec = mylib_const::DEFAULT_VECTOR3;

	// 2点間の距離取得
	float fPosLength = GetPosLength(TargetPoint1, TargetPoint2);

	// ベクトル
	vec = TargetPoint2 - TargetPoint1;
	D3DXVec3Normalize(&vec, &vec);

	// 90度傾ける
	newvec.x = vec.z;
	newvec.z = -vec.x;

	// 目標の視点の向き
	m_rotVDest.y = atan2f((0.0f - newvec.x), (0.0f - newvec.z));

	//現在と目標の差分を求める
	float fRotDiff = m_rotVDest.y - m_rot.y;
	RotNormalize(fRotDiff);

	m_rot.y += fRotDiff * (ROTHOSEI * 0.1f);
	RotNormalize(m_rot.y);






	// 注視点の代入処理
	m_posRDest.x = m_TargetPos.x;
	m_posRDest.z = m_TargetPos.z;
	m_posRDest.y = m_TargetPos.y + 100.0f;

	// 補正する
	m_posR += (m_posRDest - m_posR) * (POSR_HOSEI * 0.1f);

	// 視点の代入処理
	m_posVDest.x = m_posR.x + cosf(m_rot.z) * sinf(m_rot.y) * -m_fDistance;
	m_posVDest.z = m_posR.z + cosf(m_rot.z) * cosf(m_rot.y) * -m_fDistance;
	m_posVDest.y = m_posR.y + sinf(m_rot.z) * -m_fDistance;

	// 補正する
	m_posV += (m_posVDest - m_posV) * (POSV_HOSEI * 0.1f);
}

//==================================================================================
// カメラの背面自動追従
//==================================================================================
void CCamera::MoveCameraFollow(void)
{

}

//==================================================================================
// カメラの設定処理
//==================================================================================
void CCamera::SetCamera(void)
{
	// デバイスの取得
	LPDIRECT3DDEVICE9 pDevice = CManager::GetInstance()->GetRenderer()->GetDevice();

	// ビューポートの設定
	pDevice->SetViewport(&m_viewport);

	// プロジェクションマトリックスの初期化
	D3DXMatrixIdentity(&m_mtxProjection);

	// プロジェクションマトリックスの作成
	D3DXMatrixPerspectiveFovLH(&m_mtxProjection,
								D3DXToRadian(45.0f),	// 視野角
								(float)m_viewport.Width / (float)m_viewport.Height,	// アスペクト比
								10.0f,		// 奥行きの制限
								20000.0f);	// 奥行きの制限

	// プロジェクションマトリックスの設定
	pDevice->SetTransform(D3DTS_PROJECTION, &m_mtxProjection);

	// ビューマトリックスの初期化
	D3DXMatrixIdentity(&m_mtxView);

	// ビューマトリックスの作成
	D3DXMatrixLookAtLH(&m_mtxView,
		&m_posV,	// 視点
		&m_posR,	// 注視点
		&m_vecU);	// 上方向ベクトル

	// ビューマトリックスの設定
	pDevice->SetTransform(D3DTS_VIEW, &m_mtxView);

}

//==================================================================================
// 目標の長さ設定
//==================================================================================
void CCamera::SetLenDest(float fLength, int nCntTime, float DecrementValue, float fCorrection)
{
	// 目標の距離設定
	m_fDestDistance = fLength;
	m_nCntDistance = nCntTime;
	m_nOriginCntDistance = m_nCntDistance;	// 元の距離カウンター
	m_fDistanceCorrection = fCorrection;	// 減少補正係数
	m_fDistanceDecrementValue = DecrementValue;	// 減少量
}

//==================================================================================
// カメラの振動設定処理
//==================================================================================
void CCamera::SetShake(int nTime, float fLength, float fLengthY)
{
	// 振動状態に設定
	m_state = CAMERASTATE_SHAKE;
	m_nCntState = nTime;		// 状態遷移カウンター
	m_nShakeLength = fLength;	// 揺れの大きさ
	m_nShakeLengthY = fLengthY;	// Yの揺れの大きさ
}

//==================================================================================
// 目標の位置設定
//==================================================================================
void CCamera::SetTargetPosition(const D3DXVECTOR3 pos)
{
	// 目標の位置
	m_TargetPos = pos;
}

//==================================================================================
// 目標の位置取得
//==================================================================================
D3DXVECTOR3 CCamera::GetTargetPosition(void)
{
	return m_TargetPos;
}

//==================================================================================
// 追従の種類設定
//==================================================================================
void CCamera::SetChaseType(CHASETYPE type)
{
	m_ChaseType = type;
}

//==================================================================================
// 追従の種類取得
//==================================================================================
CCamera::CHASETYPE CCamera::GetChaseType(void)
{
	return m_ChaseType;
}


//==================================================================================
// カメラの振動処理
//==================================================================================
void CCamera::Shake(void)
{
	// 長さ取得
	int nLength = (int)m_nShakeLength;
	int nLengthY = (int)m_nShakeLengthY;

	m_fMoveShake = (float)Random(-nLength, nLength) * sinf((float)Random(-314, 314) * 0.01f);	// 揺れの移動量
	m_fMoveShakeY = (float)Random(-nLengthY, nLengthY);						// Yの揺れの移動量

	// 視点の代入処理
	m_posV.x += (float)Random(-nLength, nLength) * sinf((float)Random(-314, 314) * 0.01f);
	m_posV.y += (float)Random(-nLengthY, nLengthY);
	m_posV.z += (float)Random(-nLength, nLength) * cosf((float)Random(-314, 314) * 0.01f);

	// 注視点の代入処理
	m_posR.x += (float)Random(-nLength, nLength) * sinf((float)Random(-314, 314) * 0.01f);
	m_posR.y += (float)Random(-nLengthY, nLengthY);
	m_posR.z += (float)Random(-nLength, nLength) * cosf((float)Random(-314, 314) * 0.01f);

	// 慣性補正
	InertiaCorrection(m_nShakeLength, 0.0f, 0.1f);
	InertiaCorrection(m_nShakeLengthY, 0.0f, 0.1f);
}

//==================================================================================
// カメラの状態更新処理
//==================================================================================
void CCamera::UpdateState(void)
{
	switch (m_state)
	{
	case CAMERASTATE_NONE:
		break;

	case CAMERASTATE_SHAKE:

		if (m_nCntState % 3 == 0)
		{
			// 画面揺れ
			Shake();
		}
		else
		{
			// 視点の代入処理
			m_posV.x += m_fMoveShake;
			m_posV.y += m_fMoveShakeY;
			m_posV.z += m_fMoveShake;

			// 注視点の代入処理
			m_posR.x += m_fMoveShake;
			m_posR.y += m_fMoveShakeY;
			m_posR.z += m_fMoveShake;

			// 慣性補正
			InertiaCorrection(m_nShakeLength, 0.0f, 0.01f);
			InertiaCorrection(m_nShakeLengthY, 0.0f, 0.01f);
		}

		// 状態遷移カウンター減算
		m_nCntState--;

		if (m_nCntState <= 0)
		{
			m_nCntState = 0;
			m_state = CAMERASTATE_NONE;
		}
		break;
	}
}

//==========================================================================
// リセット
//==========================================================================
void CCamera::Reset(CScene::MODE mode)
{
	
	m_bFollow = true;										// 追従するかどうか

	switch (mode)
	{
	case CScene::MODE_NONE:
		break;

	case CScene::MODE_TITLE:
		ResetTitle();
		break;

	case CScene::MODE_TUTORIAL:
		ResetGame();
		break;

	case CScene::MODE_GAME:
		ResetGame();
		break;

	case CScene::MODE_RESULT:
		ResetResult();
		break;

	case CScene::MODE_RANKING:
		ResetRanking();
		break;

	case CScene::MODE_MAX:
		break;

	default:
		break;
	}

	// 視点の代入処理
	SetCameraV();
}

//==========================================================================
// ゲームのリセット
//==========================================================================
void CCamera::ResetGame(void)
{
	m_posR = D3DXVECTOR3(0.0f, 200.0f, 0.0f);				// 注視点(見たい場所)
	m_posV = D3DXVECTOR3(0.0f, 300.0f, m_posR.z + -400.0f);	// 視点(カメラの位置)
	m_posVDest = m_posV;									// 目標の視点
	m_posRDest = m_posR;									// 目標の注視点
	m_vecU = D3DXVECTOR3(0.0f, 1.0f, 0.0f);					// 上方向ベクトル
	m_move = D3DXVECTOR3(0.0f, 0.0f, 0.0f);					// 移動量
	m_rot = D3DXVECTOR3(0.0f, 0.0f, -0.15f);					// 向き
	m_rotVDest = m_rot;										// 目標の視点の向き
	m_TargetPos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);			// 目標の位置
	m_fDistance = START_CAMERALEN;							// 距離
	m_fDestDistance = START_CAMERALEN;						// 目標の距離
	m_fOriginDistance = START_CAMERALEN;					// 元の距離
	m_fDiffHeight = 0.0f;									// 高さの差分
	m_fDiffHeightSave = 0.0f;								// 高さの差分保存用
	m_fDiffHeightDest = 0.0f;								// 目標の高さの差分
	m_state = CAMERASTATE_NONE;								// 状態
	m_nCntState = 0;							// 状態カウンター
	m_nCntDistance = 0;							// 距離カウンター
	m_nOriginCntDistance = 0;					// 元の距離カウンター
	m_fDistanceCorrection = 0;					// 距離の慣性補正係数
	m_fDistanceDecrementValue = 0.0f;			// 距離の減少係数
	m_fHeightMaxDest = 0.0f;					// カメラの最大高さの目標
	m_ChaseType = CHASETYPE_MAP;				// 追従の種類
	m_OldChaseType = m_ChaseType;			// 前回の追従の種類
}

//==========================================================================
// リセット
//==========================================================================
void CCamera::ResetBoss(void)
{
	m_posR = CManager::GetInstance()->GetScene()->GetPlayer()->GetPosition();				// 注視点(見たい場所)
	m_posV = D3DXVECTOR3(0.0f, 300.0f, m_posR.z + -400.0f);	// 視点(カメラの位置)
	m_posVDest = m_posV;									// 目標の視点
	m_posRDest = m_posR;									// 目標の注視点
	m_vecU = D3DXVECTOR3(0.0f, 1.0f, 0.0f);					// 上方向ベクトル
	m_move = D3DXVECTOR3(0.0f, 0.0f, 0.0f);					// 移動量
	m_rot = D3DXVECTOR3(0.0f, 0.0f, -0.25f);					// 向き
	m_rotVDest = m_rot;										// 目標の視点の向き
	m_TargetPos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);			// 目標の位置
	m_fDistance = 1600.0f;							// 距離
	m_fDestDistance = m_fDistance;						// 目標の距離
	m_fOriginDistance = m_fDistance;					// 元の距離
	m_fDiffHeight = 0.0f;									// 高さの差分
	m_fDiffHeightSave = 0.0f;								// 高さの差分保存用
	m_fDiffHeightDest = 0.0f;								// 目標の高さの差分
	m_state = CAMERASTATE_NONE;								// 状態
	m_nCntState = 0;							// 状態カウンター
	m_nCntDistance = 0;							// 距離カウンター
	m_nOriginCntDistance = 0;					// 元の距離カウンター
	m_fDistanceCorrection = 0;					// 距離の慣性補正係数
	m_fDistanceDecrementValue = 0.0f;			// 距離の減少係数
	m_fHeightMaxDest = 0.0f;					// カメラの最大高さの目標
	m_ChaseType = CHASETYPE_MAP;				// 追従の種類
	m_OldChaseType = m_ChaseType;			// 前回の追従の種類

	// プレイヤーの情報取得
	CPlayer *pPlayer = CManager::GetInstance()->GetScene()->GetPlayer();
	if (pPlayer == NULL)
	{// NULLだったら
		return;
	}

	D3DXVECTOR3 PlayerPos = pPlayer->GetPosition();

	// 注視点の代入
	m_posR = D3DXVECTOR3(PlayerPos.x, PlayerPos.y + 100.0f, PlayerPos.z);	// 注視点(見たい場所)

	// 目標の視点の向き
	m_rot.y = atan2f((m_TargetPos.x - PlayerPos.x), (m_TargetPos.z - PlayerPos.z));
	m_rotVDest.y = m_rot.y;

	// 視点の代入
	m_posV.x = m_posR.x + cosf(m_rot.z) * sinf(m_rot.y) * -m_fDistance;
	m_posV.z = m_posR.z + cosf(m_rot.z) * cosf(m_rot.y) * -m_fDistance;
	m_posV.y = m_posR.y + sinf(m_rot.z) * -m_fDistance;
	m_posVDest = m_posV;								// 目標の視点
	m_posRDest = m_posR;								// 目標の注視点
}

//==========================================================================
// タイトルのリセット
//==========================================================================
void CCamera::ResetTitle(void)
{
	m_posR = D3DXVECTOR3(0.0f, 0.0f, 0.0f);	// 注視点(見たい場所)
	m_posV = D3DXVECTOR3(2401.44f, 1542.82f, -2355.46f);	// 視点(カメラの位置)
	m_posVDest = m_posV;								// 目標の視点
	m_posRDest = m_posR;								// 目標の注視点
	m_vecU = D3DXVECTOR3(0.0f, 1.0f, 0.0f);				// 上方向ベクトル
	m_move = D3DXVECTOR3(0.0f, 0.0f, 0.0f);				// 移動量
	m_rot = TITLECAMERAROT_NONE;						// 向き
	m_rotVDest = m_rot;									// 目標の視点の向き
	m_TargetPos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);		// 目標の位置
	m_fDistance = TITLE_LEN_DEST;						// 距離
	m_fDestDistance = TITLE_LEN_DEST;					// 目標の距離
	m_fOriginDistance = TITLE_LEN_DEST;					// 元の距離
	m_fDiffHeight = 0.0f;								// 高さの差分
	m_fDiffHeightSave = 0.0f;							// 高さの差分保存用
	m_fDiffHeightDest = 0.0f;							// 目標の高さの差分
	m_state = CAMERASTATE_NONE;							// 状態
	m_nCntState = 0;									// 状態カウンター
	m_nCntDistance = 0;									// 距離カウンター
	m_nOriginCntDistance = 0;							// 元の距離カウンター
	m_fDistanceCorrection = 0;							// 距離の慣性補正係数
	m_fDistanceDecrementValue = 0.0f;					// 距離の減少係数
	m_fHeightMaxDest = 0.0f;							// カメラの最大高さの目標
	m_ChaseType = CHASETYPE_MAP;				// 追従の種類
	m_OldChaseType = m_ChaseType;			// 前回の追従の種類
}

//==========================================================================
// リザルトリセット
//==========================================================================
void CCamera::ResetResult(void)
{
	m_vecU = D3DXVECTOR3(0.0f, 1.0f, 0.0f);				// 上方向ベクトル
	m_move = D3DXVECTOR3(0.0f, 0.0f, 0.0f);				// 移動量
	//m_rot = D3DXVECTOR3(0.0f, 0.0f, -0.35f);			// 向き
	m_rot = D3DXVECTOR3(0.0f, 0.03f, -0.15f);			// 向き
	m_rotVDest = m_rot;									// 目標の視点の向き
	m_TargetPos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);		// 目標の位置
	m_fDistance = RESULT_LEN;						// 距離
	m_fDestDistance = RESULT_LEN;					// 目標の距離
	m_fOriginDistance = RESULT_LEN;					// 元の距離
	m_fDiffHeight = 0.0f;								// 高さの差分
	m_fDiffHeightSave = 0.0f;							// 高さの差分保存用
	m_fDiffHeightDest = 0.0f;							// 目標の高さの差分
	m_state = CAMERASTATE_NONE;							// 状態
	m_nCntState = 0;									// 状態カウンター
	m_nCntDistance = 0;									// 距離カウンター
	m_nOriginCntDistance = 0;							// 元の距離カウンター
	m_fDistanceCorrection = 0;							// 距離の慣性補正係数
	m_fDistanceDecrementValue = 0.0f;					// 距離の減少係数
	m_fHeightMaxDest = 0.0f;							// カメラの最大高さの目標
	m_ChaseType = CHASETYPE_MAP;				// 追従の種類
	m_OldChaseType = m_ChaseType;			// 前回の追従の種類

	// 注視点の代入
	m_posR = D3DXVECTOR3(-57.76f, 312.0f, 76.42f);	// 注視点(見たい場所)

	// 視点の代入
	m_posV.x = m_posR.x + cosf(m_rot.z) * sinf(m_rot.y) * -m_fDistance;
	m_posV.z = m_posR.z + cosf(m_rot.z) * cosf(m_rot.y) * -m_fDistance;
	m_posV.y = m_posR.y + sinf(m_rot.z) * -m_fDistance;
	m_posVDest = m_posV;								// 目標の視点
	m_posRDest = m_posR;								// 目標の注視点
}

//==========================================================================
// ランキングのリセット
//==========================================================================
void CCamera::ResetRanking(void)
{
	m_posR = D3DXVECTOR3(0.0f, 0.0f, 0.0f);	// 注視点(見たい場所)
	m_posV = D3DXVECTOR3(2401.44f, 1542.82f, -2355.46f);	// 視点(カメラの位置)
	m_posVDest = m_posV;								// 目標の視点
	m_posRDest = m_posR;								// 目標の注視点
	m_vecU = D3DXVECTOR3(0.0f, 1.0f, 0.0f);				// 上方向ベクトル
	m_move = D3DXVECTOR3(0.0f, 0.0f, 0.0f);				// 移動量
	m_TargetPos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);		// 目標の位置
	m_fDistance = RANKNIG_LEN_DEST;						// 距離
	m_fDestDistance = RANKNIG_LEN_DEST;					// 目標の距離
	m_fOriginDistance = RANKNIG_LEN_DEST;				// 元の距離
	m_fDiffHeight = 0.0f;								// 高さの差分
	m_fDiffHeightSave = 0.0f;							// 高さの差分保存用
	m_fDiffHeightDest = 0.0f;							// 目標の高さの差分
	m_state = CAMERASTATE_NONE;							// 状態
	m_nCntState = 0;									// 状態カウンター
	m_nCntDistance = 0;									// 距離カウンター
	m_nOriginCntDistance = 0;							// 元の距離カウンター
	m_fDistanceCorrection = 0;							// 距離の慣性補正係数
	m_fDistanceDecrementValue = 0.0f;					// 距離の減少係数
	m_fHeightMaxDest = 0.0f;							// カメラの最大高さの目標
	m_ChaseType = CHASETYPE_MAP;				// 追従の種類
	m_OldChaseType = m_ChaseType;			// 前回の追従の種類

	m_rot = RANKINGROT_NONE;							// 向き
	m_rotVDest = m_rot;									// 目標の視点の向き

	// 注視点の代入
	m_posR = D3DXVECTOR3(0.0f, 400.0f, 0.0f);			// 注視点(見たい場所)

	// 視点の代入
	m_posV.x = m_posR.x + cosf(m_rot.z) * sinf(m_rot.y) * -m_fDistance;
	m_posV.z = m_posR.z + cosf(m_rot.z) * cosf(m_rot.y) * -m_fDistance;
	m_posV.y = m_posR.y + sinf(m_rot.z) * -m_fDistance;
	m_posVDest = m_posV;								// 目標の視点
	m_posRDest = m_posR;								// 目標の注視点
}

//==========================================================================
// カメラの位置取得
//==========================================================================
D3DXVECTOR3 CCamera::GetPositionV(void) const
{
	return m_posV;
}

//==========================================================================
// カメラの注視点取得
//==========================================================================
D3DXVECTOR3 CCamera::GetPositionR(void) const
{
	return m_posR;
}

//==========================================================================
// 向き設定
//==========================================================================
void CCamera::SetRotation(const D3DXVECTOR3 rot)
{
	m_rot = rot;
}

//==========================================================================
// 向き取得
//==========================================================================
D3DXVECTOR3 CCamera::GetRotation(void) const
{
	return m_rot;
}

//==========================================================================
// 目標の向き設定
//==========================================================================
void CCamera::SetDestRotation(const D3DXVECTOR3 rot)
{
	m_rotVDest = rot;
}

//==========================================================================
// 目標の向き取得
//==========================================================================
D3DXVECTOR3 CCamera::GetDestRotation(void)
{
	return m_rotVDest;
}

//==========================================================================
// 元になるカメラの距離設定
//==========================================================================
void CCamera::SetOriginDistance(float fDistance)
{
	m_fOriginDistance = fDistance;
}

//==========================================================================
// 元になるカメラの距離取得
//==========================================================================
float CCamera::GetOriginDistance(void)
{
	return m_fOriginDistance;
}
