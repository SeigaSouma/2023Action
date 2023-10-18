//=============================================================================
// 
//  瓦礫処理 [ballast.cpp]
//  Author : 相馬靜雅
// 
//=============================================================================
#include "game.h"
#include "ballast.h"
#include "manager.h"
#include "renderer.h"
#include "calculation.h"
#include "objectX.h"
#include "elevation.h"

//==========================================================================
// マクロ定義
//==========================================================================
#define BAI		(0.4f)
#define LIFE	(40)	// 寿命
#define MOVE	(2.0f)	// 移動量
#define MOVE_Y	(30.0f * BAI)	// 移動量

//==========================================================================
// 静的メンバ変数宣言
//==========================================================================
const char *CBallast::m_pFileName[TYPE_MAX] = // ファイル名
{
	"data\\MODEL\\ballast_01.x",
	"data\\MODEL\\magic_ice.x"
};
int CBallast::m_nIdxXFile[TYPE_MAX] = {};			// Xファイルのインデックス番号

//==========================================================================
// コンストラクタ
//==========================================================================
CBallast::CBallast(int nPriority) : CObject(nPriority)
{
	m_posOrigin = D3DXVECTOR3(0.0f, 0.0f, 0.0f);	// 原点
	m_moveOrigin = D3DXVECTOR3(0.0f, 0.0f, 0.0f);	// 元の移動量
	m_col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);		// 色
	m_colOrigin = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);	// 原色
	m_nCntParabola = 0;	// 瓦礫の移動カウント
	m_nNumAll = 0;		// 総数
	m_nLife = 0;		// 寿命
	m_nMaxLife = 0;		// 最大寿命
	m_type =TYPE_STONE;	// 種類
	memset(&m_pObjX[0], NULL, sizeof(m_pObjX));	// オブジェクトXのポインタ
}

//==========================================================================
// デストラクタ
//==========================================================================
CBallast::~CBallast()
{
	
}

//==========================================================================
// 生成処理
//==========================================================================
CBallast *CBallast::Create(D3DXVECTOR3 pos, D3DXVECTOR3 move, int nNum, float fAlpha, TYPE type)
{
	// 生成用のオブジェクト
	CBallast *pBallast = NULL;

	if (pBallast == NULL)
	{// NULLだったら

		// メモリの確保
		pBallast = DEBUG_NEW CBallast;

		if (pBallast != NULL)
		{// メモリの確保が出来ていたら

			if (nNum >= MAX_BALLAST)
			{
				nNum = MAX_BALLAST;
			}

			// 総数設定
			pBallast->m_nNumAll = nNum;

			// 種類設定
			pBallast->m_type = type;

			// 位置設定
			pBallast->m_posOrigin = pos;

			// 移動量設定
			pBallast->m_moveOrigin = move;

			// 原色設定
			pBallast->m_colOrigin.a = fAlpha;

			// 初期化処理
			HRESULT hr = pBallast->Init();

			if (FAILED(hr))
			{
				return NULL;
			}
		}

		return pBallast;
	}

	return NULL;
}

//==========================================================================
// 初期化処理
//==========================================================================
HRESULT CBallast::Init(void)
{
	// 種類の設定
	SetType(TYPE_BALLAST);

	// 寿命設定
	m_nLife = LIFE * m_nNumAll;
	m_nMaxLife = m_nLife;

	for (int nCntBallast = 0; nCntBallast < m_nNumAll; nCntBallast++)
	{
		// 生成処理
		m_pObjX[nCntBallast] = CObjectX::Create(
			m_pFileName[m_type],
			D3DXVECTOR3(m_posOrigin.x + Random(-20, 20), m_posOrigin.y, m_posOrigin.z + Random(-20, 20)),
			D3DXVECTOR3((float)Random(-314, 314) * 0.01f, (float)Random(-314, 314) * 0.01f, (float)Random(-314, 314) * 0.01f), false);

		if (m_pObjX[nCntBallast] == NULL)
		{// 失敗したとき

			// オブジェクトXの終了処理
			delete m_pObjX[nCntBallast];
			m_pObjX[nCntBallast] = NULL;
			return E_FAIL;
		}
		
		// 移動量設定
		m_pObjX[nCntBallast]->SetMove(D3DXVECTOR3(
			(float)Random(-314, 314) * 0.01f * m_moveOrigin.x,
			m_moveOrigin.y + ((float)Random(-20, 20) * 0.1f),
			(float)Random(-314, 314) * 0.01f * m_moveOrigin.z));

		// スケール設定
		switch (m_type)
		{
		case TYPE_STONE:

			m_pObjX[nCntBallast]->SetScale(D3DXVECTOR3(
				(float)Random(-20, 20) * 0.05f,
				(float)Random(-20, 20) * 0.05f,
				(float)Random(-20, 20) * 0.05f));
			break;

		case TYPE_ICE:
			m_pObjX[nCntBallast]->SetScale(D3DXVECTOR3(
				(float)Random(-10, 10) * 0.1f,
				(float)Random(-10, 10) * 0.1f,
				(float)Random(-10, 10) * 0.1f));
			break;
		}
	}

	return S_OK;
}

//==========================================================================
// 終了処理
//==========================================================================
void CBallast::Uninit(void)
{
	for (int nCntBallast = 0; nCntBallast < MAX_BALLAST; nCntBallast++)
	{
		if (m_pObjX[nCntBallast] != NULL)
		{// NULLじゃなかったら

			// オブジェクトXの終了処理
			m_pObjX[nCntBallast]->Uninit();
			m_pObjX[nCntBallast] = NULL;
		}
	}

	// 情報削除
	Release();
}

//==========================================================================
// 更新処理
//==========================================================================
void CBallast::Update(void)
{
	// 瓦礫の移動カウント加算
	m_nCntParabola++;

	for (int nCntBallast = 0; nCntBallast < m_nNumAll; nCntBallast++)
	{
		if (m_pObjX[nCntBallast] == NULL)
		{// NULLだったら

			// 寿命更新
			m_nLife = 0;
			continue;
		}

		// 位置取得
		D3DXVECTOR3 pos = m_pObjX[nCntBallast]->GetPosition();

		// 移動量取得
		D3DXVECTOR3 move = m_pObjX[nCntBallast]->GetMove();

		// 向き取得
		D3DXVECTOR3 rot = m_pObjX[nCntBallast]->GetRotation();

		if (m_type == TYPE_ICE)
		{
			// 向き取得
			D3DXVECTOR3 scale = m_pObjX[nCntBallast]->GetScale();

			scale *= 0.98f;

			// サイズ設定
			m_pObjX[nCntBallast]->SetScale(scale);

		}

		float PosY = pos.y;
		if (CGame::GetElevation()->IsHit(D3DXVECTOR3(pos.x, PosY, pos.z)) == true || GetPosition().y <= mylib_const::KILL_Y)
		{// 地面を下回ったら

			// 寿命更新
			m_nLife--;

			pos.y = PosY;
		}
		else
		{
			// 位置更新
			pos = (D3DXVECTOR3(0.0f, -0.5f * BAI, 0.0f) * (float)(m_nCntParabola * m_nCntParabola) + move * (float)m_nCntParabola) + m_posOrigin;

			// 向き更新
			rot += D3DXVECTOR3(0.05f, 0.05f, -0.02f);
		}

		// 不透明度更新
		m_col.a = m_colOrigin.a * ((float)m_nLife / (float)m_nMaxLife);

		// 位置設定
		m_pObjX[nCntBallast]->SetPosition(pos);

		// 向き設定
		m_pObjX[nCntBallast]->SetRotation(rot);
	}

	if (m_nLife <= 0)
	{// 寿命が尽きたら

		// 終了処理
		Uninit();
	}
}

//==========================================================================
// 描画処理
//==========================================================================
void CBallast::Draw(void)
{
	// デバイスの取得
	LPDIRECT3DDEVICE9 pDevice = CManager::GetInstance()->GetRenderer()->GetDevice();

	for (int nCntBallast = 0; nCntBallast < m_nNumAll; nCntBallast++)
	{
		if (m_pObjX[nCntBallast] != NULL)
		{// NULLじゃなかったら44

			// 描画処理
			m_pObjX[nCntBallast]->Draw(m_col.a);
		}
	}

}
