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
	//// 総数リセット
	//m_ppMapManager = DEBUG_NEW (CCameraChaseChanger*);
	//*m_ppMapManager = NULL;

	return S_OK;
}

//==========================================================================
// 位置作成
//==========================================================================
void CCameraChaseChanger::CreatePos(CCamera::CHASETYPE type, D3DXVECTOR3 pos)
{
	sChaseChangeInfo InitInfo;
	memset(&InitInfo, NULL, sizeof(InitInfo));

	// 位置生成
	m_ChaseChangeInfo.push_back(InitInfo);
	m_ChaseChangeInfo[m_nNumAll].pos = pos;
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
	for (int i = 0; i < m_nNumAll; i++)
	{
		if (m_apObjX[i] == NULL)
		{
			m_apObjX[i] = CObjectX::Create(MARKOBJ, mylib_const::DEFAULT_VECTOR3, mylib_const::DEFAULT_VECTOR3, false);	// オブジェクトX
			m_apObjX[i]->SetType(CObject::TYPE_BALLAST);
		}

		m_apObjX[i]->SetPosition(m_ChaseChangeInfo[i].pos);
		m_pMultiNumber[i]->SetPosition(D3DXVECTOR3(m_ChaseChangeInfo[i].pos.x, m_ChaseChangeInfo[i].pos.y + 50.0f, m_ChaseChangeInfo[i].pos.z));
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
// 軸設定
//==========================================================================
void CCameraChaseChanger::SetAxis(D3DXVECTOR3 pos, int nIdx)
{
	m_ChaseChangeInfo[nIdx].pos = pos;
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

	if (nIdx >= (int)m_ChaseChangeInfo.size())
	{// 要素数を超えていたら

		int nMaxIdx = (int)m_ChaseChangeInfo.size() - 1;
		return m_ChaseChangeInfo[nMaxIdx].pos;
	}
	return m_ChaseChangeInfo[nIdx].pos;
}

//==========================================================================
// 変更の情報取得
//==========================================================================
CCameraChaseChanger::sChaseChangeInfo CCameraChaseChanger::GetChaseChangeInfo(int nIdx)
{
	sChaseChangeInfo InitInfo;
	memset(&InitInfo, NULL, sizeof(InitInfo));

	if (m_ChaseChangeInfo.size() <= 0)
	{// サイズ無し
		return InitInfo;
	}

	return m_ChaseChangeInfo[nIdx];
}
