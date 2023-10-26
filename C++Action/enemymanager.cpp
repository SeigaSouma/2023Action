//=============================================================================
// 
//  敵のマネージャ処理 [enemymanager.cpp]
//  Author : 相馬靜雅
// 
//=============================================================================
#include "debugproc.h"
#include "enemymanager.h"
#include "calculation.h"
#include "manager.h"
#include "game.h"
#include "gamemanager.h"
#include "renderer.h"
#include "enemy.h"
#include "particle.h"
#include "fade.h"
#include "elevation.h"
#include "player.h"
#include "impactwave.h"
#include "camera.h"
#include "sound.h"
#include "enemyfixedmove_manager.h"
#include "enemybase.h"

//==========================================================================
// マクロ定義
//==========================================================================
#define SPAWN_STRENGTH	(100.0f)	// スポーン時の高さ
#define KILL_INT		(30)		// 倒したインターバル

//==========================================================================
// 静的メンバ変数宣言
//==========================================================================
CEnemyManager::Pattern CEnemyManager::m_aPattern[mylib_const::MAX_PATTEN_ENEMY] = {};			// 読み込みデータ
std::string CEnemyManager::sMotionFileName[mylib_const::MAX_PATTEN_ENEMY] = {};					// モーションファイル名
int CEnemyManager::m_nPatternNum = 0;	// 出現パターン数
CEnemy *CEnemyManager::m_pEnemy[mylib_const::MAX_OBJ] = {};
int CEnemyManager::m_nNumAll = 0;		// 総数
int CEnemyManager::m_nNumChara = 0;		// 敵の種類の総数
int CEnemyManager::m_nCntSpawn = 0;		// 出現カウント
bool CEnemyManager::m_bLoadPattern = false;	// パターン読み込み判定
bool CEnemyManager::m_bHitStop = false;	// ヒットストップの判定
CEnemyManager::STATE CEnemyManager::m_state = CEnemyManager::STATE_NONE;		// 状態
CEnemyManager::sRushWave *CEnemyManager::m_pRushWaveInfo = NULL;					// ラッシュのウェーブ情報
int CEnemyManager::m_nNumWave = 0;		// ラッシュウェーブの総数
int CEnemyManager::m_nNumRushEnemy = 0;	// ラッシュ中の敵の総数

//==========================================================================
// コンストラクタ
//==========================================================================
CEnemyManager::CEnemyManager()
{
	// 値のクリア
	m_state = STATE_NONE;	// 状態
	m_nNumRushEnemy = 0;	// ラッシュ中の敵の数
	m_nNowWave = 0;			// 現在のウェーブ

	// 総数リセット
	m_nNumAll = 0;
}

//==========================================================================
// デストラクタ
//==========================================================================
CEnemyManager::~CEnemyManager()
{

}

//==========================================================================
// 生成処理
//==========================================================================
CEnemyManager *CEnemyManager::Create(const std::string pTextFile)
{
	// 生成用のオブジェクト
	CEnemyManager *pModel = NULL;

	if (pModel == NULL)
	{// NULLだったら

		// メモリの確保
		pModel = DEBUG_NEW CEnemyManager;

		if (pModel != NULL)
		{// メモリの確保が出来ていたら

			// 初期化処理
			HRESULT hr = pModel->ReadText(pTextFile);
			if (FAILED(hr))
			{// 失敗していたら
				return NULL;
			}

			// 初期化処理
			hr = pModel->Init();

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
HRESULT CEnemyManager::Init(void)
{
	// 総数リセット
	m_nNumAll = 0;
	m_nCntSpawn = 0;

	return S_OK;
}

//==========================================================================
// 終了処理
//==========================================================================
void CEnemyManager::Uninit(void)
{
	for (int nCntEnemy = 0; nCntEnemy < mylib_const::MAX_OBJ; nCntEnemy++)
	{
		if (m_pEnemy[nCntEnemy] != NULL)
		{
			m_pEnemy[nCntEnemy] = NULL;
		}
	}

	if (m_pRushWaveInfo != NULL)
	{
		for (int nCntWave = 0; nCntWave < m_nNumWave; nCntWave++)
		{
			if (m_pRushWaveInfo[nCntWave].pRushInfo != NULL)
			{// NULLじゃなかったら
				delete[] m_pRushWaveInfo[nCntWave].pRushInfo;
				m_pRushWaveInfo[nCntWave].pRushInfo = NULL;
			}
		}
	}

	if (m_pRushWaveInfo != NULL)
	{
		delete[] m_pRushWaveInfo;
		m_pRushWaveInfo = NULL;
	}
}

//==========================================================================
// 破棄
//==========================================================================
void CEnemyManager::Release(int nIdx)
{
	if (m_pEnemy[nIdx] != NULL)
	{
		m_pEnemy[nIdx] = NULL;
	}

	if (CManager::GetInstance()->GetMode() == CScene::MODE_GAME && CGame::GetGameManager()->GetType() == CGameManager::SCENE_RUSH)
	{// 敵のラッシュ中

		// ラッシュ中の敵の数加算
		m_nNumRushEnemy--;
	}

	// 総数減算
	m_nNumAll--;
}

//==========================================================================
// 更新処理
//==========================================================================
void CEnemyManager::Update(void)
{

	//if (m_nNumAll <= 0)
	//{
	//	SetEnemy(D3DXVECTOR3(0.0f, 200.0f, 0.0f), D3DXVECTOR3(0.0f, 0.0f, 0.0f), 0);
	//	/*SetEnemy(D3DXVECTOR3(0.0f, 200.0f, 0.0f), D3DXVECTOR3(0.0f, 0.0f, 0.0f), 1);
	//	SetEnemy(D3DXVECTOR3(0.0f, 200.0f, 0.0f), D3DXVECTOR3(0.0f, 0.0f, 0.0f), 2);
	//	SetEnemy(D3DXVECTOR3(0.0f, 200.0f, 0.0f), D3DXVECTOR3(0.0f, 0.0f, 0.0f), 3);*/
	//	SetEnemy(D3DXVECTOR3(0.0f, 200.0f, 0.0f), D3DXVECTOR3(0.0f, 0.0f, 0.0f), 4);
	//}

	if (CGame::GetGameManager()->GetType() == CGameManager::SCENE_RUSH)
	{// 敵のラッシュ中
		if (m_nNumRushEnemy <= 0)
		{// 総数がゼロになったら

			if (m_nNumWave <= m_nNowWave)
			{// ウェーブの総数を超えたら
				// シーンの種類設定
				CGame::GetGameManager()->SetType(CGameManager::SCENE_MAIN);

				// ラッシュの終了判定有効
				CGame::GetGameManager()->SetEnableRush();
				return;
			}

			for (int nCntEnemy = 0; nCntEnemy < m_pRushWaveInfo[m_nNowWave].nWaveNumEnemy; nCntEnemy++)
			{
				int nAxisNum = CGame::GetEnemyBase()->GetAxisNum();
				int nBase = m_pRushWaveInfo[m_nNowWave].pRushInfo[nCntEnemy].nBase;

				CEnemyBase::sInfo info = CGame::GetEnemyBase()->GetChaseChangeInfo(nBase);
				int nType = m_pRushWaveInfo[m_nNowWave].pRushInfo[nCntEnemy].nPatternType;

				// 敵配置
				SetEnemy(CGame::GetEnemyBase()->GetAxis(nBase), info.nMapIdx, info.fMapMoveValue, nType);

				// ラッシュ中の敵の数加算
				m_nNumRushEnemy += m_aPattern[nType].nNumEnemy;
			}

			// ウェーブ加算
			m_nNowWave++;
		}
	}

	// テキストの描画
	CManager::GetInstance()->GetDebugProc()->Print(
		"---------------- 敵情報 ----------------\n"
		"【残り人数】[%d]\n", m_nNumAll);
}

//==========================================================================
// 敵配置
//==========================================================================
CEnemy **CEnemyManager::SetEnemy(D3DXVECTOR3 pos, D3DXVECTOR3 rot, int nPattern)
{
	int nNumSpawn = m_aPattern[nPattern].nNumEnemy;	// スポーンする数
	int nCntNULL = 0;
	int nCntStart = 0;
	Pattern NowPattern = m_aPattern[nPattern];
	CEnemy *pEnemy[mylib_const::MAX_PATTEN_ENEMY] = {};
	memset(&pEnemy[0], NULL, sizeof(pEnemy));

	for (int nCntEnemy = 0; nCntEnemy < nNumSpawn; nCntEnemy++)
	{
		for (nCntNULL = nCntStart; nCntNULL < mylib_const::MAX_OBJ; nCntNULL++, nCntStart++)
		{
			if (m_pEnemy[nCntNULL] != NULL)
			{// 情報が入ってたら
				continue;
			}

			int nType = NowPattern.EnemyData[nCntEnemy].nType;

			// 敵の生成
			m_pEnemy[nCntNULL] = CEnemy::Create(
				nCntNULL,						// インデックス番号
				sMotionFileName[nType].c_str(),	// ファイル名
				pos,							// 位置
				(CEnemy::TYPE)nType);			// 種類

			if (m_pEnemy[nCntNULL] == NULL)
			{// 失敗していたら

				delete m_pEnemy[nCntNULL];
				m_pEnemy[nCntNULL] = NULL;
				break;
			}

			pEnemy[nCntEnemy] = m_pEnemy[nCntNULL];

			// 向き設定
			m_pEnemy[nCntNULL]->SetRotation(rot);
			m_pEnemy[nCntNULL]->SetRotDest(rot.y);
			m_pEnemy[nCntNULL]->SetMapMoveValueOrigin(NowPattern.EnemyData[nCntEnemy].fStartMoveValue);

			// 敵の一定の動きマネージャポインタ取得
			CEnemyFixedMoveManager *pFixed = m_pEnemy[nCntNULL]->GetFixedManager();
			if (pFixed == NULL)
			{// 失敗していたら
				return &pEnemy[0];
			}
			pFixed->Set(NowPattern.nFixedType);
			pFixed->StartSet(NowPattern.EnemyData[nCntEnemy].nStartKey, NowPattern.EnemyData[nCntEnemy].nStartFrame);

			// 総数加算
			m_nNumAll++;
			break;
		}
	}

	return &pEnemy[0];
}

//==========================================================================
// 敵配置(オーバーロード)
//==========================================================================
CEnemy **CEnemyManager::SetEnemy(D3DXVECTOR3 pos, int nMapIndex, float fMapMoveValue, int nPattern)
{
	int nNumSpawn = m_aPattern[nPattern].nNumEnemy;	// スポーンする数
	int nCntNULL = 0;
	int nCntStart = 0;
	Pattern NowPattern = m_aPattern[nPattern];
	CEnemy *pEnemy[mylib_const::MAX_PATTEN_ENEMY] = {};
	memset(&pEnemy[0], NULL, sizeof(pEnemy));

	for (int nCntEnemy = 0; nCntEnemy < nNumSpawn; nCntEnemy++)
	{
		for (nCntNULL = nCntStart; nCntNULL < mylib_const::MAX_OBJ; nCntNULL++, nCntStart++)
		{
			if (m_pEnemy[nCntNULL] != NULL)
			{// 情報が入ってたら
				continue;
			}

			int nType = NowPattern.EnemyData[nCntEnemy].nType;

			// 敵の生成
			m_pEnemy[nCntNULL] = CEnemy::Create(
				nCntNULL,						// インデックス番号
				sMotionFileName[nType].c_str(),	// ファイル名
				pos,							// 位置
				(CEnemy::TYPE)nType);			// 種類

			if (m_pEnemy[nCntNULL] == NULL)
			{// 失敗していたら

				delete m_pEnemy[nCntNULL];
				m_pEnemy[nCntNULL] = NULL;
				break;
			}

			pEnemy[nCntEnemy] = m_pEnemy[nCntNULL];

			// 初期情報設定
			m_pEnemy[nCntNULL]->SetMapIndexOrigin(nMapIndex);
			m_pEnemy[nCntNULL]->SetMapIndex(nMapIndex);
			m_pEnemy[nCntNULL]->SetMapMoveValueOrigin(NowPattern.EnemyData[nCntEnemy].fStartMoveValue + fMapMoveValue);

			// 敵の一定の動きマネージャポインタ取得
			CEnemyFixedMoveManager *pFixed = m_pEnemy[nCntNULL]->GetFixedManager();
			if (pFixed == NULL)
			{// 失敗していたら
				return &pEnemy[0];
			}
			pFixed->Set(NowPattern.nFixedType);
			pFixed->StartSet(NowPattern.EnemyData[nCntEnemy].nStartKey, NowPattern.EnemyData[nCntEnemy].nStartFrame);

			// 総数加算
			m_nNumAll++;
			break;
		}
	}

	return &pEnemy[0];
}

//==========================================================================
// パターン数
//==========================================================================
int CEnemyManager::GetPatternNum(void)
{
	return m_nPatternNum;
}

//==========================================================================
// パターン取得
//==========================================================================
CEnemyManager::Pattern CEnemyManager::GetPattern(int nPattern)
{
	return m_aPattern[nPattern];
}

//==========================================================================
// 敵の総数取得
//==========================================================================
int CEnemyManager::GetNumAll(void)
{
	return m_nNumAll;
}

//==========================================================================
// テキスト読み込み処理
//==========================================================================
HRESULT CEnemyManager::ReadText(const std::string pTextFile)
{

	if (m_bLoadPattern == true)
	{// 既に読み込まれていたら
		return S_OK;
	}

	FILE *pFile = NULL;	// ファイルポインタを宣言

	// ファイルを開く
	pFile = fopen(pTextFile.c_str(), "r");

	if (pFile == NULL)
	{//ファイルが開けた場合
		return E_FAIL;
	}

	// 読み込み判定ON
	m_bLoadPattern = true;

	char aComment[MAX_COMMENT];	// コメント
	int nType = 0;				// 配置する種類
	int nCntPatten = 0;			// パターンのカウント
	int nCntFileName = 0;

	while (1)
	{// END_SCRIPTが来るまで繰り返す

		// 文字列の読み込み
		fscanf(pFile, "%s", &aComment[0]);

		// キャラクター数の設定
		if (strcmp(aComment, "NUM_CHARACTER") == 0)
		{// NUM_CHARACTERがきたら

			fscanf(pFile, "%s", &aComment[0]);	// =の分
			fscanf(pFile, "%d", &m_nNumChara);	// キャラクター数
		}

		while (nCntFileName != m_nNumChara)
		{// モデルの数分読み込むまで繰り返し

			// 文字列の読み込み
			fscanf(pFile, "%s", &aComment[0]);

			// モデル名の設定
			if (strcmp(aComment, "MOTION_FILENAME") == 0)
			{// NUM_MODELがきたら

				fscanf(pFile, "%s", &aComment[0]);	// =の分
				fscanf(pFile, "%s", &aComment[0]);	// ファイル名

				// ファイル名保存
				sMotionFileName[nCntFileName] = aComment;

				nCntFileName++;	// ファイル数加算
			}
		}

		// 各パターンの設定
		if (strcmp(aComment, "PATTERNSET") == 0)
		{// 配置情報の読み込みを開始

			int nCntEnemy = 0;			// 敵の配置カウント

			while (strcmp(aComment, "END_PATTERNSET") != 0)
			{// END_PATTERNSETが来るまで繰り返し

				fscanf(pFile, "%s", &aComment[0]);	//確認する

				if (strcmp(aComment, "FIXEDMOVE") == 0)
				{// FIXEDMOVEが来たら一定の動きの種類読み込み

					fscanf(pFile, "%s", &aComment[0]);	// =の分
					fscanf(pFile, "%d", &m_aPattern[nCntPatten].nFixedType);	// 一定の動きの種類
				}

				if (strcmp(aComment, "ENEMYSET") == 0)
				{// ENEMYSETで敵情報の読み込み開始

					while (strcmp(aComment, "END_ENEMYSET") != 0)
					{// END_ENEMYSETが来るまで繰り返す

						fscanf(pFile, "%s", &aComment[0]);	// 確認する

						if (strcmp(aComment, "TYPE") == 0)
						{// TYPEが来たらキャラファイル番号読み込み

							fscanf(pFile, "%s", &aComment[0]);	// =の分
							fscanf(pFile, "%d", &m_aPattern[nCntPatten].EnemyData[nCntEnemy].nType);	// キャラファイル番号
						}

						if (strcmp(aComment, "STARTKEY") == 0)
						{// STARTKEYが来たら初期キー読み込み

							fscanf(pFile, "%s", &aComment[0]);	// =の分
							fscanf(pFile, "%d", &m_aPattern[nCntPatten].EnemyData[nCntEnemy].nStartKey);	// 初期キー
						}

						if (strcmp(aComment, "STARTFRAME") == 0)
						{// STARTFRAMEが来たら初期フレーム読み込み

							fscanf(pFile, "%s", &aComment[0]);	// =の分
							fscanf(pFile, "%d", &m_aPattern[nCntPatten].EnemyData[nCntEnemy].nStartFrame);	// 初期フレーム
						}

						if (strcmp(aComment, "STARTMOVEVALUE") == 0)
						{// STARTMOVEVALUEが来たら初期マップ移動量読み込み

							fscanf(pFile, "%s", &aComment[0]);	// =の分
							fscanf(pFile, "%f", &m_aPattern[nCntPatten].EnemyData[nCntEnemy].fStartMoveValue);	// 初期マップ移動量
						}

					}// END_ENEMYSETのかっこ

					nCntEnemy++;	// 敵のカウントを加算
					m_aPattern[nCntPatten].nNumEnemy++;	// 敵のカウントを加算
				}
			}// END_PATTERNSETのかっこ

			nCntPatten++;	// パターン加算
		}

		if (strcmp(aComment, "END_SCRIPT") == 0)
		{// 終了文字でループを抜ける
			break;
		}
	}

	// パターン数
	m_nPatternNum = nCntPatten;

	// ファイルを閉じる
	fclose(pFile);

	// ラッシュ情報読み込み
	if (FAILED(ReadRushInfo()))
	{
		return E_FAIL;
	}

	return S_OK;
}

//==========================================================================
// 外部ファイル読み込み処理
//==========================================================================
HRESULT CEnemyManager::ReadRushInfo(void)
{
	FILE *pFile = NULL;	// ファイルポインタを宣言

	// ファイルを開く
	pFile = fopen("data\\TEXT\\set_enemy_info.txt", "r");

	if (pFile == NULL)
	{//ファイルが開けた場合
		return E_FAIL;
	}

	char aComment[MAX_COMMENT];	// コメント
	int nType = 0;				// 配置する種類
	int nCntWave = 0;			// パターンのカウント

	while (1)
	{// END_SCRIPTが来るまで繰り返す

		// 文字列の読み込み
		fscanf(pFile, "%s", &aComment[0]);

		// キャラクター数の設定
		if (strcmp(aComment, "NUM_WAVE") == 0)
		{// NUM_WAVEがきたら

			fscanf(pFile, "%s", &aComment[0]);	// =の分
			fscanf(pFile, "%d", &m_nNumWave);	// ウェーブ数読み込み

			if (m_pRushWaveInfo == NULL)
			{// NULLだったら

				// ウェーブ数分でメモリ確保
				m_pRushWaveInfo = DEBUG_NEW sRushWave[m_nNumWave];
				memset(&m_pRushWaveInfo[0], NULL, sizeof(sRushWave) * m_nNumWave);
				//m_pRushWaveInfo = NULL;
			}
		}

		// 各パターンの設定
		if (strcmp(aComment, "WAVESET") == 0)
		{// 配置情報の読み込みを開始

			int nCntEnemy = 0;			// 敵の配置カウント

			while (strcmp(aComment, "END_WAVESET") != 0)
			{// END_WAVESETが来るまで繰り返し

				fscanf(pFile, "%s", &aComment[0]);	//確認する

				// キャラクター数の設定
				if (strcmp(aComment, "NUM_ENEMY") == 0)
				{// NUM_WAVEがきたら


					fscanf(pFile, "%s", &aComment[0]);	// =の分
					fscanf(pFile, "%d", &m_pRushWaveInfo[nCntWave].nWaveNumEnemy);	// 敵の数読み込み

					if (m_pRushWaveInfo[nCntWave].pRushInfo == NULL)
					{// NULLだったら

						// 敵の数分でメモリ確保
						m_pRushWaveInfo[nCntWave].pRushInfo = DEBUG_NEW sRushInfo[m_pRushWaveInfo[nCntWave].nWaveNumEnemy];
						memset(&m_pRushWaveInfo[nCntWave].pRushInfo[0], NULL, sizeof(sRushInfo) * m_pRushWaveInfo[nCntWave].nWaveNumEnemy);
					}
				}

				if (strcmp(aComment, "ENEMYSET") == 0)
				{// ENEMYSETで敵情報の読み込み開始

					while (strcmp(aComment, "END_ENEMYSET") != 0)
					{// END_ENEMYSETが来るまで繰り返す

						fscanf(pFile, "%s", &aComment[0]);	// 確認する

						if (strcmp(aComment, "TYPE") == 0)
						{// TYPEが来たらキャラファイル番号読み込み

							fscanf(pFile, "%s", &aComment[0]);	// =の分
							fscanf(pFile, "%d", &m_pRushWaveInfo[nCntWave].pRushInfo[nCntEnemy].nPatternType);	// キャラファイル番号
						}

						if (strcmp(aComment, "BASE") == 0)
						{// BASEが来たら出現拠点読み込み

							fscanf(pFile, "%s", &aComment[0]);	// =の分
							fscanf(pFile, "%d", &m_pRushWaveInfo[nCntWave].pRushInfo[nCntEnemy].nBase);	// 出現拠点番号
						}

					}// END_ENEMYSETのかっこ

					 // 敵の数加算
					nCntEnemy++;
				}
			}// END_WAVESETのかっこ
			
			// ウェーブ数加算
			nCntWave++;
		}

		if (strcmp(aComment, "END_SCRIPT") == 0)
		{// 終了文字でループを抜ける
			break;
		}
	}

	// ファイルを閉じる
	fclose(pFile);

	return S_OK;
}

//==========================================================================
// 敵取得
//==========================================================================
CEnemy **CEnemyManager::GetEnemy(void)
{
	return &m_pEnemy[0];
}

//==========================================================================
// 敵のモーションファイル名取得
//==========================================================================
const char *CEnemyManager::GetMotionFilename(int nType)
{
	return &sMotionFileName[nType][0];
}
