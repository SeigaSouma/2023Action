//=============================================================================
// 
//  弾爆発処理 [bulletexplosion.cpp]
//  Author : 相馬靜雅
// 
//=============================================================================
#include "bulletexplosion.h"
#include "manager.h"
#include "texture.h"
#include "renderer.h"
#include "sound.h"
#include "enemy.h"
#include "calculation.h"
#include "mapmanager.h"
#include "game.h"
#include "player.h"
#include "effect_thunderring.h"

//==========================================================================
// マクロ定義
//==========================================================================
#define TEXTURE			"data\\TEXTURE\\sunder_03.png"

//==========================================================================
// 静的メンバ変数宣言
//==========================================================================
int CBulletExplosion::m_nNumAll = 0;		// 弾の総数

//==========================================================================
// コンストラクタ
//==========================================================================
CBulletExplosion::CBulletExplosion(int nPriority) : CMeshSphere(nPriority)
{
	// 値のクリア
	m_nLife = 0;	// 寿命
	m_nLifeMax = 0;	// 寿命の最大値
	m_nTexIdx = 0;	// テクスチャのインデックス番号
	m_fDestSize = 0.0f;		// 目標のサイズ
	rotmove = mylib_const::DEFAULT_VECTOR3;	// 向きの移動量
	m_pEffectThunderRing = NULL;	// 雷のリングのエフェクト
	memset(&m_pBulletAppearance[0], NULL, sizeof(m_pBulletAppearance));	// 見た目だけの弾

	// 総数加算
	m_nNumAll++;
}

//==========================================================================
// デストラクタ
//==========================================================================
CBulletExplosion::~CBulletExplosion()
{
	
}

//==========================================================================
// 生成処理
//==========================================================================
CBulletExplosion *CBulletExplosion::Create(const D3DXVECTOR3 pos, const float fSize)
{
	// 生成用のオブジェクト
	CBulletExplosion *pBullet = NULL;

	if (pBullet == NULL)
	{// NULLだったら

		// メモリの確保
		pBullet = DEBUG_NEW CBulletExplosion;

		if (pBullet != NULL)
		{// メモリの確保が出来ていたら

			// 位置割り当て
			pBullet->SetPosition(pos);

			// サイズ設定
			pBullet->m_fDestSize = fSize;		// 目標のサイズ
			pBullet->SetWidthLen(0.0f);
			pBullet->SetHeightLen(0.0f);
			pBullet->SetWidthBlock(16);
			pBullet->SetHeightBlock(16);

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
HRESULT CBulletExplosion::Init(void)
{
	HRESULT hr;

	// 各種変数の初期化
	m_nLifeMax = 40;
	m_nLife = m_nLifeMax;	// 寿命
	SetColor(D3DXCOLOR(1.0f, 0.2f, 1.0f, 0.7f));
	rotmove.x = D3DX_PI * (Random(50, 100) * 0.001f);
	rotmove.y = D3DX_PI * (Random(50, 100) * 0.001f);
	rotmove.z = rotmove.x;

	// テクスチャの割り当て
	m_nTexIdx = CManager::GetInstance()->GetTexture()->Regist(TEXTURE);

	// テクスチャの割り当て
	BindTexture(m_nTexIdx);

	// 種類の設定
	CObject::SetType(TYPE_BULLET);

	// 初期化処理
	hr = CMeshSphere::Init();
	if (FAILED(hr))
	{// 失敗したとき
		return E_FAIL;
	}

	if (m_pEffectThunderRing == NULL)
	{
		// 半径
		float fSize = GetWidthLen();

		// 雷のリング生成
		m_pEffectThunderRing = CThunderRing::Create(GetPosition(), D3DXVECTOR2(fSize + 150.0f, fSize + 150.0f));

		// 色設定
		m_pEffectThunderRing->SetColor(D3DXCOLOR(1.0f, 0.3f, 1.0f, 1.0f));

		// 自動で消えないようにする
		m_pEffectThunderRing->SetDesableAutoDeath();
	}

	// テクスチャの割り当て
	int nTex = CManager::GetInstance()->GetTexture()->Regist("data\\TEXTURE\\effect\\effect000.jpg");

	// 見た目だけの弾
	for (int i = 0; i < mylib_const::BULLETAPPEARANCE_NUM; i++)
	{
		if (m_pBulletAppearance[i] != NULL)
		{
			continue;
		}
		m_pBulletAppearance[i] = CObjectBillboard::Create(GetPosition(), mylib_const::DEFAULT_VECTOR3);
		m_pBulletAppearance[i]->SetSize(D3DXVECTOR2(GetWidthLen(), GetHeightLen()));
		m_pBulletAppearance[i]->SetColor(D3DXCOLOR(0.3f, 0.3f, 1.0f, 1.0f));
		m_pBulletAppearance[i]->BindTexture(nTex);
		m_pBulletAppearance[i]->SetType(CObject::TYPE_NONE);
	}

	return S_OK;
}

//==========================================================================
// 終了処理
//==========================================================================
void CBulletExplosion::Uninit(void)
{
	if (m_pEffectThunderRing != NULL)
	{// 雷のリングのエフェクト

		// 終了処理
		m_pEffectThunderRing->Uninit();
		m_pEffectThunderRing = NULL;
	}

	// 見た目だけの弾
	for (int i = 0; i < mylib_const::BULLETAPPEARANCE_NUM; i++)
	{
		if (m_pBulletAppearance[i] != NULL)
		{
			m_pBulletAppearance[i]->Uninit();
			m_pBulletAppearance[i] = NULL;
		}
	}

	// 終了処理
	CMeshSphere::Uninit();

	// 総数減算
	m_nNumAll--;
}

//==========================================================================
// 更新処理
//==========================================================================
void CBulletExplosion::Update(void)
{
	// 位置更新
	UpdatePos();

	// 当たり判定
	CollisionEnemy();

	if (IsDeath() == true)
	{// 死亡フラグが立っていたら
		return;
	}

	if (m_pEffectThunderRing != NULL)
	{// 雷のリングのエフェクト

		// 位置更新
		m_pEffectThunderRing->SetPosition(GetPosition());

		// サイズ更新
		float fSize = GetWidthLen();
		m_pEffectThunderRing->SetSize(D3DXVECTOR2(fSize + 550.0f, fSize + 550.0f));
	}

	// 見た目だけの弾の更新
	for (int i = 0; i < mylib_const::BULLETAPPEARANCE_NUM; i++)
	{
		if (m_pBulletAppearance[i] == NULL)
		{
			continue;
		}
		m_pBulletAppearance[i]->Update();
		m_pBulletAppearance[i]->SetPosition(GetPosition());
		float fSize = GetWidthLen();
		m_pBulletAppearance[i]->SetSize(D3DXVECTOR2(fSize, fSize) * 0.9f);
	}

	// 寿命減算
	m_nLife--;

	if (m_nLife <= 0)
	{// 寿命が尽きたら
		Uninit();
		return;
	}

	// 頂点情報設定
	SetVtx();
}

//==========================================================================
// 移動処理
//==========================================================================
void CBulletExplosion::UpdatePos(void)
{
	// 向き取得
	D3DXVECTOR3 rot = GetRotation();

	// サイズ取得
	float fWidthLen = GetWidthLen();
	float fHeightLen = GetHeightLen();

	// 目標のサイズへ更新
	fWidthLen += (m_fDestSize - fWidthLen) * 0.25f;
	fHeightLen += (m_fDestSize - fHeightLen) * 0.25f;

	// 回転
	rot += rotmove;

	// 向き設定
	SetRotation(rot);

	// サイズ設定
	SetWidthLen(fWidthLen);
	SetHeightLen(fHeightLen);
}

//==========================================================================
// 敵との判定
//==========================================================================
void CBulletExplosion::CollisionEnemy(void)
{
	// 敵マネージャ取得
	CEnemyManager *pEnemyManager = CGame::GetEnemyManager();
	if (pEnemyManager == NULL)
	{// NULLだったら
		return;
	}

	// 敵情報取得
	CEnemy **ppEnemy = pEnemyManager->GetEnemy();
	int nNumEnemy = pEnemyManager->GetNumAll();

	// 情報取得
	D3DXVECTOR3 pos = GetPosition();
	float fRadius = GetWidthLen();

	int nUse = 0;
	for (int nCntEnemy = 0; nUse < nNumEnemy; nCntEnemy++)
	{
		if (ppEnemy[nCntEnemy] == NULL)
		{// NULLだったら
			continue;
		}
		nUse++;

		// 敵の情報取得
		D3DXVECTOR3 EnemyPosition = ppEnemy[nCntEnemy]->GetPosition();
		float fEnemyRadius = ppEnemy[nCntEnemy]->GetRadius();

		if (SphereRange(pos, EnemyPosition, fRadius, fEnemyRadius))
		{// 当たっていたら

			// ヒット処理
			ppEnemy[nCntEnemy]->Hit(mylib_const::DMG_BOUNCE);
		}
	}

}

//==========================================================================
// 描画処理
//==========================================================================
void CBulletExplosion::Draw(void)
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

	// ビルボードの描画
	CMeshSphere::Draw();

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

	// 見た目だけの弾の描画
	for (int i = 0; i < mylib_const::BULLETAPPEARANCE_NUM; i++)
	{
		if (m_pBulletAppearance[i] == NULL)
		{
			continue;
		}
		m_pBulletAppearance[i]->Draw();
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

//==========================================================================
// 頂点情報設定処理
//==========================================================================
void CBulletExplosion::SetVtx(void)
{
	// 頂点設定
	CMeshSphere::SetVtx();
}

//==========================================================================
// 総数取得
//==========================================================================
int CBulletExplosion::GetNumAll(void)
{
	return m_nNumAll;
}
