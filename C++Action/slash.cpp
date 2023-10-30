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
#include "enemymanager.h"
#include "enemy.h"
#include "effect_slashhit.h"
#include "hitscore.h"

//==========================================================================
// マクロ定義
//==========================================================================
#define TEXTURE			"data\\TEXTURE\\anim_slash.png"
#define ANIM_SPEED		(1)								// 読み込み間隔
#define MAX_PATTERN_U	(28)							// Uの分割数
#define MAX_PATTERN_V	(1)								// Vの分割数
#define MAX_PATTERN		(MAX_PATTERN_U)					// アニメーションパターンの最大数
#define MOVE_U			(1.0f / (float)MAX_PATTERN_U)	// U座標移動量
#define MOVE_V			(1.0f / (float)MAX_PATTERN_V)	// V座標移動量
#define WIDTH	(32)

//==========================================================================
// 静的メンバ変数宣言
//==========================================================================

//==========================================================================
// コンストラクタ
//==========================================================================
CSlash::CSlash(int nPriority) : CImpactWave(nPriority)
{
	m_nCntAnim = 0;			// アニメーションカウンター
	m_nPatternAnim = 0;		// アニメーションパターンNo.
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
// テクスチャ読み込み
//==========================================================================
void CSlash::LoadTexture(void)
{
	// テクスチャの読み込み
	CManager::GetInstance()->GetTexture()->Regist(TEXTURE);
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
			pObjMeshCylinder->SetTexDivision(1);			// テクスチャ分割数設定
			pObjMeshCylinder->SetIsAddBlend(bAddBlend);		// 加算合成するか
			pObjMeshCylinder->m_fSizeDest = fWidth + fCenterDistance + (fAddSizeValue * nLife);
			pObjMeshCylinder->SetMoveAngle(angle);			// 向き

			// テクスチャの割り当て
			pObjMeshCylinder->m_nTexIdx = CManager::GetInstance()->GetTexture()->Regist(TEXTURE);

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

	float sy = sqrtf(mtxCollision._11 * mtxCollision._11 + mtxCollision._21 * mtxCollision._21);
	if (sy > 1e-6) 
	{
		collisionRotation.x = atan2f(mtxCollision._32, mtxCollision._33);
		collisionRotation.y = atan2f(-mtxCollision._31, sy);
		collisionRotation.z = atan2f(mtxCollision._21, mtxCollision._11);
	}
	else 
	{
		collisionRotation.x = atan2f(-mtxCollision._23, mtxCollision._22);
		collisionRotation.y = atan2f(-mtxCollision._31, sy);
		collisionRotation.z = 0;
	}

	// ワールド行列から回転成分を計算
	/*collisionRotation.x = atan2f(mtxCollision._32, mtxCollision._33);
	collisionRotation.y = atan2f(-mtxCollision._31, sqrtf(mtxCollision._32 * mtxCollision._32 + mtxCollision._33 * mtxCollision._33));
	collisionRotation.z = atan2f(mtxCollision._21, mtxCollision._11);*/

	collisionRotation = rot;
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

	SetRotation(rotOrigin);
	SetOriginRotation(rot);

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

	// 色取得
	D3DXCOLOR col = GetColor();

	// 不透明度更新
	col.a = m_colOrigin.a * (1.0f - (float)m_nPatternAnim / (float)(MAX_PATTERN_U + 1));

	// 色設定
	SetColor(col);

	// 外側の幅
	SetOutWidth(fOutWidth);

	// 頂点更新
	CImpactWave::SetVtx();

	if (m_pObj3D != NULL)
	{
		m_pObj3D->Update();
		m_pObj3D->SetSize(D3DXVECTOR3(fOutWidth, 0.0f, fOutWidth));
	}

	// 当たり判定
	Collision();

	// テクスチャ座標更新
	UpdateTex();
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
	int nUse = 0;
	for (int nCntBullet = 0; nUse < nNumAll; nCntBullet++)
	{
		if (ppBullet[nCntBullet] == NULL)
		{// NULLだったら
			continue;
		}
		nUse++;

		// 弾の位置
		D3DXVECTOR3 BulletPosition = ppBullet[nCntBullet]->GetPosition();
		D3DXVECTOR3 BulletMove = ppBullet[nCntBullet]->GetMove();

		// 弾との判定
		float fBulletRadius = ppBullet[nCntBullet]->GetWidthLen();

		if (ppBullet[nCntBullet]->GetState() == CBullet::STATE_NONE &&
			ppBullet[nCntBullet]->GetType() == CBullet::TYPE_ENEMY &&
			IsHit(BulletPosition, fBulletRadius) == true)
		{// 球の判定

			float fRot = 0.0f;
			D3DXVECTOR3 SlashMove = mylib_const::DEFAULT_VECTOR3;

			float fFabsMove = fabsf(BulletMove.x * 2.5f);
			fFabsMove = 20.0f;

			// 移動量設定
			switch (GetMoveAngle())
			{
			case CObject::ANGLE_UP:
				fRot = D3DX_PI + GetOriginRotation().y;

				ppBullet[nCntBullet]->SetMove(D3DXVECTOR3(
					sinf(D3DX_PI + fRot) * fFabsMove,
					0.0f,
					cosf(D3DX_PI + fRot) * fFabsMove));
				break;

			case CObject::ANGLE_DOWN:
			{
				int nAngle = 1;

				if (ppBullet[nCntBullet]->GetMoveAngle() == ANGLE_LEFT)
				{
					nAngle = -1;
				}

				fRot = D3DX_PI + GetOriginRotation().y/* - ((D3DX_PI * 0.5f) * nAngle)*/;

				ppBullet[nCntBullet]->SetMove(D3DXVECTOR3(
					sinf(D3DX_PI + fRot) * fFabsMove,
					0.0f,
					cosf(D3DX_PI + fRot) * fFabsMove));
			}
			break;

			default:	// 左右

				float fRotX = GetOriginRotation().x;
				float fRotY = GetOriginRotation().x;
				if (GetMoveAngle() == ANGLE_LEFT)
				{
					fRotX += D3DX_PI;
				}

				if (ppBullet[nCntBullet]->GetMoveAngle() == ANGLE_UP ||
					ppBullet[nCntBullet]->GetMoveAngle() == ANGLE_DOWN)
				{// 弾の向きが奥か手前
					ppBullet[nCntBullet]->SetMapIndex(GetMapIndex());
					ppBullet[nCntBullet]->SetMapMoveValue(GetMapMoveValue());
				}

				SlashMove = D3DXVECTOR3(cosf(fRotX) * fFabsMove, sinf(D3DX_PI + fRotY) * fFabsMove, 0.0f);
				ppBullet[nCntBullet]->SetMove(SlashMove);
				break;
			}

			ppBullet[nCntBullet]->SetType(CBullet::TYPE_PLAYER);
			ppBullet[nCntBullet]->SetState(CBullet::STATE_DMG, 3);
			ppBullet[nCntBullet]->SetMoveAngle(GetMoveAngle());

			// ヒットストップ
			CManager::GetInstance()->SetEnableHitStop(8);

			// 振動
			CManager::GetInstance()->GetCamera()->SetShake(15, 25.0f, 0.0f);

			// 斬撃のヒットエフェクト生成
			CEffectSlashHit::Create(BulletPosition);

			// ヒットスコア加算
			CGame::GetHitScore()->Add(1);
		}
	}



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
	bool bHit = false;

	nUse = 0;
	for (int nCntEnemy = 0; nUse < nNumEnemy; nCntEnemy++)
	{
		if (ppEnemy[nCntEnemy] == NULL)
		{// NULLだったら
			continue;
		}
		nUse++;

		// 敵の情報取得
		D3DXVECTOR3 EnemyPosition = ppEnemy[nCntEnemy]->GetCenterPosition();
		D3DXVECTOR3 EnemyRotation = ppEnemy[nCntEnemy]->GetRotation();
		float fEnemyRadius = ppEnemy[nCntEnemy]->GetRadius();
		CEnemy::STATE EnemyState = (CEnemy::STATE)ppEnemy[nCntEnemy]->GetState();

		if (IsHit(EnemyPosition, fEnemyRadius) == true)
		{// 当たっていたら

			if (EnemyState != CEnemy::STATE_DMG && EnemyState != CEnemy::STATE_DEAD)
			{
				CImpactWave *pImpactWave = CImpactWave::Create
				(
					D3DXVECTOR3(EnemyPosition.x, EnemyPosition.y + 50.0f, EnemyPosition.z),	// 位置
					D3DXVECTOR3((float)(Random(-31, 31) * 0.1f), EnemyRotation.y, 0.0f),			// 向き
					D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.4f),			// 色
					60.0f,										// 幅
					0.0f,										// 高さ
					10,											// 寿命
					15.0f,										// 幅の移動量
					CImpactWave::TYPE_PURPLE4,				// テクスチャタイプ
					true										// 加算合成するか
				);
				pImpactWave->SetEnableHitstopMove();
			}

			if (ppEnemy[nCntEnemy]->GetState() != CEnemy::STATE_DMG && ppEnemy[nCntEnemy]->GetState() != CEnemy::STATE_DEAD)
			{
				// ヒットスコア加算
				CGame::GetHitScore()->Add(1);
			}

			// ヒット処理
			ppEnemy[nCntEnemy]->Hit(mylib_const::DMG_SLASH);

			bHit = true;
		}
	}

}

//==========================================================================
// 角度付きの判定
//==========================================================================
bool CSlash::IsHit(D3DXVECTOR3 TargetPos, float fTargetRadius)
{
	// 情報取得
	D3DXVECTOR3 pos = GetPosition();
	D3DXVECTOR3 rot = GetOriginRotation();
	float fOutWidth = GetOutWidth();

	if (SphereRange(pos, TargetPos, 50.0f, fTargetRadius) == true)
	{// 中心からの小さい球で当たる
		return true;
	}

	float fLength = sqrtf(fOutWidth * fOutWidth + fOutWidth * fOutWidth);	// 対角線の長さ
	float fAngle = atan2f(fOutWidth, fOutWidth);							// 対角線の向き

	if (GetMoveAngle() == ANGLE_LEFT)
	{
		rot.x += D3DX_PI;
	}
	rot.y += D3DX_PI;

	float fPosY = sinf(rot.x) * fOutWidth;

	// 判定する四角の4頂点
	bool bLine1 = false, bLine2 = false, bLine3 = false, bLine4 = false;

	D3DXVECTOR3 LeftUp = D3DXVECTOR3(
		pos.x + cosf(rot.x) * sinf(rot.y - fAngle) * fLength,
		pos.y + fPosY,
		pos.z + cosf(rot.x) * cosf(rot.y - fAngle) * fLength);

	D3DXVECTOR3 RightUp = D3DXVECTOR3(
		pos.x + cosf(rot.x) * sinf(rot.y + fAngle) * fLength,
		pos.y + fPosY,
		pos.z + cosf(rot.x) * cosf(rot.y + fAngle) * fLength);

	D3DXVECTOR3 LeftDown = D3DXVECTOR3(
		pos.x + cosf(rot.x) * sinf(rot.y - D3DX_PI + fAngle) * fLength,
		pos.y - fPosY,
		pos.z + cosf(rot.x) * cosf(rot.y - D3DX_PI + fAngle) * fLength);

	D3DXVECTOR3 RightDown = D3DXVECTOR3(
		pos.x + cosf(rot.x) * sinf(rot.y + D3DX_PI - fAngle) * fLength,
		pos.y - fPosY,
		pos.z + cosf(rot.x) * cosf(rot.y + D3DX_PI - fAngle) * fLength);

	float fWidth = sqrtf(
		((LeftUp.x - RightUp.x) * (LeftUp.x - RightUp.x)) +
		((LeftUp.z - RightUp.z) * (LeftUp.z - RightUp.z))) * 0.5f;

	float fHeight = sqrtf(
		((RightUp.x - RightDown.x) * (RightUp.x - RightDown.x)) +
		((RightUp.z - RightDown.z) * (RightUp.z - RightDown.z))) * 0.5f;

	if (fWidth == 0.0f)
	{// 0の場合少し厚みを持たせる
		fWidth = 5.0f;
	}
	if (fHeight == 0.0f)
	{// 0の場合少し厚みを持たせる
		fHeight = 5.0f;
	}

	D3DXVECTOR3 collisionRot = D3DXVECTOR3(0.0f, rot.y, 0.0f);
	bool bHitY = CollisionCircleSquare2D(TargetPos, pos, D3DXVECTOR3(0.0f, rot.y, 0.0f), fTargetRadius * 0.5f, D3DXVECTOR2(fWidth, fHeight));

	if (bHitY == false)
	{// Yですら当たってなかったら
		return false;
	}

	bool bTri1 = false, bTri2 = false;
	float fTriHeight1 = 0.0f, fTriHeight2 = 0.0f;
	fTriHeight1 = GetVtxHeight(TargetPos, LeftUp, LeftDown, RightUp, bTri1);
	fTriHeight2 = GetVtxHeight(TargetPos, RightDown, RightUp, LeftDown, bTri2);

	if (bTri1 == false && bTri2 == false)
	{// どっちの三角にもあたってなかったら
		return false;
	}

	if (std::isnan(fTriHeight1) == true ||
		std::isnan(fTriHeight2) == true)
	{// ナンインドだったら
		
		if (pos.y + fOutWidth >= TargetPos.y - fTargetRadius &&
			pos.y - fOutWidth <= TargetPos.y + fTargetRadius)
		{
			return true;
		}
	}

	if (bTri1 == true &&
		fTriHeight1 >= TargetPos.y - fTargetRadius &&
		fTriHeight1 <= TargetPos.y + fTargetRadius)
	{// 1つ目の三角にあたってたら && 半径以内に入ってたら
		return true;
	}
	
	if (bTri2 == true &&
		fTriHeight2 >= TargetPos.y - fTargetRadius &&
		fTriHeight2 <= TargetPos.y + fTargetRadius)
	{// 2つ目の三角にあたってたら && 半径以内に入ってたら
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

	//m_pObj3D->Draw();

	// ライティングを有効にする
	pDevice->SetRenderState(D3DRS_LIGHTING, TRUE);

	// カリングのデフォルト
	pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);

}

//==========================================================================
// テクスチャ座標更新
//==========================================================================
void CSlash::UpdateTex(void)
{
	D3DXVECTOR2 *pVtxTex = GetVtxTex();	// テクスチャ座標取得

	if (pVtxTex== NULL)
	{
		return;
	}

	// テクスチャ分割数
	int nTexDivision = GetTexDivision();

	// 分割数取得
	int nHeightBlock = GetHeightBlock();
	int nWidthBlock = GetWidthBlock();



	// カウントを更新
	m_nCntAnim = (m_nCntAnim + 1) % ANIM_SPEED;

	// パターン更新
	if ((m_nCntAnim % ANIM_SPEED) == 0)
	{
		// パターンNo.を更新
		m_nPatternAnim = (m_nPatternAnim + 1) % MAX_PATTERN;

		if (m_nPatternAnim == 0)
		{// パターンが一周した時

			// オブジェクト破棄
			Uninit();
			return;
		}
	}



	// 頂点情報の設定
	for (int nCntHeight = 0; nCntHeight < nHeightBlock + 1; nCntHeight++)
	{// 縦の頂点数分繰り返す

		for (int nCntWidth = 0; nCntWidth < nWidthBlock + 1; nCntWidth++)
		{// 横の頂点数分繰り返す

			float fTex = m_nPatternAnim * MOVE_U;
			float fTexDest = (m_nPatternAnim + 1) * MOVE_U;

			if (nCntHeight == 1)
			{
				fTex = (m_nPatternAnim + 1) * MOVE_U;
			}

			// スタート + 
			float f = fTex + nCntWidth * (MOVE_U / (float)nWidthBlock);

			pVtxTex[nCntWidth + (nCntHeight * (nWidthBlock + 1))] = D3DXVECTOR2
			(
				f,
				nCntHeight * (1.0f / (float)(nHeightBlock))
			);
		}
	}
}
