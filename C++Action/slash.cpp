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
#include "debugproc.h"

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
	collisionRotation = mylib_const::DEFAULT_VECTOR3;	// 当たり判定の向き
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
			pObjMeshCylinder->m_nTexIdx = CManager::GetInstance()->GetTexture()->Regist(GetFileName(nTexType));

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
	LPDIRECT3DDEVICE9 pDevice = CManager::GetInstance()->GetRenderer()->GetDevice();

	// 種類設定
	SetType(TYPE_MESHDONUTS);

	// オブジェクト3Dメッシュの初期化処理
	hr = CObject3DMesh::Init(CObject3DMesh::TYPE_DONUTS);
	if (FAILED(hr))
	{// 失敗していたら
		return E_FAIL;
	}

	// 計算用マトリックス宣言
	D3DXMATRIX mtxRot, mtxTrans, mtxRotOrigin, mtxCollision;

#if 0
	D3DXVECTOR3 rotOrigin = GetRotation();
	D3DXVECTOR3 rot = GetOriginRotation();
#else
	D3DXVECTOR3 rotOrigin = GetOriginRotation();
	D3DXVECTOR3 rot = GetRotation();
#endif
	D3DXVECTOR3 pos = GetPosition();
	RotNormalize(rotOrigin);
	RotNormalize(rot);

	// ワールドマトリックスの初期化
	D3DXMatrixIdentity(&mtxCollision);
	D3DXMatrixIdentity(&mtxRotOrigin);
#if 1
	// 元の向きを反映する
	D3DXMatrixRotationYawPitchRoll(&mtxRotOrigin, rotOrigin.y, rotOrigin.x, rotOrigin.z);
	D3DXMatrixMultiply(&mtxCollision, &mtxCollision, &mtxRotOrigin);

	// 向きを反映する
	D3DXMatrixRotationYawPitchRoll(&mtxRot, rot.y, rot.x, rot.z);
	D3DXMatrixMultiply(&mtxCollision, &mtxCollision, &mtxRot);
#else

	// 向きを反映する
	D3DXMatrixRotationYawPitchRoll(&mtxRot, rot.y, rot.x, rot.z);
	D3DXMatrixMultiply(&mtxCollision, &mtxCollision, &mtxRot);

	// 元の向きを反映する
	D3DXMatrixRotationYawPitchRoll(&mtxRotOrigin, rotOrigin.y, rotOrigin.x, rotOrigin.z);
	D3DXMatrixMultiply(&mtxCollision, &mtxCollision, &mtxRotOrigin);

#endif
	// 位置を反映する
	D3DXMatrixTranslation(&mtxTrans, pos.x, pos.y, pos.z);
	D3DXMatrixMultiply(&mtxCollision, &mtxCollision, &mtxTrans);

	// ワールド行列から回転成分を計算
	collisionRotation.x = atan2f(mtxCollision._32, mtxCollision._33);
	collisionRotation.y = atan2f(-mtxCollision._31, sqrtf(mtxCollision._32 * mtxCollision._32 + mtxCollision._33 * mtxCollision._33));
	collisionRotation.z = atan2f(mtxCollision._21, mtxCollision._11);
	RotNormalize(collisionRotation);

	// 外側の幅
	float fOutWidth = GetOutWidth();

	m_pObj3D = CObject3D::Create();
	m_pObj3D->SetSize(D3DXVECTOR3(fOutWidth, 0.0f, fOutWidth));
	m_pObj3D->SetPosition(pos);

#if 0
	m_pObj3D->SetRotation(rot);
	m_pObj3D->SetOriginRotation(rotOrigin);
#else
	m_pObj3D->SetRotation(rotOrigin);
	m_pObj3D->SetOriginRotation(rot);
#endif

	return S_OK;
}

//==========================================================================
// 終了処理
//==========================================================================
void CSlash::Uninit(void)
{
	// 終了処理
	CImpactWave::Uninit();

	m_pObj3D->Uninit();
	m_pObj3D = NULL;

}

//==========================================================================
// 更新処理
//==========================================================================
void CSlash::Update(void)
{
	if (IsDeath() == true)
	{
		return;
	}

	// 外側の幅
	float fOutWidth = GetOutWidth();

	fOutWidth += (m_fSizeDest - fOutWidth) * 0.15f;

	// 外側の幅
	SetOutWidth(fOutWidth);

	// 更新処理
	CImpactWave::Update();

	// 外側の幅
	SetOutWidth(fOutWidth);

	if (m_pObj3D != NULL)
	{
		m_pObj3D->Update();
		m_pObj3D->SetSize(D3DXVECTOR3(fOutWidth, 0.0f, fOutWidth));
	}

	// 当たり判定
	Collision();
}

//==========================================================================
// 当たり判定
//==========================================================================
void  CSlash::Collision(void)
{

	// 外側の幅
	float fOutWidth = GetOutWidth();

	// 弾情報取得
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
			IsHit(BulletPosition, fBulletRadius) == true)
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

			default:	// 左右

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

				SlashMove = D3DXVECTOR3(cosf(fSlashRot) * BulletMove.x, sinf(D3DX_PI + fSlashRot) * 5.0f, 0.0f);
				ppBullet[nCntBullet]->SetMove(SlashMove);
				break;
			}

			ppBullet[nCntBullet]->SetType(CBullet::TYPE_PLAYER);
			ppBullet[nCntBullet]->SetState(CBullet::STATE_DMG, 3);
			ppBullet[nCntBullet]->SetMoveAngle(GetMoveAngle());

			// ヒットストップ
			CManager::GetInstance()->SetEnableHitStop(6);

			// 振動
			CManager::GetInstance()->GetCamera()->SetShake(12, 25.0f, 0.0f);
		}
	}
}

//==========================================================================
// 角度付きの判定
//==========================================================================
bool CSlash::IsHit(D3DXVECTOR3 TargetPos, float fTargetRadius)
{
	// 外側の幅
	float fOutWidth = GetOutWidth();

	D3DXVECTOR3 rectCenter = GetPosition();							// 矩形の中心座標
	D3DXVECTOR3 rectSize = D3DXVECTOR3(fOutWidth, 10.0f, fOutWidth);	// 矩形のサイズ
	D3DXVECTOR3 sphereCenter = TargetPos;							// 球の中心座標
	float sphereRadius = fTargetRadius;								// 球の半径

	// 矩形の幅を一時代入
	float halfWidth = rectSize.x;
	float halfHeight = rectSize.y;
	float halfDepth = rectSize.z;

	// 球の中心を矩形に移動
	D3DXMATRIX rotationMatrix;
	D3DXMatrixRotationYawPitchRoll(&rotationMatrix, collisionRotation.y, collisionRotation.x, collisionRotation.z);
	D3DXVECTOR3 transformedSphereCenter = sphereCenter - rectCenter;
	D3DXVec3TransformCoord(&transformedSphereCenter, &transformedSphereCenter, &rotationMatrix);

	// 球の中心と矩形の内部の最も近い点を求める
	float closestX = max(-halfWidth, min(transformedSphereCenter.x, halfWidth));
	float closestY = max(-halfHeight, min(transformedSphereCenter.y, halfHeight));
	float closestZ = max(-halfDepth, min(transformedSphereCenter.z, halfDepth));

	// 上の最も近い点と球との距離計算
	float distanceSquared =
		(closestX - transformedSphereCenter.x) * (closestX - transformedSphereCenter.x) +
		(closestY - transformedSphereCenter.y) * (closestY - transformedSphereCenter.y) +
		(closestZ - transformedSphereCenter.z) * (closestZ - transformedSphereCenter.z);

	// 球の判定
	if (distanceSquared <= (sphereRadius * sphereRadius))
	{
		return true;
	}

	return false;

}

//==========================================================================
// 描画処理
//==========================================================================
void CSlash::Draw(void)
{
	// 描画処理
	CImpactWave::Draw();


	// デバイスの取得
	LPDIRECT3DDEVICE9 pDevice = CManager::GetInstance()->GetRenderer()->GetDevice();


	// 背面のカリングなし
	pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	// ライティングを無効にする
	pDevice->SetRenderState(D3DRS_LIGHTING, FALSE);

	m_pObj3D->Draw();

	// ライティングを有効にする
	pDevice->SetRenderState(D3DRS_LIGHTING, TRUE);

	// カリングのデフォルト
	pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);

}
