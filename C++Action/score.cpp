//=============================================================================
// 
//  背景処理 [score.cpp]
//  Author : 相馬靜雅
// 
//=============================================================================
#include "score.h"
#include "manager.h"
#include "texture.h"
#include "renderer.h"
#include "object2D.h"
#include "multinumber.h"

//==========================================================================
// マクロ定義
//==========================================================================
#define TEXTURE			"data\\TEXTURE\\number_blackclover_01.png"	// テクスチャのファイル
#define MAX_VALUE		(999999)						// 値の最大値
#define TEX_U			(0.1f)							// Uの分割
#define WIDTH			(40.0f)							// 横幅
#define HEIGHT			(50.0f)							// 縦幅
#define DIS_X			(50.0f)							// 間隔
#define SCROLL_SPEED	(-0.005f)						// スクロール速度

//==========================================================================
// 静的メンバ変数宣言
//==========================================================================

//==========================================================================
// コンストラクタ
//==========================================================================
CScore::CScore(int nPriority)
{
	// 値のクリア
	m_apNumber = NULL;
	m_nNum = 0;		// 値
	m_nTexIdx = 0;		// テクスチャのインデックス番号
}

//==========================================================================
// デストラクタ
//==========================================================================
CScore::~CScore()
{

}

//==========================================================================
// 生成処理
//==========================================================================
CScore *CScore::Create(void)
{
	// 生成用のオブジェクト
	CScore *pScore = NULL;

	if (pScore == NULL)
	{// NULLだったら

		// メモリの確保
		pScore = DEBUG_NEW CScore;

		if (pScore != NULL)
		{// メモリの確保が出来ていたら

			// 初期化処理
			pScore->Init();
		}

		return pScore;
	}

	return NULL;
}

//==========================================================================
// 生成処理
//==========================================================================
CScore *CScore::Create(D3DXVECTOR3 pos)
{
	// 生成用のオブジェクト
	CScore *pScore = NULL;

	if (pScore == NULL)
	{// NULLだったら

		// メモリの確保
		pScore = DEBUG_NEW CScore;

		if (pScore != NULL)
		{// メモリの確保が出来ていたら

			// 初期化処理
			pScore->Init(pos);
		}

		return pScore;
	}

	return NULL;
}

//==========================================================================
// 初期化処理
//==========================================================================
HRESULT CScore::Init(void)
{
	
	return S_OK;
}

//==========================================================================
// 初期化処理
//==========================================================================
HRESULT CScore::Init(D3DXVECTOR3 pos)
{

	// 生成処理
	m_apNumber = CMultiNumber::Create(pos, D3DXVECTOR2(WIDTH, HEIGHT), NUM_SCORE, CNumber::OBJECTTYPE_2D);
	return S_OK;
}

//==========================================================================
// 終了処理
//==========================================================================
void CScore::Uninit(void)
{
	// 数字のオブジェクトの終了処理
	if (m_apNumber != NULL)
	{
		m_apNumber->Uninit();
		delete m_apNumber;
		m_apNumber = NULL;
	}
}

//==========================================================================
// 更新処理
//==========================================================================
void CScore::Update(void)
{
	// 値の設定処理
	m_apNumber->SetValue(m_nNum);
}

//==========================================================================
// 値の設定処理
//==========================================================================
void CScore::SetValue(void)
{
	int aTexU[NUM_SCORE];
	int nDigit = 1;		// aTexU計算用

	// 計算用割り出し
	for (int nCntDigit = 0; nCntDigit < NUM_SCORE; nCntDigit++)
	{
		nDigit *= 10;
	}

	// テクスチャ座標に代入する
	for (int nCntTex = 0; nCntTex < NUM_SCORE; nCntTex++)
	{// 桁数分設定

		aTexU[nCntTex] = m_nNum % nDigit / (nDigit / 10);
		nDigit /= 10;
	}

	
}

//==========================================================================
// 描画処理
//==========================================================================
void CScore::Draw(void)
{
	m_apNumber->Draw();
}

//==========================================================================
// 加算処理
//==========================================================================
void CScore::Add(int nValue)
{
	// 値加算
	m_nNum += nValue;

	// スコアの加算
	CManager::GetResultManager()->AddScore(nValue);

	if (m_nNum >= MAX_VALUE)
	{// 最大値で固定
		m_nNum = MAX_VALUE;
	}

	// 値の設定処理
	SetValue();
}

//==========================================================================
// 加算処理
//==========================================================================
void CScore::Add(CResultManager::ADDTYPE AddType)
{
	// 値加算
	/*switch (AddType)
	{
	case CResultManager::ADDTYPE_NONE:
		break;

	case CResultManager::ADDTYPE_NORMAL:
		m_nNum += mylib_const::COMBOBONUS_1;
		break;

	case CResultManager::ADDTYPE_BLUE:
		m_nNum += mylib_const::COMBOBONUS_2;
		break;

	case CResultManager::ADDTYPE_GOLD:
		m_nNum += mylib_const::COMBOBONUS_3;
		break;

	default:
		break;
	}*/

	// スコアの加算
	CManager::GetResultManager()->AddScoreNum(AddType);

	if (m_nNum >= MAX_VALUE)
	{// 最大値で固定
		m_nNum = MAX_VALUE;
	}

	// 値の設定処理
	SetValue();
}
