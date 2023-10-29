//=============================================================================
// 
//  弾処理 [effect_addscore.cpp]
//  Author : 相馬靜雅
// 
//=============================================================================
#include "effect_addscore.h"
#include "manager.h"
#include "texture.h"
#include "renderer.h"
#include "sound.h"
#include "calculation.h"
#include "game.h"
#include "score.h"

//==========================================================================
// マクロ定義
//==========================================================================
#define TEXTURE			"data\\TEXTURE\\effect\\Star01.png"
#define SIZE			(30.0f)							// サイズ
#define MOVE_SPEED		(15.0f)							// 移動速度

//==========================================================================
// 静的メンバ変数宣言
//==========================================================================

//==========================================================================
// 関数ポインタ
//==========================================================================

//==========================================================================
// コンストラクタ
//==========================================================================
CEffectAddScore::CEffectAddScore(int nPriority) : CObject(nPriority)
{
	// 値のクリア
	m_state;				// 状態
	m_nCntState = 0;			// 状態遷移カウンター
	m_nLife = 0;				// 寿命
	m_nLifeMax = 0;				// 寿命の最大値
	m_nTexIdx = 0;				// テクスチャのインデックス番号
	memset(&m_pEffect[0], NULL, sizeof(m_pEffect));	// 見た目だけの弾
}

//==========================================================================
// デストラクタ
//==========================================================================
CEffectAddScore::~CEffectAddScore()
{
	
}

//==========================================================================
// 生成処理
//==========================================================================
CEffectAddScore *CEffectAddScore::Create(const D3DXVECTOR3 pos)
{
	// 生成用のオブジェクト
	CEffectAddScore *pBullet = NULL;

	if (pBullet == NULL)
	{// NULLだったら

		// メモリの確保
		pBullet = DEBUG_NEW CEffectAddScore;

		if (pBullet != NULL)
		{// メモリの確保が出来ていたら

			// 位置割り当て
			pBullet->SetPosition(pos);

			// 初期化処理
			pBullet->Init();
		}

		return pBullet;
	}

	return NULL;
}

//==========================================================================
// 初期化処理
//==========================================================================
HRESULT CEffectAddScore::Init(void)
{
	// 各種変数の初期化
	m_nLifeMax = 60 * 5;
	m_nLife = m_nLifeMax;	// 寿命

	// テクスチャの割り当て
	m_nTexIdx = CManager::GetInstance()->GetTexture()->Regist(TEXTURE);

	// 種類の設定
	CObject::SetType(TYPE_BULLET);

	// 見た目だけの弾
	float fff = (D3DX_PI * 2.0f) / ADDSCORE_CIRCLE;
	for (int nCntCircle = 0; nCntCircle < ADDSCORE_CIRCLE; nCntCircle++)
	{
		if (m_pEffect[nCntCircle] != NULL)
		{
			continue;
		}

		// 今回の向き
		float fRot = fff * nCntCircle;

		// バラバラ向き
		float fRotRand = (float)Random(-10, 10) / 100.0f;

		float fMove = Random(100, 200) * 0.1f;
		fMove *= 2.5f;

		//移動量の設定
		D3DXVECTOR3 move;
		move.x = sinf(fRot + fRotRand) * fMove;
		move.z = 0.0f;
		move.y = cosf(fRot + fRotRand) * fMove;

		float fRadius = 45.0f;

		// ランダムで半径減衰
		int nRand = Random(0, 2);
		if (nRand == 0)
		{
			fRadius *= 0.95f;
		}
		else if (nRand == 1)
		{
			fRadius *= 0.98f;
		}
		else if (nRand == 2)
		{
			fRadius *= 0.92f;
		}

		m_pEffect[nCntCircle] = CObject2D::Create(7);
		m_pEffect[nCntCircle]->SetPosition(GetPosition());
		m_pEffect[nCntCircle]->SetMove(move);
		m_pEffect[nCntCircle]->SetSize(D3DXVECTOR2(fRadius, fRadius));
		m_pEffect[nCntCircle]->SetColor(D3DXCOLOR(1.0f, 0.2f, 1.0f, 1.0f));
		m_pEffect[nCntCircle]->BindTexture(m_nTexIdx);
		m_pEffect[nCntCircle]->SetType(CObject::TYPE_NONE);
	}


	return S_OK;
}

//==========================================================================
// 終了処理
//==========================================================================
void CEffectAddScore::Uninit(void)
{
	// 終了処理
	Release();
}

//==========================================================================
// 更新処理
//==========================================================================
void CEffectAddScore::Update(void)
{
	// 位置更新
	UpdatePos();
}

//==========================================================================
// 移動処理
//==========================================================================
void CEffectAddScore::UpdatePos(void)
{
	// スコアの位置取得
	D3DXVECTOR3 ScorePos = CGame::GetScore()->GetPosition();

	for (int nCntCircle = 0; nCntCircle < ADDSCORE_CIRCLE; nCntCircle++)
	{
		if (m_pEffect[nCntCircle] == NULL)
		{
			continue;
		}

		// 情報取得
		D3DXVECTOR3 pos = m_pEffect[nCntCircle]->GetPosition();
		D3DXVECTOR3 rot = m_pEffect[nCntCircle]->GetRotation();
		D3DXVECTOR3 move = m_pEffect[nCntCircle]->GetMove();
		D3DXVECTOR2 size = m_pEffect[nCntCircle]->GetSize();

		// 目標の角度を求める
		float fRotDest = atan2f((pos.x - ScorePos.x), (pos.y - ScorePos.y));

		// 目標との差分
		float fRotDiff = fRotDest - rot.z;
		RotNormalize(fRotDiff);

		//角度の補正をする
		rot.z += fRotDiff * 0.1f;

		// 角度の正規化
		RotNormalize(rot.z);

		// 移動量補正
		move.x += ((sinf(D3DX_PI + rot.z) * 6.0f) - move.x) * 0.15f;
		move.y += ((cosf(D3DX_PI + rot.z) * 6.0f) - move.y) * 0.15f;

		// 位置更新
		pos += move;

		if (CircleRange2D(pos, ScorePos, size.x, 50.0f))
		{
			m_pEffect[nCntCircle]->Uninit();
			m_pEffect[nCntCircle] = NULL;
			continue;
		}

		// 情報設定
		m_pEffect[nCntCircle]->SetPosition(pos);
		m_pEffect[nCntCircle]->SetRotation(rot);
		m_pEffect[nCntCircle]->SetMove(move);
		m_pEffect[nCntCircle]->SetSize(size);

		m_pEffect[nCntCircle]->Update();
	}
}

//==========================================================================
// 描画処理
//==========================================================================
void CEffectAddScore::Draw(void)
{
	// デバイスの取得
	LPDIRECT3DDEVICE9 pDevice = CManager::GetInstance()->GetRenderer()->GetDevice();

	// アルファテストを有効にする
	pDevice->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
	pDevice->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);
	pDevice->SetRenderState(D3DRS_ALPHAREF, 0);

	// αブレンディングを加算合成に設定
	pDevice->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
	pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);

	// Zテストを無効にする
	pDevice->SetRenderState(D3DRS_ZFUNC, D3DCMP_ALWAYS);
	pDevice->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);	//常に描画する

	for (int nCntCircle = 0; nCntCircle < ADDSCORE_CIRCLE; nCntCircle++)
	{
		if (m_pEffect[nCntCircle] == NULL)
		{
			continue;
		}
		m_pEffect[nCntCircle]->Draw();
	}

	// Zテストを有効にする
	pDevice->SetRenderState(D3DRS_ZFUNC, D3DCMP_LESSEQUAL);
	pDevice->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);

	// アルファテストを無効にする
	pDevice->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
	pDevice->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_ALWAYS);
	pDevice->SetRenderState(D3DRS_ALPHAREF, 0);

	// αブレンディングを元に戻す
	pDevice->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
	pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
}

