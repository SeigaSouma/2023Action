//=============================================================================
// 
//  リザルト画面処理 [result_screen.cpp]
//  Author : 相馬靜雅
// 
//=============================================================================
#include "result_screen.h"
#include "manager.h"
#include "renderer.h"
#include "texture.h"
#include "object2D.h"
#include "calculation.h"
#include "multinumber.h"
#include "resultscore.h"
#include "sound.h"

//==========================================================================
// マクロ定義
//==========================================================================
#define NUMBERTEX	"data\\TEXTURE\\number_blackclover_01.png"
#define DETAILSCORE_POSX		(400.0f)		// スコア詳細のY位置
#define DETAILSCORE_POSY		(450.0f)		// スコア詳細のY位置
#define DETAILSCORE_DISTANCE	(420.0f * 0.6f)	// スコア詳細の間隔
#define NUMBERDIS_X				(30.0f)			// 数字の感覚
#define INIT_POSX				(1600.0f)		// 初期位置
#define TEXT_MOVETIME			(20)			// 移動時間
#define INT_NUMBERSET			(60)			// スコアの変動時間

//==========================================================================
// 静的メンバ変数宣言
//==========================================================================
const char *CResultScreen::m_apTextureFile[VTX_LOGO_MAX] =				// テクスチャのファイル
{
	"data\\TEXTURE\\resultlogo_01.png",
	"data\\TEXTURE\\resultscore_01.png",
	"data\\TEXTURE\\resultscore_02.png",
	"data\\TEXTURE\\resultscore_03.png",
};

//==========================================================================
// コンストラクタ
//==========================================================================
CResultScreen::CResultScreen(int nPriority) : CObject(nPriority)
{
	// 値のクリア
	for (int nCntTex = 0; nCntTex < VTX_LOGO_MAX; nCntTex++)
	{
		m_nTexIdx[nCntTex] = 0;		// テクスチャのインデックス番号
		m_pObj2D[nCntTex] = NULL;
	}
	memset(&m_fPosDestObj2D[0], NULL, sizeof(m_fPosDestObj2D));	// オブジェ2Dの目標位置
	memset(&m_fPosDestNumber[0], NULL, sizeof(m_fPosDestNumber));	// 数字の目標位置
	memset(&m_apNumber[0], NULL, sizeof(m_apNumber));	// 数字のオブジェクト
	memset(&m_nNumDest[0], NULL, sizeof(m_nNumDest));	// ボーナス数の目標値
	memset(&m_fNum[0], NULL, sizeof(m_fNum));	// ボーナス数
	m_nTexIdxNumber = 0;								// 数字テクスチャのインデックス番号
	m_nArrival = false;	// 到着したかの判定
	m_bSetNumber = false;										// 設定したかの判定
	m_pResultScore = NULL;						// リザルトスコアのオブジェクト
}

//==========================================================================
// デストラクタ
//==========================================================================
CResultScreen::~CResultScreen()
{

}

//==========================================================================
// 生成処理
//==========================================================================
CResultScreen *CResultScreen::Create(void)
{
	// 生成用のオブジェクト
	CResultScreen *pTitleScreen = NULL;

	if (pTitleScreen == NULL)
	{// NULLだったら

		// メモリの確保
		pTitleScreen = DEBUG_NEW CResultScreen;

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
HRESULT CResultScreen::Init(void)
{
	// 種類の設定
	SetType(TYPE_RESULTSCREEN);

	for (int nCntSelect = 0; nCntSelect < VTX_LOGO_MAX; nCntSelect++)
	{
		// 生成処理
		m_pObj2D[nCntSelect] = CObject2D::Create(7);

		// 種類の設定
		m_pObj2D[nCntSelect]->SetType(TYPE_RESULTSELECT);

		// テクスチャの割り当て
		m_nTexIdx[nCntSelect] = CManager::GetInstance()->GetTexture()->Regist(m_apTextureFile[nCntSelect]);

		// テクスチャの割り当て
		m_pObj2D[nCntSelect]->BindTexture(m_nTexIdx[nCntSelect]);

		int nNum = nCntSelect - VTX_RESULTSCORE_RED;
		float fPosX = DETAILSCORE_POSX + nNum * DETAILSCORE_DISTANCE;

		if (nCntSelect >= VTX_RESULTSCORE_NORMAL)
		{
			int nIdx = nCntSelect - VTX_RESULTSCORE_NORMAL;
			m_fPosDestObj2D[nIdx] = fPosX;
		}

		// 各種変数の初期化
		switch (nCntSelect)
		{
		case VTX_LOGO:
			m_pObj2D[nCntSelect]->SetSize(CManager::GetInstance()->GetTexture()->GetImageSize(m_nTexIdx[nCntSelect]) * 0.4f);	// サイズ
			m_pObj2D[nCntSelect]->SetPosition(D3DXVECTOR3(640.0f, 150.0f, 0.0f));	// 位置
			break;

		case VTX_RESULTSCORE_NORMAL:
			m_pObj2D[nCntSelect]->SetSize(CManager::GetInstance()->GetTexture()->GetImageSize(m_nTexIdx[nCntSelect]) * 0.25f);	// サイズ
			m_pObj2D[nCntSelect]->SetPosition(D3DXVECTOR3(INIT_POSX, DETAILSCORE_POSY, 0.0f));	// 位置
			break;

		case VTX_RESULTSCORE_RED:
			m_pObj2D[nCntSelect]->SetSize(CManager::GetInstance()->GetTexture()->GetImageSize(m_nTexIdx[nCntSelect]) * 0.25f);	// サイズ
			m_pObj2D[nCntSelect]->SetPosition(D3DXVECTOR3(INIT_POSX, DETAILSCORE_POSY, 0.0f));	// 位置
			break;

		case VTX_RESULTSCORE_GOLD:
			m_pObj2D[nCntSelect]->SetSize(CManager::GetInstance()->GetTexture()->GetImageSize(m_nTexIdx[nCntSelect]) * 0.25f);	// サイズ
			m_pObj2D[nCntSelect]->SetPosition(D3DXVECTOR3(INIT_POSX, DETAILSCORE_POSY, 0.0f));	// 位置
			break;
		}
	}


	// 数字テクスチャの割り当て
	m_nTexIdxNumber = CManager::GetInstance()->GetTexture()->Regist(NUMBERTEX);

	for (int nCntType = 0; nCntType < MAX_SCORETYPE; nCntType++)
	{
		float fPosX = (DETAILSCORE_POSX + 40.0f) + (nCntType - 1) * DETAILSCORE_DISTANCE;

		// 生成処理
		m_apNumber[nCntType] = CMultiNumber::Create(D3DXVECTOR3(INIT_POSX, DETAILSCORE_POSY + 5.0f, 0.0f), D3DXVECTOR2(40.0f, 50.0f) * 0.6f, SCORETYPE_DIGIT, CNumber::OBJECTTYPE_2D);

		//m_fPosDestNumber[nCntType] = fPosX + nCntScore * NUMBERDIS_X;
		m_fPosDestNumber[nCntType] = fPosX;
	}

	// リザルトのスコア設定
	m_pResultScore = CResultScore::Create();

	return S_OK;
}

//==========================================================================
// 終了処理
//==========================================================================
void CResultScreen::Uninit(void)
{
	// リザルトスコア
	m_pResultScore = NULL;

	for (int nCntSelect = 0; nCntSelect < VTX_LOGO_MAX; nCntSelect++)
	{
		if (m_pObj2D[nCntSelect] != NULL)
		{// NULLじゃなかったら

			// 終了処理
			//m_pObj2D[nCntSelect]->Uninit();
			m_pObj2D[nCntSelect] = NULL;
		}
	}

	for (int nCntType = 0; nCntType < MAX_SCORETYPE; nCntType++)
	{
		// 数字のオブジェクトの終了処理
		if (m_apNumber[nCntType] != NULL)
		{
			m_apNumber[nCntType]->Uninit();
			delete m_apNumber[nCntType];
			m_apNumber[nCntType] = NULL;
		}
	}

	// 情報削除
	Release();
}

//==========================================================================
// 更新処理
//==========================================================================
void CResultScreen::Update(void)
{
	if (m_nArrival == false)
	{
		for (int nCntSelect = 0; nCntSelect < VTX_LOGO_MAX; nCntSelect++)
		{
			if (m_pObj2D[nCntSelect] != NULL)
			{// NULLじゃなかったら

				if (m_pResultScore->IsSetBase() == true && nCntSelect >= VTX_RESULTSCORE_NORMAL)
				{
					// オブジェ2Dの移動処理
					MoveObj2D(nCntSelect);
				}

				// 頂点情報設定
				SetVtx(nCntSelect);
			}
		}
	}

	if (m_bSetNumber == false)
	{
		for (int nCntType = 0; nCntType < MAX_SCORETYPE; nCntType++)
		{
			if (m_pResultScore->IsSetBase() == true)
			{
				// 数字の移動処理
				MoveNumber(nCntType);
			}

			// 情報削除
			m_apNumber[nCntType]->SetValue((int)m_fNum[nCntType]);
		}
	}
}

//==========================================================================
// オブジェ2Dの移動処理
//==========================================================================
void CResultScreen::MoveObj2D(int nCntType)
{
	int nIdx = nCntType - VTX_RESULTSCORE_NORMAL;

	// 位置取得
	D3DXVECTOR3 pos = m_pObj2D[nCntType]->GetPosition();

	// 移動
	pos.x += (m_fPosDestObj2D[nIdx] - INIT_POSX) / (float)TEXT_MOVETIME;
	if (pos.x <= m_fPosDestObj2D[nIdx])
	{// 補正完了
		pos.x = m_fPosDestObj2D[nIdx];
		m_nArrival = true;	// 到着の判定
	}

	// 位置設定
	m_pObj2D[nCntType]->SetPosition(pos);
}

//==========================================================================
// 数字の移動処理
//==========================================================================
void CResultScreen::MoveNumber(int nCntType)
{

	// 位置取得
	D3DXVECTOR3 pos = m_apNumber[nCntType]->GetPosition();
	if (pos.x == INIT_POSX)
	{
		// サウンド再生
		CManager::GetInstance()->GetSound()->PlaySound(CSound::LABEL_SE_NUMBERMOVE);
	}

	// 移動
	pos.x += (m_fPosDestNumber[nCntType] - INIT_POSX) / (float)TEXT_MOVETIME;
	if (pos.x <= m_fPosDestNumber[nCntType])
	{// 補正完了
		pos.x = m_fPosDestNumber[nCntType];
		//m_nArrival = true;	// 到着の判定
	}

	// 位置設定
	m_apNumber[nCntType]->SetPosition(pos);

	if (m_nArrival == true)
	{// 到着していたら

		// 目標の数値
#if _DEBUG
		m_nNumDest[nCntType] = 100;
#else
		m_nNumDest[nCntType] = CManager::GetInstance()->GetResultManager()->GetScoreTypeNum((CResultManager::ADDTYPE)(nCntType + 1));
#endif
		if (m_nNumDest[nCntType] > m_fNum[nCntType])
		{// 規定時間かけて補正

			// サウンド再生
			CManager::GetInstance()->GetSound()->PlaySound(CSound::LABEL_SE_NUMBER);

			m_fNum[nCntType] += ((float)m_nNumDest[nCntType] / (float)INT_NUMBERSET);
		}
		else
		{// 目標の値で固定
			m_fNum[nCntType] = (float)m_nNumDest[nCntType];
			m_bSetNumber = true;
		}
	}

}

//==========================================================================
// 全ての到着設定
//==========================================================================
void CResultScreen::SetAllArrival(void)
{

	// 全ての到着処理
	m_pResultScore->SetAllArrival();

	m_nArrival = true;		// 到着したかの判定
	m_bSetNumber = true;	// 設定したかの判定

	for (int nCntSelect = 0; nCntSelect < VTX_LOGO_MAX; nCntSelect++)
	{
		if (m_pObj2D[nCntSelect] != NULL && nCntSelect >= VTX_RESULTSCORE_NORMAL)
		{// NULLじゃなかったら

			int nIdx = nCntSelect - VTX_RESULTSCORE_NORMAL;

			// 位置取得
			D3DXVECTOR3 pos = m_pObj2D[nCntSelect]->GetPosition();

			// 移動
			pos.x = m_fPosDestObj2D[nIdx];

			// 位置設定
			m_pObj2D[nCntSelect]->SetPosition(pos);

			// 頂点情報設定
			SetVtx(nCntSelect);
		}
	}

	for (int nCntType = 0; nCntType < MAX_SCORETYPE; nCntType++)
	{
		// 位置取得
		D3DXVECTOR3 pos = m_apNumber[nCntType]->GetPosition();

		// 移動
		pos.x = m_fPosDestNumber[nCntType];

		// 位置設定
		m_apNumber[nCntType]->SetPosition(pos);

		// 目標の値取得
		m_nNumDest[nCntType] = CManager::GetInstance()->GetResultManager()->GetScoreTypeNum((CResultManager::ADDTYPE)(nCntType + 1));
		m_fNum[nCntType] = (float)m_nNumDest[nCntType];

		// 情報削除
		m_apNumber[nCntType]->SetValue((int)m_fNum[nCntType]);
	}

}

//==========================================================================
// 描画処理
//==========================================================================
void CResultScreen::Draw(void)
{
	// デバイスの取得
	LPDIRECT3DDEVICE9 pDevice = CManager::GetInstance()->GetRenderer()->GetDevice();

	//// アルファテストを有効にする
	//pDevice->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
	//pDevice->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);
	//pDevice->SetRenderState(D3DRS_ALPHAREF, 0);

	//for (int nCntSelect = 0; nCntSelect < VTX_LOGO_MAX; nCntSelect++)
	//{
	//	// 描画処理
	//	m_pObj2D[nCntSelect]->Draw();
	//}

	//// アルファテストを無効にする
	//pDevice->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
	//pDevice->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_ALWAYS);
	//pDevice->SetRenderState(D3DRS_ALPHAREF, 0);
}

//==========================================================================
// 頂点情報設定処理
//==========================================================================
void CResultScreen::SetVtx(void)
{

}

//==========================================================================
// 頂点情報設定処理
//==========================================================================
void CResultScreen::SetVtx(int nCntSelect)
{
	// 頂点設定
	m_pObj2D[nCntSelect]->SetVtx();
}
