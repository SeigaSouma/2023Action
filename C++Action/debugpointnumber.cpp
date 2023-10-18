//=============================================================================
// 
//  ポイントの数字処理 [debugpointnumber.cpp]
//  Author : 相馬靜雅
// 
//=============================================================================
#include "debugpointnumber.h"
#include "manager.h"
#include "renderer.h"
#include "calculation.h"
#include "debugproc.h"
#include "input.h"
#include "3D_effect.h"
#include "multinumber.h"
#include "mapmanager.h"

//==========================================================================
// マクロ定義
//==========================================================================
DWORD dwGameStartsTime = 0;
//==========================================================================
// 静的メンバ変数宣言
//==========================================================================

//==========================================================================
// コンストラクタ
//==========================================================================
CDebugPointNumber::CDebugPointNumber(int nPriority) : CObject(nPriority)
{
	// ゼロクリア
	m_nIdxParent = 0;		// 親の番号
	m_pMultiNumber = NULL;	// 数字のオブジェクト
	dwGameStartsTime = timeGetTime();
}

//==========================================================================
// デストラクタ
//==========================================================================
CDebugPointNumber::~CDebugPointNumber()
{

}

//==========================================================================
// 生成処理
//==========================================================================
CDebugPointNumber *CDebugPointNumber::Create(int nIdx)
{
	// 生成用のオブジェクト
	CDebugPointNumber *pObjectX = NULL;

	if (pObjectX == NULL)
	{// NULLだったら

		// メモリの確保
		pObjectX = DEBUG_NEW CDebugPointNumber;

		if (pObjectX != NULL)
		{// メモリの確保が出来ていたら

			// 親の番号
			pObjectX->m_nIdxParent = nIdx;

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
HRESULT CDebugPointNumber::Init(void)
{
	//if (CManager::GetInstance()->GetScene()->GetMapManager() == NULL)
	//{// NULLで抜ける
	//	return E_FAIL;
	//}

	//// マップマネージャ取得
	//CMapManager *pMapManager = CManager::GetInstance()->GetScene()->GetMapManager();

	// 数字生成
	m_pMultiNumber = CMultiNumber::Create(
		D3DXVECTOR3(0.0f, 0.0f, 0.0f),
		D3DXVECTOR2(50.0f, 60.0f),
		GetDigit(m_nIdxParent),
		CNumber::OBJECTTYPE_BILLBOARD);	// 数字のオブジェクト

	// 種類設定
	SetType(CObject::TYPE_NUMBER);

	return S_OK;
}

//==========================================================================
// 終了処理
//==========================================================================
void CDebugPointNumber::Uninit(void)
{
	// 数字のオブジェクトの終了処理
	if (m_pMultiNumber != NULL)
	{
		m_pMultiNumber->Uninit();
		delete m_pMultiNumber;
		m_pMultiNumber = NULL;
	}

	// 終了処理
	Release();
}

//==========================================================================
// 更新処理
//==========================================================================
void CDebugPointNumber::Update(void)
{
	// マップマネージャ取得
	CMapManager *pMapManager = CManager::GetInstance()->GetScene()->GetMapManager();
	if (pMapManager == NULL)
	{// NULLで抜ける
		return;
	}

	// 値の設定処理
	m_pMultiNumber->SetValue(m_nIdxParent);

	// 位置更新
	D3DXVECTOR3 pos = pMapManager->GetControlPoint(m_nIdxParent + 1);
	pos.y += 50.0f;
	m_pMultiNumber->SetPosition(pos);

	//D3DXCOLOR col = m_pMultiNumber->GetColor();

	//DWORD dwGameTime = timeGetTime();	// 現在時刻を取得
	//float fTime = (float)(dwGameTime - dwGameStartsTime);
	//fTime *= 0.001f;

	//if (fTime >= 1.0f)
	//{
	//	dwGameStartsTime = timeGetTime();	// 現在時刻を取得
	//}

	//col.a = Lerp(0.0f, 1.0f, fTime);

	//m_pMultiNumber->SetColor(col);
}

//==========================================================================
// 描画処理
//==========================================================================
void CDebugPointNumber::Draw(void)
{

}
