//=============================================================================
// 
// エフェクト処理 [3D_effect.cpp]
// Author : 相馬靜雅
// 
//=============================================================================
#include "renderer.h"
#include "texture.h"
#include "manager.h"
#include "3D_effect.h"

//==========================================================================
// マクロ定義
//==========================================================================
#define DEF_RADIUS			(20.0f)
#define EFFECT_3D_LIFE		(30)
#define EFFECT_3DSIZE1		(0.97f)
#define EFFECT_3DSIZE2		(0.98f)
#define EFFECT_3DSIZE3		(0.99f)

//==========================================================================
// 静的メンバ変数宣言
//==========================================================================
const char *CEffect3D::m_apTextureFile[] =					// ファイル読み込み
{
	"data\\TEXTURE\\effect\\effect000.jpg",	   // 通常エフェクト
	"data\\TEXTURE\\effect\\smoke_05.tga",	   // 煙エフェクト
	"data\\TEXTURE\\effect\\smoke_05.tga",	   // 黒煙
	"data\\TEXTURE\\effect\\effect000.png",	   // 黒エフェクト
	"data\\TEXTURE\\effect\\effect001.png",	   // 十字エフェクト
	"data\\TEXTURE\\effect\\sand_01.png",	   // 十字エフェクト
	"data\\TEXTURE\\effect\\sand_02.png",	   // 十字エフェクト
	"data\\TEXTURE\\effect\\line_02.png",	   // 線エフェクト
	"data\\TEXTURE\\grassblades_01.png",	   // 草エフェクト
	"data\\TEXTURE\\effect\\sweat_01.png",	   // 汗エフェクト
};
int CEffect3D::m_nNumAll = 0;	// 総数
int CEffect3D::m_nTexIdx[TYPE_MAX] = {};				// テクスチャのインデックス番号

//==========================================================================
// コンストラクタ
//==========================================================================
CEffect3D::CEffect3D(int nPriority) : CObjectBillboard(nPriority)
{
	// 値のクリア
	m_posOrigin = D3DXVECTOR3(0.0f, 0.0f, 0.0f);		// 原点
	m_updatePosition = D3DXVECTOR3(0.0f, 0.0f, 0.0f);	// 更新後の位置
	m_setupPosition = D3DXVECTOR3(0.0f, 0.0f, 0.0f);	// セットアップ位置
	m_colOrigin = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);	// 色の元
	m_fRadius = 0.0f;							// 半径
	m_fMaxRadius = 0.0f;						// 最大半径
	m_fAddSizeValue = 0.0f;						// サイズ変更量
	m_fSetupRotation = 0.0f;					// セットアップの向き
	m_fSetupVec = 0.0f;							// セットアップの強さ
	m_nLife = 0;								// 寿命
	m_nMaxLife = 0;								// 最大寿命(固定)
	m_moveType = MOVEEFFECT_NONE;				// 移動の種類
	m_nType = TYPE_NORMAL;						// 種類
	m_pParent = NULL;							// 親のポインタ
	m_bAddAlpha = true;							// 加算合成の判定

	// 総数加算
	m_nNumAll++;
}

//==========================================================================
// デストラクタ
//==========================================================================
CEffect3D::~CEffect3D()
{
	
}

//==========================================================================
// 生成処理
//==========================================================================
CEffect3D *CEffect3D::Create(void)
{
	// 生成用のオブジェクト
	CEffect3D *pEffect = NULL;

	if (pEffect == NULL)
	{// NULLだったら

		// メモリの確保
		pEffect = DEBUG_NEW CEffect3D;

		//if (pEffect->GetID() < 0)
		//{// メモリ確保に失敗していたら

		//	delete pEffect;
		//	return NULL;
		//}

		if (pEffect != NULL)
		{// メモリの確保が出来ていたら

			// 初期化処理
			pEffect->Init();
		}

		return pEffect;
	}

	return NULL;
}

//==========================================================================
// 生成処理
//==========================================================================
CEffect3D *CEffect3D::Create(const D3DXVECTOR3 pos, const D3DXVECTOR3 move, const D3DXCOLOR col, const float fRadius, const int nLife, const int moveType, const TYPE type, const float fAddSizeValue)
{
	// 生成用のオブジェクト
	CEffect3D *pEffect = NULL;

	if (pEffect == NULL)
	{// NULLだったら

		// メモリの確保
		pEffect = DEBUG_NEW CEffect3D;

		//if (pEffect->GetID() < 0)
		//{// メモリ確保に失敗していたら

		//	delete pEffect;
		//	return NULL;
		//}

		if (pEffect != NULL)
		{// メモリの確保が出来ていたら

			// 初期化処理
			pEffect->m_fAddSizeValue = fAddSizeValue;	// サイズ変更量
			HRESULT hr = pEffect->Init(pos, move, col, fRadius, nLife, moveType, type);
			if (FAILED(hr))
			{
				return NULL;
			}
		}

		return pEffect;
	}

	return NULL;
}

//==================================================================================
// エフェクトの初期化処理
//==================================================================================
HRESULT CEffect3D::Init(void)
{
	HRESULT hr;

	// 各種変数の初期化
	m_posOrigin = D3DXVECTOR3(0.0f, 0.0f, 0.0f);		// 原点
	m_updatePosition = D3DXVECTOR3(0.0f, 0.0f, 0.0f);	// 更新後の位置
	m_setupPosition = D3DXVECTOR3(0.0f, 0.0f, 0.0f);	// セットアップ位置
	m_colOrigin = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);	// 色の元
	m_fRadius = DEF_RADIUS;						// 半径
	m_fMaxRadius = m_fRadius;					// 最大半径
	m_nLife = EFFECT_3D_LIFE;					// 寿命
	m_nMaxLife = m_nLife;						// 最大寿命(固定)
	m_moveType = MOVEEFFECT_NONE;				// 移動の種類
	m_nType = TYPE_NORMAL;						// 種類
	m_bAddAlpha = true;							// 加算合成の判定

	// 種類の設定
	SetType(TYPE_EFFECT3D);

	// テクスチャの割り当て
	if (m_nTexIdx[m_nType] == 0)
	{
		m_nTexIdx[m_nType] = CManager::GetTexture()->Regist(m_apTextureFile[m_nType]);
	}

	// テクスチャの割り当て
	BindTexture(m_nTexIdx[m_nType]);

	// 初期化処理
	hr = CObjectBillboard::Init();

	if (FAILED(hr))
	{// 失敗したとき

		Uninit();
		return E_FAIL;
	}

	return S_OK;
}

//==================================================================================
// エフェクトの初期化処理
//==================================================================================
HRESULT CEffect3D::Init(const D3DXVECTOR3 pos, const D3DXVECTOR3 move, const D3DXCOLOR col, const float fRadius, const int nLife, const int moveType, const TYPE type)
{
	HRESULT hr;

	// 各種変数の初期化
	m_posOrigin = pos;							// 原点
	m_updatePosition = D3DXVECTOR3(0.0f, 0.0f, 0.0f);	// 更新後の位置
	m_setupPosition = D3DXVECTOR3(0.0f, 0.0f, 0.0f);	// セットアップ位置
	SetPosition(pos);							// 位置
	SetMove(move);								// 移動量
	m_colOrigin = col;							// 色の元
	SetColor(col);								// 色
	m_fRadius = fRadius;						// 半径
	m_fMaxRadius = m_fRadius;					// 最大半径
	SetSize(D3DXVECTOR2(m_fRadius, m_fRadius));	// サイズ設定
	m_nLife = nLife;							// 寿命
	m_nMaxLife = m_nLife;						// 最大寿命(固定)
	m_moveType = moveType;						// 移動の種類
	m_nType = type;								// 種類

	if (m_nType >= TYPE_MAX)
	{
		int n = 0;
	}

	// 加算合成の判定
	switch (m_nType)
	{
	case TYPE_NORMAL:
		m_bAddAlpha = true;
		break;

	case TYPE_SMOKE:
		m_bAddAlpha = true;
		break;

	case TYPE_SMOKEBLACK:
		m_bAddAlpha = false;
		break;

	case TYPE_BLACK:
		m_bAddAlpha = false;
		break;

	case TYPE_JUJI:
		m_bAddAlpha = true;
		break;

	case TYPE_SAND:
		m_bAddAlpha = true;
		break;

	case TYPE_SANDBLACK:
		m_bAddAlpha = false;
		break;

	case TYPE_LINE:
		m_bAddAlpha = true;
		break;

	case TYPE_GRASS:
		m_bAddAlpha = false;
		break;

	case TYPE_SWEAT:
		m_bAddAlpha = false;
		break;

	default:
		m_bAddAlpha = true;
		break;
	}

	// 種類の設定
	SetType(TYPE_EFFECT3D);

	// テクスチャの割り当て
	if (m_nTexIdx[m_nType] == 0)
	{
		m_nTexIdx[m_nType] = CManager::GetTexture()->Regist(m_apTextureFile[m_nType]);
	}

	// テクスチャの割り当て
	BindTexture(m_nTexIdx[m_nType]);

	// 初期化処理
	hr = CObjectBillboard::Init();

	if (FAILED(hr))
	{// 失敗したとき
		Uninit();
		return E_FAIL;
	}

	return S_OK;
}

//==================================================================================
// セットアップ
//==================================================================================
void CEffect3D::SetUp(D3DXVECTOR3 setup, CObject *pObj, int nParentIdx)
{
	// 親のポインタ渡す
	if (m_pParent == NULL)
	{
		m_pParent = pObj;
	}

	// 親のインデックス番号
	m_nParentIdx = nParentIdx;

	// セットアップ位置
	m_setupPosition = setup;

	// セットアップの向き
	m_fSetupRotation = atan2f((m_setupPosition.x - 0.0f), (m_setupPosition.z - 0.0f));

	// セットアップの強さ
	m_fSetupVec =
		sqrtf((0.0f - m_setupPosition.x) * (0.0f - m_setupPosition.x)
			+ (0.0f - m_setupPosition.z) * (0.0f - m_setupPosition.z));
}

//==================================================================================
// エフェクトの終了処理
//==================================================================================
void CEffect3D::Uninit(void)
{
	if (m_pParent != NULL)
	{
		// エフェクトの開放
		m_pParent->ReleaseEffect(m_nParentIdx);
		m_pParent = NULL;
	}

	// 総数減算
	m_nNumAll--;

	// 終了処理
	CObjectBillboard::Uninit();
}

//==================================================================================
// 親の破棄
//==================================================================================
void CEffect3D::UninitParent(void)
{
	// 親をNULLにする
	m_pParent = NULL;
}

//==================================================================================
// エフェクトの更新処理
//==================================================================================
void CEffect3D::Update(void)
{

	// 位置取得
	D3DXVECTOR3 pos = GetPosition();

	// 移動量取得
	D3DXVECTOR3 move = GetMove();

	// 色取得
	D3DXCOLOR col = GetColor();

	// 位置更新
	m_updatePosition += move;
	pos = m_posOrigin + m_updatePosition;

	// 位置設定
	SetPosition(pos);

	// 移動量設定
	SetMove(move);

	// 小さくしていく処理の場合
	if (m_moveType == MOVEEFFECT_SUB)
	{// エフェクトを小さくしていく

		// 縮小処理
		SubSize();
	}
	else if (m_moveType == MOVEEFFECT_SUPERSUB)
	{// エフェクトを小さくしていく

		// 超縮小処理
		SuperSubSize();
	}
	else if (m_moveType == MOVEEFFECT_ADD)
	{// エフェクトを大きくしていく

		// 拡大処理
		AddSize();
	}

	// 寿命の更新
	m_nLife--;

	// 不透明度の更新
	col.a = m_colOrigin.a * ((float)m_nLife / (float)m_nMaxLife);

	// サイズ設定
	SetSize(D3DXVECTOR2(m_fRadius, m_fRadius));

	// 色設定
	SetColor(col);

	if (m_nLife <= 0)
	{// 寿命が尽きたら

		// エフェクトの削除
		Uninit();
		return;
	}

	// 頂点座標の設定
	SetVtx();

}

//==================================================================================
// 位置更新
//==================================================================================
void CEffect3D::UpdatePosition(D3DXVECTOR3 pos, D3DXVECTOR3 rot)
{
	// 親の情報取得
	D3DXVECTOR3 ParentPos = pos;
	D3DXVECTOR3 ParentRot = rot;

	// 原点更新
	m_posOrigin =
		D3DXVECTOR3(
			ParentPos.x + sinf(ParentRot.y + m_fSetupRotation) * m_fSetupVec,
			ParentPos.y,
			ParentPos.z + cosf(ParentRot.y + m_fSetupRotation) * m_fSetupVec) +
		D3DXVECTOR3(
			sinf(ParentRot.y + m_fSetupRotation) * m_fSetupVec,
			m_setupPosition.y,
			cosf(ParentRot.y + m_fSetupRotation) * m_fSetupVec);
}

//==================================================================================
// エフェクトの縮小処理
//==================================================================================
void CEffect3D::SubSize(void)
{
	int nEffect_3DType = rand() % 3;

	if (nEffect_3DType == 0)
	{
		m_fRadius *= EFFECT_3DSIZE1;
	}
	else if (nEffect_3DType == 1)
	{
		m_fRadius *= EFFECT_3DSIZE2;
	}
	else if (nEffect_3DType == 2)
	{
		m_fRadius *= EFFECT_3DSIZE3;
	}
}

//==================================================================================
// エフェクトの縮小処理
//==================================================================================
void CEffect3D::SuperSubSize(void)
{
	m_fRadius = m_fMaxRadius * (float)m_nLife / (float)m_nMaxLife;
}

//==================================================================================
// エフェクトの拡大処理
//==================================================================================
void CEffect3D::AddSize(void)
{
	// 拡大
	m_fRadius += m_fAddSizeValue;
}

//==================================================================================
// エフェクトの描画処理
//==================================================================================
void CEffect3D::Draw(void)
{
	// デバイスの取得
	LPDIRECT3DDEVICE9 pDevice = CManager::GetRenderer()->GetDevice();

	// ライティングを無効にする
	pDevice->SetRenderState(D3DRS_LIGHTING, FALSE);

	// Zテストを無効にする
	pDevice->SetRenderState(D3DRS_ZFUNC, D3DCMP_ALWAYS);
	pDevice->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);	//常に描画する

	// アルファテストを有効にする
	pDevice->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
	pDevice->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);
	pDevice->SetRenderState(D3DRS_ALPHAREF, 0);

	// αブレンディングを加算合成に設定
	if (m_bAddAlpha == true)
	{
		pDevice->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
		pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
		pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);
	}

	// 描画処理
	CObjectBillboard::Draw();

	// αブレンディングを元に戻す
	pDevice->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
	pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

	// アルファテストを無効にする
	pDevice->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
	pDevice->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_ALWAYS);
	pDevice->SetRenderState(D3DRS_ALPHAREF, 0);

	// Zテストを有効にする
	pDevice->SetRenderState(D3DRS_ZFUNC, D3DCMP_LESSEQUAL);
	pDevice->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);

	// ライティングを有効にする
	pDevice->SetRenderState(D3DRS_LIGHTING, TRUE);
}

//==========================================================================
// 頂点情報設定処理
//==========================================================================
void CEffect3D::SetVtx(void)
{
	// 頂点設定
	CObjectBillboard::SetVtx();
}

//==========================================================================
// 総数取得
//==========================================================================
int CEffect3D::GetNumAll(void)
{
	return m_nNumAll;
}
