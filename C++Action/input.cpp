//=============================================================================
//
// 入力処理 [input.cpp]
// Author : 相馬靜雅
//
//=============================================================================
#include "input.h"
#include "debugproc.h"
#include "manager.h"

//マクロ定義
#define SHOT_FPS	(15)	// 弾の間隔
#define DMG_TIME	(30)	// バイブの時間
#define WIN_TIME	(15)	// バイブの時間
#define MOUSE_SENS	(0.5f)	// マウス感度の補正

//==========================================================================
// 静的メンバ変数宣言
//==========================================================================
LPDIRECTINPUT8 CInput::m_pInput = NULL;		// DirectInputオブジェクトへのポインタ
int CInput::nNumAll = 0;					// デバイスの総数

//==========================================================================
//グローバル変数宣言
//==========================================================================

//==================================================================================
// コンストラクタ
//==================================================================================
CInput::CInput()
{
	// 総数加算
	nNumAll++;
}

//==================================================================================
// デストラクタ
//==================================================================================
CInput::~CInput()
{
	// 総数減算
	nNumAll--;
}

//==================================================================================
// 初期化処理
//==================================================================================
HRESULT CInput::Init(HINSTANCE hInstance, HWND hWnd)
{
	HRESULT hr;

	if (m_pInput == NULL)
	{// まだ生成していなかったら

		// DirectInputオブジェクトの生成
		hr = DirectInput8Create(hInstance, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&m_pInput, NULL);
		if (FAILED(hr))
		{
			return E_FAIL;
		}
	}

	return S_OK;
}

//==================================================================================
// 終了処理
//==================================================================================
void CInput::Uninit(void)
{
	// 入力デバイスの破棄
	if (m_pDevice != NULL)
	{
		m_pDevice->Unacquire();	// アクセス権を放棄
		m_pDevice->Release();
		m_pDevice = NULL;
	}

	if (nNumAll <= 1)
	{// 最後のデバイスの時

		// オブジェクトの破棄
		if (m_pInput != NULL)
		{
			m_pInput->Release();
			m_pInput = NULL;
		}
	}
}

//--------------------------------------------------- [ ここからキーボード ] ---------------------------------------------------//

//==================================================================================
// コンストラクタ
//==================================================================================
CInputKeyboard::CInputKeyboard()
{
	for (int nCntKey = 0; nCntKey < NUM_KEY_MAX; nCntKey++)
	{
		m_aKeyState[nCntKey] = NULL;		// プレス情報
		m_aKeyStateTrigger[nCntKey] = NULL;	// トリガー情報
		m_aKeyStateRelease[nCntKey] = NULL;	// リリース情報
		m_aKeyStateRepeat[nCntKey] = NULL;	// リピート情報
		m_nCntKeyRepeat[nCntKey] = 0;		// リピート用カウント
		m_nPatternRepeat[nCntKey] = 1;		// リピ―トの間隔
	}
}

//==================================================================================
// デストラクタ
//==================================================================================
CInputKeyboard::~CInputKeyboard()
{

}

//==================================================================================
// 初期化処理
//==================================================================================
HRESULT CInputKeyboard::Init(HINSTANCE hInstance, HWND hWnd)
{
	// 初期化処理
	CInput::Init(hInstance, hWnd);

	// 入力デバイス(キーボード)の生成
	if (FAILED(m_pInput->CreateDevice(GUID_SysKeyboard, &m_pDevice, NULL)))
	{
		return E_FAIL;
	}

	// データフォーマットを設定
	if (FAILED(m_pDevice->SetDataFormat(&c_dfDIKeyboard)))
	{
		return E_FAIL;
	}

	// 協調モードを設定
	if (FAILED(m_pDevice->SetCooperativeLevel(hWnd, (DISCL_FOREGROUND | DISCL_NONEXCLUSIVE))))
	{
		return E_FAIL;
	}

	// キーボードへのアクセス権を獲得
	m_pDevice->Acquire();

	return S_OK;
}

//==================================================================================
// 終了処理
//==================================================================================
void CInputKeyboard::Uninit(void)
{
	// 終了処理
	CInput::Uninit();
}

//==================================================================================
// 更新処理
//==================================================================================
void CInputKeyboard::Update(void)
{
	BYTE aKeyState[NUM_KEY_MAX];	// キーボードの入力情報

	// 入力デバイスからデータを取得
	if (SUCCEEDED(m_pDevice->GetDeviceState(sizeof(aKeyState), &aKeyState[0])))
	{
		for (int nCntKey = 0; nCntKey < NUM_KEY_MAX; nCntKey++)
		{
			// トリガー情報を保存
			m_aKeyStateTrigger[nCntKey] = (m_aKeyState[nCntKey] ^ aKeyState[nCntKey]) & aKeyState[nCntKey];

			// リリース情報を保存
			m_aKeyStateRelease[nCntKey] = (m_aKeyState[nCntKey] ^ aKeyState[nCntKey]) & m_aKeyState[nCntKey];

			// キーボードのプレス情報を保存
			m_aKeyState[nCntKey] = aKeyState[nCntKey];

			if (GetPress(nCntKey))
			{
				// リピートカウント加算
				m_nCntKeyRepeat[nCntKey] = (m_nCntKeyRepeat[nCntKey] + 1) % m_nPatternRepeat[nCntKey];

				if ((m_nCntKeyRepeat[nCntKey] % m_nPatternRepeat[nCntKey]) == 0)
				{
					// リリース情報を保存
					m_nCntKeyRepeat[nCntKey] = (m_nCntKeyRepeat[nCntKey] + 1) % m_nPatternRepeat[nCntKey];

					// プレス情報を保存
					m_aKeyStateRepeat[nCntKey] = m_aKeyState[nCntKey];
				}
				else
				{
					// リピートにリリースの情報保存
					m_aKeyStateRepeat[nCntKey] = m_aKeyStateRelease[nCntKey];
				}
			}
			else
			{
				// リピートにリリースの情報保存
				m_aKeyStateRepeat[nCntKey] = m_aKeyStateRelease[nCntKey];
			}
		}
	}
	else
	{
		// キーボードへのアクセス権を獲得
		m_pDevice->Acquire();
	}
}

//==================================================================================
// キーボードのプレス情報を取得
//==================================================================================
bool CInputKeyboard::GetPress(int nKey)
{
	return (m_aKeyState[nKey] & 0x80) ? true: false;
}

//==================================================================================
// キーボードのトリガー情報を取得
//==================================================================================
bool CInputKeyboard::GetTrigger(int nKey)
{
	return (m_aKeyStateTrigger[nKey] & 0x80) ? true : false;
}

//==================================================================================
// キーボードのリリース情報を取得
//==================================================================================
bool CInputKeyboard::GetRelease(int nKey)
{
	return (m_aKeyStateRelease[nKey] & 0x80) ? true : false;
}

//==================================================================================
// キーボードのリピート情報を取得
//==================================================================================
bool CInputKeyboard::GetRepeat(int nKey, int nPattern)
{
	m_nPatternRepeat[nKey] = nPattern;		// リピ―トの間隔
	return (m_aKeyStateRepeat[nKey] & 0x80) ? true : false;
}

//--------------------------------------------------- [ ここからゲームパッド ] ---------------------------------------------------//
//==================================================================================
// コンストラクタ
//==================================================================================
CInputGamepad::CInputGamepad()
{
	for (int nCntPlayer = 0; nCntPlayer < MAX_PLAYER; nCntPlayer++)
	{
		memset(&m_aGamepadState[nCntPlayer], 0, sizeof(XINPUT_STATE));
		memset(&m_aGamepadStateTrigger[nCntPlayer], 0, sizeof(XINPUT_STATE));
		memset(&m_aGamepadStateRepeat[nCntPlayer], 0, sizeof(XINPUT_STATE));
		memset(&m_aGamepadStateRelease[nCntPlayer], 0, sizeof(XINPUT_STATE));
		memset(&m_aGamepadStateVib[nCntPlayer], 0, sizeof(XINPUT_VIBRATION));
		m_VibrationState[nCntPlayer] = VIBRATION_STATE_NONE;	// 振動の種類
		m_nCntVibration[nCntPlayer] = 0;						// 振動の時間
		m_nMaxCntVibration[nCntPlayer] = 0;						// 振動の時間
	}
	m_nLeftStickCount = 0;					// 左トリガーの選択カウント
	m_bLeftStickSelect[STICK_X] = false;				// 左トリガーの選択判定
	m_bLeftStickSelect[STICK_Y] = false;				// 左トリガーの選択判定
	m_bVibrationUse = false;				// バイブを使用するかどうか

	m_nCntPadrepeat = 0;									// リピート用カウント
}

//==================================================================================
// デストラクタ
//==================================================================================
CInputGamepad::~CInputGamepad()
{

}

//==================================================================================
//ゲームパッドの初期化処理
//==================================================================================
HRESULT CInputGamepad::Init(HINSTANCE hInstance, HWND hWnd)
{

	//XInputを有効化
	XInputEnable(true);

	// 左スティックの情報を初期化
	m_bLeftStickSelect[STICK_X] = false;				// 左トリガーの選択判定
	m_bLeftStickSelect[STICK_Y] = false;				// 左トリガーの選択判定
	m_nLeftStickCount = 0;

	// バイブの情報を初期化
	m_bVibrationUse = true;

	// メモリクリア
	for (int nCntPlayer = 0; nCntPlayer < MAX_PLAYER; nCntPlayer++)
	{
		m_nCntVibration[nCntPlayer] = 0;
		m_VibrationState[nCntPlayer] = VIBRATION_STATE_NONE;
		m_nMaxCntVibration[nCntPlayer] = 0;

		memset(&m_aGamepadState[nCntPlayer], 0, sizeof(XINPUT_STATE));
		memset(&m_aGamepadStateTrigger[nCntPlayer], 0, sizeof(XINPUT_STATE));
		memset(&m_aGamepadStateRepeat[nCntPlayer], 0, sizeof(XINPUT_STATE));
		memset(&m_aGamepadStateRelease[nCntPlayer], 0, sizeof(XINPUT_STATE));
		memset(&m_aGamepadStateVib[nCntPlayer], 0, sizeof(XINPUT_VIBRATION));
	}

	return S_OK;
}

//==================================================================================
//ゲームパッドの終了処理
//==================================================================================
void CInputGamepad::Uninit(void)
{
	//XInputを無効化
	XInputEnable(false);
}

//==================================================================================
//ゲームパッドの更新処理
//==================================================================================
void CInputGamepad::Update(void)
{
	XINPUT_STATE  aGamepadState[MAX_PLAYER];
	int nCntPlayer;

	// 入力デバイスからデータを取得
	for (nCntPlayer = 0; nCntPlayer < MAX_PLAYER; nCntPlayer++)
	{
		if (XInputGetState(nCntPlayer, &aGamepadState[nCntPlayer]) == ERROR_SUCCESS)
		{
			// キーボードのトリガー情報を保存
			m_aGamepadStateTrigger[nCntPlayer].Gamepad.wButtons =
				(~m_aGamepadState[nCntPlayer].Gamepad.wButtons) & aGamepadState[nCntPlayer].Gamepad.wButtons;

			// リリース情報を保存
			m_aGamepadStateRelease[nCntPlayer].Gamepad.wButtons = 
				(m_aGamepadState[nCntPlayer].Gamepad.wButtons ^ aGamepadState[nCntPlayer].Gamepad.wButtons) & m_aGamepadState[nCntPlayer].Gamepad.wButtons;


			// キーボードのプレス情報を保存
			m_aGamepadState[nCntPlayer] = aGamepadState[nCntPlayer];
			
			if ((m_nCntPadrepeat % SHOT_FPS) == 0)
			{
				// リピート情報を保存
				m_nCntPadrepeat = (m_nCntPadrepeat + 1) % SHOT_FPS;
				m_aGamepadStateRepeat[nCntPlayer].Gamepad.wButtons = aGamepadState[nCntPlayer].Gamepad.wButtons;
			}
			else
			{
				// リピートにリリースの情報保存
				m_aGamepadStateRepeat[nCntPlayer].Gamepad.wButtons =
					(m_aGamepadState[nCntPlayer].Gamepad.wButtons ^ aGamepadState[nCntPlayer].Gamepad.wButtons) & m_aGamepadState[nCntPlayer].Gamepad.wButtons;
			}
		}

		//タイマーを減算
		m_nCntVibration[nCntPlayer]--;

		if (m_nCntVibration[nCntPlayer] < 0)
		{// カウントがゼロ以下

			// 何もしてない状態に設定
			m_VibrationState[nCntPlayer] = VIBRATION_STATE_NONE;

			// メモリ初期化
			memset(&m_aGamepadStateVib[nCntPlayer], 0, sizeof(XINPUT_VIBRATION));
		}
		
		if (m_VibrationState[nCntPlayer] != VIBRATION_STATE_NONE)
		{// ダメージの時

			if (m_VibrationState[nCntPlayer] == VIBRATION_STATE_ITEM)
			{// アイテムの時は増えていく

				m_aGamepadStateVib[nCntPlayer].wLeftMotorSpeed += (USHRT_MAX / m_nMaxCntVibration[nCntPlayer]);
				m_aGamepadStateVib[nCntPlayer].wRightMotorSpeed += (USHRT_MAX / m_nMaxCntVibration[nCntPlayer]);
			}
			else
			{
				m_aGamepadStateVib[nCntPlayer].wLeftMotorSpeed -= (USHRT_MAX / m_nMaxCntVibration[nCntPlayer]);
				m_aGamepadStateVib[nCntPlayer].wRightMotorSpeed -= (USHRT_MAX / m_nMaxCntVibration[nCntPlayer]);
			}

			if (m_aGamepadStateVib[nCntPlayer].wLeftMotorSpeed <= 0)
			{// スピードが0以下

				m_aGamepadStateVib[nCntPlayer].wLeftMotorSpeed = 0;
			}
			else if (m_aGamepadStateVib[nCntPlayer].wRightMotorSpeed <= 0)
			{// スピードが0以下

				m_aGamepadStateVib[nCntPlayer].wRightMotorSpeed = 0;
			}
		}


		// コントローラーにバイブの情報をXINPUTに送る
		XInputSetState(nCntPlayer, &m_aGamepadStateVib[nCntPlayer]);
	}

	
	if (GetStickMoveL(0).y == 0)
	{// スティックがもとに戻っているとき

		// カウントを初期化
		m_nLeftStickCount = 0;

		// 判定をOFF
		m_bLeftStickSelect[STICK_Y] = false;
	}
	if (GetStickMoveL(0).x == 0)
	{// スティックがもとに戻っているとき

		// カウントを初期化
		m_nLeftStickCount = 0;

		// 判定をOFF
		m_bLeftStickSelect[STICK_X] = false;
	}

}

//==================================================================================
//バイブの設定処理
//==================================================================================
void CInputGamepad::SetVibration(VIBRATION_STATE VibState, int nCntPlayer)
{
	if (m_bVibrationUse == true)
	{

		// 状態を代入
		m_VibrationState[nCntPlayer] = VibState;

		switch (VibState)
		{
		case VIBRATION_STATE_DMG:

			m_nCntVibration[nCntPlayer] = 15;
			m_nMaxCntVibration[nCntPlayer] = m_nCntVibration[nCntPlayer];
			m_aGamepadStateVib[nCntPlayer].wLeftMotorSpeed = USHRT_MAX * (short)0.8f;
			m_aGamepadStateVib[nCntPlayer].wRightMotorSpeed = USHRT_MAX * (short)0.8f;
			break;

		case VIBRATION_STATE_ITEM:
			m_nCntVibration[nCntPlayer] = 100;
			m_nMaxCntVibration[nCntPlayer] = m_nCntVibration[nCntPlayer];
			m_aGamepadStateVib[nCntPlayer].wLeftMotorSpeed = (USHRT_MAX * (short)0.00001f);
			m_aGamepadStateVib[nCntPlayer].wRightMotorSpeed = (USHRT_MAX * (short)0.00001f);
			break;
		}

		// コントローラーにバイブの情報をXINPUTに送る
		XInputSetState(nCntPlayer, &m_aGamepadStateVib[nCntPlayer]);

	}
}

//==================================================================================
// ゲームパッドのバイブ設定処理
//==================================================================================
void CInputGamepad::SetEnableVibration(void)
{
	//切り替え
	m_bVibrationUse ^= true;
}

//==================================================================================
// ゲームパッドのバイブ設定処理
//==================================================================================
bool CInputGamepad::GetEnableVibration(void)
{
	return m_bVibrationUse;
}

//==================================================================================
// ゲームパッドのプレス処理
//==================================================================================
bool CInputGamepad::GetPress(BUTTON nKey, int nCntPlayer)
{
	return (m_aGamepadState[nCntPlayer].Gamepad.wButtons & (0x01 << nKey)) ? true : false;
}

//==================================================================================
// ゲームパッドのトリガー処理
//==================================================================================
bool CInputGamepad::GetTrigger(BUTTON nKey, int nCntPlayer)
{
	return (m_aGamepadStateTrigger[nCntPlayer].Gamepad.wButtons & (0x01 << nKey)) ? true : false;
}

//==================================================================================
// ゲームパッドのリリース処理
//==================================================================================
bool CInputGamepad::GetRelease(int nKey, int nCntPlayer)
{
	return (m_aGamepadStateRelease[nCntPlayer].Gamepad.wButtons & (0x01 << nKey)) ? true : false;
}

//==================================================================================
// ゲームパッドのリピート処理
//==================================================================================
bool CInputGamepad::GetRepeat(BUTTON nKey, int nCntPlayer)
{
	return (m_aGamepadStateRepeat[nCntPlayer].Gamepad.wButtons & (0x01 << nKey)) ? true : false;
}

//==================================================================================
// ゲームパッドのプレス処理,LT
//==================================================================================
bool CInputGamepad::GetPressLTrigger(BUTTON nKey, int nCntPlayer)
{
	return (m_aGamepadState[nCntPlayer].Gamepad.bLeftTrigger & (0x01 << nKey)) ? true : false;
}

//==================================================================================
// ゲームパッドのプレス処理,RT
//==================================================================================
bool CInputGamepad::GetPressRTrigger(BUTTON nKey, int nCntPlayer)
{
	return (m_aGamepadState[nCntPlayer].Gamepad.bRightTrigger & (0x01 << nKey)) ? true : false;
}

//==================================================================================
//  Lスティックの移動量取得
//==================================================================================
D3DXVECTOR3 CInputGamepad::GetStickMoveL(int nCntPlayer)
{

	// Lスティックの移動量
	D3DXVECTOR3 StickMoveL = 
		D3DXVECTOR3(
			(float)m_aGamepadState[nCntPlayer].Gamepad.sThumbLX * 0.001f,
			(float)m_aGamepadState[nCntPlayer].Gamepad.sThumbLY * 0.001f,
			0.0f);

	return StickMoveL;
}

//==================================================================================
//  Rスティックの移動量取得
//==================================================================================
D3DXVECTOR3 CInputGamepad::GetStickMoveR(int nCntPlayer)
{

	// Rスティックの移動量
	D3DXVECTOR3 StickMoveR =
		D3DXVECTOR3(
			(float)m_aGamepadState[nCntPlayer].Gamepad.sThumbRX * 0.001f,
			(float)m_aGamepadState[nCntPlayer].Gamepad.sThumbRY * 0.001f,
			0.0f);

	return StickMoveR;
}


//==================================================================================
//  Lスティックの割合取得
//==================================================================================
D3DXVECTOR3 CInputGamepad::GetStickPositionRatioL(int nCntPlayer)
{
	// Lスティックの割合
	D3DXVECTOR3 StickRatio = 
		D3DXVECTOR3(
		(float)m_aGamepadState[nCntPlayer].Gamepad.sThumbLX / (float)SHRT_MAX,
		(float)m_aGamepadState[nCntPlayer].Gamepad.sThumbLY / (float)SHRT_MAX,
		0.0f);

	return StickRatio;
}

//==================================================================================
//  Rスティックの割合取得
//==================================================================================
D3DXVECTOR3 CInputGamepad::GetStickPositionRatioR(int nCntPlayer)
{
	// Lスティックの割合
	D3DXVECTOR3 StickRatio =
		D3DXVECTOR3(
		(float)m_aGamepadState[nCntPlayer].Gamepad.sThumbRX / (float)SHRT_MAX,
			(float)m_aGamepadState[nCntPlayer].Gamepad.sThumbRY / (float)SHRT_MAX,
			0.0f);

	return StickRatio;
}

//==================================================================================
// ゲームパッドのスティックの向き取得
//==================================================================================
float CInputGamepad::GetStickRotL(int nCntPlayer)
{
	// 角度を求める
	float fRot = atan2f(
		((float)m_aGamepadState[nCntPlayer].Gamepad.sThumbLX - 0.0f),
		((float)m_aGamepadState[nCntPlayer].Gamepad.sThumbLY - 0.0f));

	return fRot;
}

//==================================================================================
// ゲームパッドのスティックの向き取得
//==================================================================================
float CInputGamepad::GetStickRotR(int nCntPlayer)
{
	// 角度を求める
	float fRot = atan2f(
		((float)m_aGamepadState[nCntPlayer].Gamepad.sThumbRX - 0.0f),
		((float)m_aGamepadState[nCntPlayer].Gamepad.sThumbRY - 0.0f));

	return fRot;
}

//==================================================================================
// ゲームパッドのLトリガー
//==================================================================================
BYTE CInputGamepad::GetLeftTriggerPress(int nPlayer)
{
	return m_aGamepadState[nPlayer].Gamepad.bLeftTrigger;
}

//==================================================================================
// ゲームパッドのRトリガー
//==================================================================================
BYTE CInputGamepad::GetRightTriggerPress(int nPlayer)
{
	return m_aGamepadState[nPlayer].Gamepad.bRightTrigger;
}

//==================================================================================
// 左スティックの判定を取得
//==================================================================================
bool CInputGamepad::GetStickSelect(int nXY)
{
	return m_bLeftStickSelect[nXY];
}

//==================================================================================
// 左スティックの判定
//==================================================================================
void CInputGamepad::SetEnableStickSelect(bool bStick, int nXY)
{
	m_bLeftStickSelect[nXY] = bStick;
}

//==================================================================================
// パッドのリピートFPS
//==================================================================================
int CInputGamepad::GetnCntPad(void)
{
	return m_nCntPadrepeat;
}

#if 1

//==================================================================================
// コンストラクタ
//==================================================================================
CInputMouse::CInputMouse()
{
	memset(&m_MouseState, 0, sizeof(DIMOUSESTATE2));
	memset(&m_MouseStateTrigger, 0, sizeof(XINPUT_STATE));
	memset(&m_MouseStateRerease, 0, sizeof(XINPUT_STATE));
	memset(&m_MouseStateRepeat, 0, sizeof(XINPUT_STATE));

	for (int nCnt = 0; nCnt < 8; nCnt++)
	{
		aOldState[nCnt] = NULL;	// 前回の入力情報
	}
}

//==================================================================================
// デストラクタ
//==================================================================================
CInputMouse::~CInputMouse()
{

}

//==========================================
//  マウスの初期化
//==========================================
HRESULT CInputMouse::Init(HINSTANCE hInstance, HWND hWnd)
{
	// 初期化処理
	CInput::Init(hInstance, hWnd);

	// 入力デバイスの設定
	if (FAILED(m_pInput->CreateDevice(GUID_SysMouse, &m_pDevice, NULL)))
	{
		return E_FAIL;
	}

	// データフォーマットを設定
	if (FAILED(m_pDevice->SetDataFormat(&c_dfDIMouse2)))
	{
		return E_FAIL;
	}

	// 協調モードを設定
	if (FAILED(m_pDevice->SetCooperativeLevel(hWnd, (DISCL_FOREGROUND | DISCL_NONEXCLUSIVE))))
	{
		return E_FAIL;
	}

	// デバイスの設定
	DIPROPDWORD diprop;
	diprop.diph.dwSize = sizeof(diprop);
	diprop.diph.dwHeaderSize = sizeof(diprop.diph);
	diprop.diph.dwObj = 0;
	diprop.diph.dwHow = DIPH_DEVICE;
	diprop.dwData = DIPROPAXISMODE_REL; // 相対値モードで設定(絶対値はDIPROPAXISMODE_ABS)

	if (FAILED(m_pDevice->SetProperty(DIPROP_AXISMODE, &diprop.diph)))
	{
		// デバイスの設定に失敗
		return E_FAIL;
	}

	// マウスへのアクセス権を取得
	m_pDevice->Acquire();

	// マウスカーソルの消去
	//ShowCursor(false);

	return S_OK;
}

//==========================================
// マウスの終了処理
//==========================================
void CInputMouse::Uninit(void)
{
	// 終了処理
	CInput::Uninit();
}

//==========================================
// マウスの更新処理
//==========================================
void CInputMouse::Update(void)
{
	DIMOUSESTATE2 mouse; // マウスの入力情報

	// 入力デバイスからデータを取得
	if (SUCCEEDED(m_pDevice->GetDeviceState(sizeof(mouse), &mouse)))
	{
		//入力情報の保存
		m_MouseState = mouse;
	}
	else
	{
		// マウスへのアクセス権を獲得
		m_pDevice->Acquire();
	}
}

//==========================================
// マウスのプレス情報
//==========================================
bool CInputMouse::GetPress(BUTTON nKey)
{
	return (m_MouseState.rgbButtons[nKey] & 0x80) ? true : false;
}

//==========================================
// マウスの移動量
//==========================================
D3DXVECTOR3 CInputMouse::GetMouseMove(void)
{
	return D3DXVECTOR3(((float)m_MouseState.lX) * MOUSE_SENS, (-(float)m_MouseState.lY) * MOUSE_SENS, (-(float)m_MouseState.lZ) * MOUSE_SENS);
}

#endif

