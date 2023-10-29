//=============================================================================
// 
//  ヒットスコア処理 [hitscore.cpp]
//  Author : 相馬靜雅
// 
//=============================================================================
#include "hitscore.h"
#include "manager.h"
#include "texture.h"
#include "renderer.h"
#include "object2D.h"
#include "multinumber.h"
#include "calculation.h"
#include "game.h"
#include "score.h"
#include "effect_addscore.h"

//==========================================================================
// マクロ定義
//==========================================================================
#define TEXTURE			"data\\TEXTURE\\number_blackclover_02.png"	// テクスチャのファイル
#define HIT_TEXTURE		"data\\TEXTURE\\hit.png"	// テクスチャのファイル
#define NUM_SCORE		(4)			// 桁数
#define MAX_VALUE		(999999)	// 値の最大値
#define TEX_U			(0.1f)		// Uの分割
#define WIDTH			(40.0f)		// 横幅
#define HEIGHT			(40.0f)		// 縦幅
#define MOVETIME		(20)		// 移動時間
#define COOLTIME		(60 * 5 + MOVETIME)	// 移動までのクールタイム
#define DEST_POSITION	(1000.0f)	// 目標の位置

//==========================================================================
// 静的メンバ変数宣言
//==========================================================================

//==========================================================================
// コンストラクタ
//==========================================================================
CHitScore::CHitScore(int nPriority)
{
	// 値のクリア
	m_apNumber = NULL;
	m_pObj2D = NULL;	// オブジェクト2Dのオブジェクト
	m_nTexIdx = 0;		// テクスチャのインデックス番号
	m_nCntCooltime = 0;	// クールタイムのカウンター
	m_posOrigin = mylib_const::DEFAULT_VECTOR3;	// 元の位置
}

//==========================================================================
// デストラクタ
//==========================================================================
CHitScore::~CHitScore()
{

}

//==========================================================================
// 生成処理
//==========================================================================
CHitScore *CHitScore::Create(void)
{
	// 生成用のオブジェクト
	CHitScore *pScore = NULL;

	if (pScore == NULL)
	{// NULLだったら

		// メモリの確保
		pScore = DEBUG_NEW CHitScore;

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
CHitScore *CHitScore::Create(D3DXVECTOR3 pos)
{
	// 生成用のオブジェクト
	CHitScore *pScore = NULL;

	if (pScore == NULL)
	{// NULLだったら

		// メモリの確保
		pScore = DEBUG_NEW CHitScore;

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
HRESULT CHitScore::Init(void)
{
	// 生成処理
	m_apNumber = CMultiNumber::Create(mylib_const::DEFAULT_VECTOR3, D3DXVECTOR2(WIDTH, HEIGHT), NUM_SCORE, CNumber::OBJECTTYPE_2D, TEXTURE, true);

	// 値の設定処理
	m_apNumber->SetValue(0);
	return S_OK;
}

//==========================================================================
// 初期化処理
//==========================================================================
HRESULT CHitScore::Init(D3DXVECTOR3 pos)
{
	// 元の位置
	m_posOrigin = pos;

	// 生成処理
	m_apNumber = CMultiNumber::Create(m_posOrigin, D3DXVECTOR2(WIDTH, HEIGHT), NUM_SCORE, CNumber::OBJECTTYPE_2D, TEXTURE, true);

	// オブジェクト2Dのオブジェクト
	m_pObj2D = CObject2D::Create(7);
	m_pObj2D->SetType(CObject::TYPE_BALLAST);

	int nHitTex = CManager::GetInstance()->GetTexture()->Regist(HIT_TEXTURE);
	m_pObj2D->BindTexture(nHitTex);

	m_pObj2D->SetSize(CManager::GetInstance()->GetTexture()->GetImageSize(nHitTex) * 0.3f);
	m_pObj2D->SetPosition(D3DXVECTOR3(m_posOrigin.x + (WIDTH * NUM_SCORE) + m_pObj2D->GetSize().x - 20.0f, m_posOrigin.y, m_posOrigin.z));

	// 値の設定処理
	m_apNumber->SetValue(0);
	return S_OK;
}

//==========================================================================
// 終了処理
//==========================================================================
void CHitScore::Uninit(void)
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
void CHitScore::Update(void)
{
	// 移動処理
	UpdateMove();

	// マルチナンバーの更新
	m_apNumber->Update();
}

//==========================================================================
// 移動処理
//==========================================================================
void CHitScore::UpdateMove(void)
{
	// クールタイム減算
	m_nCntCooltime--;

	// 位置取得
	D3DXVECTOR3 pos = m_apNumber->GetPosition();
	D3DXVECTOR3 posOrigin = pos;
	D3DXVECTOR3 Hitpos = m_pObj2D->GetPosition();

	if (m_nCntCooltime == MOVETIME)
	{
		// スコア計算後加算
		CalScore();

		// ヒット数リセット
		CGame::GetHitScore()->Reset();
	}

	if (m_nCntCooltime <= MOVETIME)
	{// 移動時間以下で移動

		// 移動
		pos.x += (m_posOrigin.x - pos.x) * 0.1f;
	}
	else
	{
		// 移動
		pos.x += (DEST_POSITION - pos.x) * 0.1f;
	}

	if (m_nCntCooltime <= 0)
	{
		m_nCntCooltime = 0;
	}

	float fDiff = pos.x - posOrigin.x;
	Hitpos.x += fDiff;

	// 位置設定
	m_apNumber->SetPosition(pos);
	m_pObj2D->SetPosition(Hitpos);
}

//==========================================================================
// スコア計算
//==========================================================================
void CHitScore::CalScore(void)
{
	// 値の取得
	int nNum = m_apNumber->GetValue();

	// 倍率計算
	int nMultiply = nNum / 100;

	// スコア加算量
	int nAddValue = (1.0f + ((float)nMultiply * 0.1f)) * (nNum * 10);

	// スコア加算
	CGame::GetScore()->Add(nAddValue);

	// 加算エフェクト
	CEffectAddScore::Create(m_apNumber->GetPosition());

}

//==========================================================================
// 描画処理
//==========================================================================
void CHitScore::Draw(void)
{
	// 描画処理
	m_apNumber->Draw();
}

//==========================================================================
// 加算処理
//==========================================================================
void CHitScore::Add(int nValue)
{
	// 値の取得
	int nNum = m_apNumber->GetValue();

	// 値加算
	nNum += nValue;

	if (nNum >= MAX_VALUE)
	{// 最大値で固定
		nNum = MAX_VALUE;
	}

	// 値の設定
	m_apNumber->SetValue(nNum);

	// クールタイムのカウンターリセット
	m_nCntCooltime = COOLTIME;

}

//==========================================================================
// リセット
//==========================================================================
void CHitScore::Reset(void)
{
	// 値の設定
	m_apNumber->SetValue(0);
}
