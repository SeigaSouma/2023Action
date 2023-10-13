//=============================================================================
// 
//  起伏処理 [elevation.cpp]
//  Author : 相馬靜雅
// 
//=============================================================================
#include "elevation.h"
#include "manager.h"
#include "renderer.h"
#include "game.h"
#include "texture.h"
#include "input.h"
#include "calculation.h"
#include "debugproc.h"
#include "particle.h"
#include "3D_effect.h"
#include "player.h"
#include "model.h"
#include "ballast.h"
#include "enemy.h"
#include "impactwave.h"
#include "camera.h"
#include "enemymanager.h"
#include "sound.h"
#include "tutorial.h"
#include "tutorialstep.h"
#include "ranking.h"
#include "targetpoint.h"

//==========================================================================
// マクロ定義
//==========================================================================
#define TEXTURE		"data\\TEXTURE\\glass.jpg"	// テクスチャ
#define MAX_MOVE	(8.0f)		// 移動量
#define MAX_RANGE	(200.0f)	// 範囲
#define MOVE_LEN	(1.0f)		// 幅移動量
#define MIN_HEIGHT	(-400.0f)	// 最低の高さ
#define MOVE		(5.0f)
#define IMPACTSHAKE_DIRECT	(30.0f)
#define IMPACTSHAKE_IMPACT	(20.0f)
#define SHAKETIME_DIRECT	(15 + 5)
#define SHAKETIME_IMPACT	(10 + 5)
#define SPAWN_WAITTIME		(20)
#if 1
#define CHANGE	(0)
#endif

//==========================================================================
// 静的メンバ変数宣言
//==========================================================================
bool CElevation::m_bLoadInfo = false;		// 情報読み込み判定

//==========================================================================
// コンストラクタ
//==========================================================================
CElevation::CElevation(int nPriority) : CObject3DMesh(nPriority)
{
	m_fBrushStrength = 0.0f;					// ブラシ強さ
	m_fBrushRange = 0.0f;						// 範囲
	m_bEdit = false;							// エディットON/OFF
	m_nTexIdx = 0;								// テクスチャのインデックス番号
	memset(&m_VtxPos, 0, sizeof(m_VtxPos));		// 頂点座標

	m_aInfo.TextureFileName = {};				// テクスチャファイル名
	m_aInfo.pos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);		// 位置
	m_aInfo.fWidthLength = 0.0f;				// 横長さ
	m_aInfo.fHeightLength = 0.0f;				// 縦長さ
	m_aInfo.nWidthBlock = 0;					// 横分割数
	m_aInfo.nHeightBlock = 0;					// 縦分割数
	m_aInfo.pVtxPos = NULL;						// 頂点座標
	m_pTargetP = NULL;							// 目標の地点
}

//==========================================================================
// デストラクタ
//==========================================================================
CElevation::~CElevation()
{

}

//==========================================================================
// 生成処理
//==========================================================================
CElevation *CElevation::Create(const char *pText)
{
	// 生成用のオブジェクト
	CElevation *pObjMeshField = NULL;

	if (pObjMeshField == NULL)
	{// NULLだったら

		// メモリの確保
		pObjMeshField = DEBUG_NEW CElevation;

		if (pObjMeshField != NULL)
		{// メモリの確保が出来ていたら

			// 初期化処理
			pObjMeshField->Init(pText);
		}

		return pObjMeshField;
	}

	return NULL;
}

//==========================================================================
// 初期化処理
//==========================================================================
HRESULT CElevation::Init(void)
{
	return E_FAIL;
}

//==========================================================================
// 初期化処理
//==========================================================================
HRESULT CElevation::Init(const char *pText)
{
	HRESULT hr;

	// デバイスの取得
	LPDIRECT3DDEVICE9 pDevice = CManager::GetRenderer()->GetDevice();

	// テクスチャの割り当て
	m_nTexIdx = CManager::GetTexture()->Regist(TEXTURE);

	// テクスチャの割り当て
	BindTexture(m_nTexIdx);

	// 種類設定
	SetType(TYPE_ELEVATION);

	// ロード処理
	hr = Load(pText);

	if (FAILED(hr))
	{// 失敗していたら
		return E_FAIL;
	}

	// 各種変数初期化
	SetPosition(m_aInfo.pos);				// 位置
	SetWidthBlock(m_aInfo.nWidthBlock);		// 幅部活
	SetHeightBlock(m_aInfo.nHeightBlock);	// 縦分割
	SetWidthLen(m_aInfo.fWidthLength);		// 縦長さ
	SetHeightLen(m_aInfo.fHeightLength);	// 横長さ
	m_fBrushStrength = MAX_MOVE;			// ブラシ強さ
	m_fBrushRange = MAX_RANGE;				// 範囲

	//for (int nCntHeight = 0; nCntHeight < m_aInfo.nHeightBlock + 1; nCntHeight++)
	//{// 縦の分割分繰り返す

	//	for (int nCntWidth = 0; nCntWidth < m_aInfo.nWidthBlock + 1; nCntWidth++)
	//	{// 横の分割分繰り返す
	//		m_VtxPos[nCntWidth + (nCntHeight * (m_aInfo.nWidthBlock + 1))].y = m_aInfo.pVtxPos[nCntWidth + (nCntHeight * (m_aInfo.nWidthBlock + 1))].y;
	//	}
	//}

	// オブジェクト3Dメッシュの初期化処理
	hr = CObject3DMesh::Init(CObject3DMesh::TYPE_FIELD);

	if (FAILED(hr))
	{// 失敗していたら
		return E_FAIL;
	}

	return S_OK;
}

//==========================================================================
// 終了処理
//==========================================================================
void CElevation::Uninit(void)
{
	// 頂点座標の破棄
	if (m_aInfo.pVtxPos != NULL)
	{
		delete[] m_aInfo.pVtxPos;
		m_aInfo.pVtxPos = NULL;
	}

	// 目標の地点消す
	if (m_pTargetP != NULL)
	{
		m_pTargetP->Uninit();
		m_pTargetP = NULL;
	}

	// 終了処理
	CObject3DMesh::Uninit();
}

//==========================================================================
// 解放処理
//==========================================================================
void CElevation::Release(void)
{

}

//==========================================================================
// 更新処理
//==========================================================================
void CElevation::Update(void)
{
	// キーボード情報取得
	CInputKeyboard *pInputKeyboard = CManager::GetInputKeyboard();

	// コンボ演出中は抜ける
	if (CGame::GetEnemyManager()->GetState() == CEnemyManager::STATE_COMBOANIM)
	{
		return;
	}

	if (pInputKeyboard->GetTrigger(DIK_F3) == true)
	{// 切り替え
		m_bEdit = m_bEdit ? false : true;

		// 目標の地点消す
		if (m_pTargetP != NULL)
		{
			m_pTargetP->Uninit();
			m_pTargetP = NULL;
		}
		// 目標の地点生成
		else if (m_pTargetP == NULL)
		{
			m_pTargetP = CTargetPoint::Create(D3DXVECTOR3(0.0f, 0.0f, 0.0f), 400.0f, 800.0f);
		}
	}

	// 頂点操作
	if (m_bEdit == true)
	{// エディット中なら
		UPVtxField(m_pTargetP->GetPosition());
	}
	else
	{
		// 状態更新処理
		UpdateState();
	}

	// 頂点情報設定
	SetVtx();
}

//==========================================================================
// 状態更新処理
//==========================================================================
void CElevation::UpdateState(void)
{

}

//==========================================================================
// 描画処理
//==========================================================================
void CElevation::Draw(void)
{
	//  デバイスの取得
	LPDIRECT3DDEVICE9 pDevice = CManager::GetRenderer()->GetDevice();

	if (CManager::IsWireframe() == true)
	{
		pDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);	// ワイヤーフレームモード
	}

	// 描画処理
	CObject3DMesh::Draw();

	pDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);	// 埋めるモード
}

//==========================================================================
// 高さ取得
//==========================================================================
float CElevation::GetHeight(D3DXVECTOR3& pos, bool &bLand)
{
	// ベクトルと法線
	D3DXVECTOR3 vec1, vec2, nor;
	float fHeight = 0.0f;
	D3DXVECTOR3 *pVtxPos = GetVtxPos();

	// フィールドの位置
	D3DXVECTOR3 posfield = GetPosition();

	// 長さ取得
	float fWidthLen = GetWidthLen();
	float fHeightLen = GetHeightLen();

	// 最大の長さ
	float fMaxWidthLen = fWidthLen * GetWidthBlock();
	float fMaxHeightLen = -fWidthLen * GetHeightBlock();

	// 判定する頂点の計算
	int nCntWidth = 0;
	int nCntHeight = 0;
	int nWidthPoint = ((pos.x - posfield.x) - fMaxWidthLen * 0.5f) / fWidthLen + GetWidthBlock();
	int nHeightPoint = GetHeightBlock() - ((pos.z - posfield.z) - fMaxHeightLen * 0.5f) / fHeightLen;

	for (int nCntH = 0; nCntH < 2; nCntH++)
	{
		for (int nCntW = 0; nCntW < 2; nCntW++)
		{
			// 横頂点
			nCntWidth = nWidthPoint + (nCntW - 1);
			nCntHeight = nHeightPoint + (nCntH - 1);

			if (nCntWidth < 0 || m_aInfo.nWidthBlock < nCntWidth)
			{// 範囲外で先頭
				continue;
			}

			if (nCntHeight < 0 || m_aInfo.nHeightBlock < nCntHeight)
			{// 範囲外で先頭
				continue;
			}

			// 今回の頂点
			int nNowPoint = (nCntWidth + 1) + (nCntHeight * (m_aInfo.nWidthBlock + 1));
			int nVerTexW = (m_aInfo.nWidthBlock + 1) + 1;

			int nLeft = nCntWidth + (nCntHeight * (m_aInfo.nWidthBlock + 1));
			int nRight = nCntWidth + (nCntHeight * (m_aInfo.nWidthBlock + 1)) + nVerTexW;

			if (nLeft >= GetNumVertex() || nRight >= GetNumVertex())
			{
				continue;
			}

			if (CollisionTriangle(pVtxPos[nNowPoint] + posfield, pVtxPos[nLeft] + posfield, pVtxPos[nRight] + posfield, pos, pos) == true)
			{// 三角に入っていたら

				// ベクトルを計算
				vec1 = pVtxPos[nRight] - pVtxPos[nNowPoint];
				vec2 = pVtxPos[nLeft] - pVtxPos[nNowPoint];

				// 外積を求める
				D3DXVec3Cross(&nor, &vec1, &vec2);

				// 外積の正規化をして法線にする
				D3DXVec3Normalize(&nor, &nor);

				if (nor.y != 0.0f)
				{// 法線が0.0fじゃなかったら

					// 高さを求める
					fHeight = ((pos.x - pVtxPos[nNowPoint].x - posfield.x) * nor.x + (pos.z - pVtxPos[nNowPoint].z - posfield.z) * nor.z + (pVtxPos[nNowPoint].y * -nor.y)) / -nor.y;
					fHeight += GetPosition().y;
					bLand = true;
				}
				break;
			}

			if (CollisionTriangle(pVtxPos[nNowPoint + GetWidthBlock()] + posfield, pVtxPos[nRight] + posfield, pVtxPos[nLeft] + posfield, pos, pos) == true)
			{// 三角に入っていたら

				// ベクトルを計算
				vec1 = pVtxPos[nLeft] - pVtxPos[nNowPoint + m_aInfo.nWidthBlock];
				vec2 = pVtxPos[nRight] - pVtxPos[nNowPoint + m_aInfo.nWidthBlock];

				// 外積を求める
				D3DXVec3Cross(&nor, &vec1, &vec2);

				// 外積の正規化をして法線にする
				D3DXVec3Normalize(&nor, &nor);

				if (nor.y != 0.0f)
				{// 法線が0.0fじゃなかったら

					// 高さを求める
					fHeight = ((pos.x - pVtxPos[nNowPoint + GetWidthBlock()].x - posfield.x) * nor.x + (pos.z - pVtxPos[nNowPoint + GetWidthBlock()].z - posfield.z) * nor.z + (pVtxPos[nNowPoint + GetWidthBlock()].y * -nor.y)) / -nor.y;
					fHeight += GetPosition().y;
					bLand = true;
					break;
				}
			}

		}
	}

	if (bLand == true)
	{// 着地していたら

		return fHeight;
	}

	// 高さを取得
	return pos.y;
}

//==========================================================================
// 地面に当たったか
//==========================================================================
bool CElevation::IsHit(D3DXVECTOR3& pos)
{
	bool bHit = false;

	// ベクトルと法線
	D3DXVECTOR3 vec1 = D3DXVECTOR3(0.0f, 0.0f, 0.0f), vec2 = D3DXVECTOR3(0.0f, 0.0f, 0.0f), nor = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	float fHeight = pos.y;
	D3DXVECTOR3 *pVtxPos = GetVtxPos();
	D3DXVECTOR3 posfield = GetPosition();

	// 長さ取得
	float fWidthLen = GetWidthLen();
	float fHeightLen = GetHeightLen();

	// 最大の長さ
	float fMaxWidthLen = fWidthLen * m_aInfo.nWidthBlock + posfield.x;
	float fMaxHeightLen = -fWidthLen * m_aInfo.nHeightBlock + posfield.z;

	// 頂点決め打ち
	int nCntWidth = 0;
	int nCntHeight = 0;
	int nWidthPoint = (int)((pos.x - fMaxWidthLen * 0.5f) / fWidthLen) + m_aInfo.nWidthBlock;
	int nHeightPoint = m_aInfo.nHeightBlock - (int)((pos.z - fMaxHeightLen * 0.5f) / fHeightLen);

	for (int nCntH = 0; nCntH < 2; nCntH++)
	{
		for (int nCntW = 0; nCntW < 2; nCntW++)
		{
			// 横頂点
			nCntWidth = nWidthPoint + (nCntW - 1);
			nCntHeight = nHeightPoint + (nCntH - 1);

			if (nCntWidth < 0 || m_aInfo.nWidthBlock < nCntWidth)
			{// 範囲外で先頭
				continue;
			}

			if (nCntHeight < 0 || m_aInfo.nHeightBlock < nCntHeight)
			{// 範囲外で先頭
				continue;
			}

			// 今回の頂点
			int nNowPoint = (nCntWidth + 1) + (nCntHeight * (m_aInfo.nWidthBlock + 1));
			int nVerTexW = (m_aInfo.nWidthBlock + 1) + 1;

			int nLeft = nCntWidth + (nCntHeight * (m_aInfo.nWidthBlock + 1));
			int nRight = nCntWidth + (nCntHeight * (m_aInfo.nWidthBlock + 1)) + nVerTexW;

			if (nLeft >= GetNumVertex() || nRight >= GetNumVertex())
			{
				continue;
			}

			if (CollisionTriangle(pVtxPos[nNowPoint], pVtxPos[nLeft], pVtxPos[nRight], pos, pos) == true)
			{// 三角に入っていたら

				// ベクトルを計算
				vec1 = pVtxPos[nRight] - pVtxPos[nNowPoint];
				vec2 = pVtxPos[nLeft] - pVtxPos[nNowPoint];

				// 外積を求める
				D3DXVec3Cross(&nor, &vec1, &vec2);

				// 外積の正規化をして法線にする
				D3DXVec3Normalize(&nor, &nor);

				if (nor.y != 0.0f)
				{// 法線が0.0fじゃなかったら

					// 高さを求める
					fHeight = ((pos.x - pVtxPos[nNowPoint].x) * nor.x + (pos.z - pVtxPos[nNowPoint].z) * nor.z + (pVtxPos[nNowPoint].y * -nor.y)) / -nor.y;
					fHeight += GetPosition().y;
					break;
				}
			}

			if (CollisionTriangle(pVtxPos[nNowPoint + m_aInfo.nWidthBlock], pVtxPos[nRight], pVtxPos[nLeft], pos, pos) == true)
			{// 三角に入っていたら

				// ベクトルを計算
				vec1 = pVtxPos[nLeft] - pVtxPos[nNowPoint + m_aInfo.nWidthBlock];
				vec2 = pVtxPos[nRight] - pVtxPos[nNowPoint + m_aInfo.nWidthBlock];

				// 外積を求める
				D3DXVec3Cross(&nor, &vec1, &vec2);

				// 外積の正規化をして法線にする
				D3DXVec3Normalize(&nor, &nor);

				if (nor.y != 0.0f)
				{// 法線が0.0fじゃなかったら

					// 高さを求める
					fHeight = ((pos.x - pVtxPos[nNowPoint + m_aInfo.nWidthBlock].x) * nor.x + (pos.z - pVtxPos[nNowPoint + m_aInfo.nWidthBlock].z) * nor.z + (pVtxPos[nNowPoint + m_aInfo.nWidthBlock].y * -nor.y)) / -nor.y;
					fHeight += GetPosition().y;
					break;
				}
			}
		}
	}

	if (fHeight > pos.y)
	{// 前回の位置よりも下だったら(当たっていたら)
		bHit = true;
		pos.y = fHeight;
	}

	// 判定を返す
	return bHit;
}

//==========================================================================
// 頂点上げ下げ
//==========================================================================
void CElevation::UPVtxField(D3DXVECTOR3 pos)
{

	float m_fWidthLen = GetWidthLen();
	float m_fHeightLen = GetHeightLen();
	D3DXVECTOR3 *pVtxPos = GetVtxPos();

	// デバッグ表示
	CManager::GetDebugProc()->Print(
		"------------------[ 起伏エディット情報 ]------------------\n"
		"ブラシのサイズ：[1, 2] 【%f】\n"
		"ブラシの強さ：  [3, 4] 【%f】\n"
		"頂点上げ下げ：  [5, 6]\n"
		"幅拡縮：        [7, 8] 【%f, %f】\n"
		"まっ平：        [ ９ ]\n"
		"頂点リセット：  [delete]\n"
		"起伏情報保存：  [F9]\n\n", m_fBrushRange, m_fBrushStrength, m_fWidthLen, m_fHeightLen);

	// キーボード情報取得
	CInputKeyboard *pInputKeyboard = CManager::GetInputKeyboard();

	if (pInputKeyboard->GetTrigger(DIK_F9) == true)
	{// セーブ
		Save();
	}

	// ブラシのサイズ
	if (pInputKeyboard->GetPress(DIK_1))
	{// 1で下げる
		m_fBrushRange -= 1.0f;
	}

	if (pInputKeyboard->GetPress(DIK_2))
	{// 2で上げる
		m_fBrushRange += 1.0f;
	}

	// ブラシの強さ
	if (pInputKeyboard->GetPress(DIK_3))
	{// 3で下げる
		m_fBrushStrength -= 0.01f;
	}

	if (pInputKeyboard->GetPress(DIK_4))
	{// 4で上げる
		m_fBrushStrength += 0.01f;
	}

	// フィールドの幅
	if (pInputKeyboard->GetPress(DIK_7))
	{// 7で上げる
		m_fWidthLen -= MOVE_LEN;
		m_fHeightLen -= MOVE_LEN;
	}

	if (pInputKeyboard->GetPress(DIK_8))
	{// 8で下げる
		m_fWidthLen += MOVE_LEN;
		m_fHeightLen += MOVE_LEN;
	}

	// 値の正規化
	ValueNormalize(m_fWidthLen, 99999999.0f, 10.0f);
	ValueNormalize(m_fHeightLen, 99999999.0f, 10.0f);

	// 値の正規化
	ValueNormalize(m_fBrushStrength, 99999999.0f, 0.1f);
	ValueNormalize(m_fBrushRange, 99999999.0f, 1.0f);

	// 頂点情報の設定
	for (int nCntHeight = 0; nCntHeight < m_aInfo.nHeightBlock + 1; nCntHeight++)
	{//縦の頂点数分繰り返す

		for (int nCntWidth = 0; nCntWidth < m_aInfo.nWidthBlock + 1; nCntWidth++)
		{// 横の頂点数分繰り返す

			if (pInputKeyboard->GetTrigger(DIK_DELETE))
			{// deleteでリセット
				m_aInfo.pVtxPos[nCntWidth + (nCntHeight * (m_aInfo.nWidthBlock + 1))].y = 0.0f;
			}

			float fNowLength =
				sqrtf((pos.x - pVtxPos[nCntWidth + (nCntHeight * (m_aInfo.nWidthBlock + 1))].x) * (pos.x - pVtxPos[nCntWidth + (nCntHeight * (m_aInfo.nWidthBlock + 1))].x)
					+ (pos.z - pVtxPos[nCntWidth + (nCntHeight * (m_aInfo.nWidthBlock + 1))].z) * (pos.z - pVtxPos[nCntWidth + (nCntHeight * (m_aInfo.nWidthBlock + 1))].z));

			float Wariai = fNowLength / m_fBrushRange;

			if (Wariai >= 1.0f)
			{// 割合が1.0f以下だけ
				continue;
			}

			D3DXVECTOR3 move = D3DXVECTOR3(0.0f, 0.0f, 0.0f);

			if (pInputKeyboard->GetPress(DIK_5))
			{// 5で下げる
				move.y = -m_fBrushStrength - (-m_fBrushStrength * Wariai);
			}

			if (pInputKeyboard->GetPress(DIK_6))
			{// 6で上げる
				move.y = m_fBrushStrength - (m_fBrushStrength * Wariai);
			}

			if (pInputKeyboard->GetTrigger(DIK_9))
			{// 9でまっ平
				m_aInfo.pVtxPos[nCntWidth + (nCntHeight * (m_aInfo.nWidthBlock + 1))].y = pos.y;
				move.y = 0.0f;
			}

			m_aInfo.pVtxPos[nCntWidth + (nCntHeight * (m_aInfo.nWidthBlock + 1))].y += move.y;
			// 各頂点座標確認
			//pVtxPos[nCntWidth + (nCntHeight * (m_aInfo.nWidthBlock + 1))].y += move.y;
		}
	}

	// 頂点設定
	SetVtxPos(pVtxPos);
	SetWidthLen(m_fWidthLen);
	SetHeightLen(m_fHeightLen);

	if (m_pTargetP != NULL)
	{
		// 長さ変更
		m_pTargetP->SetWidthLen(m_fBrushRange);

		// カメラの情報取得
		CCamera *pCamera = CManager::GetCamera();

		// カメラの向き取得
		D3DXVECTOR3 Camerarot = pCamera->GetRotation();

		if (pInputKeyboard->GetPress(DIK_LEFT) == true)
		{// ←キーが押された,左移動

			if (pInputKeyboard->GetPress(DIK_UP) == true)
			{// A+W,左上移動

				pos.x += sinf(-D3DX_PI * MOVE_LRDW + Camerarot.y) * MOVE;
				pos.z += cosf(-D3DX_PI * MOVE_LRDW + Camerarot.y) * MOVE;
			}
			else if (pInputKeyboard->GetPress(DIK_DOWN) == true)
			{// A+S,左下移動

				pos.x += sinf(-D3DX_PI * MOVE_LRUP + Camerarot.y) * MOVE;
				pos.z += cosf(-D3DX_PI * MOVE_LRUP + Camerarot.y) * MOVE;
			}
			else
			{// A,左移動

				pos.x += sinf(-D3DX_PI * MOVE_LR + Camerarot.y) * MOVE;
				pos.z += cosf(-D3DX_PI * MOVE_LR + Camerarot.y) * MOVE;
			}
		}
		else if (pInputKeyboard->GetPress(DIK_RIGHT) == true)
		{// Dキーが押された,右移動

			if (pInputKeyboard->GetPress(DIK_UP) == true)
			{// D+W,右上移動

				pos.x += sinf(D3DX_PI * MOVE_LRDW + Camerarot.y) * MOVE;
				pos.z += cosf(D3DX_PI * MOVE_LRDW + Camerarot.y) * MOVE;
			}
			else if (pInputKeyboard->GetPress(DIK_DOWN) == true)
			{// D+S,右下移動

				pos.x += sinf(D3DX_PI * MOVE_LRUP + Camerarot.y) * MOVE;
				pos.z += cosf(D3DX_PI * MOVE_LRUP + Camerarot.y) * MOVE;
			}
			else
			{// D,右移動

				pos.x += sinf(D3DX_PI * MOVE_LR + Camerarot.y) * MOVE;
				pos.z += cosf(D3DX_PI * MOVE_LR + Camerarot.y) * MOVE;
			}
		}
		else if (pInputKeyboard->GetPress(DIK_UP) == true)
		{// Wが押された、奥移動

			pos.x += sinf(Camerarot.y) * MOVE;
			pos.z += cosf(Camerarot.y) * MOVE;
		}
		else if (pInputKeyboard->GetPress(DIK_DOWN) == true)
		{// Sが押された、手前移動

			pos.x += sinf(D3DX_PI + Camerarot.y) * MOVE;
			pos.z += cosf(D3DX_PI + Camerarot.y) * MOVE;
		}

		// 位置設定
		m_pTargetP->SetPosition(pos);
	}

}

//==========================================================================
// 頂点情報設定処理
//==========================================================================
void CElevation::SetVtx(void)
{

	D3DXVECTOR3 *pVtxPos = GetVtxPos();
	D3DXVECTOR3 *pVtxNor = GetVtxNor();
	D3DXVECTOR3 vec1, vec2, nor;
	D3DXVECTOR3 VtxRight, VtxLeft, VtxNow;
	float fWidthLen = GetWidthLen();
	float fHeightLen = GetHeightLen();

	for (int nCntHeight = 0; nCntHeight < m_aInfo.nHeightBlock + 1; nCntHeight++)
	{// 縦の分割分繰り返す

		for (int nCntWidth = 0; nCntWidth < m_aInfo.nWidthBlock + 1; nCntWidth++)
		{// 横の分割分繰り返す

			m_VtxPos[nCntWidth + (nCntHeight * (m_aInfo.nWidthBlock + 1))].x = pVtxPos[nCntWidth + (nCntHeight * (m_aInfo.nWidthBlock + 1))].x;
			m_VtxPos[nCntWidth + (nCntHeight * (m_aInfo.nWidthBlock + 1))].z = pVtxPos[nCntWidth + (nCntHeight * (m_aInfo.nWidthBlock + 1))].z;

			// 合計値を代入する
			pVtxPos[nCntWidth + (nCntHeight * (m_aInfo.nWidthBlock + 1))] = m_VtxPos[nCntWidth + (nCntHeight * (m_aInfo.nWidthBlock + 1))];
			pVtxPos[nCntWidth + (nCntHeight * (m_aInfo.nWidthBlock + 1))].y += m_aInfo.pVtxPos[nCntWidth + (nCntHeight * (m_aInfo.nWidthBlock + 1))].y;

			// 今回の頂点
			int nNowPoint = (nCntWidth + 1) + (nCntHeight * (m_aInfo.nWidthBlock + 1));
			int nVerTexW = (m_aInfo.nWidthBlock + 1) + 1;

			int nLeft = nCntWidth + (nCntHeight * (m_aInfo.nWidthBlock + 1));
			int nRight = nCntWidth + (nCntHeight * (m_aInfo.nWidthBlock + 1)) + nVerTexW;

			if (nNowPoint >= (m_aInfo.nWidthBlock + 1) * (m_aInfo.nHeightBlock + 1))
			{
				continue;
			}

			if (nRight >= (m_aInfo.nWidthBlock + 1) * (m_aInfo.nHeightBlock + 1))
			{// 頂点数超えたら

				// 頂点座標の設定
				VtxRight = D3DXVECTOR3(
					(fWidthLen * nCntWidth) - ((fWidthLen * m_aInfo.nWidthBlock) * 0.5f),
					0.0f,
					-((fHeightLen * nCntHeight) - ((fHeightLen * m_aInfo.nHeightBlock) * 0.5f)));
			}
			else
			{
				VtxRight = pVtxPos[nRight];
			}

			if (nLeft >= (m_aInfo.nWidthBlock + 1) * (m_aInfo.nHeightBlock + 1))
			{// 頂点数超えたら

				// 頂点座標の設定
				VtxLeft = D3DXVECTOR3(
					(fWidthLen * nCntWidth) - ((fWidthLen * m_aInfo.nWidthBlock) * 0.5f),
					0.0f,
					-((fHeightLen * nCntHeight) - ((fHeightLen * m_aInfo.nHeightBlock) * 0.5f)));
			}
			else
			{
				VtxLeft = pVtxPos[nLeft];
			}

			if (nNowPoint >= (m_aInfo.nWidthBlock + 1) * (m_aInfo.nHeightBlock + 1))
			{// 頂点数超えたら

				// 頂点座標の設定
				VtxNow = D3DXVECTOR3(
					(fWidthLen * nCntWidth) - ((fWidthLen * m_aInfo.nWidthBlock) * 0.5f),
					0.0f,
					-((fHeightLen * nCntHeight) - ((fHeightLen * m_aInfo.nHeightBlock) * 0.5f)));
			}
			else
			{
				VtxNow = pVtxPos[nNowPoint];
			}

			// ベクトルを計算
			vec1 = VtxRight - VtxNow;
			vec2 = VtxLeft - VtxNow;

			// 外積を求める
			D3DXVec3Cross(&nor, &vec1, &vec2);

			// 外積の正規化をして法線にする
			D3DXVec3Normalize(&nor, &nor);

			// 法線
			pVtxNor[nNowPoint] = nor;
		}
	}

	// 頂点情報更新
	CObject3DMesh::SetVtx();
}

//==========================================================================
// ロード処理
//==========================================================================
HRESULT CElevation::Load(const char *pText)
{
	char aComment[mylib_const::MAX_STRING] = {};	//コメント用

	FILE *pFile = NULL;	// ファイルポインタを宣言

	// ファイルを開く
	pFile = fopen("data\\TEXT\\elevation_field.txt", "r");

	if (pFile == NULL)
	{// ファイルが開けなかった場合
		fclose(pFile);
		return E_FAIL;
	}

	int nCntStage = 0;	// ステージのカウント

	while (1)
	{// END_SCRIPTが来るまで繰り返す

		// 文字列の読み込み
		fscanf(pFile, "%s", &aComment[0]);

		if (strcmp(&aComment[0], "TEXTURE_FILENAME") == 0)
		{// TEXTYPEが来たら種類読み込み

			fscanf(pFile, "%s", &aComment[0]);	// =の分
			fscanf(pFile, "%s", &aComment[0]);	// テクスチャファイル名

			// ファイル名保存
			m_aInfo.TextureFileName = aComment;
		}

		// メッシュフィールドの設定
		if (strcmp(&aComment[0], "FIELDSET") == 0)
		{// モデルの読み込みを開始

			while (strcmp(&aComment[0], "END_FIELDSET"))
			{// END_FIELDSETが来るまで繰り返し

				fscanf(pFile, "%s", &aComment[0]);	// 確認する

				if (strcmp(&aComment[0], "POS") == 0)
				{// POSが来たら位置読み込み

					fscanf(pFile, "%s", &aComment[0]);	// =の分
					fscanf(pFile, "%f", &m_aInfo.pos.x);		// X座標
					fscanf(pFile, "%f", &m_aInfo.pos.y);		// Y座標
					fscanf(pFile, "%f", &m_aInfo.pos.z);		// Z座標
				}

				if (strcmp(&aComment[0], "BLOCK") == 0)
				{// BLOCKが来たら向き読み込み

					fscanf(pFile, "%s", &aComment[0]);			// =の分
					fscanf(pFile, "%d", &m_aInfo.nWidthBlock);	// 横の分割数
					fscanf(pFile, "%d", &m_aInfo.nHeightBlock);	// 縦の分割数
				}

				if (strcmp(&aComment[0], "SIZE") == 0)
				{// SIZEが来たら向き読み込み

					fscanf(pFile, "%s", &aComment[0]);		// =の分
					fscanf(pFile, "%f", &m_aInfo.fWidthLength);	// 横の長さ
					fscanf(pFile, "%f", &m_aInfo.fHeightLength);	// 縦の長さ
				}

				if (strcmp(&aComment[0], "VTXPOS") == 0)
				{// VTXPOSが来たら頂点読み込み

					// 頂点数
					int nVtxNum = (m_aInfo.nHeightBlock + 1) * (m_aInfo.nWidthBlock + 1);
					int nCntVtx = 0;

					// 頂点数でメモリ確保
					if (nCntStage == 0)
					{
						m_aInfo.pVtxPos = DEBUG_NEW D3DXVECTOR3[nVtxNum];
					}

					// 頂点読み込み用
					std::string VtxPos;

					while (1)
					{// END_VTXPOSが来るまで繰り返し

						fscanf(pFile, "%s", (char*)VtxPos.c_str());

						if (strcmp(VtxPos.c_str(), "END_VTXPOS") == 0)
						{// 途中終了されてたら

							for (int nCnt = nCntVtx; nCnt < nVtxNum; nCnt++)
							{
								// 0クリア
								m_aInfo.pVtxPos[nCnt].y = 0.0f;	// 高さ
							}
							break;
						}

						// float変換
						if (nCntStage == 0)
						{
							m_aInfo.pVtxPos[nCntVtx].y = 0.0f;	// 高さ
						}
						m_aInfo.pVtxPos[nCntVtx].y = std::strtof(VtxPos.c_str(), nullptr);	// 代入
						nCntVtx++;
					}
					// ステージ数加算
					nCntStage++;
				}

			}// END_FIELDSETのかっこ
		}

		if (strcmp(&aComment[0], "END_SCRIPT") == 0)
		{// 終了文字でループを抜ける

			break;
		}
	}

	// ファイルを閉じる
	fclose(pFile);
	return S_OK;

}

//==========================================================================
// セーブ処理
//==========================================================================
void CElevation::Save(void)
{
	FILE *pFile = NULL;	// ファイルポインタを宣言

	// ファイルを開く
	pFile = fopen("data\\TEXT\\elevation_field_save.txt", "w");

	if (pFile == NULL)
	{// ファイルが開けなかった場合
		return;
	}

	// ファイルに書き出す
	fprintf(pFile,
		"#------------------------------------------------------------------------------\n"
		"# テクスチャファイル名\n"
		"#------------------------------------------------------------------------------\n");

	// テクスチャファイル名
	fprintf(pFile, "TEXTURE_FILENAME = %s\t\t\n", m_aInfo.TextureFileName.c_str());
	
	// フィールドの設置
	fprintf(pFile,
		"\n"
		"#==============================================================================\n"
		"# フィールドの設置\n"
		"#==============================================================================\n");

	float m_fWidthLen = GetWidthLen();
	float m_fHeightLen = GetHeightLen();

	fprintf(pFile,
		"FIELDSET\n"
		"\tPOS = %.2f %.2f %.2f\n"
		"\tBLOCK = %d %d\n"
		"\tSIZE = %.2f %.2f\n"
		"\tVTXPOS\n", m_aInfo.pos.x, m_aInfo.pos.y, m_aInfo.pos.z,
		m_aInfo.nWidthBlock, m_aInfo.nHeightBlock,
		m_fWidthLen, m_fHeightLen);

	// 頂点取得
	D3DXVECTOR3 *pVtxPos = GetVtxPos();

	// 頂点情報の設定
	for (int nCntHeight = 0; nCntHeight < m_aInfo.nHeightBlock + 1; nCntHeight++)
	{//縦の頂点数分繰り返す

		fprintf(pFile, "\t");
		for (int nCntWidth = 0; nCntWidth < m_aInfo.nWidthBlock + 1; nCntWidth++)
		{// 横の頂点数分繰り返す

			fprintf(pFile, "%.2f ", pVtxPos[nCntWidth + (nCntHeight * (m_aInfo.nWidthBlock + 1))].y);
		}
		fprintf(pFile, "\n");
	}
	fprintf(pFile, "\nEND_VTXPOS\n");

	fprintf(pFile, "\nEND_FIELDSET\n");
	fprintf(pFile, "\nEND_SCRIPT		# この行は絶対消さないこと！");

	//ファイルを閉じる
	fclose(pFile);
}

//==========================================================================
// エディットの判定
//==========================================================================
bool CElevation::IsEdit(void)
{
	return m_bEdit;
}

//==========================================================================
// 起伏の情報取得
//==========================================================================
CElevation *CElevation::GetElevation(void)
{
	return this;
}
