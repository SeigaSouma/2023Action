//=============================================================================
// 
// ステージ切り替え処理 [stageswitch.cpp]
// Author : 相馬靜雅
// 
//=============================================================================
#include "stageswitch.h"
#include "renderer.h"
#include "texture.h"
#include "camera.h"
#include "manager.h"
#include "number.h"
#include "game.h"
#include "wavemanager.h"
#include "elevation.h"
#include "title.h"
#include "sound.h"

//==========================================================================
// マクロ定義
//==========================================================================
#define TEXT_SIZE		(250.0f)	// 横幅
#define NUMBER_SIZE		(80.0f)		// 数字のサイズ
#if _DEBUG
#define MAX_LIFE		(360)		// 最大寿命
#else
#define MAX_LIFE		(360)		// 最大寿命
#endif
#define NUMBER_TEXTURE	"data\\TEXTURE\\number_blackclover_01.png"	// テクスチャのファイル

//==========================================================================
// 静的メンバ変数宣言
//==========================================================================
const char *CStageSwitch::m_apTextureFile[] =	// ファイル読み込み
{
	"data\\TEXTURE\\stagetext_01.png",		// 通常エフェクト
};
int CStageSwitch::m_nTexIdx[VTX_MAX] = {};		// テクスチャのインデックス番号
int CStageSwitch::m_nNumberTexIdx = 0;				// 数字テクスチャのインデックス番号

//==========================================================================
// コンストラクタ
//==========================================================================
CStageSwitch::CStageSwitch(int nPriority) : CObject2D(nPriority)
{
	// 値のクリア
	m_nLife = 0;	// 寿命

	for (int nCntNumber = 0; nCntNumber < NUM_STAGE; nCntNumber++)
	{
		m_apNumber[nCntNumber] = {};	// 数字のオブジェクト
	}
}

//==========================================================================
// デストラクタ
//==========================================================================
CStageSwitch::~CStageSwitch()
{

}

//==========================================================================
// 生成処理
//==========================================================================
CStageSwitch *CStageSwitch::Create(void)
{
	// 生成用のオブジェクト
	CStageSwitch *pStageSwitch = NULL;

	if (pStageSwitch == NULL)
	{// NULLだったら

		// メモリの確保
		pStageSwitch = DEBUG_NEW CStageSwitch;

		if (pStageSwitch != NULL)
		{// メモリの確保が出来ていたら

			// 初期化処理
			pStageSwitch->Init();
		}

		return pStageSwitch;
	}

	return NULL;
}

//==================================================================================
// 初期化処理
//==================================================================================
HRESULT CStageSwitch::Init(void)
{
	HRESULT hr;

	// 各種変数の初期化
	SetPosition(D3DXVECTOR3(450.0f, SCREEN_HEIGHT * 0.5f, 0.0f));
	SetColor(D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f));	// 色
	SetSize(D3DXVECTOR2(TEXT_SIZE * 1.0f, TEXT_SIZE * 0.45f));		// サイズ設定
	m_nLife = MAX_LIFE;	// 寿命

	// 種類の設定
	SetType(TYPE_STAGESWITCH);

	// テクスチャデータの配列分繰り返す
	for (int nCntTex = 0; nCntTex < VTX_MAX; nCntTex++)
	{
		if (m_nTexIdx[nCntTex] == 0)
		{
			// テクスチャの割り当て
			m_nTexIdx[nCntTex] = CManager::GetTexture()->Regist(m_apTextureFile[nCntTex]);
		}

		// テクスチャの割り当て
		BindTexture(m_nTexIdx[nCntTex]);
	}

	// 初期化処理
	hr = CObject2D::Init();

	if (FAILED(hr))
	{// 失敗したとき
		return E_FAIL;
	}

	// 位置取得
	D3DXVECTOR3 pos = GetPosition();

#if 0
	for (int nCntNumber = 0; nCntNumber < NUM_STAGE; nCntNumber++)
	{
		// 生成処理
		m_apNumber[nCntNumber] = CNumber::Create();

		// 各種変数の初期化
		m_apNumber[nCntNumber]->GetObject2D()->SetSize(D3DXVECTOR2(NUMBER_SIZE, NUMBER_SIZE));	// サイズ
		m_apNumber[nCntNumber]->GetObject2D()->SetPosition(D3DXVECTOR3(pos.x + TEXT_SIZE + (nCntNumber + 1) * NUMBER_SIZE, pos.y, 0.0f));	// 位置

		// 種類の設定
		m_apNumber[nCntNumber]->GetObject2D()->SetType(CObject::TYPE_STAGESWITCH);

		// テクスチャの割り当て
		m_nNumberTexIdx = CManager::GetTexture()->Regist(NUMBER_TEXTURE);

		// テクスチャの割り当て
		m_apNumber[nCntNumber]->GetObject2D()->BindTexture(m_nNumberTexIdx);
	}
#endif


	// サウンド再生
	CManager::GetSound()->PlaySound(CSound::LABEL_SE_STAGESWITCH);

	CCamera *pCamera = CManager::GetCamera();
	switch (CManager::GetMode())
	{
	case CScene::MODE_GAME:
		// 目標の距離設定
		pCamera->SetLenDest(pCamera->GetOriginDistance() + 1000.0f, MAX_LIFE / 2, 2.5f, 0.01f);

		break;

	case CScene::MODE_TITLE:
		break;

	default:
		break;
	}

	return S_OK;
}

//==================================================================================
// 終了処理
//==================================================================================
void CStageSwitch::Uninit(void)
{
	for (int nCntScore = 0; nCntScore < NUM_STAGE; nCntScore++)
	{
		// 終了処理
		if (m_apNumber[nCntScore] != NULL)
		{// メモリの確保がされていたら

			// 終了処理
			m_apNumber[nCntScore]->Release();
			m_apNumber[nCntScore]->Uninit();
			delete m_apNumber[nCntScore];
			m_apNumber[nCntScore] = NULL;
		}
	}

	// 終了処理
	CObject2D::Uninit();
}

//==================================================================================
// 解放処理
//==================================================================================
void CStageSwitch::Release(void)
{
	for (int nCntScore = 0; nCntScore < NUM_STAGE; nCntScore++)
	{
		// 終了処理
		if (m_apNumber[nCntScore] != NULL)
		{// メモリの確保がされていたら

			// 解放処理
			m_apNumber[nCntScore]->Release();
			m_apNumber[nCntScore]->Uninit();
			delete m_apNumber[nCntScore];
			m_apNumber[nCntScore] = NULL;
		}
	}
}

//==================================================================================
// 更新処理
//==================================================================================
void CStageSwitch::Update(void)
{
	// 死亡の判定
	if (IsDeath() == true)
	{// 死亡フラグが立っていたら
		return;
	}

	switch (CManager::GetMode())
	{
	case CScene::MODE_GAME:
		UpdateGame();
		break;

	case CScene::MODE_TITLE:
		UpdateTitle();
		break;

	default:
		break;
	}
}

//==================================================================================
// ゲームの更新
//==================================================================================
void CStageSwitch::UpdateGame(void)
{
	// 位置取得
	D3DXVECTOR3 pos = GetPosition();

	// 移動量取得
	D3DXVECTOR3 move = GetMove();

	// 現在のステージ取得
	int nNowStage = CManager::GetScene()->GetWaveManager()->GetNowStage();

	// ステージ切り替え処理
	nNowStage = CManager::GetScene()->GetWaveManager()->GetNowStage();
	CGame::GetObject3DMesh()->StageSwitch(nNowStage, nNowStage + 1, MAX_LIFE - m_nLife, MAX_LIFE);
	CScene::GetElevationOutside(CElevation::POSTYPE_UP)->StageSwitch(nNowStage, nNowStage + 1, MAX_LIFE - m_nLife, MAX_LIFE);
	CScene::GetElevationOutside(CElevation::POSTYPE_RIGHT)->StageSwitch(nNowStage, nNowStage + 1, MAX_LIFE - m_nLife, MAX_LIFE);
	CScene::GetElevationOutside(CElevation::POSTYPE_DOWN)->StageSwitch(nNowStage, nNowStage + 1, MAX_LIFE - m_nLife, MAX_LIFE);
	CScene::GetElevationOutside(CElevation::POSTYPE_LEFT)->StageSwitch(nNowStage, nNowStage + 1, MAX_LIFE - m_nLife, MAX_LIFE);

	// 寿命減らす
	m_nLife--;

	if (m_nLife <= 60)
	{
		nNowStage += 1;
	}

	if (m_nLife <= 0)
	{
		// サウンド停止
		CManager::GetSound()->StopSound(CSound::LABEL_SE_STAGESWITCH);

		CManager::GetScene()->GetWaveManager()->SetEnableStageEnd();
		// 終了する
		Release();
		Uninit();
		return;
	}

	// 値の設定
	SetValue(nNowStage);

	// 位置更新
	pos += move;

	// 位置設定
	SetPosition(pos);

	// 移動量設定
	SetMove(move);

	// 頂点座標の設定
	SetVtx();
}

//==================================================================================
// タイトルの更新
//==================================================================================
void CStageSwitch::UpdateTitle(void)
{
	// 現在のステージ取得
	CTitle *pTitle = (CTitle*)CManager::GetScene();
	int nNowStage = pTitle->GetSwitchWave();
	int nNextStage = (nNowStage + 1)/* % CScene::GetWaveManager()->GetMaxStage()*/;

	if (nNextStage > CManager::GetScene()->GetWaveManager()->GetMaxStage())
	{
		nNextStage = 0;
	}

	// ステージ切り替え処理
	CScene::GetObject3DMesh()->StageSwitch(nNowStage, nNextStage, MAX_LIFE - m_nLife, MAX_LIFE);
	CScene::GetElevationOutside(CElevation::POSTYPE_UP)->StageSwitch(nNowStage, nNextStage, MAX_LIFE - m_nLife, MAX_LIFE);
	CScene::GetElevationOutside(CElevation::POSTYPE_RIGHT)->StageSwitch(nNowStage, nNextStage, MAX_LIFE - m_nLife, MAX_LIFE);
	CScene::GetElevationOutside(CElevation::POSTYPE_DOWN)->StageSwitch(nNowStage, nNextStage, MAX_LIFE - m_nLife, MAX_LIFE);
	CScene::GetElevationOutside(CElevation::POSTYPE_LEFT)->StageSwitch(nNowStage, nNextStage, MAX_LIFE - m_nLife, MAX_LIFE);

	// 寿命減らす
	m_nLife--;
	if (m_nLife <= 0)
	{
		// サウンド停止
		CManager::GetSound()->StopSound(CSound::LABEL_SE_STAGESWITCH);

		CManager::GetScene()->GetWaveManager()->SetEnableStageEnd();
		// 終了する
		Release();
		Uninit();
		return;
	}
}

//==================================================================================
// 値の設定処理
//==================================================================================
void CStageSwitch::SetValue(int nValue)
{
	int aTexU[NUM_STAGE];
	int nDigit = 1;		// aTexU計算用

	// 計算用割り出し
	for (int nCntDigit = 0; nCntDigit < NUM_STAGE; nCntDigit++)
	{
		nDigit *= 10;
	}

	// テクスチャ座標に代入する
	for (int nCntTex = 0; nCntTex < NUM_STAGE; nCntTex++)
	{// 桁数分設定

		aTexU[nCntTex] = nValue % nDigit / (nDigit / 10);
		nDigit /= 10;
	}

	for (int nCntNumber = 0; nCntNumber < NUM_STAGE; nCntNumber++)
	{
		if (m_apNumber[nCntNumber] != NULL)
		{// NULLじゃなかったら

			// 頂点設定
			m_apNumber[nCntNumber]->GetObject2D()->SetVtx();

			D3DXVECTOR2 *pTex = m_apNumber[nCntNumber]->GetObject2D()->GetTex();

			// テクスチャ座標の設定
			pTex[0] = D3DXVECTOR2(aTexU[nCntNumber] * 0.1f, 0.0f);
			pTex[1] = D3DXVECTOR2(aTexU[nCntNumber] * 0.1f + 0.1f, 0.0f);
			pTex[2] = D3DXVECTOR2(aTexU[nCntNumber] * 0.1f, 1.0f);
			pTex[3] = D3DXVECTOR2(aTexU[nCntNumber] * 0.1f + 0.1f, 1.0f);
		}
	}
}

//==================================================================================
// 描画処理
//==================================================================================
void CStageSwitch::Draw(void)
{
	// デバイスの取得
	LPDIRECT3DDEVICE9 pDevice = CManager::GetRenderer()->GetDevice();

	// アルファテストを有効にする
	pDevice->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
	pDevice->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);
	pDevice->SetRenderState(D3DRS_ALPHAREF, 0);

	// 描画処理
	//CObject2D::Draw();

	// アルファテストを有効にする
	pDevice->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
	pDevice->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_ALWAYS);
	pDevice->SetRenderState(D3DRS_ALPHAREF, 0);
}

//==========================================================================
// 頂点情報設定処理
//==========================================================================
void CStageSwitch::SetVtx(void)
{
	// 頂点設定
	CObject2D::SetVtx();
}

