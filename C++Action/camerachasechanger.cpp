//=============================================================================
// 
//  カメラの軸処理 [camerachasechanger.cpp]
//  Author : 相馬靜雅
// 
//=============================================================================
#include "debugproc.h"
#include "camerachasechanger.h"
#include "calculation.h"
#include "game.h"
#include "3D_effect.h"
#include "debugpointnumber.h"
#include "objectX.h"
#include "mapmanager.h"
#include "stage.h"
#include "manager.h"

//==========================================================================
// マクロ定義
//==========================================================================
#define MARKOBJ	"data\\MODEL\\crowd_01.x"

//==========================================================================
// 静的メンバ変数宣言
//==========================================================================
int CCameraChaseChanger::m_nNumChaseAxis = 0;		// 軸追従の総数
int CCameraChaseChanger::m_nNumChaseMap = 0;		// マップ追従の総数
int CCameraChaseChanger::m_nNumAll = 0;		// 総数

//==========================================================================
// コンストラクタ
//==========================================================================
CCameraChaseChanger::CCameraChaseChanger()
{
	// 値のクリア
	memset(&m_pMultiNumber[0], NULL, sizeof(m_pMultiNumber));
	memset(&m_apObjX[0], NULL, sizeof(m_apObjX));	// オブジェクトX
}

//==========================================================================
// デストラクタ
//==========================================================================
CCameraChaseChanger::~CCameraChaseChanger()
{

}

//==========================================================================
// 生成処理
//==========================================================================
CCameraChaseChanger *CCameraChaseChanger::Create(const std::string pFileName)
{
	// 生成用のオブジェクト
	CCameraChaseChanger *pModel = NULL;

	if (pModel == NULL)
	{// NULLだったら

		// メモリの確保
		pModel = DEBUG_NEW CCameraChaseChanger;

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
HRESULT CCameraChaseChanger::Init(void)
{
	return S_OK;
}

//==========================================================================
// 位置作成
//==========================================================================
void CCameraChaseChanger::CreatePos(CCamera::CHASETYPE type, int nIdx, float fMoveValue)
{
	sChaseChangeInfo InitInfo;
	memset(&InitInfo, NULL, sizeof(InitInfo));

	// 位置生成
	m_ChaseChangeInfo.push_back(InitInfo);
	m_ChaseChangeInfo[m_nNumAll].nMapIdx = nIdx;
	m_ChaseChangeInfo[m_nNumAll].fMapMoveValue = fMoveValue;
	m_ChaseChangeInfo[m_nNumAll].chaseType = type;

	// 総数加算
	switch (type)
	{
	case CCamera::CHASETYPE_NORMAL:
		m_ChaseChangeInfo[m_nNumAll].nByTypeIdx = m_nNumChaseAxis;
		m_nNumChaseAxis++;
		break;
	case CCamera::CHASETYPE_MAP:
		m_ChaseChangeInfo[m_nNumAll].nByTypeIdx = m_nNumChaseMap;
		m_nNumChaseMap++;
		break;
	}

	// デバッグ用数字の生成
	m_pMultiNumber[m_nNumAll] = CDebugPointNumber::Create(m_ChaseChangeInfo[m_nNumAll].nByTypeIdx);
	m_apObjX[m_nNumAll] = CObjectX::Create(MARKOBJ, mylib_const::DEFAULT_VECTOR3, mylib_const::DEFAULT_VECTOR3, false);	// オブジェクトX
	m_apObjX[m_nNumAll]->SetType(CObject::TYPE_BALLAST);

	// 総数加算
	m_nNumAll++;

}

//==========================================================================
// 位置削除
//==========================================================================
void CCameraChaseChanger::DeletePos(int nIdx)
{
	// ソート処理
	CalSort(&m_ChaseChangeInfo[0], nIdx, m_nNumAll);

	// 位置生成
	m_ChaseChangeInfo.pop_back();

	// 総数加算
	m_nNumAll--;
}

//==========================================================================
// 終了処理
//==========================================================================
void CCameraChaseChanger::Uninit(void)
{
	for (int i = 0; i < mylib_const::MAX_CAMERAAXIS; i++)
	{
		if (m_pMultiNumber[i] != NULL)
		{// NULLじゃなかったら
			//m_pMultiNumber[i]->Uninit();
			m_pMultiNumber[i] = NULL;
		}
	}
}

//==========================================================================
// 更新処理
//==========================================================================
void CCameraChaseChanger::Update(void)
{
#if _DEBUG

	// マップマネージャの取得
	CMapManager *pMapManager = CGame::GetMapManager();
	if (pMapManager == NULL)
	{// NULLだったら
		return;
	}

	// Xファイルとの判定
	CStage *pStage = CGame::GetStage();
	if (pStage == NULL)
	{// NULLだったら
		return;
	}

	for (int i = 0; i < m_nNumAll; i++)
	{
		if (m_apObjX[i] == NULL)
		{
			m_apObjX[i] = CObjectX::Create(MARKOBJ, mylib_const::DEFAULT_VECTOR3, mylib_const::DEFAULT_VECTOR3, false);	// オブジェクトX
			m_apObjX[i]->SetType(CObject::TYPE_BALLAST);
		}

		// マップ情報から位置取得
		D3DXVECTOR3 pos = pMapManager->GetTargetPosition(m_ChaseChangeInfo[i].nMapIdx, m_ChaseChangeInfo[i].fMapMoveValue);

		float fHeight = 0.0f;
		for (int nCntStage = 0; nCntStage < pStage->GetNumAll(); nCntStage++)
		{
			// オブジェクト取得
			CObjectX *pObjX = pStage->GetObj(nCntStage);

			if (pObjX == NULL)
			{// NULLだったら
				continue;
			}

			// 高さ取得
			bool bLand = false;
			pos.y = pObjX->GetHeight(pos, bLand);
		}

		m_apObjX[i]->SetPosition(pos);
		m_pMultiNumber[i]->SetPosition(D3DXVECTOR3(pos.x, pos.y + 50.0f, pos.z));
	}
#endif
}

//==========================================================================
// テキスト読み込み処理
//==========================================================================
HRESULT CCameraChaseChanger::ReadText(const std::string pFileName)
{

	// ファイルを開く
	FILE *pFile = fopen(pFileName.c_str(), "rb");

	if (pFile != NULL)
	{// ファイルが開けた場合

		// ファイルに数値を読み込む
		fread(&m_nNumAll, sizeof(int), 1, pFile);
		fread(&m_nNumChaseAxis, sizeof(int), 1, pFile);
		fread(&m_nNumChaseMap, sizeof(int), 1, pFile);

		// 生成する
		for (int i = 0; i < m_nNumAll; i++)
		{
			// 位置生成
			sChaseChangeInfo InitInfo;
			memset(&InitInfo, NULL, sizeof(InitInfo));
			m_ChaseChangeInfo.push_back(InitInfo);
			m_pMultiNumber[i] = CDebugPointNumber::Create(i);
		}

		if (m_ChaseChangeInfo.size() > 0)
		{
			fread(&m_ChaseChangeInfo[0], sizeof(sChaseChangeInfo), m_nNumAll, pFile);
		}

		// マップマネージャの取得
		CMapManager *pMapManager = CGame::GetMapManager();
		if (pMapManager == NULL)
		{// NULLだったら
			return S_OK;
		}
		for (int i = 0; i < m_nNumAll; i++)
		{
			m_ChaseChangeInfo[i].pos = pMapManager->GetTargetPosition(m_ChaseChangeInfo[i].nMapIdx, m_ChaseChangeInfo[i].fMapMoveValue);
		}

		// ファイルを閉じる
		fclose(pFile);
	}
	else
	{// ファイルが開けなかった場合

		m_nNumAll = 0;
		m_nNumChaseAxis = 0;
		m_nNumChaseMap = 0;

		// 生成する
		for (int i = 0; i < m_nNumAll; i++)
		{
			// 位置生成
			sChaseChangeInfo InitInfo;
			memset(&InitInfo, NULL, sizeof(InitInfo));
			m_ChaseChangeInfo.push_back(InitInfo);
			m_pMultiNumber[i] = CDebugPointNumber::Create(i);
		}

		// 全部ゼロ
		if (m_ChaseChangeInfo.size() > 0)
		{
			memset(&m_ChaseChangeInfo[0], NULL, sizeof(m_ChaseChangeInfo));
		}

		// セーブ
		Save(pFileName);
	}

	return S_OK;
}

//==========================================================================
// 外部ファイル書き出し処理
//==========================================================================
void CCameraChaseChanger::Save(const std::string pFileName)
{
	// ファイルを開く
	FILE *pFile = fopen(pFileName.c_str(), "wb");

	if (pFile != NULL)
	{// ファイルが開けた場合

		// ファイルに数値を書き出す
		fwrite(&m_nNumAll, sizeof(int), 1, pFile);
		fwrite(&m_nNumChaseAxis, sizeof(int), 1, pFile);
		fwrite(&m_nNumChaseMap, sizeof(int), 1, pFile);

		if (m_ChaseChangeInfo.size() > 0)
		{
			fwrite(&m_ChaseChangeInfo[0], sizeof(sChaseChangeInfo), m_nNumAll, pFile);
		}

		// ファイルを閉じる
		fclose(pFile);
	}
}

//==========================================================================
// 軸数取得
//==========================================================================
int CCameraChaseChanger::GetAxisNum(void)
{
	return m_ChaseChangeInfo.size() - 1;
}

//==========================================================================
// 軸取得
//==========================================================================
D3DXVECTOR3 CCameraChaseChanger::GetAxis(int nIdx)
{
	if (nIdx < 0)
	{
		nIdx = 0;
	}

	// マップマネージャの取得
	CMapManager *pMapManager = CGame::GetMapManager();
	if (pMapManager == NULL)
	{// NULLだったら
		return mylib_const::DEFAULT_VECTOR3;
	}

	D3DXVECTOR3 pos = mylib_const::DEFAULT_VECTOR3;

	if (nIdx >= (int)m_ChaseChangeInfo.size())
	{// 要素数を超えていたら

		int nMaxIdx = (int)m_ChaseChangeInfo.size() - 1;

		// マップ情報から位置取得
		pos = pMapManager->GetTargetPosition(m_ChaseChangeInfo[nMaxIdx].nMapIdx, m_ChaseChangeInfo[nMaxIdx].fMapMoveValue);
		return pos;
	}

	// マップ情報から位置取得
	pos = pMapManager->GetTargetPosition(m_ChaseChangeInfo[nIdx].nMapIdx, m_ChaseChangeInfo[nIdx].fMapMoveValue);

	return pos;
}

//==========================================================================
// 軸設定
//==========================================================================
void CCameraChaseChanger::SetAxis(int nIdx, int nMapIdx, float fMapMoveValue)
{
	if (nIdx < 0)
	{
		return;
	}

	// 情報渡す
	m_ChaseChangeInfo[nIdx].nMapIdx = nMapIdx;
	m_ChaseChangeInfo[nIdx].fMapMoveValue = fMapMoveValue;
}

//==========================================================================
// 変更の情報取得
//==========================================================================
CCameraChaseChanger::sChaseChangeInfo CCameraChaseChanger::GetChaseChangeInfo(int nIdx)
{
	sChaseChangeInfo InitInfo;
	memset(&InitInfo, NULL, sizeof(InitInfo));

	if (m_ChaseChangeInfo.size() <= 0 || (int)m_ChaseChangeInfo.size() <= nIdx)
	{// サイズ無し
		return InitInfo;
	}

	return m_ChaseChangeInfo[nIdx];
}
