//=============================================================================
// 
//  カメラ追従種類変更エディット処理 [edit_camerachasechanger.cpp]
//  Author : 相馬靜雅
// 
//=============================================================================
#include "edit_camerachasechanger.h"
#include "camerachasechanger.h"
#include "objectX.h"
#include "XLoad.h"
#include "manager.h"
#include "renderer.h"
#include "calculation.h"
#include "debugproc.h"
#include "input.h"
#include "elevation.h"
#include "3D_effect.h"
#include "mapmanager.h"

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
CEditCameraChaseChanger::CEditCameraChaseChanger()
{
	// ゼロクリア
	m_pos = mylib_const::DEFAULT_VECTOR3;	// 位置
	m_ChaseType = CCamera::CHASETYPE_NORMAL;	// 追従の種類
	m_nIdxMapPoint = 0;			// マップポイントのインデックス番号
	m_fPointRatio = 0.0f;		// 移動割合
	m_fMoveValue = 0.0f;		// 移動量
}

//==========================================================================
// デストラクタ
//==========================================================================
CEditCameraChaseChanger::~CEditCameraChaseChanger()
{

}

//==========================================================================
// 生成処理
//==========================================================================
CEditCameraChaseChanger *CEditCameraChaseChanger::Create(void)
{
	// 生成用のオブジェクト
	CEditCameraChaseChanger *pObjectX = NULL;

	if (pObjectX == NULL)
	{// NULLだったら

		// メモリの確保
		pObjectX = DEBUG_NEW CEditCameraChaseChanger;

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
HRESULT CEditCameraChaseChanger::Init(void)
{
	m_pos = CManager::GetInstance()->GetCamera()->GetPositionR();
	return S_OK;
}

//==========================================================================
// 終了処理
//==========================================================================
void CEditCameraChaseChanger::Uninit(void)
{

}

//==========================================================================
// 更新処理
//==========================================================================
void CEditCameraChaseChanger::Update(void)
{
	// キーボード情報取得
	CInputKeyboard *pInputKeyboard = CManager::GetInstance()->GetInputKeyboard();

	// 操作
	Control(m_pos);

	switch (m_ChaseType)
	{
	case CCamera::CHASETYPE_NORMAL:
		CEffect3D::Create(m_pos, D3DXVECTOR3(0.0f, 0.0f, 0.0f), D3DXCOLOR(0.0f, 1.0f, 1.0f, 1.0f), 50.0f, 2, CEffect3D::MOVEEFFECT_NONE, CEffect3D::TYPE_NORMAL);
		break;

	case CCamera::CHASETYPE_MAP:
		CEffect3D::Create(m_pos, D3DXVECTOR3(0.0f, 0.0f, 0.0f), D3DXCOLOR(1.0f, 0.0f, 1.0f, 1.0f), 50.0f, 2, CEffect3D::MOVEEFFECT_NONE, CEffect3D::TYPE_NORMAL);
		break;
	}

	// 掴み処理
	if (pInputKeyboard->GetPress(DIK_SPACE) == true)
	{// SPACEで掴む
		Grab();
	}

	if (pInputKeyboard->GetTrigger(DIK_C) == true)
	{// Cキーで追従種類変更
		m_ChaseType = (CCamera::CHASETYPE)(((int)m_ChaseType + 1) % (int)CCamera::CHASETYPE_MAX);	// 追従の種類
	}

	if (pInputKeyboard->GetTrigger(DIK_RETURN) == true)
	{// ENTERで配置

		// マップマネージャの取得
		CCameraChaseChanger *pCmaeraChanger = CManager::GetInstance()->GetScene()->GetCameraChaseChanger();
		if (pCmaeraChanger == NULL)
		{
			return;
		}
		pCmaeraChanger->CreatePos(m_ChaseType, m_nIdxMapPoint, m_fMoveValue);
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
		CCameraChaseChanger *pCmaeraChanger = CManager::GetInstance()->GetScene()->GetCameraChaseChanger();
		pCmaeraChanger->Save("data\\BIN\\camerachanger.bin");
	}

	// デバッグ情報
	CManager::GetInstance()->GetDebugProc()->Print(
		"------------------[ 追従種類変更情報 ]------------------\n"
		"<配置>         [ENTER]\n"
		"<ファイル保存> [F9] 【data/TEXT/edit_info.txt】\n"
		"<種類>         [C：%d]\n"
		"<移動>         高速：[↑,↓,←,→]　低速：[W,A,S,D]\n"
		"<上昇,下降>    [I,K]\n"
		"<掴み移動>     [SPACE]\n"
		"<削除>         [DELETE]\n"
		"<位置>         [X：%f Y：%f Z：%f]\n"
		"\n", m_ChaseType, m_pos.x, m_pos.y, m_pos.z);
}

//==========================================================================
// 操作処理
//==========================================================================
void CEditCameraChaseChanger::Control(D3DXVECTOR3 &pos)
{
	// カメラの情報取得
	CCamera *pCamera = CManager::GetInstance()->GetCamera();

	// カメラの向き取得
	D3DXVECTOR3 Camerarot = pCamera->GetRotation();

	// キーボード情報取得
	CInputKeyboard *pInputKeyboard = CManager::GetInstance()->GetInputKeyboard();

	if (pInputKeyboard->GetPress(DIK_LEFT) == true)
	{// ←キーが押された,左移動

		m_fMoveValue -= MOVE;
	}
	else if (pInputKeyboard->GetPress(DIK_RIGHT) == true)
	{// Dキーが押された,右移動

		m_fMoveValue += MOVE;
	}

	if (pInputKeyboard->GetPress(DIK_A) == true)
	{// ←キーが押された,左移動

		m_fMoveValue -= MOVE_SLOW;
	}
	else if (pInputKeyboard->GetPress(DIK_D) == true)
	{// Dキーが押された,右移動

		m_fMoveValue += MOVE_SLOW;
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

	// マップマネージャの取得
	CMapManager *pMapManager = CManager::GetInstance()->GetScene()->GetMapManager();
	if (pMapManager == NULL)
	{// NULLだったら
		return;
	}

	// 位置更新
	pos = pMapManager->UpdateNowPosition(m_nIdxMapPoint, m_fPointRatio, m_fMoveValue, pos.y);

}

//==========================================================================
// モデル掴む
//==========================================================================
void CEditCameraChaseChanger::Grab(void)
{

	// キーボード情報取得
	CInputKeyboard *pInputKeyboard = CManager::GetInstance()->GetInputKeyboard();

	// マップマネージャ取得
	CCameraChaseChanger *pCmaeraChanger = CManager::GetInstance()->GetScene()->GetCameraChaseChanger();
	if (pCmaeraChanger == NULL)
	{// NULLで抜ける
		return;
	}

	bool bAll = false;
	if (pInputKeyboard->GetPress(DIK_RCONTROL) == true)
	{// 右コントロールで全選択
		bAll = true;
	}

	for (int i = 0; i < pCmaeraChanger->GetNumAll(); i++)
	{
		D3DXVECTOR3 TargetPoint = pCmaeraChanger->GetAxis(i);
		if (bAll == true || SphereRange(m_pos, TargetPoint, 50.0f, 50.0f))
		{// 球に当たってたら

			// 操作
			Control(TargetPoint);

			// 位置設定
			pCmaeraChanger->SetAxis(i, m_nIdxMapPoint, m_fMoveValue);
		}
	}
}

//==========================================================================
// モデル掴む
//==========================================================================
void CEditCameraChaseChanger::Delete(void)
{
	// キーボード情報取得
	CInputKeyboard *pInputKeyboard = CManager::GetInstance()->GetInputKeyboard();

	if (CManager::GetInstance()->GetScene()->GetMapManager() == NULL)
	{// NULLで抜ける
		return;
	}

	// マップマネージャ取得
	CCameraChaseChanger *pCmaeraChanger = CManager::GetInstance()->GetScene()->GetCameraChaseChanger();

	for (int i = 0; i < pCmaeraChanger->GetNumAll(); i++)
	{
		D3DXVECTOR3 TargetPoint = pCmaeraChanger->GetAxis(i);
		if (SphereRange(m_pos, TargetPoint, 50.0f, 50.0f))
		{// 球に当たってたら
			pCmaeraChanger->DeletePos(i);
		}
	}
}

//==========================================================================
// 描画処理
//==========================================================================
void CEditCameraChaseChanger::Draw(void)
{

}
