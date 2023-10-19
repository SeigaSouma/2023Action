//=============================================================================
// 
//  カメラの軸処理 [cameraaxis.cpp]
//  Author : 相馬靜雅
// 
//=============================================================================
#include "debugproc.h"
#include "cameraaxis.h"
#include "calculation.h"
#include "game.h"
#include "3D_effect.h"
#include "debugpointnumber.h"
#include "objectX.h"

//==========================================================================
// マクロ定義
//==========================================================================
#define MARKOBJ	"data\\MODEL\\map\\mark_01.x"

//==========================================================================
// 静的メンバ変数宣言
//==========================================================================
int CCameraAxis::m_nNumAll = 0;		// 総数

//==========================================================================
// コンストラクタ
//==========================================================================
CCameraAxis::CCameraAxis()
{
	// 値のクリア
	memset(&m_pMultiNumber[0], NULL, sizeof(m_pMultiNumber));
	memset(&m_apObjX[0], NULL, sizeof(m_apObjX));	// オブジェクトX
}

//==========================================================================
// デストラクタ
//==========================================================================
CCameraAxis::~CCameraAxis()
{

}

//==========================================================================
// 生成処理
//==========================================================================
CCameraAxis *CCameraAxis::Create(const std::string pFileName)
{
	// 生成用のオブジェクト
	CCameraAxis *pModel = NULL;

	if (pModel == NULL)
	{// NULLだったら

		// メモリの確保
		pModel = DEBUG_NEW CCameraAxis;

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
HRESULT CCameraAxis::Init(void)
{
	//// 総数リセット
	//m_ppMapManager = DEBUG_NEW (CCameraAxis*);
	//*m_ppMapManager = NULL;

	return S_OK;
}

//==========================================================================
// 位置作成
//==========================================================================
void CCameraAxis::CreatePos(D3DXVECTOR3 pos)
{
	// 位置生成
	m_posAll.push_back(pos);

	// デバッグ用数字の生成
	m_pMultiNumber[m_nNumAll] = CDebugPointNumber::Create(m_nNumAll);
	m_apObjX[m_nNumAll] = CObjectX::Create(MARKOBJ, mylib_const::DEFAULT_VECTOR3, mylib_const::DEFAULT_VECTOR3, false);	// オブジェクトX
	m_apObjX[m_nNumAll]->SetType(CObject::TYPE_BALLAST);

	// 総数加算
	m_nNumAll++;

}

//==========================================================================
// 位置削除
//==========================================================================
void CCameraAxis::DeletePos(int nIdx)
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
void CCameraAxis::Uninit(void)
{
	for (int i = 0; i < 32; i++)
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
void CCameraAxis::Update(void)
{
#if _DEBUG
	for (int i = 0; i < m_nNumAll; i++)
	{
		CEffect3D::Create(m_posAll[i], D3DXVECTOR3(0.0f, 0.0f, 0.0f), D3DXCOLOR(1.0f, 0.0f, 0.0f, 1.0f), 50.0f, 2, CEffect3D::MOVEEFFECT_NONE, CEffect3D::TYPE_NORMAL);
		m_apObjX[i]->SetPosition(m_posAll[i]);
		m_pMultiNumber[i]->SetPosition(D3DXVECTOR3(m_posAll[i].x, m_posAll[i].y + 50.0f, m_posAll[i].z));
	}
#endif
}

//==========================================================================
// テキスト読み込み処理
//==========================================================================
HRESULT CCameraAxis::ReadText(const std::string pFileName)
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
			m_pMultiNumber[i] = CDebugPointNumber::Create(i);
			m_apObjX[i] = CObjectX::Create(MARKOBJ, mylib_const::DEFAULT_VECTOR3, mylib_const::DEFAULT_VECTOR3, false);	// オブジェクトX
			m_apObjX[i]->SetType(CObject::TYPE_BALLAST);
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
			m_pMultiNumber[i] = CDebugPointNumber::Create(i);
			m_apObjX[i] = CObjectX::Create(MARKOBJ, mylib_const::DEFAULT_VECTOR3, mylib_const::DEFAULT_VECTOR3, false);	// オブジェクトX
			m_apObjX[i]->SetType(CObject::TYPE_BALLAST);
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
void CCameraAxis::Save(const std::string pFileName)
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
// 軸数取得
//==========================================================================
int CCameraAxis::GetAxisNum(void)
{
	return m_posAll.size() - 1;
}

//==========================================================================
// 軸設定
//==========================================================================
void CCameraAxis::SetAxis(D3DXVECTOR3 pos, int nIdx)
{
	m_posAll[nIdx] = pos;
}

//==========================================================================
// 軸取得
//==========================================================================
D3DXVECTOR3 CCameraAxis::GetAxis(int nIdx)
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

