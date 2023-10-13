//=============================================================================
//
// 入力処理 [input.h]
// Author : 相馬靜雅
//
//=============================================================================

#ifndef _INPUT_H_	//このマクロ定義がされていなかったら
#define _INPUT_H_	//二重インクルード防止のマクロを定義する

#include "main.h"
#pragma comment(lib, "xinput.lib")	//コントローラーに必要

//==========================================================================
// マクロ定義
//==========================================================================
#define NUM_KEY_MAX	(256)	// キーの最大数
#define MAX_PLAYER	(1)		// 最大プレイヤー

//==========================================================================
// クラス定義
//==========================================================================
// 入力クラス
class CInput
{
public:
	CInput();
	virtual ~CInput();

	virtual HRESULT Init(HINSTANCE hInstance, HWND hWnd);
	virtual void Uninit(void);
	virtual void Update(void) = 0;

protected:
	static int nNumAll;					// 総数
	static LPDIRECTINPUT8 m_pInput;		// DirectInputオブジェクトへのポインタ
	LPDIRECTINPUTDEVICE8 m_pDevice;		// 入力デバイスへのポインタ
};

// キーボードクラス
class CInputKeyboard : public CInput
{
public:

	CInputKeyboard();
	~CInputKeyboard();

	HRESULT Init(HINSTANCE hInstance, HWND hWnd);
	void Uninit(void);
	void Update(void);
	bool GetPress(int nKey);
	bool GetTrigger(int nKey);
	bool GetRelease(int nKey);
	bool GetRepeat(int nKey, int nPattern);

private:
	int m_nPatternRepeat[NUM_KEY_MAX];		// リピ―トの間隔
	int m_nCntKeyRepeat[NUM_KEY_MAX];		// リピート用カウント
	BYTE m_aKeyState[NUM_KEY_MAX];			// プレス情報
	BYTE m_aKeyStateTrigger[NUM_KEY_MAX];	// トリガー情報
	BYTE m_aKeyStateRelease[NUM_KEY_MAX];	// リリース情報
	BYTE m_aKeyStateRepeat[NUM_KEY_MAX];	// リピート情報
};

// ジョイパッドクラス
class CInputGamepad
{
public:

	// 列挙型定義
	typedef enum
	{
		BUTTON_UP = 0,
		BUTTON_DOWN,
		BUTTON_LEFT,
		BUTTON_RIGHT,
		BUTTON_START,
		BUTTON_BACK,
		BUTTON_LSTICKPUSH,
		BUTTON_RSTICKPUSH,
		BUTTON_LB,
		BUTTON_RB,
		BUTTON_LT,
		BUTTON_RT,
		BUTTON_A,
		BUTTON_B,
		BUTTON_X,
		BUTTON_Y,
		BUTTON_LX,
		BUTTON_LY,
		BUTTON_18,
		BUTTON_19,
		BUTTON_MAX
	}BUTTON;

	// 列挙型定義
	enum STICK
	{
		STICK_X = 0,
		STICK_Y,
		STICK_MAX
	};

	typedef enum
	{
		VIBRATION_STATE_NONE = 0,	//何もしてない状態
		VIBRATION_STATE_DMG,		//ダメージ状態
		VIBRATION_STATE_ITEM,		//アイテム

	}VIBRATION_STATE;

	CInputGamepad();
	~CInputGamepad();

	HRESULT Init(HINSTANCE hInstance, HWND hWnd);
	void Uninit(void);
	void Update(void);
	bool GetPress(BUTTON nKey, int nCntPlayer);
	bool GetPressLTrigger(BUTTON nKey, int nCntPlayer);
	bool GetPressRTrigger(BUTTON nKey, int nCntPlayer);
	bool GetTrigger(BUTTON nKey, int nCntPlayer);
	bool GetRepeat(BUTTON nKey, int nCntPlayer);
	bool GetRelease(int nKey, int nCntPlayer);
	D3DXVECTOR3 GetStickMoveL(int nCntPlayer);
	D3DXVECTOR3 GetStickMoveR(int nCntPlayer);
	D3DXVECTOR3 GetStickPositionRatioL(int nCntPlayer);
	D3DXVECTOR3 GetStickPositionRatioR(int nCntPlayer);
	float GetStickRotL(int nCntPlayer);					// スティックの向き取得
	float GetStickRotR(int nCntPlayer);					// スティックの向き取得
	BYTE GetLeftTriggerPress(int nPlayer);
	BYTE GetRightTriggerPress(int nPlayer);
	void SetVibration(VIBRATION_STATE VibState, int nCntPlayer);
	void SetEnableStickSelect(bool bStick, int nXY);
	bool GetStickSelect(int nXY);
	void SetEnableVibration(void);
	bool GetEnableVibration(void);
	int GetnCntPad(void);

private:
	XINPUT_STATE m_aGamepadState[MAX_PLAYER];				// プレス情報
	XINPUT_STATE m_aGamepadStateTrigger[MAX_PLAYER];		// トリガー情報
	XINPUT_STATE m_aGamepadStateRepeat[MAX_PLAYER];			// リピート情報
	XINPUT_STATE m_aGamepadStateRelease[MAX_PLAYER];		// リリース情報
	XINPUT_VIBRATION m_aGamepadStateVib[MAX_PLAYER];		// バイブレーション
	VIBRATION_STATE m_VibrationState[MAX_PLAYER];			// 振動の種類
	int m_nCntVibration[MAX_PLAYER];						// 振動の時間
	int m_nMaxCntVibration[MAX_PLAYER];						// 振動の時間
	int m_nCntPadrepeat;									// リピート用カウント
	int m_nLeftStickCount;									// 左トリガーの選択カウント
	bool m_bLeftStickSelect[STICK_MAX];								// 左トリガーの選択判定
	bool m_bVibrationUse;									// バイブを使用するかどうか
};

// マウスクラス
class CInputMouse : public CInput
{
public:

	// 列挙型定義
	typedef enum
	{
		BUTTON_LEFT = 0,
		BUTTON_RIGHT,
		BUTTON_WHEEL,
		BUTTON_SIDE_1,
		BUTTON_SIDE_2
	}BUTTON;

	CInputMouse();
	~CInputMouse();

	HRESULT Init(HINSTANCE hInstance, HWND hWnd);
	void Uninit(void);
	void Update(void);
	bool GetPress(BUTTON nKey);
	D3DXVECTOR3 GetMouseMove(void);

private:
	DIMOUSESTATE2 m_MouseState;			//全入力情報の保管
	DIMOUSESTATE2 m_MouseStateTrigger;	//トリガー情報
	DIMOUSESTATE2 m_MouseStateRerease;	//リリース情報
	DIMOUSESTATE2 m_MouseStateRepeat;	//リピート情報
	BYTE aOldState[8];					//前回の入力情報を保存
};


typedef enum
{
	STICK_0 = 0,
	STICK_MAX
}STICK;

#endif