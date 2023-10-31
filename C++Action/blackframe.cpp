//=============================================================================
// 
//  黒フレーム処理 [blackframe.cpp]
//  Author : 相馬靜雅
// 
//=============================================================================
#include "blackframe.h"
#include "manager.h"
#include "renderer.h"
#include "texture.h"
#include "object2D.h"
#include "calculation.h"

//==========================================================================
// マクロ定義
//==========================================================================
#define SIZE		(D3DXVECTOR2(640.0f, 50.0f))
#define START_UP	(D3DXVECTOR3(640.0f, -SIZE.y, 0.0f))
#define START_DOWN	(D3DXVECTOR3(640.0f, SCREEN_HEIGHT + SIZE.y, 0.0f))
#define MOVETIME	(50)

//==========================================================================
// 静的メンバ変数宣言
//==========================================================================
const D3DXVECTOR3 CBlackFrame::m_DestPosition[VTX_MAX] =	// 目標の位置
{
	D3DXVECTOR3(640.0f, SIZE.y, 0.0f),
	D3DXVECTOR3(640.0f, SCREEN_HEIGHT - SIZE.y, 0.0f),
};

//==========================================================================
// コンストラクタ
//==========================================================================
CBlackFrame::CBlackFrame(int nPriority)
{
	// 値のクリア
	memset(&m_pObj2D[0], NULL, sizeof(m_pObj2D));	// オブジェクト2Dのオブジェクト
	m_state = STATE_NONE;	// 状態
	m_nCntMove = 0;	// 移動カウント
}

//==========================================================================
// デストラクタ
//==========================================================================
CBlackFrame::~CBlackFrame()
{

}

//==========================================================================
// 生成処理
//==========================================================================
CBlackFrame *CBlackFrame::Create(void)
{
	// 生成用のオブジェクト
	CBlackFrame *pTitleScreen = NULL;

	if (pTitleScreen == NULL)
	{// NULLだったら

		// メモリの確保
		pTitleScreen = DEBUG_NEW CBlackFrame;

		if (pTitleScreen != NULL)
		{// メモリの確保が出来ていたら

			// 初期化処理
			pTitleScreen->Init();
		}

		return pTitleScreen;
	}

	return NULL;
}

//==========================================================================
// 初期化処理
//==========================================================================
HRESULT CBlackFrame::Init(void)
{
	
	for (int nCntSelect = 0; nCntSelect < VTX_MAX; nCntSelect++)
	{
		// 生成処理
		m_pObj2D[nCntSelect] = CObject2D::Create(8);

		// 種類の設定
		//m_pObj2D[nCntSelect]->SetType(CObject::TYPE_TITLESELECT);

		// テクスチャの割り当て
		m_pObj2D[nCntSelect]->BindTexture(0);

		// 各種変数の初期化
		switch (nCntSelect)
		{
		case VTX_UP:
			// サイズ取得
			m_pObj2D[nCntSelect]->SetSize(SIZE);	// サイズ
			m_pObj2D[nCntSelect]->SetPosition(START_UP);	// 位置
			m_pObj2D[nCntSelect]->SetColor(D3DXCOLOR(0.0f, 0.0f, 0.0f, 1.0f));	// 色
			break;

		case VTX_DOWN:
			// サイズ取得
			m_pObj2D[nCntSelect]->SetSize(SIZE);	// サイズ
			m_pObj2D[nCntSelect]->SetPosition(START_DOWN);	// 位置
			m_pObj2D[nCntSelect]->SetColor(D3DXCOLOR(0.0f, 0.0f, 0.0f, 1.0f));	// 色
			break;
		}
	}
	m_state = STATE_OUTCOMPLETION;	// 状態

	return S_OK;
}

//==========================================================================
// 終了処理
//==========================================================================
void CBlackFrame::Uninit(void)
{
	for (int nCntSelect = 0; nCntSelect < VTX_MAX; nCntSelect++)
	{
		if (m_pObj2D[nCntSelect] != NULL)
		{// NULLじゃなかったら

			// 終了処理
			m_pObj2D[nCntSelect]->Uninit();
			delete m_pObj2D[nCntSelect];
			m_pObj2D[nCntSelect] = NULL;
		}
	}
}

//==========================================================================
// 更新処理
//==========================================================================
void CBlackFrame::Update(void)
{
	for (int nCntSelect = 0; nCntSelect < VTX_MAX; nCntSelect++)
	{
		if (m_pObj2D[nCntSelect] == NULL)
		{// NULLだったら
			continue;
		}

		// 状態更新処理
		UpdateState(nCntSelect);

		// 頂点情報設定
		m_pObj2D[nCntSelect]->SetVtx();
	}

	// 移動カウント加算
	m_nCntMove++;
}

//==========================================================================
// 状態更新処理
//==========================================================================
void CBlackFrame::UpdateState(int nCntVtx)
{
	switch (m_state)
	{
	case CBlackFrame::STATE_NONE:
		break;

	case CBlackFrame::STATE_IN:
		StateIn(nCntVtx);
		break;

	case CBlackFrame::STATE_OUT:
		StateOut(nCntVtx);
		break;

	case CBlackFrame::STATE_INCOMPLETION:
		break;

	case CBlackFrame::STATE_OUTCOMPLETION:
		break;
	}
}

//==========================================================================
// 侵入
//==========================================================================
void CBlackFrame::StateIn(int nCntVtx)
{
	if (m_nCntMove >= MOVETIME)
	{// 移動時間が規定値超えたら
		m_nCntMove = MOVETIME;
		m_state = STATE_INCOMPLETION;
		return;
	}

	// 情報取得
	D3DXVECTOR3 pos = m_pObj2D[nCntVtx]->GetPosition();

	// 初期位置
	D3DXVECTOR3 start = mylib_const::DEFAULT_VECTOR3;
	if (nCntVtx == VTX_UP)
	{
		start = START_UP;
	}
	else if (nCntVtx == VTX_DOWN)
	{
		start = START_DOWN;
	}

	// タイム割合
	float fTime = (float)m_nCntMove / (float)MOVETIME;

	// 位置更新
	pos.x = Lerp(start.x, m_DestPosition[nCntVtx].x, fTime);
	pos.y = Lerp(start.y, m_DestPosition[nCntVtx].y, fTime);

	// 情報設定
	m_pObj2D[nCntVtx]->SetPosition(pos);
}

//==========================================================================
// 捌け
//==========================================================================
void CBlackFrame::StateOut(int nCntVtx)
{
	if (m_nCntMove >= MOVETIME)
	{// 移動時間が規定値超えたら
		m_nCntMove = MOVETIME;
		m_state = STATE_OUTCOMPLETION;
		return;
	}

	// 情報取得
	D3DXVECTOR3 pos = m_pObj2D[nCntVtx]->GetPosition();

	// 初期位置
	D3DXVECTOR3 start = mylib_const::DEFAULT_VECTOR3;
	if (nCntVtx == VTX_UP)
	{
		start = START_UP;
	}
	else if (nCntVtx == VTX_DOWN)
	{
		start = START_DOWN;
	}

	// タイム割合
	float fTime = (float)m_nCntMove / (float)MOVETIME;

	// 位置更新
	pos.x = Lerp(m_DestPosition[nCntVtx].x, start.x, fTime);
	pos.y = Lerp(m_DestPosition[nCntVtx].y, start.y, fTime);

	// 情報設定
	m_pObj2D[nCntVtx]->SetPosition(pos);
}

//==========================================================================
// 描画処理
//==========================================================================
void CBlackFrame::Draw(void)
{
	// デバイスの取得
	LPDIRECT3DDEVICE9 pDevice = CManager::GetInstance()->GetRenderer()->GetDevice();

	// アルファテストを有効にする
	pDevice->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
	pDevice->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);
	pDevice->SetRenderState(D3DRS_ALPHAREF, 0);

	for (int nCntSelect = 0; nCntSelect < VTX_MAX; nCntSelect++)
	{
		if (m_pObj2D[nCntSelect] == NULL)
		{
			continue;
		}

		// 描画処理
		m_pObj2D[nCntSelect]->Draw();
	}

	// アルファテストを無効にする
	pDevice->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
	pDevice->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_ALWAYS);
	pDevice->SetRenderState(D3DRS_ALPHAREF, 0);
}

//==========================================================================
// 状態設定
//==========================================================================
void CBlackFrame::SetState(STATE state)
{
	if (m_state == STATE_INCOMPLETION ||
		m_state == STATE_OUTCOMPLETION)
	{
		m_state = state;
		m_nCntMove = 0;
	}
}
