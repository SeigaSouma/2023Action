//=============================================================================
// 
//  敵の拠点エディター処理 [edit_enemybase.cpp]
//  Author : 相馬靜雅
// 
//=============================================================================
#include "edit_enemybase.h"
#include "enemybase.h"
#include "objectX.h"
#include "XLoad.h"
#include "manager.h"
#include "renderer.h"
#include "calculation.h"
#include "debugproc.h"
#include "input.h"
#include "elevation.h"
#include "mapmanager.h"
#include "game.h"
#include "enemymanager.h"
#include "enemy.h"
#include "enemyfixedmove_manager.h"
#include "player.h"

//==========================================================================
// マクロ定義
//==========================================================================
#define MOVE		(10.0f)
#define MOVE_SLOW	(1.0f)
#define MARKOBJ	"data\\MODEL\\koko.x"

//==========================================================================
// 静的メンバ変数宣言
//==========================================================================

//==========================================================================
// コンストラクタ
//==========================================================================
CEditEnemyBase::CEditEnemyBase()
{
	// ゼロクリア
	m_pos = mylib_const::DEFAULT_VECTOR3;	// 位置
	m_rot = mylib_const::DEFAULT_VECTOR3;			// 位置
	m_nEnemyType = 0;			// 追従の種類
	m_nIdxMapPoint = 0;			// マップポイントのインデックス番号
	m_fPointRatio = 0.0f;		// 移動割合
	m_fMoveValue = 0.0f;		// 移動量
	m_MoveAngle = CObject::ANGLE_UP;	// 移動方向
	memset(&m_pEnemy[0], NULL, sizeof(m_pEnemy));	// 敵へのポインタ
	m_apObjX = NULL;			// オブジェクトX
	m_nRush = 0;				// ラッシュ用かどうか
}

//==========================================================================
// デストラクタ
//==========================================================================
CEditEnemyBase::~CEditEnemyBase()
{

}

//==========================================================================
// 生成処理
//==========================================================================
CEditEnemyBase *CEditEnemyBase::Create(void)
{
	// 生成用のオブジェクト
	CEditEnemyBase *pObjBase = NULL;

	if (pObjBase == NULL)
	{// NULLだったら

		// メモリの確保
		pObjBase = DEBUG_NEW CEditEnemyBase;

		if (pObjBase != NULL)
		{// メモリの確保が出来ていたら

			// 初期化処理
			HRESULT hr = pObjBase->Init();
			if (FAILED(hr))
			{// 失敗していたら
				return NULL;
			}
		}

		return pObjBase;
	}

	return NULL;
}

//==========================================================================
// 初期化処理
//==========================================================================
HRESULT CEditEnemyBase::Init(void)
{
	m_pos = CManager::GetInstance()->GetCamera()->GetPositionR();
	m_apObjX = CObjectX::Create(MARKOBJ, mylib_const::DEFAULT_VECTOR3, mylib_const::DEFAULT_VECTOR3, false);	// オブジェクトX
	m_apObjX->SetType(CObject::TYPE_BALLAST);
	m_nIdxMapPoint = CManager::GetInstance()->GetScene()->GetPlayer()->GetMapIndex();
	return S_OK;
}

//==========================================================================
// 終了処理
//==========================================================================
void CEditEnemyBase::Uninit(void)
{
	
}

//==========================================================================
// 終了処理
//==========================================================================
void CEditEnemyBase::Release(void)
{
	if (m_apObjX != NULL)
	{
		m_apObjX->Uninit();
		m_apObjX = NULL;
	}
}

//==========================================================================
// 更新処理
//==========================================================================
void CEditEnemyBase::Update(void)
{
	// キーボード情報取得
	CInputKeyboard *pInputKeyboard = CManager::GetInstance()->GetInputKeyboard();

	// 操作
	Control(m_pos);
	ChangeType();

	// 目印
	m_apObjX->SetPosition(m_pos);

	for (int nCntEnemy = 0; nCntEnemy < mylib_const::MAX_PATTEN_ENEMY; nCntEnemy++)
	{
		if (m_pEnemy[nCntEnemy] != NULL)
		{// NULLじゃなかったら
			D3DXVECTOR3 posOrigin = m_pEnemy[nCntEnemy]->GetOriginPosition();
			m_pEnemy[nCntEnemy]->SetSpawnPosition(D3DXVECTOR3(posOrigin.x, m_pos.y, posOrigin.z));
			m_pEnemy[nCntEnemy]->SetMapIndexOrigin(m_nIdxMapPoint);
			m_pEnemy[nCntEnemy]->SetMapMoveValueOrigin(m_fMoveValue);
		}
	}


	// マップマネージャの取得
	CMapManager *pMapManager = CManager::GetInstance()->GetScene()->GetMapManager();
	if (pMapManager == NULL)
	{// NULLだったら
		return;
	}

	// 曲線作る為の4点
	int nP0 = m_nIdxMapPoint;
	int nP1 = m_nIdxMapPoint + 1;
	int nP2 = m_nIdxMapPoint + 2;

	// 目標地点
	D3DXVECTOR3 TargetPoint0 = pMapManager->GetControlPoint(nP0);
	D3DXVECTOR3 TargetPoint1 = pMapManager->GetControlPoint(nP1);
	D3DXVECTOR3 TargetPoint2 = pMapManager->GetControlPoint(nP2);

	int nAngle = 1;
	if (m_MoveAngle == CObject::ANGLE_LEFT)
	{
		nAngle = -1;
	}

	// 少し先の地点取得
	float fMoveValue = m_fMoveValue + 50.0f * nAngle;
	D3DXVECTOR3 DestPoint = pMapManager->GetTargetPosition(m_nIdxMapPoint, fMoveValue);

	// ベクトル
	D3DXVECTOR3 vec = mylib_const::DEFAULT_VECTOR3;
	D3DXVECTOR3 newvec = mylib_const::DEFAULT_VECTOR3;

	// 2点間の距離取得
	float fPosLength = GetPosLength(TargetPoint1, TargetPoint2);

	// ベクトル
	if (m_fMoveValue < fMoveValue)
	{
		vec = DestPoint - m_pos;
	}
	else
	{
		vec = m_pos - DestPoint;
	}
	D3DXVec3Normalize(&vec, &vec);

	// 90度傾ける
	newvec.x = vec.z;
	newvec.z = -vec.x;

	// 目標の視点の向き
	m_rot.y = atan2f((0.0f - newvec.x), (0.0f - newvec.z));
	m_apObjX->SetRotation(m_rot);




	// 掴み処理
	if (pInputKeyboard->GetPress(DIK_SPACE) == true)
	{// SPACEで掴む
		Grab();
	}

	if (pInputKeyboard->GetTrigger(DIK_RETURN) == true)
	{// ENTERで配置

		// 敵の拠点の取得
		CEnemyBase *pEnemyBase = CManager::GetInstance()->GetScene()->GetEnemyBase();
		if (pEnemyBase == NULL)
		{
			return;
		}
		// 生成
		pEnemyBase->CreatePos(m_nEnemyType, m_nIdxMapPoint, m_fMoveValue, m_nRush, m_pos.y);
	}

	if (pInputKeyboard->GetTrigger(DIK_DELETE) == true)
	{// deleteで削除
		Delete();
	}

	if (pInputKeyboard->GetTrigger(DIK_F9) == true)
	{// セーブ

		// 敵の拠点の取得
		CEnemyBase *pEnemyBase = CManager::GetInstance()->GetScene()->GetEnemyBase();
		if (pEnemyBase == NULL)
		{
			return;
		}
		pEnemyBase->Save();
	}

	// デバッグ情報
	CManager::GetInstance()->GetDebugProc()->Print(
		"------------------[ 敵の拠点エディター情報 ]------------------\n"
		"<配置>         [ENTER]\n"
		"<ファイル保存> [F9] 【data/BIN/enemybase.bin】\n"
		"<移動>         高速：[↑,↓,←,→]　低速：[W,A,S,D]\n"
		"<上昇,下降>    [I,K]\n"
		"<種類変更>     [1, 2][%d]\n"
		"<影の使用状況> [3][%d]\n"
		"<掴み移動>     [SPACE]\n"
		"<削除>         [DELETE]\n"
		"<位置>         [X：%f Y：%f Z：%f]\n"
		"\n", m_nEnemyType, m_nRush, m_pos.x, m_pos.y, m_pos.z);
}

//==========================================================================
// 操作処理
//==========================================================================
void CEditEnemyBase::Control(D3DXVECTOR3 &pos)
{
	// カメラの情報取得
	CCamera *pCamera = CManager::GetInstance()->GetCamera();

	// カメラの向き取得
	D3DXVECTOR3 Camerarot = pCamera->GetRotation();

	// キーボード情報取得
	CInputKeyboard *pInputKeyboard = CManager::GetInstance()->GetInputKeyboard();

	if (pInputKeyboard->GetPress(DIK_LEFT) == true)
	{// ←キーが押された,左移動

		m_fMoveValue -= MOVE;
		m_MoveAngle = CObject::ANGLE_LEFT;
	}
	else if (pInputKeyboard->GetPress(DIK_RIGHT) == true)
	{// Dキーが押された,右移動

		m_fMoveValue += MOVE;
		m_MoveAngle = CObject::ANGLE_RIGHT;
	}

	if (pInputKeyboard->GetPress(DIK_A) == true)
	{// ←キーが押された,左移動

		m_fMoveValue -= MOVE_SLOW;
	}
	else if (pInputKeyboard->GetPress(DIK_D) == true)
	{// Dキーが押された,右移動

		m_fMoveValue += MOVE_SLOW;
	}

	if (pInputKeyboard->GetPress(DIK_I) == true)
	{// Iキーが押された,上昇

		pos.y += MOVE;
	}
	else if (pInputKeyboard->GetPress(DIK_K) == true)
	{// Kキーが押された,下降

		pos.y -= MOVE;
	}


	// マップマネージャの取得
	CMapManager *pMapManager = CManager::GetInstance()->GetScene()->GetMapManager();
	if (pMapManager == NULL)
	{// NULLだったら
		return;
	}

	// 位置更新
	pos = pMapManager->UpdateNowPosition(m_nIdxMapPoint, m_fPointRatio, m_fMoveValue, pos.y);

}


//==========================================================================
// 種類変更
//==========================================================================
void CEditEnemyBase::ChangeType(void)
{
	int nNumAll = CGame::GetEnemyManager()->GetPatternNum();

	// キーボード情報取得
	CInputKeyboard *pInputKeyboard = CManager::GetInstance()->GetInputKeyboard();

	// 配置するオブジェクト変更
	bool bChange = false;
	if (pInputKeyboard->GetTrigger(DIK_1) == true)
	{// 1が押された

		// 色の種類更新
		m_nEnemyType = (m_nEnemyType + (nNumAll - 1)) % nNumAll;
		bChange = true;
	}
	else if (pInputKeyboard->GetTrigger(DIK_2) == true)
	{// 2が押された

		// 色の種類更新
		m_nEnemyType = (m_nEnemyType + 1) % nNumAll;
		bChange = true;
	}

	if (pInputKeyboard->GetTrigger(DIK_3) == true)
	{// 3が押された

		// 影の使用状況切り替え
		m_nRush = m_nRush ? 0 : 1;
	}

	if (bChange)
	{
		for (int nCntEnemy = 0; nCntEnemy < mylib_const::MAX_PATTEN_ENEMY; nCntEnemy++)
		{
			if (m_pEnemy[nCntEnemy] != NULL)
			{// NULLにする
				m_pEnemy[nCntEnemy]->Kill();
				m_pEnemy[nCntEnemy]->Uninit();
				m_pEnemy[nCntEnemy] = NULL;
			}
		}

		CEnemyManager *pEnemyManager = CGame::GetEnemyManager();

		// 敵配置
		CEnemy **ppEnemy = pEnemyManager->SetEnemy(m_pos, m_rot, m_nEnemyType);

		for (int nCntEnemy = 0; nCntEnemy < mylib_const::MAX_PATTEN_ENEMY; nCntEnemy++)
		{
			if (ppEnemy[nCntEnemy] != NULL)
			{
				m_pEnemy[nCntEnemy] = ppEnemy[nCntEnemy];
			}
		}
	}

}

//==========================================================================
// モデル掴む
//==========================================================================
void CEditEnemyBase::Grab(void)
{

	// キーボード情報取得
	CInputKeyboard *pInputKeyboard = CManager::GetInstance()->GetInputKeyboard();

	// 敵の拠点の取得
	CEnemyBase *pEnemyBase = CManager::GetInstance()->GetScene()->GetEnemyBase();
	if (pEnemyBase == NULL)
	{
		return;
	}

	bool bAll = false;
	if (pInputKeyboard->GetPress(DIK_RCONTROL) == true)
	{// 右コントロールで全選択
		bAll = true;
	}

	for (int i = 0; i < pEnemyBase->GetNumAll(); i++)
	{
		D3DXVECTOR3 TargetPoint = pEnemyBase->GetAxis(i);
		if (bAll == true || SphereRange(m_pos, TargetPoint, 50.0f, 50.0f))
		{// 球に当たってたら

			// 操作
			Control(TargetPoint);

			// 位置設定
			pEnemyBase->SetSpawnPoint(i, m_nIdxMapPoint, m_fMoveValue, TargetPoint.y);
		}
	}
}

//==========================================================================
// モデル掴む
//==========================================================================
void CEditEnemyBase::Delete(void)
{
	// キーボード情報取得
	CInputKeyboard *pInputKeyboard = CManager::GetInstance()->GetInputKeyboard();

	// 敵の拠点の取得
	CEnemyBase *pEnemyBase = CManager::GetInstance()->GetScene()->GetEnemyBase();
	if (pEnemyBase == NULL)
	{
		return;
	}

	for (int i = 0; i < pEnemyBase->GetNumAll(); i++)
	{
		D3DXVECTOR3 TargetPoint = pEnemyBase->GetAxis(i);
		if (SphereRange(m_pos, TargetPoint, 50.0f, 50.0f))
		{// 球に当たってたら
			pEnemyBase->DeletePos(i);
		}
	}
}

//==========================================================================
// 描画処理
//==========================================================================
void CEditEnemyBase::Draw(void)
{

}
