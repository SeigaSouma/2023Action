//=============================================================================
// 
//  ステージ処理 [stage.cpp]
//  Author : 相馬靜雅
// 
//=============================================================================
#include "stage.h"
#include "manager.h"
#include "renderer.h"
#include "calculation.h"
#include "objectX.h"
#include "shadow.h"

//==========================================================================
// 静的メンバ変数宣言
//==========================================================================

//==========================================================================
// コンストラクタ
//==========================================================================
CStage::CStage()
{
	// ゼロクリア
	m_nNumAll = 0;
	memset(&m_pObjX[0], NULL, sizeof(m_pObjX));	// オブジェクトXのポインタ
}

//==========================================================================
// デストラクタ
//==========================================================================
CStage::~CStage()
{

}

//==========================================================================
// 生成処理
//==========================================================================
CStage *CStage::Create(const char *pTextFile)
{
	// 生成用のオブジェクト
	CStage *pObjectX = NULL;

	if (pObjectX == NULL)
	{// NULLだったら

		// メモリの確保
		pObjectX = DEBUG_NEW CStage;

		if (pObjectX != NULL)
		{// メモリの確保が出来ていたら

			// 初期化処理
			HRESULT hr = pObjectX->Init();
			if (FAILED(hr))
			{// 失敗していたら
				return NULL;
			}

			// 外部テキスト読み込み処理
			hr = pObjectX->ReadText(pTextFile);
			if (FAILED(hr))
			{// 失敗していたら
				return NULL;
			}
		}

		return pObjectX;
	}

	return NULL;
}


//==========================================================================
// 初期化処理
//==========================================================================
HRESULT CStage::Init(void)
{
	// 総数
	m_nNumAll = 0;

	return S_OK;
}

//==========================================================================
// 終了処理
//==========================================================================
void CStage::Uninit(void)
{
	// 終了処理
	for (int nCntObj = 0; nCntObj < mylib_const::MAX_STAGE; nCntObj++)
	{
		if (m_pObjX[nCntObj] != NULL)
		{// NULLじゃなかったら
			m_pObjX[nCntObj] = NULL;
		}
	}
}

//==========================================================================
// 解放処理
//==========================================================================
void CStage::Release(void)
{
	// 終了処理
	for (int nCntObj = 0; nCntObj < mylib_const::MAX_STAGE; nCntObj++)
	{
		if (m_pObjX[nCntObj] != NULL)
		{// NULLじゃなかったら
			m_pObjX[nCntObj]->Uninit();
			m_pObjX[nCntObj] = NULL;
		}
	}
}

//==========================================================================
// 更新処理
//==========================================================================
void CStage::Update(void)
{
	//for (int nCntObj = 0; nCntObj < mylib_const::MAX_STAGE; nCntObj++)
	//{
	//	if (m_pObjX[nCntObj] == NULL)
	//	{// NULLだったら
	//		continue;
	//	}

	//}
}

//==========================================================================
// 描画処理
//==========================================================================
void CStage::Draw(void)
{

}

//==========================================================================
// 外部ファイル書き込み処理
//==========================================================================
HRESULT CStage::SaveText(void)
{
#if TOPCUR
	FILE *pFile = NULL;	// ファイルポインタを宣言

	// ファイルを開く
	pFile = fopen("data\\TEXT\\stage_info.txt", "w");

	if (pFile == NULL)
	{// ファイルが開けなかった場合
		return E_FAIL;
	}

	// ファイルに書き出す
	fprintf(pFile,
		"\n"
		"#------------------------------------------------------------------------------\n"
		"# モデル数\n"
		"#------------------------------------------------------------------------------\n"
		"NUM_MODEL = %d\n\n", m_nNumAll);

	fprintf(pFile,
		"#------------------------------------------------------------------------------\n"
		"# モデルファイル名\n"
		"#------------------------------------------------------------------------------\n");

	// モデルファイル名分
	for (int nCntFileNum = 0; nCntFileNum < m_nNumAll; nCntFileNum++)
	{
		fprintf(pFile, "MODEL_FILENAME = %s\t\t# [%d]\n", &ModelFile[nCntFileNum][0], nCntFileNum);
	}


	fprintf(pFile,
		"\n"
		"#==============================================================================\n"
		"# モデルの配置\n"
		"#==============================================================================\n");

	for (int nCntPriority = 0; nCntPriority < mylib_const::PRIORITY_NUM; nCntPriority++)
	{
		// 先頭を保存
		CObject *pObj = CObject::GetTop(nCntPriority);

		while (pObj != NULL)
		{// NULLが来るまで無限ループ

			// 次のオブジェクトを一時保存
			CObject *pObjNext = pObj->GetNext();

			// 種類の取得
			CObject::TYPE TargetType = pObj->GetType();

			if (TargetType == CObject::TYPE_XFILE)
			{// Xファイルのモデルだったら

				// Xファイルの情報取得
				CObjectX *pObjX = pObj->GetObjectX();

				int nType = pObjX->GetIdxXFile();		// 種類
				D3DXVECTOR3 pos = pObjX->GetPosition();	// 位置
				D3DXVECTOR3 rot = pObjX->GetRotation();	// 向き
				int nShadow = 0;						// 影使うかどうか

				if (pObjX->GetUseShadow() == true)
				{// 使っている場合
					nShadow = 1;
				}

				// 出力
				fprintf(pFile,
					"MODELSET\n"
					"\tTYPE = %d\n"
					"\tPOS = %.2f %.2f %.2f\n"
					"\tROT = %.2f %.2f %.2f\n"
					"\tSHADOW = %d\n"
					"END_MODELSET\n\n",
					nType, pos.x, pos.y, pos.z,
					rot.x, rot.y, rot.z, nShadow);
			}

			// 次のオブジェクトを代入
			pObj = pObjNext;
		}
	}

	fprintf(pFile, "\nEND_SCRIPT		# この行は絶対消さないこと！");

	//ファイルを閉じる
	fclose(pFile);
#endif
	return S_OK;
}

//==========================================================================
// モデル読み込み処理
//==========================================================================
HRESULT CStage::ReadXFile(const char *pTextFile)
{
	// リセット
	int nNumFileAll = 0;

	char aComment[MAX_COMMENT] = {};	// コメント用
	int nFileNum = 0;					// ファイルの数
	int nCntTexture = 0;				// テクスチャ読み込みカウント

	// ファイルポインタ
	FILE *pFile = NULL;

	//ファイルを開く
	pFile = fopen(pTextFile, "r");

	if (pFile == NULL)
	{//ファイルが開けた場合
		return E_FAIL;
	}

	while (1)
	{// END_SCRIPTが来るまで繰り返す

		// 文字列の読み込み
		fscanf(pFile, "%s", &aComment[0]);

		// モデル数の設定
		if (strcmp(&aComment[0], "NUM_MODEL") == 0)
		{// NUM_MODELがきたら

			fscanf(pFile, "%s", &aComment[0]);	// =の分
			fscanf(pFile, "%d", &nFileNum);	// モデル数
		}

		while (nNumFileAll != nFileNum)
		{// モデルの数分読み込むまで繰り返し

			// 文字列の読み込み
			fscanf(pFile, "%s", &aComment[0]);

			// モデル名の設定
			if (strcmp(&aComment[0], "MODEL_FILENAME") == 0)
			{// NUM_MODELがきたら

				fscanf(pFile, "%s", &aComment[0]);	// =の分
				fscanf(pFile, "%s", &aComment[0]);	// ファイル名

				// 最後尾に追加
				ModelFile.push_back(&aComment[0]);

				// Xファイルのデータ取得
				CXLoad *pXLoad = CScene::GetXLoad();

				// ファイル読み込み
				pXLoad->XLoad(&ModelFile[m_nNumAll][0]);

				// ファイル数
				nNumFileAll++;
			}
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
// 外部テキスト読み込み処理
//==========================================================================
HRESULT CStage::ReadText(const char *pTextFile)
{

	// ファイルから読み込み
	if (FAILED(ReadXFile(pTextFile)))
	{// 失敗した場合
		return E_FAIL;
	}

	char aComment[MAX_COMMENT] = {};	//コメント用

	// ファイルポインタ
	FILE *pFile = NULL;

	//ファイルを開く
	pFile = fopen(pTextFile, "r");

	if (pFile == NULL)
	{//ファイルが開けた場合
		return E_FAIL;
	}

	while (1)
	{// END_SCRIPTが来るまで繰り返す

		// 文字列の読み込み
		fscanf(pFile, "%s", &aComment[0]);

		// モデルの設定
		if (strcmp(&aComment[0], "MODELSET") == 0)
		{// モデルの読み込みを開始

			int nType = 0;
			int nShadow = 0;
			D3DXVECTOR3 pos = mylib_const::DEFAULT_VECTOR3;
			D3DXVECTOR3 rot = mylib_const::DEFAULT_VECTOR3;


			while (strcmp(&aComment[0], "END_MODELSET"))
			{// END_MODELSETが来るまで繰り返し

				fscanf(pFile, "%s", &aComment[0]);	// 確認する

				if (strcmp(&aComment[0], "TYPE") == 0)
				{// TYPEが来たら種類読み込み

					fscanf(pFile, "%s", &aComment[0]);	// =の分
					fscanf(pFile, "%d", &nType);		// モデル種類の列挙
				}

				if (strcmp(&aComment[0], "POS") == 0)
				{// POSが来たら位置読み込み

					fscanf(pFile, "%s", &aComment[0]);	// =の分
					fscanf(pFile, "%f", &pos.x);		// X座標
					fscanf(pFile, "%f", &pos.y);		// Y座標
					fscanf(pFile, "%f", &pos.z);		// Z座標
				}

				if (strcmp(&aComment[0], "ROT") == 0)
				{// ROTが来たら向き読み込み

					fscanf(pFile, "%s", &aComment[0]);	// =の分
					fscanf(pFile, "%f", &rot.x);		// Xの向き
					fscanf(pFile, "%f", &rot.y);		// Yの向き
					fscanf(pFile, "%f", &rot.z);		// Zの向き
				}

				if (strcmp(&aComment[0], "SHADOW") == 0)
				{// SHADOWが来たら向き読み込み

					fscanf(pFile, "%s", &aComment[0]);	// =の分
					fscanf(pFile, "%d", &nShadow);		// 影を使うかどうか
				}

			}// END_MODELSETのかっこ

			if (nShadow == 1)
			{// 影を使用する場合

				// タイプの物を生成
				m_pObjX[m_nNumAll] = CObjectX::Create(&ModelFile[nType][0], pos, rot, true);
			}
			else
			{
				// タイプの物を生成
				m_pObjX[m_nNumAll] = CObjectX::Create(&ModelFile[nType][0], pos, rot, false);
			}

			if (m_pObjX[m_nNumAll] == NULL)
			{// 失敗していたら
				return E_FAIL;
			}

			// 種類設定
			m_pObjX[m_nNumAll]->SetType(CObject::TYPE_STAGE);
			m_nNumAll++;	// モデル数加算
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
// オブジェクト取得
//==========================================================================
CObjectX *CStage::GetObj(int nIdx)
{
	return m_pObjX[nIdx];
}

//==========================================================================
// 総数取得
//==========================================================================
int CStage::GetNumAll(void)
{
	return m_nNumAll;
}