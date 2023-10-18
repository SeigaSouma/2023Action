//=============================================================================
// 
//  メッシュシリンダー処理 [impactwave.cpp]
//  Author : 相馬靜雅
// 
//=============================================================================
#include "impactwave.h"
#include "manager.h"
#include "renderer.h"
#include "texture.h"

//==========================================================================
// マクロ定義
//==========================================================================
#define TEXTURE		"data\\TEXTURE\\field002.png"
#define POS_IMPACTWAVE	(5000.0f)
#define POS_IMPACTWAVE_Y	(3000.0f)
#define WIDTH	(32)

//==========================================================================
// 静的メンバ変数宣言
//==========================================================================
const char *CImpactWave::m_apFilename[] =	//ファイル読み込み
{
	"data\\TEXTURE\\GRADATION\\black_01.jpg",
	"data\\TEXTURE\\GRADATION\\black_02.jpg",
	"data\\TEXTURE\\GRADATION\\orange_01.jpg",
	"data\\TEXTURE\\GRADATION\\orange_02.jpg",
	"data\\TEXTURE\\GRADATION\\blue_01.jpg",
	"data\\TEXTURE\\GRADATION\\blue_02.jpg",
	"data\\TEXTURE\\GRADATION\\green_01.jpg",
	"data\\TEXTURE\\GRADATION\\green_02.jpg",
	"data\\TEXTURE\\GRADATION\\purple_01.jpg",
	"data\\TEXTURE\\GRADATION\\purple_02.jpg",
	"data\\TEXTURE\\GRADATION\\purple_03.jpg",
	"data\\TEXTURE\\GRADATION\\purple_04.jpg",
	"data\\TEXTURE\\GRADATION\\pink_01.jpg",
	"data\\TEXTURE\\effect\\effect003.tga",
	"data\\TEXTURE\\GRADATION\\giza_01.png",
	"data\\TEXTURE\\GRADATION\\giza_02.png",
	"data\\TEXTURE\\GRADATION\\giza_03.png",
	"data\\TEXTURE\\GRADATION\\giza_06.png",
};

//==========================================================================
// コンストラクタ
//==========================================================================
CImpactWave::CImpactWave(int nPriority) : CObject3DMesh(nPriority)
{
	m_nTexIdx = 0;										// テクスチャのインデックス番号
	m_nLife = 0;										// 寿命
	m_nMaxLife = 0;										// 最大寿命
	m_nTexDivision = 0;									// テクスチャ分割
	m_colOrigin = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);	// 元の色
	m_fOutWidth = 0.0f;		// 横幅(外)
	m_fInWidth = 0.0f;		// 幅(内)
	m_fRotWidth = 0.0f;		// 1分割数あたりの角度割合
	m_fHeight = 0.0f;		// 高さ
	m_fMove = 0.0f;			// 広がる速度
	m_bAddBlend = false;	// 加算合成の判定
}

//==========================================================================
// デストラクタ
//==========================================================================
CImpactWave::~CImpactWave()
{

}

//==========================================================================
// 生成処理
//==========================================================================
CImpactWave *CImpactWave::Create(D3DXVECTOR3 pos, D3DXVECTOR3 rot, D3DXCOLOR col, float fWidth, float fHeight, int nLife, float fMove, int nTexType, bool bAddBlend)
{
	// 生成用のオブジェクト
	CImpactWave *pObjMeshCylinder = NULL;

	if (pObjMeshCylinder == NULL)
	{// NULLだったら

		// メモリの確保
		pObjMeshCylinder = DEBUG_NEW CImpactWave;

		if (pObjMeshCylinder != NULL)
		{// メモリの確保が出来ていたら

			// 位置・向き
			pObjMeshCylinder->SetPosition(pos);
			pObjMeshCylinder->SetRotation(rot);
			pObjMeshCylinder->SetColor(col);
			pObjMeshCylinder->m_colOrigin = col;
			pObjMeshCylinder->SetWidthBlock(WIDTH);
			pObjMeshCylinder->SetHeightBlock(1);
			pObjMeshCylinder->SetWidthLen(POS_IMPACTWAVE);
			pObjMeshCylinder->SetHeightLen(POS_IMPACTWAVE_Y);

			pObjMeshCylinder->m_fOutWidth = fWidth;		// 横幅(外)
			pObjMeshCylinder->m_fInWidth = fWidth;		// 幅(内)
			pObjMeshCylinder->m_fHeight = fHeight;		// 高さ
			pObjMeshCylinder->m_nLife = nLife;			// 寿命
			pObjMeshCylinder->m_fMove = fMove;			// 広がる速度
			pObjMeshCylinder->m_bAddBlend = bAddBlend;	// 加算合成の判定

			// テクスチャの割り当て
			pObjMeshCylinder->m_nTexIdx = CManager::GetInstance()->GetTexture()->Regist(m_apFilename[nTexType]);

			// テクスチャの割り当て
			pObjMeshCylinder->BindTexture(pObjMeshCylinder->m_nTexIdx);

			// 初期化処理
			pObjMeshCylinder->Init();
		}

		return pObjMeshCylinder;
	}

	return NULL;
}

//==========================================================================
// 初期化処理
//==========================================================================
HRESULT CImpactWave::Init(void)
{
	HRESULT hr;

	// デバイスの取得
	LPDIRECT3DDEVICE9 pDevice = CManager::GetInstance()->GetRenderer()->GetDevice();

	// 種類設定
	SetType(TYPE_MESHDONUTS);

	m_fRotWidth = (D3DX_PI * 2) / (float)(WIDTH);		//1分割数あたりの角度割合
	m_nMaxLife = m_nLife;	// 最大寿命
	m_nTexDivision = 4;		// テクスチャ分割

	// オブジェクト3Dメッシュの初期化処理
	hr = CObject3DMesh::Init(CObject3DMesh::TYPE_DONUTS);

	if (FAILED(hr))
	{// 失敗していたら
		return E_FAIL;
	}

	return S_OK;
}

//==========================================================================
// 終了処理
//==========================================================================
void CImpactWave::Uninit(void)
{
	// 終了処理
	CObject3DMesh::Uninit();
}

//==========================================================================
// 更新処理
//==========================================================================
void CImpactWave::Update(void)
{
	// 色取得
	D3DXCOLOR col = GetColor();

	// 広げていく
	m_fOutWidth += m_fMove;

	// 寿命更新
	m_nLife--;

	// 不透明度更新
	col.a = m_colOrigin.a * (float)m_nLife / (float)m_nMaxLife;

	// 色設定
	SetColor(col);

	// 頂点情報設定
	SetVtx();
	
	if (m_nLife <= 0)
	{// 寿命が尽きた

		// 終了処理
		Uninit();
	}
}

//==========================================================================
// 描画処理
//==========================================================================
void CImpactWave::Draw(void)
{
	// デバイスの取得
	LPDIRECT3DDEVICE9 pDevice = CManager::GetInstance()->GetRenderer()->GetDevice();


	// 背面のカリングなし
	pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	// ライティングを無効にする
	pDevice->SetRenderState(D3DRS_LIGHTING, FALSE);

	// αブレンディングを加算合成に設定
	if (m_bAddBlend == true)
	{
		pDevice->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
		pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
		pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);
	}

	// アルファテストを有効にする
	pDevice->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
	pDevice->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);
	pDevice->SetRenderState(D3DRS_ALPHAREF, 0);

	// 描画処理
	CObject3DMesh::Draw();

	// アルファテストを無効にする
	pDevice->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
	pDevice->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_ALWAYS);
	pDevice->SetRenderState(D3DRS_ALPHAREF, 0);

	// αブレンディングを元に戻す
	pDevice->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
	pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

	// ライティングを有効にする
	pDevice->SetRenderState(D3DRS_LIGHTING, TRUE);

	// カリングのデフォルト
	pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
}

//==========================================================================
// 頂点情報設定処理
//==========================================================================
void CImpactWave::SetVtx(void)
{
	D3DXVECTOR3 *pVtxPos = GetVtxPos();	// 頂点座標取得
	D3DXVECTOR2 *pVtxTex = GetVtxTex();	// テクスチャ座標取得

	// 分割数取得
	int nHeightBlock = GetHeightBlock();
	int nWidthBlock = GetWidthBlock();

	// 頂点情報の設定
	for (int nCntHeight = 0; nCntHeight < nHeightBlock + 1; nCntHeight++)
	{// 縦の頂点数分繰り返す

		for (int nCntWidth = 0; nCntWidth < nWidthBlock + 1; nCntWidth++)
		{// 横の頂点数分繰り返す

			float fLength = (m_fOutWidth - nCntHeight * m_fInWidth);
			if (fLength <= 0.0f)
			{
				fLength = 0.0f;
			}

			// 頂点座標の設定
			pVtxPos[nCntWidth + (nCntHeight * (nWidthBlock + 1))] = D3DXVECTOR3
			(
				sinf(nCntWidth % nWidthBlock * m_fRotWidth) * fLength,
				nCntHeight * m_fHeight,
				cosf(nCntWidth % nWidthBlock * m_fRotWidth) * fLength
			);

			pVtxTex[nCntWidth + (nCntHeight * (nWidthBlock + 1))] = D3DXVECTOR2
			(
				nCntWidth * (1.0f / (float)(nWidthBlock / m_nTexDivision)),
				nCntHeight * (1.0f / (float)(nHeightBlock))
			);
		}
	}

	// 頂点座標設定
	SetVtxPos(pVtxPos);

	// 頂点情報更新
	CObject3DMesh::SetVtx();
}

//==========================================================================
// 寿命設定
//==========================================================================
void CImpactWave::SetLife(const int nLife)
{
	m_nLife = nLife;
}

//==========================================================================
// 寿命取得
//==========================================================================
int CImpactWave::GetLife(void) const
{
	return m_nLife;
}

//==========================================================================
// 最大寿命設定
//==========================================================================
void CImpactWave::SetMaxLife(const int nLife)
{
	m_nMaxLife = nLife;
}

//==========================================================================
// 最大寿命取得
//==========================================================================
int CImpactWave::GetMaxLife(void) const
{
	return m_nMaxLife;
}

//==========================================================================
// 元の色設定
//==========================================================================
void CImpactWave::SetColOrigin(const D3DXCOLOR col)
{
	m_colOrigin = col;
}

//==========================================================================
// 元の色取得
//==========================================================================
D3DXCOLOR CImpactWave::GetColOrigin(void) const
{
	return m_colOrigin;
}

//==========================================================================
// 外周設定
//==========================================================================
void CImpactWave::SetOutWidth(const float fOutWidth)
{
	m_fOutWidth = fOutWidth;
}

//==========================================================================
// 外周取得
//==========================================================================
float CImpactWave::GetOutWidth(void) const
{
	return m_fOutWidth;
}

//==========================================================================
// 内周設定
//==========================================================================
void CImpactWave::SetInWidth(const float fInWidth)
{
	m_fInWidth = fInWidth;
}

//==========================================================================
// 内周取得
//==========================================================================
float CImpactWave::GetInWidth(void) const
{
	return m_fInWidth;
}

//==========================================================================
// 一分割あたりの角度設定
//==========================================================================
void CImpactWave::SetRotWidth(const float fRot)
{
	m_fRotWidth = fRot;
}

//==========================================================================
// 一分割あたりの角度取得
//==========================================================================
float CImpactWave::GetRotWidth(void) const
{
	return m_fRotWidth;
}

//==========================================================================
// サイズの移動量設定
//==========================================================================
void CImpactWave::SetMoveSize(const float fMoveSize)
{
	m_fMove = fMoveSize;
}

//==========================================================================
// サイズの移動量取得
//==========================================================================
float CImpactWave::GetMoveSize(void) const
{
	return m_fMove;
}

//==========================================================================
// テクスチャ分割数設定
//==========================================================================
void CImpactWave::SetTexDivision(const int nDivision)
{
	m_nTexDivision = nDivision;
}

//==========================================================================
// テクスチャ分割数取得
//==========================================================================
int CImpactWave::GetTexDivision(void) const
{
	return m_nTexDivision;
}

//==========================================================================
// 加算合成の設定
//==========================================================================
void CImpactWave::SetIsAddBlend(bool bAddBlend)
{
	m_bAddBlend = bAddBlend;
}

//==========================================================================
// ファイル名取得
//==========================================================================
const char *CImpactWave::GetFileName(int nTexType)
{
	return m_apFilename[nTexType];
}

//==========================================================================
// メッシュシリンダーオブジェクトの取得
//==========================================================================
CImpactWave *CImpactWave::GetObject3DMesh(void)
{
	return this;
}
