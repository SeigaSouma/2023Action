//=============================================================================
// 
//  エディット処理 [edit.cpp]
//  Author : 相馬靜雅
// 
//=============================================================================
#include "objectX.h"
#include "XLoad.h"
#include "edit.h"
#include "manager.h"
#include "renderer.h"
#include "calculation.h"
#include "debugproc.h"
#include "input.h"
#include "camera.h"
#include "map.h"
#include "game.h"
#include "elevation.h"

//==========================================================================
// マクロ定義
//==========================================================================
#define MOVE		(5.0f)
#define MOVE_SLOW	(1.0f)

//==========================================================================
// 静的メンバ変数宣言
//==========================================================================
CObjectX *CEdit::m_pObjX = NULL;
int CEdit::m_nNumAll = 0;	// 総数
int CEdit::m_nType = 0;		// タイプ
bool CEdit::m_bShadow = false;	// 影を使うかどうか

//==========================================================================
// コンストラクタ
//==========================================================================
CEdit::CEdit(int nPriority) : CObject(nPriority)
{
	m_posOld = D3DXVECTOR3(0.0f, 0.0f, 0.0f);	// 前回の位置
	m_bShadow = true;	// 影を使うかどうか

	// 総数加算
	m_nNumAll++;
}

//==========================================================================
// デストラクタ
//==========================================================================
CEdit::~CEdit()
{

}

//==========================================================================
// 生成処理
//==========================================================================
CEdit *CEdit::Create(void)
{
	// 生成用のオブジェクト
	CEdit *pObjectX = NULL;

	if (pObjectX == NULL)
	{// NULLだったら

		// メモリの確保
		pObjectX = DEBUG_NEW CEdit;

		//if (pObjectX->GetID() < 0)
		//{// メモリ確保に失敗していたら

		//	delete pObjectX;
		//	return NULL;
		//}

		if (pObjectX != NULL)
		{// メモリの確保が出来ていたら

			// 初期化処理
			HRESULT hr = pObjectX->Init();

			if (FAILED(hr))
			{// 失敗していたら
				return NULL;
			}

			// 種類設定
			pObjectX->SetType(TYPE_EDIT);
		}

		return pObjectX;
	}

	return NULL;
}

//==========================================================================
// 初期化処理
//==========================================================================
HRESULT CEdit::Init(void)
{
	// デバイスの取得
	LPDIRECT3DDEVICE9 pDevice = CManager::GetRenderer()->GetDevice();

	// 生成処理
	m_pObjX = m_pObjX->Create(CScene::GetXLoad()->GetObjectX(m_nType)->acFilename);
	m_pObjX->SetType(TYPE_EDIT);

	if (m_pObjX == NULL)
	{// 失敗していたら
		return E_FAIL;
	}

	return S_OK;
}

//==========================================================================
// 初期化処理
//==========================================================================
HRESULT CEdit::Init(const char *pFileName)
{
	// デバイスの取得
	LPDIRECT3DDEVICE9 pDevice = CManager::GetRenderer()->GetDevice();
	
	// 生成処理
	m_pObjX = m_pObjX->Create(pFileName);

	return S_OK;
}

//==========================================================================
// 終了処理
//==========================================================================
void CEdit::Uninit(void)
{
	// オブジェクトの破棄
	CObject::Release();
}

//==========================================================================
// 破棄の処理
//==========================================================================
void CEdit::Release(void)
{
	// 終了処理
	m_pObjX->CObjectX::Uninit();
	CEdit::Uninit();
}

//==========================================================================
// 更新処理
//==========================================================================
void CEdit::Update(void)
{
	// キーボード情報取得
	CInputKeyboard *pInputKeyboard = CManager::GetInputKeyboard();

	// 位置取得
	D3DXVECTOR3 pos = m_pObjX->GetPosition();

	// 向き取得
	D3DXVECTOR3 rot = m_pObjX->GetRotation();

	// 操作
	Control(m_pObjX);

	// 種類変更
	ChangeType();

	// 掴み移動
	GrabModel();

	if (pInputKeyboard->GetTrigger(DIK_RETURN) == true)
	{// ENTERで配置

		// タイプの物を生成
		CObjectX *pObjX = CObjectX::Create(&CScene::GetXLoad()->GetObjectX(m_nType)->acFilename[0], pos, rot, m_bShadow);
		pObjX->SetType(CObject::TYPE_XFILE);
	}

	if (pInputKeyboard->GetTrigger(DIK_F9) == true)
	{// セーブ

		// テキストにセーブ
		map::SaveText();
	}

	// デバッグ情報
	CManager::GetDebugProc()->Print(
		"------------------[ エディット情報 ]------------------\n"
		"<モデル配置>   [ENTER]\n"
		"<ファイル保存> [F9] 【data/TEXT/edit_info.txt】\n"
		"<移動>         高速：[↑,↓,←,→]　低速：[W,A,S,D]\n"
		"<回転>         [LSHIFT, RSHIFT]\n"
		"<上昇,下降>    [I,K]\n"
		"<種類変更>     [1, 2][%d]\n"
		"<影の使用状況> [3][%d]\n"
		"<掴み移動>     [SPACE]\n"
		"<削除>         [DELETE]\n"
		"<位置>         [X：%f Y：%f Z：%f]\n"
		"<向き>         [X：%f Y：%f Z：%f]\n"
		"\n", m_nType, (int)m_bShadow, pos.x, pos.y, pos.z, rot.x, rot.y, rot.z);
}

//==========================================================================
// 操作処理
//==========================================================================
void CEdit::Control(CObjectX *pObjX)
{
	// カメラの情報取得
	CCamera *pCamera = CManager::GetCamera();

	// カメラの向き取得
	D3DXVECTOR3 Camerarot = pCamera->GetRotation();

	// キーボード情報取得
	CInputKeyboard *pInputKeyboard = CManager::GetInputKeyboard();

	// 位置取得
	D3DXVECTOR3 pos = pObjX->GetPosition();

	// 向き取得
	D3DXVECTOR3 rot = pObjX->GetRotation();

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

	if (pInputKeyboard->GetTrigger(DIK_LSHIFT) == true)
	{//回転

		rot.y += D3DX_PI * 0.1f;
	}
	if (pInputKeyboard->GetTrigger(DIK_RSHIFT) == true)
	{// 回転

		rot.y -= D3DX_PI * 0.1f;
	}

	if (pInputKeyboard->GetPress(DIK_I) == true)
	{// Iキーが押された,上昇

		pos.y += MOVE;
	}
	else if (pInputKeyboard->GetPress(DIK_K) == true)
	{// Kキーが押された,下降

		pos.y -= MOVE;
	}

	// 角度正規化
	RotNormalize(rot.y);


	// 着地したかどうか
	bool bLand = false;

	// 高さ取得
	float fHeight = CGame::GetElevation()->GetHeight(pos, bLand);

	if (bLand == true)
	{
		// 高さ代入
		pos.y = fHeight;
	}
	else
	{
		pos.y = -100.0f;
	}

	// 向き設定
	pObjX->SetRotation(rot);

	// 位置設定
	pObjX->SetPosition(pos);
}

//==========================================================================
// 種類変更
//==========================================================================
void CEdit::ChangeType(void)
{
	int nNumAll = map::GetNumModelAll();

	// キーボード情報取得
	CInputKeyboard *pInputKeyboard = CManager::GetInputKeyboard();

	// 配置するオブジェクト変更
	if (pInputKeyboard->GetTrigger(DIK_1) == true)
	{// 1が押された

		// 色の種類更新
		m_nType = (m_nType + (nNumAll - 1)) % nNumAll;

		// オブジェクト割り当て
		m_pObjX->BindXData(m_nType);
	}
	else if (pInputKeyboard->GetTrigger(DIK_2) == true)
	{// 2が押された

		// 色の種類更新
		m_nType = (m_nType + 1) % nNumAll;

		// オブジェクト割り当て
		m_pObjX->BindXData(m_nType);
	}

	if (pInputKeyboard->GetTrigger(DIK_3) == true)
	{// 3が押された

		// 影の使用状況切り替え
		m_bShadow = m_bShadow ? false : true;
	}

}

//==========================================================================
// モデル掴む
//==========================================================================
void CEdit::GrabModel(void)
{
#if TOPCUR
	// キーボード情報取得
	CInputKeyboard *pInputKeyboard = CManager::GetInputKeyboard();

	for (int nCntPriority = 0; nCntPriority < mylib_const::PRIORITY_NUM; nCntPriority++)
	{
		// 先頭を保存
		CObject *pObj = CObject::GetTop(nCntPriority);

		while (pObj != NULL)
		{// NULLが来るまで無限ループ

			// 次のオブジェクトを一時保存
			CObject *pObjNext = pObj->GetNext();

			// 種類の取得
			CObject::TYPE TargetType = pObj->GetType();

			if (TargetType != CObject::TYPE_XFILE || pObj == CObject::GetObject())
			{// 自分と同じだったら

				// 次のオブジェクトを代入
				pObj = pObjNext;
				continue;
			}

			// オブジェクトXの情報取得
			CObjectX *pObjX = pObj->GetObjectX();

			// オブジェクトXの位置取得
			D3DXVECTOR3 pObjPos = pObjX->GetPosition();

			// オブジェクトXの向き取得
			D3DXVECTOR3 pObjRot = pObjX->GetRotation();

			// オブジェクトXの最大値取得
			D3DXVECTOR3 pObjVtxMax = pObjX->GetVtxMax();

			// オブジェクトXの最小値取得
			D3DXVECTOR3 pObjVtxMin = pObjX->GetVtxMin();

			// 位置取得
			D3DXVECTOR3 pos = m_pObjX->GetPosition();

			// 最大値取得
			D3DXVECTOR3 vtxMax = m_pObjX->GetVtxMax();

			// 最小値取得
			D3DXVECTOR3 vtxMin = m_pObjX->GetVtxMin();

			if (pObjPos.x + pObjVtxMax.x >= pos.x + vtxMin.x &&	// 右からめり込んでる
				pObjPos.x + pObjVtxMin.x <= pos.x + vtxMax.x &&	// 左からめり込んでる
				pObjPos.z + pObjVtxMax.z >= pos.z + vtxMin.z &&	// からめり込んでる
				pObjPos.z + pObjVtxMin.z <= pos.z + vtxMax.z)	// 前からめり込んでる
			{// 当たり判定内に入っていたら掴める

				if (pInputKeyboard->GetPress(DIK_SPACE) == true)
				{// SPACEが押された,つかんで移動

					// 操作処理
					Control(pObjX);
				}

				if (pInputKeyboard->GetTrigger(DIK_DELETE) == true)
				{// Deleteが押された、削除

					// 削除処理
					pObjX->Kill();
					//DeleteEditModel(nCntModel);
				}
			}

			// 次のオブジェクトを代入
			pObj = pObjNext;
		}
	}
#endif
}

//==========================================================================
// モデル削除
//==========================================================================
void CEdit::DeleteModel(void)
{

}

//==========================================================================
// 描画処理
//==========================================================================
void CEdit::Draw(void)
{

}


//==========================================================================
// 外部テキスト読み込み処理
//==========================================================================
HRESULT CEdit::ReadText(void)
{
	return S_OK;;
}

//==========================================================================
// 総数取得
//==========================================================================
int CEdit::GetNumAll(void)
{
	return m_nNumAll;
}
