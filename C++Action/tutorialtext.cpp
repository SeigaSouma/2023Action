//=============================================================================
// 
//  チュートリアルテキスト処理 [tutorialtext.cpp]
//  Author : 相馬靜雅
// 
//=============================================================================
#include "tutorialtext.h"
#include "tutorial.h"
#include "tutorialstep.h"
#include "manager.h"
#include "renderer.h"
#include "texture.h"
#include "object2D.h"
#include "calculation.h"

//==========================================================================
// マクロ定義
//==========================================================================
#define WIDTH		(640.0f)	// 横幅
#define HEIGHT		(360.0f)	// 縦幅
#define ALPHATIME	(60)		// 不透明度更新の時間
#define LOGOMOVE_TIME	(180)	// ロゴ移動の時間
#define LOGO_POSITION	(D3DXVECTOR3(640.0f, 160.0f, 0.0f))
#define TEXT_POSITION	(D3DXVECTOR3(640.0f, 500.0f, 0.0f))
#define ALPHA_MOVE	(0.02f)	// 不透明度更新量

//==========================================================================
// 静的メンバ変数宣言
//==========================================================================
const char *CTutorialText::m_apTextureFile[TEX_MAX] =	// テクスチャのファイル
{
	"data\\TEXTURE\\tutorialtalk\\window_01.png",
	"data\\TEXTURE\\tutorialtalk\\text_00.png",
	"data\\TEXTURE\\tutorialtalk\\text_01.png",
	"data\\TEXTURE\\tutorialtalk\\text_02.png",
	"data\\TEXTURE\\tutorialtalk\\text_03.png",
	"data\\TEXTURE\\tutorialtalk\\text_04.png",
	"data\\TEXTURE\\tutorialtalk\\text_05.png",
	"data\\TEXTURE\\tutorialtalk\\skip_01.png",
};

//==========================================================================
// コンストラクタ
//==========================================================================
CTutorialText::CTutorialText(int nPriority) : CObject(nPriority)
{
	// 値のクリア
	memset(&m_nTexIdx[0], 0, sizeof(m_nTexIdx));	// テクスチャのインデックス番号
	memset(&m_pObj2D[0], NULL, sizeof(m_pObj2D));	// オブジェクト2Dのオブジェクト
	m_nCntMoving = 0;	// 移動してるカウンター
}

//==========================================================================
// デストラクタ
//==========================================================================
CTutorialText::~CTutorialText()
{

}

//==========================================================================
// 生成処理
//==========================================================================
CTutorialText *CTutorialText::Create(void)
{
	// 生成用のオブジェクト
	CTutorialText *pTitleScreen = NULL;

	if (pTitleScreen == NULL)
	{// NULLだったら

		// メモリの確保
		pTitleScreen = DEBUG_NEW CTutorialText;

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
HRESULT CTutorialText::Init(void)
{
	// 種類の設定
	SetType(TYPE_TITLESCREEN);

	for (int nCntTex = 0; nCntTex < TEX_MAX; nCntTex++)
	{
		// テクスチャの割り当て
		m_nTexIdx[nCntTex] = CManager::GetTexture()->Regist(m_apTextureFile[nCntTex]);
	}

	for (int nCntVtx = 0; nCntVtx < VTX_MAX; nCntVtx++)
	{
		// 生成処理
		m_pObj2D[nCntVtx] = m_pObj2D[nCntVtx]->Create(7);

		// 種類の設定
		m_pObj2D[nCntVtx]->SetType(TYPE_TITLESELECT);

		// テクスチャの割り当て
		D3DXVECTOR2 size;
		switch (nCntVtx)
		{
		case VTX_WINDOW:
			m_pObj2D[nCntVtx]->BindTexture(m_nTexIdx[TEX_WINDOW]);

			// サイズ取得
			size = CManager::GetTexture()->GetImageSize(m_nTexIdx[TEX_WINDOW]);
			m_pObj2D[nCntVtx]->SetSize(size * 0.4f);	// サイズ
			m_pObj2D[nCntVtx]->SetPosition(D3DXVECTOR3(640.0f, SCREEN_HEIGHT - m_pObj2D[nCntVtx]->GetSize().y, 0.0f));	// 位置
			break;

		case VTX_TEXT:
			m_pObj2D[nCntVtx]->BindTexture(m_nTexIdx[TEX_TEXT_WAIT]);

			// サイズ取得
			size = CManager::GetTexture()->GetImageSize(m_nTexIdx[TEX_TEXT_WAIT]);
			m_pObj2D[nCntVtx]->SetSize(size * 0.4f);	// サイズ
			m_pObj2D[nCntVtx]->SetPosition(D3DXVECTOR3(640.0f, SCREEN_HEIGHT - m_pObj2D[nCntVtx]->GetSize().y, 0.0f));	// 位置
			break;

		case VTX_SKIP:
			m_pObj2D[nCntVtx]->BindTexture(m_nTexIdx[TEX_SKIP]);

			// サイズ取得
			size = CManager::GetTexture()->GetImageSize(m_nTexIdx[TEX_SKIP]);
			m_pObj2D[nCntVtx]->SetSize(size * 0.2f);	// サイズ
			m_pObj2D[nCntVtx]->SetPosition(D3DXVECTOR3(1020.0f, 560.0f, 0.0f));	// 位置
			break;
		}
	}

	return S_OK;
}

//==========================================================================
// 終了処理
//==========================================================================
void CTutorialText::Uninit(void)
{
	//for (int nCntSelect = 0; nCntSelect < VTX_MAX; nCntSelect++)
	//{
	//	if (m_pObj2D[nCntSelect] != NULL)
	//	{// NULLじゃなかったら

	//		// 終了処理
	//		m_pObj2D[nCntSelect]->Uninit();
	//		//delete m_pObj2D[nCntSelect];
	//		m_pObj2D[nCntSelect] = NULL;
	//	}
	//}

	// 情報削除
	Release();
}

//==========================================================================
// 更新処理
//==========================================================================
void CTutorialText::Update(void)
{
	// テクスチャ切り替え
	m_pObj2D[VTX_TEXT]->BindTexture(m_nTexIdx[TEX_TEXT_WAIT + CTutorial::GetStep()->GetNowStep()]);

	// プレイヤーが移動してる判定
	bool bMoving = CTutorial::IsMovingPlayer();

	// 移動してるカウンター加算
	if (bMoving == true)
	{
		m_nCntMoving++;
	}
	else
	{
		m_nCntMoving--;
	}

	// 値の正規化
	ValueNormalize(m_nCntMoving, 20, 0);



	for (int nCntVtx = 0; nCntVtx < VTX_MAX; nCntVtx++)
	{
		if (m_pObj2D[nCntVtx] == NULL)
		{// NULLだったら
			continue;
		}

		// 色取得
		D3DXCOLOR col = m_pObj2D[nCntVtx]->GetColor();

		if (m_nCntMoving >= 20)
		{// 行動してるとき

			col.a -= ALPHA_MOVE;
		}
		else if(m_nCntMoving <= 0)
		{
			col.a += ALPHA_MOVE;
		}


		// 値の正規化
		ValueNormalize(col.a, 1.0f, 0.3f);

		// 色設定
		m_pObj2D[nCntVtx]->SetColor(col);

		// 頂点情報設定
		SetVtx(nCntVtx);
	}
}

//==========================================================================
// 描画処理
//==========================================================================
void CTutorialText::Draw(void)
{
	//// デバイスの取得
	//LPDIRECT3DDEVICE9 pDevice = CManager::GetRenderer()->GetDevice();

	//// アルファテストを有効にする
	//pDevice->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
	//pDevice->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);
	//pDevice->SetRenderState(D3DRS_ALPHAREF, 0);

	//for (int nCntSelect = 0; nCntSelect < VTX_MAX; nCntSelect++)
	//{
	//	// 描画処理
	//	//m_pObj2D[nCntSelect]->Draw();
	//}

	//// アルファテストを無効にする
	//pDevice->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
	//pDevice->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_ALWAYS);
	//pDevice->SetRenderState(D3DRS_ALPHAREF, 0);
}

//==========================================================================
// 頂点情報設定処理
//==========================================================================
void CTutorialText::SetVtx(void)
{

}

//==========================================================================
// 頂点情報設定処理
//==========================================================================
void CTutorialText::SetVtx(int nCntSelect)
{
	// 頂点設定
	m_pObj2D[nCntSelect]->SetVtx();
}
