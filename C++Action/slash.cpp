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
// 角度付きの判定
//==========================================================================
bool CSlash::IsHit(D3DXVECTOR3 TargetPos, float fTargetRadius)
{
	// 位置取得
	D3DXVECTOR3 pos = GetPosition();

	// 外側の幅
	float fOutWidth = GetOutWidth();

#if 0

	if (SphereRange(pos, TargetPos, fOutWidth, fTargetRadius) == false)
	{// 大きい球の判定
		return false;
	}

	// 向き取得
	D3DXVECTOR3 rot = GetRotation();
	float RotX = GetOriginRotation().x;

	if (GetMoveAngle() == ANGLE_LEFT)
	{
		RotX += D3DX_PI;
	}

	float fLength = sqrtf(fOutWidth * fOutWidth + fOutWidth * fOutWidth);	// 対角線の長さ
	float fAngle = atan2f(fOutWidth, fOutWidth);									// 対角線の向き

	float fPosY = sinf(GetOriginRotation().x) * fOutWidth;

	// 判定する四角の4頂点
	bool bLine1 = false, bLine2 = false, bLine3 = false, bLine4 = false;
	
	D3DXVECTOR3 LeftUp = D3DXVECTOR3(
		pos.x + cosf(RotX) * sinf(rot.y - fAngle) * fLength,
		pos.y + fPosY,
		pos.z + cosf(RotX) * cosf(rot.y - fAngle) * fLength);

	D3DXVECTOR3 RightUp = D3DXVECTOR3(
		pos.x + cosf(RotX) * sinf(rot.y + fAngle) * fLength,
		pos.y + fPosY,
		pos.z + cosf(RotX) * cosf(rot.y + fAngle) * fLength);

	D3DXVECTOR3 LeftDown = D3DXVECTOR3(
		pos.x + cosf(RotX) * sinf(rot.y - D3DX_PI + fAngle) * fLength,
		pos.y - fPosY,
		pos.z + cosf(RotX) * cosf(rot.y - D3DX_PI + fAngle) * fLength);

	D3DXVECTOR3 RightDown = D3DXVECTOR3(
		pos.x + cosf(RotX) * sinf(rot.y + D3DX_PI - fAngle) * fLength,
		pos.y - fPosY,
		pos.z + cosf(RotX) * cosf(rot.y + D3DX_PI - fAngle) * fLength);

	if (CollisionCircleSquare2D(TargetPos, D3DXVECTOR3(pos.x, TargetPos.y, pos.z), D3DXVECTOR3(RotX, rot.y, rot.z), fTargetRadius, D3DXVECTOR2(fOutWidth, fOutWidth)))
	{
		return true;
	}


#else

	// 向き取得
	D3DXVECTOR3 rot = GetRotation();

	{

		D3DXVECTOR3 rectCenter = pos; // 回転した矩形の中心座標
		D3DXVECTOR3 rectRotation = D3DXVECTOR3(D3DX_PI - GetOriginRotation().x, rot.y, rot.z);
		D3DXVECTOR3 rectSize(fOutWidth, 1.0f, fOutWidth); // 回転した矩形の幅、高さ、奥行き
		D3DXMATRIX rectWorldMatrix = GetWorldMtx(); // 回転した矩形のワールド変換行列
		D3DXVECTOR3 sphereCenter = TargetPos; // 球の中心座標
		float sphereRadius = fTargetRadius; // 球の半径

		// 回転した矩形の半幅を計算
		float halfWidth = rectSize.x;
		float halfHeight = rectSize.y;
		float halfDepth = rectSize.z;

		// 球の中心を回転した座標系に変換
		D3DXMATRIX rotationMatrix;
		D3DXMatrixRotationYawPitchRoll(&rotationMatrix, rectRotation.y, rectRotation.x, rectRotation.z);
		D3DXVECTOR3 transformedSphereCenter = sphereCenter - rectCenter;
		D3DXVec3TransformCoord(&transformedSphereCenter, &transformedSphereCenter, &rotationMatrix);

		// 移動する球の新しい位置を計算
		D3DXVECTOR3 newSphereCenter = sphereCenter;

		// 球の中心と矩形の内部の最も近い点を求める
		float closestX = max(-halfWidth, min(transformedSphereCenter.x, halfWidth));
		float closestY = max(-halfHeight, min(transformedSphereCenter.y, halfHeight));
		float closestZ = max(-halfDepth, min(transformedSphereCenter.z, halfDepth));

		// 最も近い点と球の中心との距離を計算
		float distanceSquared = 
			(closestX - transformedSphereCenter.x) * (closestX - transformedSphereCenter.x) +
			(closestY - transformedSphereCenter.y) * (closestY - transformedSphereCenter.y) +
			(closestZ - transformedSphereCenter.z) * (closestZ - transformedSphereCenter.z);

		float sphereRadiusWithMargin = sphereRadius;  // 球の半径に追加のマージンを考慮する場合、適切な値に変更

		// 球が新しい位置で矩形と交差している場合、当たり判定
		if (distanceSquared <= (sphereRadiusWithMargin * sphereRadiusWithMargin)) {
			return true;
		}

		return false;
	}

#endif

	

	return false;
}

//==========================================================================
// 描画処理
//==========================================================================
void CSlash::Draw(void)
{
	// 描画処理
	CImpactWave::Draw();
}
