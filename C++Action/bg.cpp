//=============================================================================
// 
//  背景処理 [bg.cpp]
//  Author : 相馬靜雅
// 
//=============================================================================
#include "bg.h"
#include "manager.h"
#include "renderer.h"
#include "texture.h"
#include "object2D.h"
#include "calculation.h"

//==========================================================================
// マクロ定義
//==========================================================================
#define WIDTH			(640.0f)					// 横幅
#define HEIGHT			(360.0f)					// 縦幅

//==========================================================================
// 静的メンバ変数宣言
//==========================================================================
const char *CBG::m_apTextureFile[] =				// テクスチャのファイル
{
	"data\\TEXTURE\\bg100.png",
	"data\\TEXTURE\\bg101.png",
	"data\\TEXTURE\\bg102.png",
};
const CBG::SBgInfo CBG::m_aBgInfo[] =				// 背景の情報の情報
{
	{ 0.001f, 0.0f },	// 移動量
	{ 0.003f, 0.0f },
	{ 0.005f, 0.0f },
};

//==========================================================================
// コンストラクタ
//==========================================================================
CBG::CBG(int nPriority) : CObject(nPriority)
{
	// 値のクリア
	for (int nCntBg = 0; nCntBg < NUM_BG; nCntBg++)
	{
		m_aBg[nCntBg].pObject2D = NULL;
		m_aBg[nCntBg].Info.fTexU = 0.0f;
		m_aBg[nCntBg].Info.fTexV = 0.0f;
		m_aBg[nCntBg].Info.fMoveU = 0.0f;
		m_aBg[nCntBg].Info.fMoveV = 0.0f;
	}

	// テクスチャデータの配列分繰り返す
	for (int nCntTex = 0; nCntTex < sizeof(m_apTextureFile) / sizeof(*m_apTextureFile); nCntTex++)
	{
		m_nTexIdx[nCntTex] = 0;		// テクスチャのインデックス番号
	}
}

//==========================================================================
// デストラクタ
//==========================================================================
CBG::~CBG()
{

}

//==========================================================================
// 生成処理
//==========================================================================
CBG *CBG::Create(void)
{
	// 生成用のオブジェクト
	CBG *pBG = NULL;

	if (pBG == NULL)
	{// NULLだったら

		// メモリの確保
		pBG = DEBUG_NEW CBG;

		//if (pBG->GetID() < 0)
		//{// メモリ確保に失敗していたら

		//	delete pBG;
		//	return NULL;
		//}

		if (pBG != NULL)
		{// メモリの確保が出来ていたら

			// 初期化処理
			pBG->Init();

			// 種類の設定
			pBG->SetType(TYPE_BG);
		}

		return pBG;
	}

	return NULL;
}

//==========================================================================
// 初期化処理
//==========================================================================
HRESULT CBG::Init(void)
{
	for (int nCntBg = 0; nCntBg < NUM_BG; nCntBg++)
	{
		// 生成処理
		m_aBg[nCntBg].pObject2D = m_aBg[nCntBg].pObject2D->Create(0);

		// 各種変数の初期化
		m_aBg[nCntBg].pObject2D->SetSize(D3DXVECTOR2(WIDTH, HEIGHT));	// サイズ
		m_aBg[nCntBg].pObject2D->SetPosition(D3DXVECTOR3(640.0f, 360.0f, 0.0f));	// 位置
		m_aBg[nCntBg].Info = m_aBgInfo[nCntBg];	// 情報渡す

		// テクスチャの割り当て
		m_nTexIdx[nCntBg] = CManager::GetTexture()->Regist(m_apTextureFile[nCntBg]);

		// テクスチャの割り当て
		m_aBg[nCntBg].pObject2D->BindTexture(m_nTexIdx[nCntBg]);
	}

	return S_OK;
}

//==========================================================================
// 終了処理
//==========================================================================
void CBG::Uninit(void)
{
	for (int nCntBg = 0; nCntBg < NUM_BG; nCntBg++)
	{
		if (m_aBg[nCntBg].pObject2D != NULL)
		{// NULLじゃなかったら

			// 終了処理
			m_aBg[nCntBg].pObject2D->Uninit();
			m_aBg[nCntBg].pObject2D = NULL;
		}
	}

	// 情報削除
	Release();
}

//==========================================================================
// 更新処理
//==========================================================================
void CBG::Update(void)
{
	for (int nCntBg = 0; nCntBg < NUM_BG; nCntBg++)
	{
		if (m_aBg[nCntBg].pObject2D != NULL)
		{// NULLじゃなかったら

			// スクロール
			m_aBg[nCntBg].Info.fTexU += m_aBg[nCntBg].Info.fMoveU;
			m_aBg[nCntBg].Info.fTexV += m_aBg[nCntBg].Info.fMoveV;

			if (m_aBg[nCntBg].Info.fTexU >= 1.0f)
			{
				m_aBg[nCntBg].Info.fTexU = 0.0f;
			}
			else if (m_aBg[nCntBg].Info.fTexU <= 0.0f)
			{
				m_aBg[nCntBg].Info.fTexU = 1.0f;
			}

			if (m_aBg[nCntBg].Info.fTexV >= 1.0f)
			{
				m_aBg[nCntBg].Info.fTexV = 0.0f;
			}
			else if (m_aBg[nCntBg].Info.fTexV <= 0.0f)
			{
				m_aBg[nCntBg].Info.fTexV = 1.0f;
			}

			// 頂点情報設定
			SetVtx(nCntBg);
		}
	}
}

//==========================================================================
// 描画処理
//==========================================================================
void CBG::Draw(void)
{
	for (int nCntBg = 0; nCntBg < NUM_BG; nCntBg++)
	{
		// 描画処理
		m_aBg[nCntBg].pObject2D->Draw();
	}
}

//==========================================================================
// 頂点情報設定処理
//==========================================================================
void CBG::SetVtx(void)
{

}

//==========================================================================
// 頂点情報設定処理
//==========================================================================
void CBG::SetVtx(int nCntBg)
{
	// 頂点設定
	m_aBg[nCntBg].pObject2D->SetVtx();

	// 頂点情報へのポインタ
	VERTEX_2D *pVtx;

	// 頂点バッファをロックし、頂点情報へのポインタを取得
	m_aBg[nCntBg].pObject2D->GetVtxBuff()->Lock(0, 0, (void**)&pVtx, 0);

	// テクスチャ座標の設定
	pVtx[0].tex = D3DXVECTOR2(m_aBg[nCntBg].Info.fTexU,			m_aBg[nCntBg].Info.fTexV);
	pVtx[1].tex = D3DXVECTOR2(m_aBg[nCntBg].Info.fTexU + 1.0f,	m_aBg[nCntBg].Info.fTexV);
	pVtx[2].tex = D3DXVECTOR2(m_aBg[nCntBg].Info.fTexU,			m_aBg[nCntBg].Info.fTexV + 1.0f);
	pVtx[3].tex = D3DXVECTOR2(m_aBg[nCntBg].Info.fTexU + 1.0f,	m_aBg[nCntBg].Info.fTexV + 1.0f);

	// 頂点バッファをアンロックロック
	m_aBg[nCntBg].pObject2D->GetVtxBuff()->Unlock();
}

//==========================================================================
// 背景移動量設定）
//==========================================================================
void CBG::SetMoveTex(const D3DXVECTOR3 move)
{
	for (int nCntBg = 0; nCntBg < NUM_BG; nCntBg++)
	{
		m_aBg[nCntBg].Info.fTexU += (0.0f - -move.x) * 0.0005f;
		m_aBg[nCntBg].Info.fTexV += (0.0f - -move.y) * 0.001f;
	}
}
