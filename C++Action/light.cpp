//=============================================================================
// 
//  ライト処理 [light.cpp]
//  Author : 相馬靜雅
// 
//=============================================================================
#include "light.h"
#include "input.h"
#include "manager.h"
#include "renderer.h"


//==========================================================================
// コンストラクタ
//==========================================================================
CLight::CLight()
{
	for (int nCntLight = 0; nCntLight < TYPE_MAX; nCntLight++)
	{
		// ライトの情報をクリアする
		ZeroMemory(&m_aLight[nCntLight], sizeof(D3DLIGHT9));
	}
}

//==========================================================================
// デストラクタ
//==========================================================================
CLight::~CLight()
{

}

//==================================================================================
// ライトの初期化処理
//==================================================================================
HRESULT CLight::Init(void)
{
	//  デバイスの取得
	LPDIRECT3DDEVICE9 pDevice = CManager::GetInstance()->GetRenderer()->GetDevice();

	D3DXVECTOR3 vecDir;	// 設定用方向ベクトル

	for (int nCntLight = 0; nCntLight < TYPE_MAX; nCntLight++)
	{
		// ライトの情報をクリアする
		ZeroMemory(&m_aLight[nCntLight], sizeof(D3DLIGHT9));

		switch (nCntLight)
		{
		case TYPE_DIRECTIONAL_01:
			// ライトの種類を設定
			m_aLight[nCntLight].Type = D3DLIGHT_DIRECTIONAL;

			// ライトの拡散光を設定
			m_aLight[nCntLight].Diffuse = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);

			// ライトの方向を設定
			vecDir = D3DXVECTOR3(0.22f, -0.87f, 0.44f);
			break;

		case TYPE_DIRECTIONAL_02:
			// ライトの種類を設定
			m_aLight[nCntLight].Type = D3DLIGHT_DIRECTIONAL;

			// ライトの拡散光を設定
			m_aLight[nCntLight].Diffuse = D3DXCOLOR(0.65f, 0.65f, 0.65f, 1.0f);

			// ライトの方向を設定
			vecDir = D3DXVECTOR3(-0.18f, 0.88f, -0.44f);
			break;

		case TYPE_DIRECTIONAL_03:
			// ライトの種類を設定
			m_aLight[nCntLight].Type = D3DLIGHT_DIRECTIONAL;

			// ライトの拡散光を設定
			m_aLight[nCntLight].Diffuse = D3DXCOLOR(0.15f, 0.15f, 0.15f, 1.0f);

			// ライトの方向を設定
			vecDir = D3DXVECTOR3(0.89f, -0.11f, 0.44f);
			break;

		case TYPE_SPOT_01:
			// ライトの種類をスポットライトに設定
			m_aLight[nCntLight].Type = D3DLIGHT_DIRECTIONAL;

			// スポットライトの拡散光を設定
			m_aLight[nCntLight].Diffuse = D3DXCOLOR(0.2f, 0.2f, 0.2f, 1.0f);

			// スポットライトの方向を設定
			vecDir = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
			break;
		}

		// ベクトルを正規化する(1.0にする)
		D3DXVec3Normalize(&vecDir, &vecDir);
		m_aLight[nCntLight].Direction = vecDir;

		// ライトを設定する
		pDevice->SetLight(nCntLight, &m_aLight[nCntLight]);

		// ライトを有効にする
		pDevice->LightEnable(nCntLight, TRUE);
	}

	return S_OK;
}

//==================================================================================
// ライトの終了処理
//==================================================================================
void CLight::Uninit(void)
{

}

//==================================================================================
// ライトの更新処理
//==================================================================================
void CLight::Update(void)
{

}

//==================================================================================
// スポットライトの向き更新
//==================================================================================
void CLight::UpdateSpotLightDirection(D3DXVECTOR3 vec)
{
	//  デバイスの取得
	LPDIRECT3DDEVICE9 pDevice = CManager::GetInstance()->GetRenderer()->GetDevice();

	// スポットライトの方向を設定
	D3DXVECTOR3 vecDir = vec;

	// ベクトルを正規化する(1.0にする)
	D3DXVec3Normalize(&vecDir, &vecDir);
	m_aLight[TYPE_SPOT_01].Direction = vecDir;

	// ライトを設定する
	pDevice->SetLight(TYPE_SPOT_01, &m_aLight[TYPE_SPOT_01]);

	// ライトを有効にする
	pDevice->LightEnable(TYPE_SPOT_01, TRUE);
}
