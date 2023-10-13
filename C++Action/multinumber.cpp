//=============================================================================
// 
//  数字処理 [multinumber.cpp]
//  Author : 相馬靜雅
// 
//=============================================================================
#include "multinumber.h"
#include "number.h"
#include "manager.h"
#include "renderer.h"
#include "object2D.h"
#include "objectBillboard.h"
#include "texture.h"

//==========================================================================
// マクロ定義
//==========================================================================
#define TEXTURE			"data\\TEXTURE\\number_blackclover_01.png"	// テクスチャのファイル
#define TEX_U			(0.1f)							// Uの分割

//==========================================================================
// 静的メンバ変数宣言
//==========================================================================

//==========================================================================
// コンストラクタ
//==========================================================================
CMultiNumber::CMultiNumber(int nPriority)
{
	// 値のクリア
	m_nNum = 0;			// 数字
	m_nNumNumber = 0;	// 数字の数
	m_ppMultiNumber = NULL;	// 数字のオブジェクト
	m_pos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);	// 位置
	size = D3DXVECTOR2(0.0f, 0.0f);	// 数字のサイズ
	m_objType = CNumber::OBJECTTYPE_2D;	// オブジェクトの種類
}

//==========================================================================
// デストラクタ
//==========================================================================
CMultiNumber::~CMultiNumber()
{

}

//==========================================================================
// 生成処理
//==========================================================================
CMultiNumber *CMultiNumber::Create(D3DXVECTOR3 pos, D3DXVECTOR2 size, int nNum, CNumber::EObjectType objtype)
{
	// 生成用のオブジェクト
	CMultiNumber *pNumber = NULL;

	if (pNumber == NULL)
	{// NULLだったら

		// メモリの確保
		pNumber = DEBUG_NEW CMultiNumber;

		if (pNumber != NULL)
		{// メモリの確保が出来ていたら

			// オブジェクトの種類
			pNumber->m_objType = objtype;

			// サイズ
			pNumber->size = size;

			// 位置
			pNumber->m_pos = pos;

			// 数字の数
			pNumber->m_nNumNumber = nNum;

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
HRESULT CMultiNumber::Init(void)
{
	// 数分メモリ確保
	m_ppMultiNumber = DEBUG_NEW (CNumber*[m_nNumNumber]);

	// 生成処理
	int nTex = CManager::GetTexture()->Regist(TEXTURE);
	for (int nCntNum = 0; nCntNum < m_nNumNumber; nCntNum++)
	{
		// 生成処理
		m_ppMultiNumber[nCntNum] = CNumber::Create(m_objType);

		// 各種変数の初期化
		switch (m_objType)
		{
		case CNumber::OBJECTTYPE_2D:
			m_ppMultiNumber[nCntNum]->GetObject2D()->SetSize(size);	// サイズ
			m_ppMultiNumber[nCntNum]->GetObject2D()->SetPosition(D3DXVECTOR3(m_pos.x + size.y * nCntNum, m_pos.y, m_pos.z));	// 位置

			// 種類の設定
			m_ppMultiNumber[nCntNum]->GetObject2D()->SetType(CObject::TYPE_SCORE);

			// テクスチャの割り当て
			m_ppMultiNumber[nCntNum]->GetObject2D()->BindTexture(nTex);
			break;

		case CNumber::OBJECTTYPE_BILLBOARD:
			m_ppMultiNumber[nCntNum]->GetObjectBillboard()->SetSize(size);	// サイズ
			m_ppMultiNumber[nCntNum]->GetObjectBillboard()->SetPosition(D3DXVECTOR3(m_pos.x + size.y * nCntNum, m_pos.y, m_pos.z));	// 位置

			// 種類の設定
			m_ppMultiNumber[nCntNum]->GetObjectBillboard()->SetType(CObject::TYPE_SCORE);

			// テクスチャの割り当て
			m_ppMultiNumber[nCntNum]->GetObjectBillboard()->BindTexture(nTex);
			break;

		default:
			break;
		}
	}

	return S_OK;
}

//==========================================================================
// 終了処理
//==========================================================================
void CMultiNumber::Uninit(void)
{
	// 終了処理
	if (m_ppMultiNumber != NULL)
	{// NULLじゃなかったら

		// 各数字オブジェクトの破棄
		for (int nCntNum = 0; nCntNum < m_nNumNumber; nCntNum++)
		{
			if (m_ppMultiNumber[nCntNum] != NULL)
			{
				m_ppMultiNumber[nCntNum]->Uninit();
				delete m_ppMultiNumber[nCntNum];
				m_ppMultiNumber[nCntNum] = NULL;
			}
		}
		delete[] m_ppMultiNumber;
		m_ppMultiNumber = NULL;
	}
}

//==========================================================================
// 解放処理
//==========================================================================
void CMultiNumber::Release(void)
{
	// 終了処理
	if (m_ppMultiNumber != NULL)
	{// NULLじゃなかったら

		// 各数字オブジェクトの破棄
		for (int nCntNum = 0; nCntNum < m_nNumNumber; nCntNum++)
		{
			if (m_ppMultiNumber[nCntNum] != NULL)
			{
				m_ppMultiNumber[nCntNum]->Release();
				delete m_ppMultiNumber[nCntNum];
				m_ppMultiNumber[nCntNum] = NULL;
			}
		}
		delete m_ppMultiNumber;
		m_ppMultiNumber = NULL;
	}
}

//==========================================================================
// 更新処理
//==========================================================================
void CMultiNumber::Update(void)
{
	// 更新処理
}

//==========================================================================
// 描画処理
//==========================================================================
void CMultiNumber::Draw(void)
{
	for (int nCntNum = 0; nCntNum < m_nNumNumber; nCntNum++)
	{
		if (m_ppMultiNumber[nCntNum] == NULL)
		{// NULLだったら
			continue;
		}
		m_ppMultiNumber[nCntNum]->Draw();
	}
}

//==========================================================================
// 値の設定処理
//==========================================================================
void CMultiNumber::AddNumber(int nValue)
{
	m_nNum += nValue;

	// 値の設定処理
	SetValue();
}

//==========================================================================
// 値の設定処理
//==========================================================================
void CMultiNumber::SetValue(int nValue)
{
	m_nNum = nValue;

	// 値の設定処理
	SetValue();
}

//==========================================================================
// 値の設定処理
//==========================================================================
void CMultiNumber::SetValue(void)
{
	for (int nCntNum = 0; nCntNum < m_nNumNumber; nCntNum++)
	{
		if (m_ppMultiNumber[nCntNum] == NULL)
		{// NULLだったら
			continue;
		}

		int aTexU = m_nNum % (int)std::powf(10, m_nNumNumber + 1 - nCntNum) / ((int)std::powf(10, m_nNumNumber - nCntNum) / 10);

		if (aTexU < 0)
		{
			aTexU = 0;
		}

		// テクスチャポインタ取得
		D3DXVECTOR2 *pTex;
		switch (m_objType)
		{
		case CNumber::OBJECTTYPE_2D:
			// テクスチャポインタ取得
			pTex = m_ppMultiNumber[nCntNum]->GetObject2D()->GetTex();

			// テクスチャ座標の設定
			pTex[0] = D3DXVECTOR2(aTexU * TEX_U, 0.0f);
			pTex[1] = D3DXVECTOR2(aTexU * TEX_U + TEX_U, 0.0f);
			pTex[2] = D3DXVECTOR2(aTexU * TEX_U, 1.0f);
			pTex[3] = D3DXVECTOR2(aTexU * TEX_U + TEX_U, 1.0f);

			// 頂点設定
			m_ppMultiNumber[nCntNum]->GetObject2D()->SetVtx();
			break;

		case CNumber::OBJECTTYPE_BILLBOARD:
			// テクスチャポインタ取得
			pTex = m_ppMultiNumber[nCntNum]->GetObjectBillboard()->GetTex();

			// テクスチャ座標の設定
			pTex[0] = D3DXVECTOR2(aTexU * TEX_U, 0.0f);
			pTex[1] = D3DXVECTOR2(aTexU * TEX_U + TEX_U, 0.0f);
			pTex[2] = D3DXVECTOR2(aTexU * TEX_U, 1.0f);
			pTex[3] = D3DXVECTOR2(aTexU * TEX_U + TEX_U, 1.0f);

			// 頂点設定
			m_ppMultiNumber[nCntNum]->GetObjectBillboard()->SetVtx();
			break;
		}
	}
}

//==========================================================================
// 位置設定
//==========================================================================
void CMultiNumber::SetPosition(const D3DXVECTOR3 pos)
{
	// 位置設定
	m_pos = pos;

	switch (m_objType)
	{
	case CNumber::OBJECTTYPE_2D:
		for (int nCntNum = 0; nCntNum < m_nNumNumber; nCntNum++)
		{
			if (m_ppMultiNumber[nCntNum] != NULL)
			{
				m_ppMultiNumber[nCntNum]->GetObject2D()->SetPosition(D3DXVECTOR3(m_pos.x + size.y * nCntNum, m_pos.y, m_pos.z));	// 位置
			}
		}
		break;

	case CNumber::OBJECTTYPE_BILLBOARD:
		for (int nCntNum = 0; nCntNum < m_nNumNumber; nCntNum++)
		{
			if (m_ppMultiNumber[nCntNum] != NULL)
			{
				m_ppMultiNumber[nCntNum]->GetObjectBillboard()->SetPosition(D3DXVECTOR3(m_pos.x + size.y * nCntNum, m_pos.y, m_pos.z));	// 位置
			}
		}
		break;
	}
}

//==========================================================================
// 位置取得
//==========================================================================
D3DXVECTOR3 CMultiNumber::GetPosition(void) const
{
	return m_pos;
}