//=============================================================================
// 
//  マップマネージャ処理 [mapmanager.cpp]
//  Author : 相馬靜雅
// 
//=============================================================================
#include "debugproc.h"
#include "mapmanager.h"
#include "calculation.h"
#include "game.h"
#include "3D_effect.h"
#include "debugpointnumber.h"
#include "stage.h"
#include "objectX.h"

//==========================================================================
// マクロ定義
//==========================================================================

//==========================================================================
// 静的メンバ変数宣言
//==========================================================================
CMapManager *CMapManager::m_pTop = NULL;	// 先頭のポインタ
CMapManager *CMapManager::m_pCur = NULL;	// 最後尾のポインタ
int CMapManager::m_nNumAll = 0;		// 総数

//==========================================================================
// コンストラクタ
//==========================================================================
CMapManager::CMapManager()
{
	// 値のクリア
	m_pPrev = NULL;		// 前のオブジェクトへのポインタ
	m_pNext = NULL;		// 次のオブジェクトへのポインタ
	m_pos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);	// 位置
	memset(&m_pMultiNumber[0], NULL, sizeof(m_pMultiNumber));
}

//==========================================================================
// デストラクタ
//==========================================================================
CMapManager::~CMapManager()
{

}

//==========================================================================
// 生成処理
//==========================================================================
CMapManager *CMapManager::Create(const std::string pFileName)
{
	// 生成用のオブジェクト
	CMapManager *pModel = NULL;

	if (pModel == NULL)
	{// NULLだったら

		// メモリの確保
		pModel = DEBUG_NEW CMapManager;

		if (pModel != NULL)
		{// メモリの確保が出来ていたら

			// 初期化処理
			HRESULT hr = pModel->Init();
			if (FAILED(hr))
			{// 失敗していたら
				delete pModel;
				return NULL;
			}

			// 初期化処理
			hr = pModel->ReadText(pFileName);

			if (FAILED(hr))
			{// 失敗していたら
				return NULL;
			}
		}

		return pModel;
	}

	return NULL;
}

//==========================================================================
// 初期化処理
//==========================================================================
HRESULT CMapManager::Init(void)
{
	//// 総数リセット
	//m_ppMapManager = DEBUG_NEW (CMapManager*);
	//*m_ppMapManager = NULL;

	return S_OK;
}

//==========================================================================
// 位置作成
//==========================================================================
void CMapManager::CreatePos(D3DXVECTOR3 pos)
{
#if 0
	// 最後尾を保存
	CMapManager *pObjCur = m_pCur;

	if (pObjCur != NULL)
	{// 最後尾が存在していたら

		pObjCur->m_pNext = this;	// 最後尾の次に自分自身を代入
		m_pCur = this;				// 最後尾を自分自身に更新
		m_pCur->m_pPrev = pObjCur;	// 最後尾の前に過去の最後尾を代入
	}
	else
	{// 最後尾が無かったら
		m_pCur = this;	// 最後尾を自分自身に更新
	}

	if (m_pTop == NULL)
	{// 先頭に何もなかったら
		m_pTop = this;	// 先頭に自分自身のポインタを渡す
	}
#else
	//// 生成する
	//m_ppMapManager[m_nNumAll] = DEBUG_NEW CMapManager;
	//m_ppMapManager[m_nNumAll] = NULL;

	//// 位置代入
	//m_ppMapManager[m_nNumAll]->m_pos = pos;
#endif
	// 位置生成
	m_posAll.push_back(pos);


	// デバッグ用数字の生成
	m_pMultiNumber[m_nNumAll] = CDebugPointNumber::Create(m_nNumAll - 1);

	// 総数加算
	m_nNumAll++;

}

//==========================================================================
// 位置削除
//==========================================================================
void CMapManager::DeletePos(int nIdx)
{
	// ソート処理
	CalSort(&m_posAll[0], nIdx, m_nNumAll);

	// 位置生成
	m_posAll.pop_back();

	// 総数加算
	m_nNumAll--;
}

//==========================================================================
// 終了処理
//==========================================================================
void CMapManager::Uninit(void)
{
	//if (m_posAll != NULL)
	//{// 位置があったら
	//	delete[] m_posAll;
	//	m_posAll = NULL;
	//}

	//if (*m_ppMapManager != NULL)
	//{
	//	//for (int nCntPoint = 0; nCntPoint < m_nNumAll; nCntPoint++)
	//	//{
	//	//	if (m_ppMapManager[nCntPoint] != NULL)
	//	//	{// マネージャがあったら
	//	//		delete m_ppMapManager[nCntPoint];
	//	//		m_ppMapManager[nCntPoint] = NULL;
	//	//	}
	//	//}

	//	delete m_ppMapManager;
	//	m_ppMapManager = NULL;
	//}
}

//==========================================================================
// 更新処理
//==========================================================================
void CMapManager::Update(void)
{
#if _DEBUG
	for (int i = 0; i < m_nNumAll; i++)
	{
		//// Xファイルとの判定
		//CStage *pStage = CGame::GetStage();
		//if (pStage == NULL)
		//{// NULLだったら
		//	return;
		//}

		//float fHeight = 0.0f;
		//for (int nCntStage = 0; nCntStage < pStage->GetNumAll(); nCntStage++)
		//{
		//	// オブジェクト取得
		//	CObjectX *pObjX = pStage->GetObj(nCntStage);

		//	if (pObjX == NULL)
		//	{// NULLだったら
		//		continue;
		//	}

		//	// 高さ取得
		//	bool bLand = false;
		//	m_posAll[i].y = pObjX->GetHeight(m_posAll[i], bLand);
		//}

		CEffect3D::Create(m_posAll[i], D3DXVECTOR3(0.0f, 0.0f, 0.0f), D3DXCOLOR(1.0f, 0.0f, 0.0f, 1.0f), 50.0f, 2, CEffect3D::MOVEEFFECT_NONE, CEffect3D::TYPE_NORMAL);
		m_pMultiNumber[i]->SetPosition(D3DXVECTOR3(m_posAll[i].x, m_posAll[i].y + 50.0f, m_posAll[i].z));

	}
#endif
}

//==========================================================================
// テキスト読み込み処理
//==========================================================================
HRESULT CMapManager::ReadText(const std::string pFileName)
{

	// ファイルを開く
	FILE *pFile = fopen(pFileName.c_str(), "rb");

	if (pFile != NULL)
	{// ファイルが開けた場合

		// ファイルに数値を読み込む
		fread(&m_nNumAll, sizeof(int), 1, pFile);

		// 生成する
		for (int i = 0; i < m_nNumAll; i++)
		{
			// 位置生成
			m_posAll.push_back(mylib_const::DEFAULT_VECTOR3);
			m_pMultiNumber[i] = CDebugPointNumber::Create(i - 1);
		}

		fread(&m_posAll[0], sizeof(D3DXVECTOR3), m_nNumAll, pFile);

		// ファイルを閉じる
		fclose(pFile);
	}
	else
	{// ファイルが開けなかった場合

		m_nNumAll = 1;

		// 生成する
		for (int i = 0; i < m_nNumAll; i++)
		{
			// 位置生成
			m_posAll.push_back(mylib_const::DEFAULT_VECTOR3);
			m_pMultiNumber[i] = CDebugPointNumber::Create(i - 1);
		}

		for (int nCount = 0; nCount < m_nNumAll; nCount++)
		{// 全部ゼロ
			m_posAll[nCount] = mylib_const::DEFAULT_VECTOR3;
		}

		// セーブ
		Save(pFileName);
	}

	return S_OK;
}

//==========================================================================
// 外部ファイル書き出し処理
//==========================================================================
void CMapManager::Save(const std::string pFileName)
{
	// ファイルを開く
	FILE *pFile = fopen(pFileName.c_str(), "wb");

	if (pFile != NULL)
	{// ファイルが開けた場合

		// ファイルに数値を書き出す
		fwrite(&m_nNumAll, sizeof(int), 1, pFile);
		fwrite(&m_posAll[0], sizeof(D3DXVECTOR3), m_nNumAll, pFile);

		// ファイルを閉じる
		fclose(pFile);
	}
}

//==========================================================================
// 制御点数取得
//==========================================================================
int CMapManager::GetControlPointNum(void)
{
	return m_posAll.size() - 1;
}

//==========================================================================
// 制御点設定
//==========================================================================
void CMapManager::SetControlPoint(D3DXVECTOR3 pos, int nIdx)
{
	m_posAll[nIdx] = pos;
}

//==========================================================================
// 制御点取得
//==========================================================================
D3DXVECTOR3 CMapManager::GetControlPoint(int nIdx)
{
	if (nIdx < 0)
	{
		nIdx = 0;
	}

	if (nIdx >= (int)m_posAll.size())
	{// 要素数を超えていたら

		int nMaxIdx = (int)m_posAll.size() - 1;
		return m_posAll[nMaxIdx];
	}
	return m_posAll[nIdx];
}

//==========================================================================
// 目標の位置取得
//==========================================================================
D3DXVECTOR3 CMapManager::GetTargetPosition(int nIdx, float fMoveValue)
{
	D3DXVECTOR3 pos = mylib_const::DEFAULT_VECTOR3;
	float fRatio = 0.0f;

	// 曲線作る為の4点
	int nP0, nP1, nP2, nP3;
	nP0 = nIdx;
	nP1 = nIdx + 1;
	nP2 = nIdx + 2;
	nP3 = nIdx + 3;

	// 目標地点
	D3DXVECTOR3 TargetPoint0 = GetControlPoint(nP0);
	D3DXVECTOR3 TargetPoint1 = GetControlPoint(nP1);
	D3DXVECTOR3 TargetPoint2 = GetControlPoint(nP2);
	D3DXVECTOR3 TargetPoint3 = GetControlPoint(nP3);

	// 2点間の距離取得
	float fPosLength = GetPosLength(TargetPoint1, TargetPoint2);

	// 割合を求める
	fRatio = fMoveValue / fPosLength;

	if (fRatio >= 1.0f && nIdx < GetControlPointNum() - 1)
	{
		// マップの位置加算
		nIdx++;
		if (nP1 < GetControlPointNum() - 1)
		{
			fMoveValue = fMoveValue - fPosLength;
			fRatio = fMoveValue / GetPosLength(TargetPoint2, TargetPoint3);
		}
	}
	else if (fRatio < 0.0f)
	{
		// マップの位置減算
		nIdx--;
		if (nIdx < -1)
		{
			nIdx = -1;
		}

		fMoveValue = GetPosLength(TargetPoint0, TargetPoint1) - (-fMoveValue);
		fRatio = fMoveValue / GetPosLength(TargetPoint0, TargetPoint1);
	}

	// 補正の4点更新
	nP0 = nIdx;
	nP1 = nIdx + 1;
	nP2 = nIdx + 2;
	nP3 = nIdx + 3;

	// 4点の位置も更新
	TargetPoint0 = GetControlPoint(nP0);
	TargetPoint1 = GetControlPoint(nP1);
	TargetPoint2 = GetControlPoint(nP2);
	TargetPoint3 = GetControlPoint(nP3);

	pos = CatmullRomSplineInterp(TargetPoint0, TargetPoint1, TargetPoint2, TargetPoint3, fRatio);

	return pos;
}

//==========================================================================
// 現在地取得
//==========================================================================
D3DXVECTOR3 CMapManager::UpdateNowPosition(int& nIdx, float& fRatio, float& fMoveValue, float PosY)
{
	D3DXVECTOR3 pos;
	D3DXVECTOR3 sakiPos;

	// 曲線作る為の4点
	int nP0, nP1, nP2, nP3;
	nP0 = nIdx;
	nP1 = nIdx + 1;
	nP2 = nIdx + 2;
	nP3 = nIdx + 3;

	// 目標地点
	D3DXVECTOR3 TargetPoint0 = GetControlPoint(nP0);
	D3DXVECTOR3 TargetPoint1 = GetControlPoint(nP1);
	D3DXVECTOR3 TargetPoint2 = GetControlPoint(nP2);
	D3DXVECTOR3 TargetPoint3 = GetControlPoint(nP3);

	// 2点間の距離取得
	float fPosLength = GetPosLength(TargetPoint1, TargetPoint2);

	// 位置の割合
	if (fPosLength != 0.0f)
	{
		fRatio = fMoveValue / fPosLength;
	}
	else if (nIdx >= GetControlPointNum() - 1/* && move.x < 0*/)
	{// 最後
		fRatio = 1.0f;
	}

	bool bLeftArrival = false, bRightArrival = false;
	while (1)
	{
		bLeftArrival = false, bRightArrival = false;
		if (fRatio >= 1.0f && nIdx < GetControlPointNum() - 1)
		{
			// マップの位置加算
			nIdx++;
			if (nP1 < GetControlPointNum() - 1)
			{
				fMoveValue = fMoveValue - fPosLength;
				fRatio = fMoveValue / GetPosLength(TargetPoint2, TargetPoint3);
				bRightArrival = true;
			}
		}
		else if (fRatio < 0.0f)
		{
			// マップの位置減算
			nIdx--;

			if (nIdx < -1)
			{
				nIdx = -1;
			}
			fMoveValue = GetPosLength(TargetPoint0, TargetPoint1);
			fRatio = fMoveValue / GetPosLength(TargetPoint0, TargetPoint1);
			bLeftArrival = true;
		}

		if (fRatio >= 0.0f && fRatio <= 1.0f)
		{
			break;
		}
		else
		{
			nP0 = nIdx;
			nP1 = nIdx + 1;
			nP2 = nIdx + 2;
			nP3 = nIdx + 3;

			// 目標地点
			TargetPoint0 = GetControlPoint(nP0);
			TargetPoint1 = GetControlPoint(nP1);
			TargetPoint2 = GetControlPoint(nP2);
			TargetPoint3 = GetControlPoint(nP3);
		}
	}

	if (bLeftArrival == true || bRightArrival == true)
	{// 左右どっちか到着

		// 補正の4点更新
		nP0 = nIdx;
		nP1 = nIdx + 1;
		nP2 = nIdx + 2;
		nP3 = nIdx + 3;

		// 4点の位置も更新
		TargetPoint0 = GetControlPoint(nP0);
		TargetPoint1 = GetControlPoint(nP1);
		TargetPoint2 = GetControlPoint(nP2);
		TargetPoint3 = GetControlPoint(nP3);

		if (bLeftArrival == true)
		{
			// 移動の値を終端
			int nIdx1 = nP1, nIdx2 = nP2;
			fMoveValue = GetPosLength(GetControlPoint(nIdx1), GetControlPoint(nIdx2));
			if (nIdx <= -1)
			{
				nIdx1 = nP0 + 1, nIdx2 = nP1 + 1;
				TargetPoint0 = GetControlPoint(0);
				TargetPoint1 = GetControlPoint(0);
				TargetPoint2 = GetControlPoint(1);
				TargetPoint3 = GetControlPoint(2);
				fMoveValue = GetPosLength(GetControlPoint(nIdx1), GetControlPoint(nIdx2));
			}
		}
	}

	if (nIdx <= -1 && fRatio < 1.0f && fRatio > 0.0f)
	{
		fRatio = 1.0f;
		int nIdx1 = nP0 + 1, nIdx2 = nP1 + 1;
		fMoveValue = fPosLength;
	}
	else if (nIdx >= GetControlPointNum() - 1)
	{
		fRatio = 0.0f;
		fMoveValue = 0.0f;
	}

	// 曲線の位置
	pos = CatmullRomSplineInterp(TargetPoint0, TargetPoint1, TargetPoint2, TargetPoint3, fRatio);
	pos.y = PosY;

	return pos;
}

//==========================================================================
// 現在地更新
//==========================================================================
D3DXVECTOR3 CMapManager::UpdateNowPosition(int& nIdx, float& fRatio, float& fMoveValue, float PosY, CObject::ANGLE MoveAngle, float fMove)
{
	D3DXVECTOR3 pos;
	D3DXVECTOR3 sakiPos;

	// 移動用の向き
	int nAngle = 1;

	// 移動した量加算
	if (MoveAngle == CObject::ANGLE_LEFT)
	{// 左は逆移動
		nAngle = -1;
	}

	// 移動
	fMoveValue += fMove * nAngle;

	// 曲線作る為の4点
	int nP0, nP1, nP2, nP3;
	nP0 = nIdx;
	nP1 = nIdx + 1;
	nP2 = nIdx + 2;
	nP3 = nIdx + 3;

	// 目標地点
	D3DXVECTOR3 TargetPoint0 = GetControlPoint(nP0);
	D3DXVECTOR3 TargetPoint1 = GetControlPoint(nP1);
	D3DXVECTOR3 TargetPoint2 = GetControlPoint(nP2);
	D3DXVECTOR3 TargetPoint3 = GetControlPoint(nP3);

	// 2点間の距離取得
	float fPosLength = GetPosLength(TargetPoint1, TargetPoint2);

	// 位置の割合
	if (fPosLength != 0.0f)
	{
		fRatio = fMoveValue / fPosLength;
	}
	else if (nIdx >= GetControlPointNum() - 1/* && move.x < 0*/)
	{// 最後
		fRatio = 1.0f;
	}

	bool bLeftArrival = false, bRightArrival = false;
	while (1)
	{
		bLeftArrival = false, bRightArrival = false;
		if (fRatio >= 1.0f && nIdx < GetControlPointNum() - 1)
		{
			// マップの位置加算
			nIdx++;
			if (nP1 < GetControlPointNum() - 1)
			{
				fMoveValue = fMoveValue - fPosLength;
				fRatio = fMoveValue / GetPosLength(TargetPoint2, TargetPoint3);
				bRightArrival = true;
			}
		}
		else if (fRatio < 0.0f)
		{
			// マップの位置減算
			nIdx--;

			if (nIdx < -1)
			{
				nIdx = -1;
			}
			fMoveValue = GetPosLength(TargetPoint0, TargetPoint1);
			fRatio = fMoveValue / GetPosLength(TargetPoint0, TargetPoint1);
			bLeftArrival = true;
		}

		if (fRatio >= 0.0f && fRatio <= 1.0f)
		{
			break;
		}
	}

	if (bLeftArrival == true || bRightArrival == true)
	{// 左右どっちか到着

		// 補正の4点更新
		nP0 = nIdx;
		nP1 = nIdx + 1;
		nP2 = nIdx + 2;
		nP3 = nIdx + 3;

		// 4点の位置も更新
		TargetPoint0 = GetControlPoint(nP0);
		TargetPoint1 = GetControlPoint(nP1);
		TargetPoint2 = GetControlPoint(nP2);
		TargetPoint3 = GetControlPoint(nP3);

		if (bLeftArrival == true)
		{
			// 移動の値を終端
			int nIdx1 = nP1, nIdx2 = nP2;
			fMoveValue = GetPosLength(GetControlPoint(nIdx1), GetControlPoint(nIdx2));
			if (nIdx <= -1)
			{
				nIdx1 = nP0 + 1, nIdx2 = nP1 + 1;
				TargetPoint0 = GetControlPoint(0);
				TargetPoint1 = GetControlPoint(0);
				TargetPoint2 = GetControlPoint(1);
				TargetPoint3 = GetControlPoint(2);
				fMoveValue = GetPosLength(GetControlPoint(nIdx1), GetControlPoint(nIdx2));
			}
		}
	}

	if (nIdx <= -1 && fRatio < 1.0f && fRatio > 0.0f)
	{
		fRatio = 1.0f;
		int nIdx1 = nP0 + 1, nIdx2 = nP1 + 1;
		fMoveValue = fPosLength;
	}
	else if (nIdx >= GetControlPointNum() - 1)
	{
		fRatio = 0.0f;
		fMoveValue = 0.0f;
	}

	// 曲線の位置
	pos = CatmullRomSplineInterp(TargetPoint0, TargetPoint1, TargetPoint2, TargetPoint3, fRatio);
	pos.y = PosY;

	return pos;
}

//==========================================================================
// 目標が自分のどっちにいるかの判定
//==========================================================================
CObject::ANGLE CMapManager::GetTargetAngle(int myIdx, int TargetIdx, float myMoveValue, float TargetMoveValue)
{
	CObject::ANGLE angle = CObject::ANGLE_UP;

	if (myIdx > TargetIdx)
	{// 自分より左にいる
		angle = CObject::ANGLE_LEFT;
	}
	else if (myIdx < TargetIdx)
	{// 自分より右にいる
		angle = CObject::ANGLE_RIGHT;
	}
	else if (myIdx == TargetIdx)
	{// 自分と同じ

		if (myMoveValue > TargetMoveValue)
		{// 自分より左にいる
			angle = CObject::ANGLE_LEFT;
		}
		else
		{// 自分より右にいる
			angle = CObject::ANGLE_RIGHT;
		}
	}

	return angle;
}

//==========================================================================
// 目標が自分のどっちにいるかの判定
//==========================================================================
CObject::ANGLE CMapManager::GetTargetAngle(CObject *pMyObj, CObject *pTargetObj)
{
	CObject::ANGLE angle = CObject::ANGLE_UP;
	if (pMyObj == NULL || pTargetObj == NULL)
	{// NULLだったら
		return angle;
	}

	int nMyIdx = pMyObj->GetMapIndex();
	int nTargetIdx = pTargetObj->GetMapIndex();
	float fMyMoveValue = pMyObj->GetMapMoveValue();
	float fTargetMoveValue = pTargetObj->GetMapMoveValue();

	if (nMyIdx > nTargetIdx)
	{// 自分より左にいる
		angle = CObject::ANGLE_LEFT;
	}
	else if (nMyIdx < nTargetIdx)
	{// 自分より右にいる
		angle = CObject::ANGLE_RIGHT;
	}
	else if (nMyIdx == nTargetIdx)
	{// 自分と同じ

		if (fMyMoveValue > fTargetMoveValue)
		{// 自分より左にいる
			angle = CObject::ANGLE_LEFT;
		}
		else
		{// 自分より右にいる
			angle = CObject::ANGLE_RIGHT;
		}
	}

	return angle;
}

//==========================================================================
// これまでのマップ移動量取得
//==========================================================================
float CMapManager::GetThusFarMoveValue(int nIdx)
{

	int nCalIdx = -1;
	float fMoveValue = 0.0f;

	// 曲線作る為の4点
	int nP0, nP1;

	while (1)
	{
		nP0 = nCalIdx;
		nP1 = nCalIdx + 1;

		if (nP0 >= nIdx)
		{// 最終点超えたら終了
			break;
		}

		// 目標地点
		D3DXVECTOR3 TargetPoint0 = GetControlPoint(nP0);
		D3DXVECTOR3 TargetPoint1 = GetControlPoint(nP1);

		// 長さ加算
		fMoveValue += GetPosLength(TargetPoint0, TargetPoint1);
		nCalIdx++;
	}

	return fMoveValue;
}

//==========================================================================
// 制御点取得
//==========================================================================
D3DXVECTOR3 CMapManager::GetControlPoint(void)
{
	return m_pos;
}

