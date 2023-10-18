//=============================================================================
// 
//  制御点エディット処理 [edit_controlpoint.cpp]
//  Author : 相馬靜雅
// 
//=============================================================================
#include "edit_controlpoint.h"
#include "objectX.h"
#include "XLoad.h"
#include "manager.h"
#include "renderer.h"
#include "calculation.h"
#include "debugproc.h"
#include "input.h"
#include "elevation.h"
#include "camera.h"
#include "mapmanager.h"
#include "3D_effect.h"

//==========================================================================
// マクロ定義
//==========================================================================
#define MOVE		(10.0f)
#define MOVE_SLOW	(1.0f)

//==========================================================================
// 静的メンバ変数宣言
//==========================================================================

//==========================================================================
// コンストラクタ
//==========================================================================
CEditControlPoint::CEditControlPoint()
{
	// ゼロクリア
	m_pos = mylib_const::DEFAULT_VECTOR3;	// 位置

}

//==========================================================================
// デストラクタ
//==========================================================================
CEditControlPoint::~CEditControlPoint()
{

}

//==========================================================================
// 生成処理
//==========================================================================
CEditControlPoint *CEditControlPoint::Create(void)
{
	// 生成用のオブジェクト
	CEditControlPoint *pObjectX = NULL;

	if (pObjectX == NULL)
	{// NULLだったら

		// メモリの確保
		pObjectX = DEBUG_NEW CEditControlPoint;

		if (pObjectX != NULL)
		{// メモリの確保が出来ていたら

			// 初期化処理
			HRESULT hr = pObjectX->Init();
			if (FAILED(hr))
			{// 失敗していたら
				return NULL;
			}
		}

		return pObjectX;
	}

	return NULL;
}

//==========================================================================
// 初期化処理
//==========================================================================
HRESULT CEditControlPoint::Init(void)
{
	return S_OK;
}

//==========================================================================
// 終了処理
//==========================================================================
void CEditControlPoint::Uninit(void)
{

}

//==========================================================================
// 更新処理
//==========================================================================
void CEditControlPoint::Update(void)
{
	// キーボード情報取得
	CInputKeyboard *pInputKeyboard = CManager::GetInstance()->GetInputKeyboard();

	// 操作
	Control(m_pos);
	CEffect3D::Create(m_pos, D3DXVECTOR3(0.0f, 0.0f, 0.0f), D3DXCOLOR(1.0f, 0.6f, 0.1f, 1.0f), 50.0f, 2, CEffect3D::MOVEEFFECT_NONE, CEffect3D::TYPE_NORMAL);

	// 掴み処理
	if (pInputKeyboard->GetPress(DIK_SPACE) == true)
	{// SPACEで掴む
		Grab();
	}

	if (pInputKeyboard->GetTrigger(DIK_RETURN) == true)
	{// ENTERで配置

		// マップマネージャの取得
		if (CManager::GetInstance()->GetScene()->GetMapManager() == NULL)
		{
			return;
		}
		CMapManager *pMapManager = CManager::GetInstance()->GetScene()->GetMapManager();
		pMapManager->CreatePos(m_pos);
	}

	if (pInputKeyboard->GetTrigger(DIK_DELETE) == true)
	{// deleteで削除
		Delete();
	}

	if (pInputKeyboard->GetTrigger(DIK_F9) == true)
	{// セーブ

		// マップマネージャの取得
		if (CManager::GetInstance()->GetScene()->GetMapManager() == NULL)
		{
			return;
		}
		CMapManager *pMapManager = CManager::GetInstance()->GetScene()->GetMapManager();
		pMapManager->Save("data\\BIN\\maptarget.bin");
	}

	// デバッグ情報
	CManager::GetInstance()->GetDebugProc()->Print(
		"------------------[ エディット情報 ]------------------\n"
		"<配置>         [ENTER]\n"
		"<ファイル保存> [F9] 【data/TEXT/edit_info.txt】\n"
		"<移動>         高速：[↑,↓,←,→]　低速：[W,A,S,D]\n"
		"<上昇,下降>    [I,K]\n"
		"<掴み移動>     [SPACE]\n"
		"<削除>         [DELETE]\n"
		"<位置>         [X：%f Y：%f Z：%f]\n"
		"\n", m_pos.x, m_pos.y, m_pos.z);
}

//==========================================================================
// 操作処理
//==========================================================================
void CEditControlPoint::Control(D3DXVECTOR3 &pos)
{
	// カメラの情報取得
	CCamera *pCamera = CManager::GetInstance()->GetCamera();

	// カメラの向き取得
	D3DXVECTOR3 Camerarot = pCamera->GetRotation();

	// キーボード情報取得
	CInputKeyboard *pInputKeyboard = CManager::GetInstance()->GetInputKeyboard();

	if (pInputKeyboard->GetPress(DIK_LEFT) == true)
	{// ←キーが押された,左移動

		if (pInputKeyboard->GetPress(DIK_UP) == true)
		{// A+W,左上移動

			pos.x += sinf(-D3DX_PI * MOVE_LRDW + Camerarot.y) * MOVE;
			pos.z += cosf(-D3DX_PI * MOVE_LRDW + Camerarot.y) * MOVE;
		}
		else if (pInputKeyboard->GetPress(DIK_DOWN) == true)
		{// A+S,左下移動

			pos.x += sinf(-D3DX_PI * MOVE_LRUP + Camerarot.y) * MOVE;
			pos.z += cosf(-D3DX_PI * MOVE_LRUP + Camerarot.y) * MOVE;
		}
		else
		{// A,左移動

			pos.x += sinf(-D3DX_PI * MOVE_LR + Camerarot.y) * MOVE;
			pos.z += cosf(-D3DX_PI * MOVE_LR + Camerarot.y) * MOVE;
		}
	}
	else if (pInputKeyboard->GetPress(DIK_RIGHT) == true)
	{// Dキーが押された,右移動

		if (pInputKeyboard->GetPress(DIK_UP) == true)
		{// D+W,右上移動

			pos.x += sinf(D3DX_PI * MOVE_LRDW + Camerarot.y) * MOVE;
			pos.z += cosf(D3DX_PI * MOVE_LRDW + Camerarot.y) * MOVE;
		}
		else if (pInputKeyboard->GetPress(DIK_DOWN) == true)
		{// D+S,右下移動

			pos.x += sinf(D3DX_PI * MOVE_LRUP + Camerarot.y) * MOVE;
			pos.z += cosf(D3DX_PI * MOVE_LRUP + Camerarot.y) * MOVE;
		}
		else
		{// D,右移動

			pos.x += sinf(D3DX_PI * MOVE_LR + Camerarot.y) * MOVE;
			pos.z += cosf(D3DX_PI * MOVE_LR + Camerarot.y) * MOVE;
		}
	}
	else if (pInputKeyboard->GetPress(DIK_UP) == true)
	{// Wが押された、奥移動

		pos.x += sinf(Camerarot.y) * MOVE;
		pos.z += cosf(Camerarot.y) * MOVE;
	}
	else if (pInputKeyboard->GetPress(DIK_DOWN) == true)
	{// Sが押された、手前移動

		pos.x += sinf(D3DX_PI + Camerarot.y) * MOVE;
		pos.z += cosf(D3DX_PI + Camerarot.y) * MOVE;
	}

	if (pInputKeyboard->GetPress(DIK_A) == true)
	{// ←キーが押された,左移動

		if (pInputKeyboard->GetPress(DIK_W) == true)
		{// A+W,左上移動

			pos.x += sinf(-D3DX_PI * MOVE_LRDW + Camerarot.y) * MOVE_SLOW;
			pos.z += cosf(-D3DX_PI * MOVE_LRDW + Camerarot.y) * MOVE_SLOW;
		}
		else if (pInputKeyboard->GetPress(DIK_S) == true)
		{// A+S,左下移動

			pos.x += sinf(-D3DX_PI * MOVE_LRUP + Camerarot.y) * MOVE_SLOW;
			pos.z += cosf(-D3DX_PI * MOVE_LRUP + Camerarot.y) * MOVE_SLOW;
		}
		else
		{// A,左移動

			pos.x += sinf(-D3DX_PI * MOVE_LR + Camerarot.y) * MOVE_SLOW;
			pos.z += cosf(-D3DX_PI * MOVE_LR + Camerarot.y) * MOVE_SLOW;
		}
	}
	else if (pInputKeyboard->GetPress(DIK_D) == true)
	{// Dキーが押された,右移動

		if (pInputKeyboard->GetPress(DIK_W) == true)
		{// D+W,右上移動

			pos.x += sinf(D3DX_PI * MOVE_LRDW + Camerarot.y) * MOVE_SLOW;
			pos.z += cosf(D3DX_PI * MOVE_LRDW + Camerarot.y) * MOVE_SLOW;
		}
		else if (pInputKeyboard->GetPress(DIK_S) == true)
		{// D+S,右下移動

			pos.x += sinf(D3DX_PI * MOVE_LRUP + Camerarot.y) * MOVE_SLOW;
			pos.z += cosf(D3DX_PI * MOVE_LRUP + Camerarot.y) * MOVE_SLOW;
		}
		else
		{// D,右移動

			pos.x += sinf(D3DX_PI * MOVE_LR + Camerarot.y) * MOVE_SLOW;
			pos.z += cosf(D3DX_PI * MOVE_LR + Camerarot.y) * MOVE_SLOW;
		}
	}
	else if (pInputKeyboard->GetPress(DIK_W) == true)
	{// Wが押された、奥移動

		pos.x += sinf(Camerarot.y) * MOVE_SLOW;
		pos.z += cosf(Camerarot.y) * MOVE_SLOW;
	}
	else if (pInputKeyboard->GetPress(DIK_S) == true)
	{// Sが押された、手前移動

		pos.x += sinf(D3DX_PI + Camerarot.y) * MOVE_SLOW;
		pos.z += cosf(D3DX_PI + Camerarot.y) * MOVE_SLOW;
	}

	if (pInputKeyboard->GetPress(DIK_I) == true)
	{// Iキーが押された,上昇

		pos.y += MOVE;
	}
	else if (pInputKeyboard->GetPress(DIK_K) == true)
	{// Kキーが押された,下降

		pos.y -= MOVE;
	}


	// 着地したかどうか
	bool bLand = false;

	// 高さ取得
	float fHeight = CManager::GetInstance()->GetScene()->GetElevation()->GetHeight(pos, bLand);

	if (bLand == true)
	{
		// 高さ代入
		pos.y = fHeight;
	}
}

//==========================================================================
// モデル掴む
//==========================================================================
void CEditControlPoint::Grab(void)
{

	// キーボード情報取得
	CInputKeyboard *pInputKeyboard = CManager::GetInstance()->GetInputKeyboard();

	if (CManager::GetInstance()->GetScene()->GetMapManager() == NULL)
	{// NULLで抜ける
		return;
	}

	// マップマネージャ取得
	CMapManager *pMapManager = CManager::GetInstance()->GetScene()->GetMapManager();

	for (int i = 0; i < pMapManager->GetNumAll(); i++)
	{
		D3DXVECTOR3 TargetPoint = pMapManager->GetControlPoint(i);
		if (SphereRange(m_pos, TargetPoint, 50.0f, 50.0f))
		{// 球に当たってたら

			// 操作
			Control(TargetPoint);

			// 位置設定
			pMapManager->SetControlPoint(TargetPoint, i);
		}
	}
}

//==========================================================================
// モデル掴む
//==========================================================================
void CEditControlPoint::Delete(void)
{
	// キーボード情報取得
	CInputKeyboard *pInputKeyboard = CManager::GetInstance()->GetInputKeyboard();

	if (CManager::GetInstance()->GetScene()->GetMapManager() == NULL)
	{// NULLで抜ける
		return;
	}

	// マップマネージャ取得
	CMapManager *pMapManager = CManager::GetInstance()->GetScene()->GetMapManager();

	for (int i = 0; i < pMapManager->GetNumAll(); i++)
	{
		D3DXVECTOR3 TargetPoint = pMapManager->GetControlPoint(i);
		if (SphereRange(m_pos, TargetPoint, 50.0f, 50.0f))
		{// 球に当たってたら
			pMapManager->DeletePos(i);
		}
	}
}

//==========================================================================
// 描画処理
//==========================================================================
void CEditControlPoint::Draw(void)
{

}
