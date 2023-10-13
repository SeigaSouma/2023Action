//=============================================================================
// 
//  数字処理 [number.cpp]
//  Author : 相馬靜雅
// 
//=============================================================================
#include "number.h"
#include "manager.h"
#include "renderer.h"
#include "object2D.h"
#include "objectBillboard.h"

//==========================================================================
// マクロ定義
//==========================================================================
#define WIDTH			(640.0f)					// 横幅
#define HEIGHT			(360.0f)					// 縦幅
#define SCROLL_SPEED	(-0.005f)					// スクロール速度

//==========================================================================
// 静的メンバ変数宣言
//==========================================================================

//==========================================================================
// コンストラクタ
//==========================================================================
CNumber::CNumber(int nPriority)
{
	// 値のクリア
	m_objType = OBJECTTYPE_2D;	// オブジェクトの種類
	m_aObject2D = NULL;			// オブジェクト2Dのオブジェクト
	m_pObjBillboard = NULL;		// オブジェクトビルボードのオブジェクト
}

//==========================================================================
// デストラクタ
//==========================================================================
CNumber::~CNumber()
{

}

//==========================================================================
// 生成処理
//==========================================================================
CNumber *CNumber::Create(EObjectType objtype)
{
	// 生成用のオブジェクト
	CNumber *pNumber = NULL;

	if (pNumber == NULL)
	{// NULLだったら

		// メモリの確保
		pNumber = DEBUG_NEW CNumber;

		if (pNumber != NULL)
		{// メモリの確保が出来ていたら

			// オブジェクトの種類
			pNumber->m_objType = objtype;

			// 初期化処理
			pNumber->Init();
		}
		else
		{
			delete pNumber;
			pNumber = NULL;
		}

		return pNumber;
	}

	return NULL;
}

//==========================================================================
// 初期化処理
//==========================================================================
HRESULT CNumber::Init(void)
{
	// 生成処理
	switch (m_objType)
	{
	case CNumber::OBJECTTYPE_2D:
		m_aObject2D = CObject2D::Create(7);
		break;

	case CNumber::OBJECTTYPE_BILLBOARD:
		m_pObjBillboard = CObjectBillboard::Create();
		break;

	default:
		return E_FAIL;
		break;
	}

	return S_OK;
}

//==========================================================================
// 終了処理
//==========================================================================
void CNumber::Uninit(void)
{
	// 終了処理
	if (m_aObject2D != NULL)
	{// NULLじゃなかったら
		m_aObject2D = NULL;
	}

	// 終了処理
	if (m_pObjBillboard != NULL)
	{// NULLじゃなかったら
		m_pObjBillboard = NULL;
	}
}

//==========================================================================
// 解放処理
//==========================================================================
void CNumber::Release(void)
{
	if (m_aObject2D != NULL)
	{// NULLじゃなかったら
		m_aObject2D->Uninit();
		m_aObject2D = NULL;
	}

	// 終了処理
	if (m_pObjBillboard != NULL)
	{// NULLじゃなかったら
		m_pObjBillboard->Uninit();
		m_pObjBillboard = NULL;
	}
}

//==========================================================================
// 更新処理
//==========================================================================
void CNumber::Update(void)
{
	// 更新処理
	if (m_aObject2D != NULL)
	{
		m_aObject2D->Update();
	}

	// 更新処理
	if (m_pObjBillboard != NULL)
	{// NULLじゃなかったら
		m_pObjBillboard->Update();
	}
}

//==========================================================================
// 描画処理
//==========================================================================
void CNumber::Draw(void)
{
	// 描画処理
	if (m_aObject2D != NULL)
	{
		m_aObject2D->Draw();
	}

	// デバイスの取得
	LPDIRECT3DDEVICE9 pDevice = CManager::GetRenderer()->GetDevice();

	// ライティングを無効にする
	pDevice->SetRenderState(D3DRS_LIGHTING, FALSE);

	// アルファテストを有効にする
	pDevice->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
	pDevice->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);
	pDevice->SetRenderState(D3DRS_ALPHAREF, 0);

	if (m_pObjBillboard != NULL)
	{// NULLじゃなかったら
		m_pObjBillboard->Draw();
	}

	// アルファテストを無効にする
	pDevice->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
	pDevice->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_ALWAYS);
	pDevice->SetRenderState(D3DRS_ALPHAREF, 0);

	// ライティングを有効にする
	pDevice->SetRenderState(D3DRS_LIGHTING, TRUE);
}

//==========================================================================
// オブジェクト2Dオブジェクトの取得
//==========================================================================
CObject2D *CNumber::GetObject2D(void)
{
	return m_aObject2D;
}

//==========================================================================
// オブジェクトビルボードオブジェクトの取得
//==========================================================================
CObjectBillboard *CNumber::GetObjectBillboard(void)
{
	return m_pObjBillboard;
}