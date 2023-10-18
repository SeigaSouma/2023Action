//=============================================================================
// 
//  モデル処理 [motion.cpp]
//  Author : 相馬靜雅
// 
//=============================================================================
#include "motion.h"
#include "manager.h"
#include "renderer.h"
#include "calculation.h"
#include "model.h"
#include "objectChara.h"

//==========================================================================
// マクロ定義
//==========================================================================

//==========================================================================
// 静的メンバ変数宣言
//==========================================================================
std::vector<std::string> CMotion::m_sTextFile = {};		// テキストファイル名
std::vector<CMotion::Info> CMotion::m_aLoadData[MAX_MOTION] = {};	// モーションの情報
std::vector<CMotion::AttackInfo> CMotion::m_aLoadAttackData[MAX_MOTION][MAX_PARTS] = {};	// 攻撃の読み込み情報
int CMotion::m_nNumLoad = 0;								// 読み込んだ数
std::vector<int> CMotion::m_nNumLoadData = {};				// モーション毎のデータ数

//==========================================================================
// コンストラクタ
//==========================================================================
CMotion::CMotion()
{
	// 値のクリア
	m_nNumAll = 0;			// モーションの総数
	m_nType = 0;			// 現在のモーションの種類
	m_nOldType = 0;			// 前回のモーションの種類
	m_bLoop = false;		// ループするかどうか
	m_nPatternKey = 0;		// 何個目のキーか
	m_nCntFrame = 0;		// フレームのカウント
	m_nCntAllFrame = 0;		// 全てのカウント
	m_nMaxAllFrame = 0;		// 全てのカウントの最大値
	m_fSlowFactor = 0.0f;	// 遅延係数
	m_bFinish = false;		// 終了したかどうか
	m_pObjChara = NULL;		// オブジェクトのポインタ
	m_ppModel = NULL;		// モデルのポインタ
	m_nNumModel = 0;		// モデルの総数
	m_nNumMotion = 0;		// モーションの総数

	for (int nCntMotion = 0; nCntMotion < MAX_MOTION; nCntMotion++)
	{
		m_aInfo[nCntMotion].nNumKey = 0;			// キーの数
		m_aInfo[nCntMotion].nLoop = 0;				// ループ判定
		m_aInfo[nCntMotion].nMove = 0;				// 移動判定
		m_aInfo[nCntMotion].nNumAttackInfo = 0;		// 攻撃情報の数

		for (int nCntKey = 0; nCntKey < MAX_KEY; nCntKey++)
		{
			m_aInfo[nCntMotion].aKey[nCntKey].nFrame = 0;

			for (int nCntParts = 0; nCntParts < MAX_PARTS; nCntParts++)
			{
				m_aInfo[nCntMotion].aKey[nCntKey].aParts[nCntParts].rot = D3DXVECTOR3(0.0f, 0.0f, 0.0f);		// 向き
				m_aInfo[nCntMotion].aKey[nCntKey].aParts[nCntParts].rotDest = D3DXVECTOR3(0.0f, 0.0f, 0.0f);	// 目標の向き
				m_aInfo[nCntMotion].aKey[nCntKey].aParts[nCntParts].pos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);		// 位置
				m_aInfo[nCntMotion].aKey[nCntKey].aParts[nCntParts].posDest = D3DXVECTOR3(0.0f, 0.0f, 0.0f);	// 目標の位置
				m_aInfo[nCntMotion].aKey[nCntKey].aParts[nCntParts].posOrigin = D3DXVECTOR3(0.0f, 0.0f, 0.0f);	// 位置の原点
			}
		}

		for (int nCntParts = 0; nCntParts < MAX_PARTS; nCntParts++)
		{
			m_aInfo[nCntMotion].AttackInfo[nCntParts] = NULL;	// 攻撃情報
		}
	}

	for (int nCntParts = 0; nCntParts < MAX_PARTS; nCntParts++)
	{
		// 過去の情報
		aPartsOld[nCntParts].rot = D3DXVECTOR3(0.0f, 0.0f, 0.0f);		// 向き
		aPartsOld[nCntParts].rotDest = D3DXVECTOR3(0.0f, 0.0f, 0.0f);	// 目標の向き
		aPartsOld[nCntParts].pos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);		// 位置
		aPartsOld[nCntParts].posDest = D3DXVECTOR3(0.0f, 0.0f, 0.0f);	// 目標の位置
		aPartsOld[nCntParts].posOrigin = D3DXVECTOR3(0.0f, 0.0f, 0.0f);	// 位置の原点
	}

	m_nNumAll = 0;			// モーションの総数
	m_nType = 0;			// 現在のモーションの種類
	m_nOldType = 0;			// 前回のモーションの種類
	m_bLoop = false;		// ループするかどうか
	m_nPatternKey = 0;		// 何個目のキーか
	m_nCntFrame = 0;		// フレームのカウント
	m_nCntAllFrame = 0;		// 全てのカウント
	m_nMaxAllFrame = 0;		// 全てのカウントの最大値
	m_fSlowFactor = 0.0f;	// 遅延係数
	m_bFinish = false;		// 終了したかどうか
	m_pObjChara = NULL;		// オブジェクトのポインタ
	m_ppModel = NULL;		// モデルのポインタ
	m_nNumModel = 0;		// モデルの総数
	m_nNumMotion = 0;		// モーションの総数

}

//==========================================================================
// デストラクタ
//==========================================================================
CMotion::~CMotion()
{

}


//==========================================================================
// 生成処理
//==========================================================================
CMotion *CMotion::Create(void)
{
	// 生成用のオブジェクト
	CMotion *pMotion = NULL;

	if (pMotion == NULL)
	{// NULLだったら

		// メモリの確保
		pMotion = DEBUG_NEW CMotion;

		if (pMotion != NULL)
		{// メモリの確保が出来ていたら

			// 初期化処理
			pMotion->Init();
		}

		return pMotion;
	}

	return NULL;
}


//==========================================================================
// 生成処理
//==========================================================================
CMotion *CMotion::Create(const std::string pTextFile)
{
	// 生成用のオブジェクト
	CMotion *pMotion = NULL;

	if (pMotion == NULL)
	{// NULLだったら

		// メモリの確保
		pMotion = DEBUG_NEW CMotion;

		if (pMotion != NULL)
		{// メモリの確保が出来ていたら

			// 初期化処理
			pMotion->Init();
			pMotion->ReadText(pTextFile);
		}

		return pMotion;
	}

	return NULL;
}

//==========================================================================
// 初期化処理
//==========================================================================
HRESULT CMotion::Init(void)
{
	m_nNumAll = 0;				// モーションの総数
	m_nType = 0;				// 現在のモーションの種類
	m_bLoop = false;			// ループするかどうか
	m_nPatternKey = 0;			// 何個目のキーか
	m_nCntFrame = 0;			// フレームのカウント
	m_nCntAllFrame = 0;			// 全てのカウント
	m_bFinish = true;			// 終了したかどうか
	m_ppModel = NULL;			// モデルのポインタ
	m_nNumModel = 0;			// モデルの総数
	m_fSlowFactor = 1.0f;	// 遅延係数

	return S_OK;
}

//==========================================================================
// 終了処理
//==========================================================================
void CMotion::Uninit(void)
{
	// 攻撃情報の破棄
	for (int nCntMotion = 0; nCntMotion < MAX_MOTION; nCntMotion++)
	{
		for (int nCntAttack = 0; nCntAttack < MAX_PARTS; nCntAttack++)
		{
			if (m_aInfo[nCntMotion].AttackInfo[nCntAttack] != NULL)
			{
				delete m_aInfo[nCntMotion].AttackInfo[nCntAttack];
				m_aInfo[nCntMotion].AttackInfo[nCntAttack] = NULL;
			}
		}
	}
}

//==========================================================================
// モーション情報の登録
//==========================================================================
void CMotion::SetInfo(Info info)
{
	// 情報を渡す
	//m_aInfo[m_nNumAll] = info;\

	// コピー
	memcpy(m_aInfo[m_nNumAll].aKey, info.aKey, sizeof(info.aKey));

	m_aInfo[m_nNumAll].nLoop = info.nLoop;
	m_aInfo[m_nNumAll].nMove = info.nMove;
	m_aInfo[m_nNumAll].nNumKey = info.nNumKey;

	for (int nCntKey = 0; nCntKey < m_aInfo[m_nNumAll].nNumKey; nCntKey++)
	{
		int nBeforeCnt = nCntKey - 1;
		if (nBeforeCnt <= 0)
		{
			nBeforeCnt = 0;
		}

		float PosX = m_aInfo[m_nNumAll].aKey[nBeforeCnt].aParts[0].pos.x;
		float PosZ = m_aInfo[m_nNumAll].aKey[nBeforeCnt].aParts[0].pos.z;

		// 向きを求める
		m_aInfo[m_nNumAll].aKey[nCntKey].fRotMove = atan2f(
			(PosX - m_aInfo[m_nNumAll].aKey[nCntKey].aParts[0].pos.x),
			(PosZ - m_aInfo[m_nNumAll].aKey[nCntKey].aParts[0].pos.z));
	}

	// モーションの総数加算
	m_nNumAll++;
}

//==========================================================================
// 攻撃情報の登録
//==========================================================================
void CMotion::SetAttackInfo(AttackInfo info)
{
	//// メモリ確保
	m_aInfo[m_nNumAll].AttackInfo[m_aInfo[m_nNumAll].nNumAttackInfo] = DEBUG_NEW AttackInfo;

	if (m_aInfo[m_nNumAll].AttackInfo[m_aInfo[m_nNumAll].nNumAttackInfo] != NULL)
	{// NULLじゃなければ

		// 情報渡す
		*m_aInfo[m_nNumAll].AttackInfo[m_aInfo[m_nNumAll].nNumAttackInfo] = info;

		// 攻撃情報の総数加算
		m_aInfo[m_nNumAll].nNumAttackInfo++;
	}
}

//==========================================================================
// モーションをするモデルの登録
//==========================================================================
void CMotion::SetModel(CModel **pModel, int nNumModel, CObjectChara *pObjChara)
{
	// モデルのポインタを渡す
	m_ppModel = pModel;

	// モデルの総数
	m_nNumModel = nNumModel;

	// オブジェクトのポインタを渡す
	m_pObjChara = pObjChara;
}

//==========================================================================
// ポーズのリセット
//==========================================================================
void CMotion::ResetPose(int nType)
{
	for (int nCntParts = 0; nCntParts < m_nNumModel; nCntParts++)
	{// 全パーツ分繰り返す

		if (m_ppModel[nCntParts] == NULL)
		{// NULLだったら
			continue;
		}

		// 向き設定
		m_ppModel[nCntParts]->SetRotation(m_aInfo[nType].aKey[0].aParts[nCntParts].rot);
		aPartsOld[nCntParts].rot = m_aInfo[nType].aKey[0].aParts[nCntParts].rot;
	}
	aPartsOld[0].pos = m_pObjChara->GetOriginPosition() + m_aInfo[nType].aKey[0].aParts[0].pos;
}

//==========================================================================
// 更新処理
//==========================================================================
void CMotion::Update(void)
{
	if (m_bFinish == true && m_aInfo[m_nType].nLoop == LOOP_OFF)
	{// 終了してた && ループOFFだったら
		return;
	}

	// 再生フレーム
	int nFrame;

	// 再生フレーム保存
	nFrame = m_aInfo[m_nType].aKey[m_nPatternKey].nFrame;

	if (m_nType == 0 && m_nOldType != 0 && m_nPatternKey == 0)
	{// ニュートラルで0個の時
		nFrame = 10;
	}

	// 次のキー
	int nNextKey = (m_nPatternKey + 1) % m_aInfo[m_nType].nNumKey;

	if (nNextKey == 0 && m_aInfo[m_nType].nLoop == LOOP_OFF)
	{// ループしないとき

		// 最後で固定
		nNextKey = m_aInfo[m_nType].nNumKey - 1;
	}

	for (int nCntParts = 0; nCntParts < m_nNumModel; nCntParts++)
	{// 全パーツ分繰り返す

		if (m_ppModel[nCntParts] == NULL)
		{// NULLだったら

			continue;
		}

		// 次と今の向きの差分取得
		float rotDiffX = m_aInfo[m_nType].aKey[nNextKey].aParts[nCntParts].rot.x -
			aPartsOld[nCntParts].rot.x;

		float rotDiffY = m_aInfo[m_nType].aKey[nNextKey].aParts[nCntParts].rot.y -
			aPartsOld[nCntParts].rot.y;

		float rotDiffZ = m_aInfo[m_nType].aKey[nNextKey].aParts[nCntParts].rot.z -
			aPartsOld[nCntParts].rot.z;

#if 1
		// 角度の正規化
		RotNormalize(rotDiffX);
		RotNormalize(rotDiffY);
		RotNormalize(rotDiffZ);
#endif

		// パーツの向きを設定
		D3DXVECTOR3 rot = D3DXVECTOR3(0.0f, 0.0f, 0.0f);

		// パーツの向きを設定
		rot.x =
			aPartsOld[nCntParts].rot.x +
			rotDiffX *
			(
			(float)m_nCntFrame /
				(float)nFrame
				);

		// パーツの向きを設定
		rot.y =
			aPartsOld[nCntParts].rot.y +
			rotDiffY *
			(
			(float)m_nCntFrame /
				(float)nFrame
				);

		// パーツの向きを設定
		rot.z =
			aPartsOld[nCntParts].rot.z +
			rotDiffZ *
			(
			(float)m_nCntFrame /
				(float)nFrame
				);

		// 角度の正規化
		RotNormalize(rot.x);
		RotNormalize(rot.y);
		RotNormalize(rot.z);

		// 向き設定
		m_ppModel[nCntParts]->SetRotation(rot);

		// パーツの位置を設定
		if (nCntParts == 0)
		{
			// 本体の位置取得
			D3DXVECTOR3 pos = m_pObjChara->GetPosition();

			// 本体の向き取得
			D3DXVECTOR3 rot = m_pObjChara->GetRotation();

			// 元の位置取得
			D3DXVECTOR3 posOrigin = m_pObjChara->GetOriginPosition();

			// パーツの位置取得
			D3DXVECTOR3 posParts = m_ppModel[nCntParts]->GetPosition();
			D3DXVECTOR3 posPartsOld = m_ppModel[nCntParts]->GetPosition();

			// 目標の位置との差分を求める
			float posDiffX = m_aInfo[m_nType].aKey[nNextKey].aParts[nCntParts].pos.x + posOrigin.x -
				aPartsOld[nCntParts].pos.x;

			float posDiffY = m_aInfo[m_nType].aKey[nNextKey].aParts[nCntParts].pos.y + posOrigin.y -
				aPartsOld[nCntParts].pos.y;

			float posDiffZ = m_aInfo[m_nType].aKey[nNextKey].aParts[nCntParts].pos.z + posOrigin.z -
				aPartsOld[nCntParts].pos.z;

			// 親のYを補正
			posParts.y =
				aPartsOld[nCntParts].pos.y +
				posDiffY *
				(
				(float)m_nCntFrame /
					(float)nFrame);

			// 位置設定
			m_ppModel[nCntParts]->SetPosition(posParts);

			// 動いた長さを求める
			float fMoveDiff =
				sqrtf((m_aInfo[m_nType].aKey[nNextKey].aParts[nCntParts].pos.x - aPartsOld[nCntParts].pos.x) * (m_aInfo[m_nType].aKey[nNextKey].aParts[nCntParts].pos.x - aPartsOld[nCntParts].pos.x)
					+ (m_aInfo[m_nType].aKey[nNextKey].aParts[nCntParts].pos.z - aPartsOld[nCntParts].pos.z) * (m_aInfo[m_nType].aKey[nNextKey].aParts[nCntParts].pos.z - aPartsOld[nCntParts].pos.z));
			fMoveDiff /= (float)nFrame;

			// 動きの向きを一時代入
			float fRot = m_aInfo[m_nType].aKey[nNextKey].fRotMove;

			// 動きの向き方向へ移動
			pos.x += sinf(D3DX_PI + fRot + rot.y) * fMoveDiff;
			pos.z += cosf(D3DX_PI + fRot + rot.y) * fMoveDiff;

			m_pObjChara->SetPosition(pos);
		}
	}

	// フレームのカウントを加算
	m_nCntFrame++;
	m_nCntAllFrame++;

	if (m_aInfo[m_nType].nLoop == LOOP_ON)
	{// ループモーションはいつでも終わってる

		// 終了判定ON
		m_bFinish = true;
	}

	if (m_nCntFrame >= nFrame)
	{// フレームのカウントがフレーム数に達したら

		// フレームのカウントをゼロに戻す
		m_nCntFrame = 0;

		m_nOldType = m_nType;	// 前回のモーションの種類

		// パターンNO.更新
		m_nPatternKey = (m_nPatternKey + 1) % m_aInfo[m_nType].nNumKey;

		for (int nCntParts = 0; nCntParts < m_nNumModel; nCntParts++)
		{// 全パーツ分繰り返す
			aPartsOld[nCntParts].rot = m_aInfo[m_nType].aKey[m_nPatternKey].aParts[nCntParts].rot;
			aPartsOld[nCntParts].pos = m_aInfo[m_nType].aKey[m_nPatternKey].aParts[nCntParts].pos + m_pObjChara->GetOriginPosition();
		}

		if (m_nPatternKey == 0)
		{// キーがゼロの時

			// 総フレーム数リセット
			m_nCntAllFrame = 0;

			//モーションの設定
			if (m_aInfo[m_nType].nLoop == LOOP_OFF)
			{// ループモーションじゃない時

				// 終了判定ON
				m_bFinish = true;
			}
			else
			{// ループモーションの時

				// 現在と同じモーションを設定
				//Set(m_nType);
			}
		}
	}
}

//==========================================================================
// モーションの設定処理
//==========================================================================
void CMotion::Set(int nType, bool bBlend)
{
	if (nType == 15)
	{
		int n = 0;
	}

	if (nType > m_nNumAll)
	{// 総数を超えていたら
		return;
	}

	if (m_nType != nType)
	{// 現在の種類と同じじゃなかったら

		m_nOldType = m_nType;	// 前回のモーションの種類
		m_nType = nType;		// 種類設定
		m_nPatternKey = 0;		// 何個目のキーか
		m_nCntFrame = 0;		// フレームのカウント
		m_nCntAllFrame = 0;		// 全てのカウント
		m_bFinish = false;		// 終了したかどうか

		m_nMaxAllFrame = 0;
		for (int nCntKey = 0; nCntKey < m_aInfo[m_nPatternKey].nNumKey; nCntKey++)
		{
			m_nMaxAllFrame += m_aInfo[m_nPatternKey].aKey[nCntKey].nFrame;	// 全てのカウントの最大値
		}

		for (int nCntParts = 0; nCntParts < m_nNumModel; nCntParts++)
		{// 全パーツ分繰り返す

			if (m_ppModel[nCntParts] == NULL)
			{// NULLだったら
				continue;
			}

			// 過去の位置・向きを保存
			if (bBlend == true)
			{
				aPartsOld[nCntParts].rot = m_ppModel[nCntParts]->GetRotation();
				aPartsOld[nCntParts].pos = m_ppModel[nCntParts]->GetPosition();
			}
			else
			{
				aPartsOld[nCntParts].rot = m_aInfo[m_nType].aKey[0].aParts[nCntParts].rot;
				aPartsOld[nCntParts].pos = m_aInfo[m_nType].aKey[0].aParts[nCntParts].pos + m_pObjChara->GetOriginPosition();
			}
		}
	}
	else
	{
		int n = 0;
	}
}

//==========================================================================
// 現在のモーションタイプ取得
//==========================================================================
int CMotion::GetType(void)
{
	return m_nType;
}

//==========================================================================
// 前回のモーションタイプ取得
//==========================================================================
int CMotion::GetOldType(void)
{
	return m_nOldType;
}

//==========================================================================
// 終了しているかの判定
//==========================================================================
bool CMotion::IsFinish(void)
{
	return m_bFinish;
}

//==========================================================================
// 終了しているかの判定切り替え
//==========================================================================
void CMotion::ToggleFinish(bool bFinish)
{
	m_bFinish = bFinish;
}

//==========================================================================
// 遅延係数の設定
//==========================================================================
void CMotion::SetSlowFactor(float fFactor)
{
	m_fSlowFactor = fFactor;
}

//==========================================================================
// 移動の判定取得
//==========================================================================
int CMotion::IsGetMove(int nType)
{
	return m_aInfo[nType].nMove;
}

//==========================================================================
// モーション情報の取得
//==========================================================================
CMotion::Info CMotion::GetInfo(int nType)
{
	return m_aInfo[nType];
}

//==========================================================================
// モーション情報の登録
//==========================================================================
void CMotion::SetInfoSave(int nType, int nKey, int nParts, D3DXVECTOR3 rot)
{
	m_aInfo[nType].aKey[nKey].aParts[nParts].rot = rot;
}

//==========================================================================
// キー総数変更
//==========================================================================
void CMotion::ChangeKeyNum(int nType, int nNum)
{
	m_aInfo[nType].nNumKey = nNum;
}

//==========================================================================
// キーフレーム変更
//==========================================================================
void CMotion::ChangeKeyFrame(int nType, int nKey, int nNum)
{
	m_aInfo[nType].aKey[nKey].nFrame = nNum;
}

//==========================================================================
// 攻撃情報変更
//==========================================================================
void CMotion::ChangeAttackInfo(int nType, int nIdx, AttackInfo info)
{
	*m_aInfo[nType].AttackInfo[nIdx] = info;
}

//==========================================================================
// 攻撃情報の総数加算
//==========================================================================
void CMotion::AddNumAttackInfo(int nType)
{
	if (m_aInfo[nType].nNumAttackInfo >= MAX_PARTS)
	{
		return;
	}

	// メモリ確保
	m_aInfo[nType].AttackInfo[m_aInfo[nType].nNumAttackInfo] = DEBUG_NEW AttackInfo;

	if (m_aInfo[nType].AttackInfo[m_aInfo[nType].nNumAttackInfo] != NULL)
	{// NULLじゃなければ

		// 初期化
		memset(m_aInfo[nType].AttackInfo[m_aInfo[nType].nNumAttackInfo], 0, sizeof(AttackInfo));

		// 攻撃情報の総数加算
		m_aInfo[nType].nNumAttackInfo++;
	}
}

//==========================================================================
// 攻撃情報の総数減算
//==========================================================================
void CMotion::SubNumAttackInfo(int nType)
{
	if (m_aInfo[nType].nNumAttackInfo <= 1)
	{
		return;
	}
	int nIdx = m_aInfo[nType].nNumAttackInfo - 1;

	// メモリ解放
	//if (m_aInfo[nType].AttackInfo[nIdx] != NULL)
	{
		delete m_aInfo[nType].AttackInfo[nIdx];
		m_aInfo[nType].AttackInfo[nIdx] = NULL;

		// 攻撃情報の総数減算
		m_aInfo[nType].nNumAttackInfo--;
	}
}

//==========================================================================
// カウント取得
//==========================================================================
int CMotion::GetAllCount(void)
{
	return m_nCntAllFrame;
}

//==========================================================================
// 全てのカウント取得
//==========================================================================
int CMotion::GetMaxAllCount(int nType)
{
	int nAllFrame = 0;
	for (int nCntKey = 0; nCntKey < m_aInfo[nType].nNumKey; nCntKey++)
	{
		nAllFrame += m_aInfo[nType].aKey[nCntKey].nFrame;	// 全てのカウントの最大値
	}
	return nAllFrame;
}

//==========================================================================
// 外部ファイル読み込み
//==========================================================================
void CMotion::ReadText(const std::string pTextFile)
{
	FILE *pFile = NULL;	// ファイルポインタを宣言

	char aComment[MAX_COMMENT];	// コメント用
	int nCntMotion = 0, nCntKey = 0, nCntParts = 0;
	Info InitInfo = {};
	AttackInfo InitAttackInfo = {};

	// 読み込み確認
	for (int nCntData = 0; nCntData < m_nNumLoad; nCntData++)
	{
		if (m_sTextFile[nCntData] != pTextFile)
		{// ファイル名が一致してない
			continue;
		}

		for (int nCntInfo = 0; nCntInfo < m_nNumLoadData[nCntData]; nCntInfo++)
		{
			for (int nCntATK = 0; nCntATK < m_aLoadData[nCntData][nCntInfo].nNumAttackInfo; nCntATK++)
			{// 攻撃情報分繰り返す

				// 攻撃情報登録
				SetAttackInfo(m_aLoadAttackData[nCntData][nCntInfo][nCntATK]);
			}

			// 情報登録
			SetInfo(m_aLoadData[nCntData][nCntInfo]);
		}
		return;
	}

	// ファイル名保存
	m_sTextFile.push_back(pTextFile);

	// ファイルを開く
	pFile = fopen(pTextFile.c_str(), "r");

	if (pFile == NULL)
	{//ファイルが開けなかった場合
		return;
	}

	// 読み込みのデータ数加算
	m_nNumLoadData.push_back(0);

	while (1)
	{// END_SCRIPTが来るまで繰り返す

		// 文字列の読み込み
		fscanf(pFile, "%s", &aComment[0]);

		//==================================================
		// 各モーションの設定
		//==================================================
		if (strcmp(aComment, "MOTIONSET") == 0)
		{// モーション情報の読み込みを開始

			// 初期化
			m_aLoadData[m_nNumLoad].push_back(InitInfo);

			while (strcmp(aComment, "END_MOTIONSET") != 0)
			{// END_MOTIONSETが来るまで繰り返し

				fscanf(pFile, "%s", &aComment[0]);	// 確認する

				if (strcmp(aComment, "LOOP") == 0)
				{// LOOPが来たらループON/OFF読み込み

					fscanf(pFile, "%s", &aComment[0]);	// =の分
					fscanf(pFile, "%d", &m_aLoadData[m_nNumLoad][nCntMotion].nLoop);	// ループ0か1
				}

				if (strcmp(aComment, "NUM_KEY") == 0)
				{// NUM_KEYが来たらキー数読み込み

					fscanf(pFile, "%s", &aComment[0]);	// =の分
					fscanf(pFile, "%d", &m_aLoadData[m_nNumLoad][nCntMotion].nNumKey);	// キー数
				}

				if (strcmp(aComment, "MOVE") == 0)
				{// MOVEが来たら移動ON/OFF読み込み

					fscanf(pFile, "%s", &aComment[0]);	// =の分
					fscanf(pFile, "%d", &m_aLoadData[m_nNumLoad][nCntMotion].nMove);	// 移動0か1か
				}

				if (strcmp(aComment, "ATTACKINFO") == 0)
				{// ATTACKINFOが来たら

					// 初期化
					m_aLoadAttackData[m_nNumLoad][nCntMotion].push_back(InitAttackInfo);

					int nNowAttackNum = m_aLoadData[m_nNumLoad][nCntMotion].nNumAttackInfo;

					while (strcmp(aComment, "END_ATTACKINFO") != 0)
					{// END_ATTACKINFOが来るまで繰り返し

						fscanf(pFile, "%s", &aComment[0]);	// 確認する

						if (strcmp(aComment, "COLLISIONPARTS") == 0)
						{// COLLISIONPARTSが来たらパーツ番号読み込み

							fscanf(pFile, "%s", &aComment[0]);		// =の分
							fscanf(pFile, "%d", &m_aLoadAttackData[m_nNumLoad][nCntMotion][nNowAttackNum].nCollisionNum);	// 判定を取るパーツ番号
						}

						if (strcmp(aComment, "ATTACKRANGE") == 0)
						{// ATTACKRANGEが来たら攻撃サイズ読み込み

							fscanf(pFile, "%s", &aComment[0]);		// =の分
							fscanf(pFile, "%f", &m_aLoadAttackData[m_nNumLoad][nCntMotion][nNowAttackNum].fRangeSize);	// 判定のサイズ
						}

						if (strcmp(aComment, "OFFSET") == 0)
						{// OFFSETが来たらオフセット読み込み

							fscanf(pFile, "%s", &aComment[0]);		// =の分
							fscanf(pFile, "%f", &m_aLoadAttackData[m_nNumLoad][nCntMotion][nNowAttackNum].Offset.x);	// X座標
							fscanf(pFile, "%f", &m_aLoadAttackData[m_nNumLoad][nCntMotion][nNowAttackNum].Offset.y);	// Y座標
							fscanf(pFile, "%f", &m_aLoadAttackData[m_nNumLoad][nCntMotion][nNowAttackNum].Offset.z);	// Z座標
						}

						if (strcmp(aComment, "ATTACKCOUNT") == 0)
						{// ATTACKCOUNTが来たら攻撃カウント読み込み

							fscanf(pFile, "%s", &aComment[0]);		// =の分
							fscanf(pFile, "%d", &m_aLoadAttackData[m_nNumLoad][nCntMotion][nNowAttackNum].nMinCnt);	// 判定の最小カウント
							fscanf(pFile, "%d", &m_aLoadAttackData[m_nNumLoad][nCntMotion][nNowAttackNum].nMaxCnt);	// 判定の最大カウント
						}

						if (strcmp(aComment, "INPACTCOUNT") == 0)
						{// INPACTCOUNTが来たら衝撃カウント読み込み

							fscanf(pFile, "%s", &aComment[0]);		// =の分
							fscanf(pFile, "%d", &m_aLoadAttackData[m_nNumLoad][nCntMotion][nNowAttackNum].nInpactCnt);	// 衝撃のカウント
						}

						if (strcmp(aComment, "DAMAGE") == 0)
						{// DAMAGEが来たら攻撃力読み込み

							fscanf(pFile, "%s", &aComment[0]);		// =の分
							fscanf(pFile, "%d", &m_aLoadAttackData[m_nNumLoad][nCntMotion][nNowAttackNum].nDamage);	// 攻撃力
						}
					}

					// 攻撃情報の登録
					SetAttackInfo(m_aLoadAttackData[m_nNumLoad][nCntMotion][nNowAttackNum]);

					// 攻撃情報の数加算
					m_aLoadData[m_nNumLoad][nCntMotion].nNumAttackInfo++;
				}

				//==================================================
				//各キーの設定
				//==================================================
				if (strcmp(aComment, "KEYSET") == 0)
				{// KEYSETでキー情報の読み込み開始

					while (strcmp(aComment, "END_KEYSET") != 0)
					{// END_KEYSETが来るまで繰り返し

						fscanf(pFile, "%s", &aComment[0]);	// 確認する

						if (strcmp(aComment, "FRAME") == 0)
						{// FRAMEが来たら再生フレーム数読み込み

							fscanf(pFile, "%s", &aComment[0]);		// =の分
							fscanf(pFile, "%d", &m_aLoadData[m_nNumLoad][nCntMotion].aKey[nCntKey].nFrame);	// 再生フレーム
						}

						//==================================================
						// パーツの設定
						//==================================================
						if (strcmp(aComment, "PARTS") == 0)
						{// PARTSでパーツ情報の読み込み開始

							while (strcmp(aComment, "END_PARTS") != 0)
							{// END_PARTSが来るまで繰り返し

								fscanf(pFile, "%s", &aComment[0]);	// 確認する

								if (strcmp(aComment, "POS") == 0)
								{// POSが来たら位置読み込み

									fscanf(pFile, "%s", &aComment[0]);		// =の分
									fscanf(pFile, "%f", &m_aLoadData[m_nNumLoad][nCntMotion].aKey[nCntKey].aParts[nCntParts].pos.x);	// X座標
									fscanf(pFile, "%f", &m_aLoadData[m_nNumLoad][nCntMotion].aKey[nCntKey].aParts[nCntParts].pos.y);	// Y座標
									fscanf(pFile, "%f", &m_aLoadData[m_nNumLoad][nCntMotion].aKey[nCntKey].aParts[nCntParts].pos.z);	// Z座標
								}

								if (strcmp(aComment, "ROT") == 0)
								{// ROTが来たら角度読み込み

									fscanf(pFile, "%s", &aComment[0]);		// =の分
									fscanf(pFile, "%f", &m_aLoadData[m_nNumLoad][nCntMotion].aKey[nCntKey].aParts[nCntParts].rot.x);	// X角度
									fscanf(pFile, "%f", &m_aLoadData[m_nNumLoad][nCntMotion].aKey[nCntKey].aParts[nCntParts].rot.y);	// Y角度
									fscanf(pFile, "%f", &m_aLoadData[m_nNumLoad][nCntMotion].aKey[nCntKey].aParts[nCntParts].rot.z);	// Z角度
								}
							}// END_PARTSのかっこ

							nCntParts++;	//パーツのカウント加算
						}
					}// END_KEYSETのかっこ

					nCntParts = 0;	// パーツのカウントリセット
					nCntKey++;	// キーのカウント加算
				}

			}// END_MOTIONSETのかっこ

			// モーション情報の登録
			SetInfo(m_aLoadData[m_nNumLoad][nCntMotion]);

			if (m_nNumMotion == 0)
			{
				m_nMaxAllFrame = 0;
				for (int nCntKey = 0; nCntKey < m_aInfo[m_nPatternKey].nNumKey; nCntKey++)
				{
					m_nMaxAllFrame += m_aInfo[m_nPatternKey].aKey[nCntKey].nFrame;	// 全てのカウントの最大値
				}
			}

			nCntKey = 0;	// キーのカウントリセット
			nCntMotion++;	// モーションのカウント加算
			m_nNumMotion++;	// モーションのカウント加算

			// モーション毎のデータ数
			m_nNumLoadData[m_nNumLoad]++;
		}

		if (strcmp(aComment, "END_SCRIPT") == 0)
		{// 終了文字でループを抜ける

			break;
		}
	}

	// ファイルを閉じる
	fclose(pFile);

	// 読み込んだ数加算
	m_nNumLoad++;
}
