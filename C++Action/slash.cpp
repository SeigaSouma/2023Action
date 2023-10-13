//=============================================================================
// 
//  斬撃処理 [slash.cpp]
//  Author : 相馬靜雅
// 
//=============================================================================
#include "slash.h"
#include "manager.h"
#include "renderer.h"
#include "texture.h"
#include "game.h"
#include "bulletmanager.h"
#include "calculation.h"
#include "camera.h"

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

//==========================================================================
// コンストラクタ
//==========================================================================
CSlash::CSlash(int nPriority) : CImpactWave(nPriority)
{
	m_nTexIdx = 0;										// テクスチャのインデックス番号
	m_colOrigin = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);	// 元の色
	m_fSizeDest = 0.0f;		// 目標のサイズ
}

//==========================================================================
// デストラクタ
//==========================================================================
CSlash::~CSlash()
{

}

//==========================================================================
// 生成処理
//==========================================================================
CSlash *CSlash::Create(D3DXVECTOR3 pos, D3DXVECTOR3 Parentrot, D3DXVECTOR3 rot, D3DXCOLOR col, float fWidth, float fCenterDistance, int nLife, float fAddSizeValue, int nTexType, bool bAddBlend, ANGLE angle)
{
	// 生成用のオブジェクト
	CSlash *pObjMeshCylinder = NULL;

	if (pObjMeshCylinder == NULL)
	{// NULLだったら

		// メモリの確保
		pObjMeshCylinder = DEBUG_NEW CSlash;

		if (pObjMeshCylinder != NULL)
		{// メモリの確保が出来ていたら

			// 位置・向き
			pObjMeshCylinder->SetPosition(pos);				// 位置
			pObjMeshCylinder->SetOriginRotation(Parentrot);	// 元の向き設定
			pObjMeshCylinder->SetRotation(rot);				// 向き
			pObjMeshCylinder->SetColor(col);				// 色
			pObjMeshCylinder->m_colOrigin = col;			// 元の色
			pObjMeshCylinder->SetWidthBlock(WIDTH);			// 横分割
			pObjMeshCylinder->SetRotWidth((D3DX_PI * 2) / (float)(WIDTH));	// 一分割あたりの角度
			pObjMeshCylinder->SetHeightBlock(1);			// 縦分割
			pObjMeshCylinder->SetOutWidth(fWidth + fCenterDistance);			// 横幅(外)
			pObjMeshCylinder->SetInWidth(fWidth);			// 横幅(内)
			pObjMeshCylinder->SetLife(nLife);				// 寿命
			pObjMeshCylinder->SetMaxLife(nLife);			// 最大寿命
			pObjMeshCylinder->SetMoveSize(fAddSizeValue);	// 広がる速度
			pObjMeshCylinder->SetTexDivision(4);			// テクスチャ分割数設定
			pObjMeshCylinder->SetIsAddBlend(bAddBlend);		// 加算合成するか
			pObjMeshCylinder->m_fSizeDest = fWidth + fCenterDistance + (fAddSizeValue * nLife);
			pObjMeshCylinder->SetMoveAngle(angle);			// 向き

			// テクスチャの割り当て
			pObjMeshCylinder->m_nTexIdx = CManager::GetTexture()->Regist(GetFileName(nTexType));

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
HRESULT CSlash::Init(void)
{
	HRESULT hr;

	// デバイスの取得
	LPDIRECT3DDEVICE9 pDevice = CManager::GetRenderer()->GetDevice();

	// 種類設定
	SetType(TYPE_MESHDONUTS);

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
void CSlash::Uninit(void)
{
	// 終了処理
	CImpactWave::Uninit();
}

//==========================================================================
// 更新処理
//==========================================================================
void CSlash::Update(void)
{

	// 外側の幅
	float fOutWidth = GetOutWidth();

	fOutWidth += (m_fSizeDest - fOutWidth) * 0.15f;

	// 外側の幅
	SetOutWidth(fOutWidth);

	// 更新処理
	CImpactWave::Update();

	// 外側の幅
	SetOutWidth(fOutWidth);

	CBullet **ppBullet = CGame::GetBulletManager()->GetBullet();
	int nNumAll = CGame::GetBulletManager()->GetNumAll();

	// 位置取得
	D3DXVECTOR3 pos = GetPosition();
	for (int nCntBullet = 0; nCntBullet < nNumAll; nCntBullet++)
	{
		if (ppBullet[nCntBullet] == NULL)
		{// NULLだったら
			break;
		}

		// 弾の位置
		D3DXVECTOR3 BulletPosition = ppBullet[nCntBullet]->GetPosition();
		D3DXVECTOR3 BulletMove = ppBullet[nCntBullet]->GetMove();

		// 弾との判定
		float fBulletRadius = ppBullet[nCntBullet]->GetRadius();

		if (ppBullet[nCntBullet]->GetState() == CBullet::STATE_NONE && 
			ppBullet[nCntBullet]->GetType() == CBullet::TYPE_ENEMY &&
			SphereRange(pos, BulletPosition, fOutWidth, fBulletRadius) == true)
		{// 球の判定

			float fRot = 0.0f;
			D3DXVECTOR3 SlashMove = mylib_const::DEFAULT_VECTOR3;

			// 移動量設定
			switch (GetMoveAngle())
			{
			case CObject::ANGLE_UP:
				fRot = atan2f((pos.x - 0.0f), (pos.z - 0.0f));

				ppBullet[nCntBullet]->SetMove(D3DXVECTOR3(
					sinf(D3DX_PI + fRot) * 7.5f,
					0.0f,
					cosf(D3DX_PI + fRot) * 7.5f));
				break;

			case CObject::ANGLE_DOWN:
			{
				int nAngle = 1;

				if (ppBullet[nCntBullet]->GetMoveAngle() == ANGLE_LEFT)
				{
					nAngle = -1;
				}

				fRot = ppBullet[nCntBullet]->GetRotation().y - ((D3DX_PI * 0.5f) * nAngle);

				ppBullet[nCntBullet]->SetMove(D3DXVECTOR3(
					sinf(D3DX_PI + fRot) * 5.0f,
					0.0f,
					cosf(D3DX_PI + fRot) * 5.0f));
			}
				break;

			default:

				float fSlashRot = GetOriginRotation().x;
				if (GetMoveAngle() == ANGLE_LEFT)
				{
					fSlashRot += D3DX_PI;
				}

				if (BulletMove.x <= 0.0f)
				{
					BulletMove.x *= -1;
				}
				BulletMove.x *= 2.5f;

				SlashMove = D3DXVECTOR3(BulletMove.x, sinf(D3DX_PI + fSlashRot) * 2.0f, 0.0f);
				ppBullet[nCntBullet]->SetMove(SlashMove);
				break;
			}

			ppBullet[nCntBullet]->SetType(CBullet::TYPE_PLAYER);
			ppBullet[nCntBullet]->SetState(CBullet::STATE_DMG, 3);
			ppBullet[nCntBullet]->SetMoveAngle(GetMoveAngle());

			// ヒットストップ
			CManager::SetEnableHitStop(6);

			// 振動
			CManager::GetCamera()->SetShake(12, 25.0f, 0.0f);
		}
	}
}

//==========================================================================
// 描画処理
//==========================================================================
void CSlash::Draw(void)
{
	// 描画処理
	CImpactWave::Draw();
}
