//=============================================================================
// 
//  カメラの軸処理 [enemybase.cpp]
//  Author : 相馬靜雅
// 
//=============================================================================
#include "debugproc.h"
#include "enemybase.h"
#include "calculation.h"
#include "game.h"
#include "gamemanager.h"
#include "3D_effect.h"
#include "debugpointnumber.h"
#include "objectX.h"
#include "mapmanager.h"
#include "enemymanager.h"
#include "manager.h"

//==========================================================================
// マクロ定義
//==========================================================================
#define MARKOBJ	"data\\MODEL\\koko.x"
#define FILE_ENEMYBASE	"data\\BIN\\enemybase.bin"

//==========================================================================
// 静的メンバ変数宣言
//==========================================================================
int CEnemyBase::m_nNumAll = 0;		// 総数

//==========================================================================
// コンストラクタ
//==========================================================================
CEnemyBase::CEnemyBase()
{
	// 値のクリア
		memset(&m_pMultiNumber[0], NULL, sizeof(m_pMultiNumber));	// オブジェクトX
	memset(&m_apObjX[0], NULL, sizeof(m_apObjX));	// オブジェクトX
}

//==========================================================================
// デストラクタ
//==========================================================================
CEnemyBase::~CEnemyBase()
{

}

//==========================================================================
// 生成処理
//==========================================================================
CEnemyBase *CEnemyBase::Create(void)
{
	// 生成用のオブジェクト
	CEnemyBase *pModel = NULL;

	if (pModel == NULL)
	{// NULLだったら

		// メモリの確保
		pModel = DEBUG_NEW CEnemyBase;

		if (pModel != NULL)
		{// メモリの確保が出来ていたら

			// 初期化処理
			HRESULT hr = pModel->ReadText();

			if (FAILED(hr))
			{// 失敗していたら
				return NULL;
			}

			// 初期化処理
			hr = pModel->Init();
			if (FAILED(hr))
			{// 失敗していたら
				delete pModel;
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
HRESULT CEnemyBase::Init(void)
{
	// 敵マネージャ取得
	CEnemyManager *pEnemyManager = CGame::GetEnemyManager();

	if (CGame::GetGameManager()->GetType() != CGameManager::SCENE_BOSS)
	{
		// 生成する
		for (int i = 0; i < m_nNumAll; i++)
		{
			// デバッグ用数字の生成
			m_pMultiNumber[i] = CDebugPointNumber::Create(i);

			if (m_ChaseChangeInfo[i].nRush == 0)
			{// ラッシュ用じゃなかったら
				pEnemyManager->SetEnemy(
					D3DXVECTOR3(0.0f, m_ChaseChangeInfo[i].fSpawnPosY, 0.0f),
					m_ChaseChangeInfo[i].nMapIdx,
					m_ChaseChangeInfo[i].fMapMoveValue,
					m_ChaseChangeInfo[i].nPattern);
			}
		}
	}
	else if (CGame::GetGameManager()->GetType() == CGameManager::SCENE_BOSS)
	{// ボス戦中

		pEnemyManager->SetEnemy(
			D3DXVECTOR3(0.0f, 0.0f, 0.0f),
			D3DXVECTOR3(0.0f, 0.0f, 0.0f),
			0);

	}

	return S_OK;
}

//==========================================================================
// 位置作成
//==========================================================================
void CEnemyBase::CreatePos(int nPattern, int nMapIdx, float fMapMoveValue, int nRush, float PosY)
{
	sInfo InitInfo;
	memset(&InitInfo, NULL, sizeof(InitInfo));

	// 位置生成
	m_ChaseChangeInfo.push_back(InitInfo);
	m_ChaseChangeInfo[m_nNumAll].nPattern = nPattern;	// 種類
	m_ChaseChangeInfo[m_nNumAll].nMapIdx = nMapIdx;
	m_ChaseChangeInfo[m_nNumAll].fMapMoveValue = fMapMoveValue;
	m_ChaseChangeInfo[m_nNumAll].fSpawnPosY = PosY;
	m_ChaseChangeInfo[m_nNumAll].nRush = nRush;

	// 目印生成
	m_apObjX[m_nNumAll] = CObjectX::Create(MARKOBJ, mylib_const::DEFAULT_VECTOR3, mylib_const::DEFAULT_VECTOR3, false);	// オブジェクトX
	m_apObjX[m_nNumAll]->SetType(CObject::TYPE_BALLAST);
	m_apObjX[m_nNumAll]->SetPosition(D3DXVECTOR3(0.0f, PosY, 0.0f));

	// 総数加算
	m_nNumAll++;

}

//==========================================================================
// 位置削除
//==========================================================================
void CEnemyBase::DeletePos(int nIdx)
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
void CEnemyBase::Uninit(void)
{
	
}

//==========================================================================
// 更新処理
//==========================================================================
void CEnemyBase::Update(void)
{
#if _DEBUG

	// マップマネージャの取得
	CMapManager *pMapManager = CGame::GetMapManager();
	if (pMapManager == NULL)
	{// NULLだったら
		return;
	}

	for (int i = 0; i < m_nNumAll; i++)
	{
		if (m_apObjX[i] == NULL)
		{
			m_apObjX[i] = CObjectX::Create(MARKOBJ, mylib_const::DEFAULT_VECTOR3, mylib_const::DEFAULT_VECTOR3, false);	// オブジェクトX
			m_apObjX[i]->SetType(CObject::TYPE_BALLAST);
			//m_apObjX[i]->SetPositionD3DXVECTOR3(pos.x, m_apObjX[i]->GetPosition().y, pos.z);
		}

		// マップ情報から位置取得
		D3DXVECTOR3 pos = pMapManager->GetTargetPosition(m_ChaseChangeInfo[i].nMapIdx, m_ChaseChangeInfo[i].fMapMoveValue);

		m_apObjX[i]->SetPosition(D3DXVECTOR3(pos.x, m_ChaseChangeInfo[i].fSpawnPosY, pos.z));

		if (m_pMultiNumber[i] != NULL)
		{
			m_pMultiNumber[i]->SetPosition(D3DXVECTOR3(pos.x, m_ChaseChangeInfo[i].fSpawnPosY + 50.0f, pos.z));
		}
	}
#endif
}

//==========================================================================
// テキスト読み込み処理
//==========================================================================
HRESULT CEnemyBase::ReadText(void)
{
#if 0

	// ファイルを開く
	FILE *pFile = fopen(FILE_ENEMYBASE, "rb");

	if (pFile != NULL)
	{// ファイルが開けた場合

		// ファイルに数値を読み込む
		fread(&m_nNumAll, sizeof(int), 1, pFile);

		// 生成する
		for (int i = 0; i < m_nNumAll; i++)
		{
			// 位置生成
			sInfo InitInfo;
			memset(&InitInfo, NULL, sizeof(InitInfo));
			m_ChaseChangeInfo.push_back(InitInfo);
		}

		if (m_ChaseChangeInfo.size() > 0)
		{
			fread(&m_ChaseChangeInfo[0], sizeof(sInfo), m_nNumAll, pFile);
		}

		// ファイルを閉じる
		fclose(pFile);
	}
	else
	{// ファイルが開けなかった場合

		m_nNumAll = 0;

		// 生成する
		for (int i = 0; i < m_nNumAll; i++)
		{
			// 位置生成
			sInfo InitInfo;
			memset(&InitInfo, NULL, sizeof(InitInfo));
			m_ChaseChangeInfo.push_back(InitInfo);
		}

		// 全部ゼロ
		if (m_ChaseChangeInfo.size() > 0)
		{
			memset(&m_ChaseChangeInfo[0], NULL, sizeof(m_ChaseChangeInfo));
		}

		// セーブ
		Save();
	}

#else

	char aComment[MAX_COMMENT] = {};	// コメント用

	FILE *pFile = NULL;	// ファイルポインタを宣言

	// ファイルを開く
	pFile = fopen("data\\TEXT\\enemybase.txt", "r");

	if (pFile == NULL)
	{// ファイルが開けなかった場合
		return E_FAIL;
	}

	// リセット
	m_nNumAll = 0;

	while (1)
	{// END_SCRIPTが来るまで繰り返す

		// 文字列の読み込み
		fscanf(pFile, "%s", &aComment[0]);

		if (strcmp(aComment, "BASESET") == 0)
		{// BASESETで敵拠点の読み込み開始

			// 最後尾に生成
			sInfo InitInfo;
			memset(&InitInfo, NULL, sizeof(InitInfo));
			m_ChaseChangeInfo.push_back(InitInfo);

			while (strcmp(aComment, "END_BASESET") != 0)
			{// END_BASESETが来るまで繰り返す

				fscanf(pFile, "%s", &aComment[0]);	// 確認する

				if (strcmp(aComment, "PATTERN") == 0)
				{// PATTERNが来たら敵の種類読み込み

					fscanf(pFile, "%s", &aComment[0]);	// =の分
					fscanf(pFile, "%d", &m_ChaseChangeInfo[m_nNumAll].nPattern);	// キャラファイル番号
				}

				if (strcmp(aComment, "MAPIDX") == 0)
				{// MAPIDXが来たらマップインデックス番号読み込み

					fscanf(pFile, "%s", &aComment[0]);	// =の分
					fscanf(pFile, "%d", &m_ChaseChangeInfo[m_nNumAll].nMapIdx);	// マップインデックス番号
				}

				if (strcmp(aComment, "MAPMOVEVALUE") == 0)
				{// MAPMOVEVALUEが来たらマップ移動量読み込み

					fscanf(pFile, "%s", &aComment[0]);	// =の分
					fscanf(pFile, "%f", &m_ChaseChangeInfo[m_nNumAll].fMapMoveValue);	// マップ移動量
				}

				if (strcmp(aComment, "SPAWN_Y") == 0)
				{// SPAWN_Yが来たら出現高さ読み込み

					fscanf(pFile, "%s", &aComment[0]);	// =の分
					fscanf(pFile, "%f", &m_ChaseChangeInfo[m_nNumAll].fSpawnPosY);	// マップ移動量
				}

				if (strcmp(aComment, "RUSH") == 0)
				{// RUSHが来たらラッシュ用か読み込み

					fscanf(pFile, "%s", &aComment[0]);	// =の分
					fscanf(pFile, "%d", &m_ChaseChangeInfo[m_nNumAll].nRush);	// ラッシュ用
				}

			}// END_BASESETのかっこ

			// 敵の拠点数加算
			m_nNumAll++;
		}

		if (strcmp(&aComment[0], "END_SCRIPT") == 0)
		{// 終了文字でループを抜ける

			break;
		}
	}

#endif
	return S_OK;
}

//==========================================================================
// 外部ファイル書き出し処理
//==========================================================================
void CEnemyBase::Save(void)
{
#if 0
	// ファイルを開く
	FILE *pFile = fopen(FILE_ENEMYBASE, "wb");

	if (pFile != NULL)
	{// ファイルが開けた場合

		// ファイルに数値を書き出す
		fwrite(&m_nNumAll, sizeof(int), 1, pFile);

		if (m_ChaseChangeInfo.size() > 0)
		{
			fwrite(&m_ChaseChangeInfo[0], sizeof(sInfo), m_nNumAll, pFile);
		}

		// ファイルを閉じる
		fclose(pFile);
	}
#else

	FILE *pFile = NULL;	// ファイルポインタを宣言

	// ファイルを開く
	pFile = fopen("data\\TEXT\\enemybase.txt", "w");

	if (pFile == NULL)
	{// ファイルが開けなかった場合
		return;
	}

	fprintf(pFile,
		"\n"
		"#==============================================================================\n"
		"# 敵拠点の配置\n"
		"#==============================================================================\n");

	for (int i = 0; i < m_nNumAll; i++)
	{
		// 出力
		fprintf(pFile,
			"BASESET\n"
			"\tPATTERN = %d\n"
			"\tMAPIDX = %d\n"
			"\tMAPMOVEVALUE = %.2f\n"
			"\tSPAWN_Y = %.2f\n"
			"\tRUSH = %d\n"
			"END_BASESET\n\n",
			m_ChaseChangeInfo[i].nPattern,
			m_ChaseChangeInfo[i].nMapIdx,
			m_ChaseChangeInfo[i].fMapMoveValue,
			m_ChaseChangeInfo[i].fSpawnPosY,
			m_ChaseChangeInfo[i].nRush);
	}

	fprintf(pFile, "\nEND_SCRIPT		# この行は絶対消さないこと！");

	//ファイルを閉じる
	fclose(pFile);

#endif
}

//==========================================================================
// 軸数取得
//==========================================================================
int CEnemyBase::GetAxisNum(void)
{
	return m_ChaseChangeInfo.size() - 1;
}

//==========================================================================
// 軸取得
//==========================================================================
D3DXVECTOR3 CEnemyBase::GetAxis(int nIdx)
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
	pos.y = m_ChaseChangeInfo[nIdx].fSpawnPosY;

	return pos;
}

//==========================================================================
// 軸設定
//==========================================================================
void CEnemyBase::SetSpawnPoint(int nIdx, int nMapIdx, float fMapMoveValue, float PosY)
{
	if (nIdx < 0)
	{
		return;
	}

	// 情報渡す
	m_ChaseChangeInfo[nIdx].nMapIdx = nMapIdx;
	m_ChaseChangeInfo[nIdx].fMapMoveValue = fMapMoveValue;
	m_ChaseChangeInfo[nIdx].fSpawnPosY = PosY;
}

//==========================================================================
// 変更の情報取得
//==========================================================================
CEnemyBase::sInfo CEnemyBase::GetChaseChangeInfo(int nIdx)
{
	sInfo InitInfo;
	memset(&InitInfo, NULL, sizeof(InitInfo));

	if (m_ChaseChangeInfo.size() <= 0 || (int)m_ChaseChangeInfo.size() <= nIdx)
	{// サイズ無し
		return InitInfo;
	}

	return m_ChaseChangeInfo[nIdx];
}
