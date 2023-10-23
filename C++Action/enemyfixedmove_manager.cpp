//=============================================================================
// 
//  敵の一定の動き処理 [enemyfixedmove_manager.cpp]
//  Author : 相馬靜雅
// 
//=============================================================================
#include "debugproc.h"
#include "enemyfixedmove_manager.h"
#include "calculation.h"
#include "manager.h"
#include "renderer.h"
#include "enemy.h"
#include "mapmanager.h"

//==========================================================================
// マクロ定義
//==========================================================================
#define SPAWN_STRENGTH	(100.0f)	// スポーン時の高さ
#define KILL_INT		(30)		// 倒したインターバル

//==========================================================================
// 静的メンバ変数宣言
//==========================================================================
CEnemyFixedMoveManager::sFixedMove CEnemyFixedMoveManager::m_aFxedMove[mylib_const::MAX_FIXEDMOVE] = {};	// 一定の動きの情報
int CEnemyFixedMoveManager::m_nNumAll = 0;				// モーションの総数

//==========================================================================
// コンストラクタ
//==========================================================================
CEnemyFixedMoveManager::CEnemyFixedMoveManager()
{
	// 値のクリア
	m_nType = 0;				// 現在のモーションの種類
	m_nPatternKey = 0;			// 何個目のキーか
	m_nCntFrame = 0;			// フレームのカウント
	m_nCntAllFrame = 0;			// 全てのカウント
	m_nMaxAllFrame = 0;			// 全てのカウントの最大値
}

//==========================================================================
// デストラクタ
//==========================================================================
CEnemyFixedMoveManager::~CEnemyFixedMoveManager()
{

}

//==========================================================================
// 生成処理
//==========================================================================
CEnemyFixedMoveManager *CEnemyFixedMoveManager::Create(void)
{
	// 生成用のオブジェクト
	CEnemyFixedMoveManager *pModel = NULL;

	if (pModel == NULL)
	{// NULLだったら

		// メモリの確保
		pModel = DEBUG_NEW CEnemyFixedMoveManager;

		if (pModel != NULL)
		{// メモリの確保が出来ていたら

			// 初期化処理
			HRESULT hr = pModel->Init();

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
HRESULT CEnemyFixedMoveManager::Init(void)
{
	return S_OK;
}

//==========================================================================
// スタート時の設定
//==========================================================================
void CEnemyFixedMoveManager::StartSet(int nKey, int nFrame)
{
	// 現在のキー変更
	m_nPatternKey = nKey;

	// フレームのカウント
	m_nCntFrame = nFrame;

	while (1)
	{
		if (m_nCntFrame >= m_aFxedMove[m_nType].info[m_nPatternKey].nFrame)
		{// フレームのカウントがフレーム数に達したら

			// フレームのカウントをゼロに戻す(?)
			m_nCntFrame -= m_aFxedMove[m_nType].info[m_nPatternKey].nFrame;

			// パターンNO.更新
			m_nPatternKey = (m_nPatternKey + 1) % m_aFxedMove[m_nType].nNumKey;

			if (m_nPatternKey == 0)
			{// キーがゼロの時

				// 総フレーム数リセット
				m_nCntAllFrame = 0;
			}
		}

		if (m_nCntFrame < m_aFxedMove[m_nType].info[m_nPatternKey].nFrame)
		{
			break;
		}
	}
}

//==========================================================================
// 終了処理
//==========================================================================
void CEnemyFixedMoveManager::Uninit(void)
{
	
}

//==========================================================================
// 更新処理
//==========================================================================
void CEnemyFixedMoveManager::Update(void)
{

	
}

//==========================================================================
// キーフレーム更新
//==========================================================================
void CEnemyFixedMoveManager::UpdateKeyFrame(void)
{
	// 再生フレーム保存
	int nFrame = m_aFxedMove[m_nType].info[m_nPatternKey].nFrame;

	// 次のキー
	int nNextKey = (m_nPatternKey + 1) % m_aFxedMove[m_nType].nNumKey;

	// フレームのカウントを加算
	m_nCntFrame++;
	m_nCntAllFrame++;

	if (m_nCntFrame >= nFrame)
	{// フレームのカウントがフレーム数に達したら

		// フレームのカウントをゼロに戻す
		m_nCntFrame = 0;

		// パターンNO.更新
		m_nPatternKey = (m_nPatternKey + 1) % m_aFxedMove[m_nType].nNumKey;

		if (m_nPatternKey == 0)
		{// キーがゼロの時

			// 総フレーム数リセット
			m_nCntAllFrame = 0;
		}
	}
}

//==========================================================================
// 位置更新
//==========================================================================
D3DXVECTOR3 CEnemyFixedMoveManager::UpdatePosition(CEnemy *pEnemy)
{
	// 位置取得
	D3DXVECTOR3 pos = pEnemy->GetPosition();

	// マップマネージャの取得
	CMapManager *pMapManager = CManager::GetInstance()->GetScene()->GetMapManager();
	if (pMapManager == NULL)
	{// NULLだったら
		return pos;
	}

	// キー更新
	UpdateKeyFrame();

	// 出現位置取得
	D3DXVECTOR3 spawnPos = pEnemy->GetSpawnPosition();
	
	// マップ情報取得
	int nIdx = pEnemy->GetMapIndex();
	float fPointRatio = pEnemy->GetMapPointRatio();
	float fMoveValue = pEnemy->GetMapMoveValue();
	float fThusFarMoveX = 0.0f;
	float fThusFarMoveY = 0.0f;

	// 今までの移動量
	for (int nCntKey = 0; nCntKey < m_nPatternKey + 1; nCntKey++)
	{
		int nFrame = m_aFxedMove[m_nType].info[nCntKey].nFrame;
		if (nCntKey == m_nPatternKey)
		{// 同じキーだったら現在のカウントまで
			nFrame = m_nCntFrame;
		}
		fThusFarMoveX += m_aFxedMove[m_nType].info[nCntKey].move.x * nFrame;
		fThusFarMoveY += m_aFxedMove[m_nType].info[nCntKey].move.y * nFrame;
	}
	// 位置更新
	int nOriginIdx = pEnemy->GetMapIndexOrigin();
	fMoveValue = pEnemy->GetMapMoveValueOrigin() + fThusFarMoveX;
	nIdx = nOriginIdx;
	pos = pMapManager->UpdateNowPosition(nIdx, fPointRatio, fMoveValue, pos.y);
	pos.y = spawnPos.y + fThusFarMoveY;

	// マップ情報設定
	pEnemy->SetMapIndex(nIdx);
	pEnemy->SetMapPointRatio(fPointRatio);
	pEnemy->SetMapMoveValue(fMoveValue);

	return pos;
}

//==========================================================================
// 設定処理
//==========================================================================
void CEnemyFixedMoveManager::Set(int nType)
{
	if (nType > m_nNumAll)
	{// 総数を超えていたら
		return;
	}

	m_nType = nType;		// 種類設定
	m_nPatternKey = 0;		// 何個目のキーか
	m_nCntFrame = 0;		// フレームのカウント
	m_nCntAllFrame = 0;		// 全てのカウント
	m_nMaxAllFrame = 0;		// 全てのカウントの最大値

	for (int nCntKey = 0; nCntKey < m_aFxedMove[m_nType].nNumKey; nCntKey++)
	{
		m_nMaxAllFrame += m_aFxedMove[m_nType].info[nCntKey].nFrame;	// 全てのカウントの最大値
	}
}

//==========================================================================
// テキスト読み込み処理
//==========================================================================
HRESULT CEnemyFixedMoveManager::ReadText(const std::string pTextFile)
{
	FILE *pFile = NULL;	// ファイルポインタを宣言

	// ファイルを開く
	pFile = fopen(pTextFile.c_str(), "r");

	if (pFile == NULL)
	{//ファイルが開けた場合
		return E_FAIL;
	}

	char aComment[MAX_COMMENT];	// コメント
	int nType = 0;				// 配置する種類

	while (1)
	{// END_SCRIPTが来るまで繰り返す

		// 文字列の読み込み
		fscanf(pFile, "%s", &aComment[0]);

		//==================================================
		// 各モーションの設定
		//==================================================
		if (strcmp(aComment, "FIXEDSET") == 0)
		{// モーション情報の読み込みを開始

			// 行動のカウンター
			int nCntAct = 0;

			while (strcmp(aComment, "END_FIXEDSET") != 0)
			{// END_FIXEDSETが来るまで繰り返し

				fscanf(pFile, "%s", &aComment[0]);	// 確認する

				if (strcmp(aComment, "NUM_KEY") == 0)
				{// NUM_KEYが来たらキー数読み込み

					fscanf(pFile, "%s", &aComment[0]);	// =の分
					fscanf(pFile, "%d", &m_aFxedMove[m_nNumAll].nNumKey);	// キー数
				}

				//==================================================
				// 各キーの設定
				//==================================================
				if (strcmp(aComment, "ACTSET") == 0)
				{// ACTSETでキー情報の読み込み開始

					while (strcmp(aComment, "END_ACTSET") != 0)
					{// END_ACTSETが来るまで繰り返し

						fscanf(pFile, "%s", &aComment[0]);	// 確認する

						if (strcmp(aComment, "FRAME") == 0)
						{// FRAMEが来たら再生フレーム数読み込み

							fscanf(pFile, "%s", &aComment[0]);		// =の分
							fscanf(pFile, "%d", &m_aFxedMove[m_nNumAll].info[nCntAct].nFrame);	// 再生フレーム
						}

						if (strcmp(aComment, "MOVE") == 0)
						{// MOVEが来たら移動量読み込み

							fscanf(pFile, "%s", &aComment[0]);		// =の分
							fscanf(pFile, "%f", &m_aFxedMove[m_nNumAll].info[nCntAct].move.x);	// X移動量
							fscanf(pFile, "%f", &m_aFxedMove[m_nNumAll].info[nCntAct].move.y);	// Y移動量
							fscanf(pFile, "%f", &m_aFxedMove[m_nNumAll].info[nCntAct].move.z);	// Z移動量
						}

					}// END_ACTSETのかっこ

					nCntAct++;	// 行動のカウント加算
				}

			}// END_FIXEDSETのかっこ

			// データ数加算
			m_nNumAll++;
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
