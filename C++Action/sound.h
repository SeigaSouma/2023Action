//=============================================================================
//
// サウンド処理 [sound.h]
// Author : 相馬靜雅
//
//=============================================================================
#ifndef _SOUND_H_
#define _SOUND_H_

#include "main.h"

//==========================================================================
// クラス定義
//==========================================================================
// サウンドクラス
class CSound
{
public:

	// 列挙型定義
	typedef enum
	{
		LABEL_BGM_TITLE = 0,	// タイトル
		LABEL_BGM_GAME,		// タイトル
		LABEL_BGM_RESULT,	// リザルト
		LABEL_SE_EXPLOSION,	// 爆発
		LABEL_SE_KYUPIIN,	// キュピーン
		LABEL_SE_WALK,		// 歩行
		LABEL_SE_JUMP,		// ジャンプ
		LABEL_SE_FIELD,		// 地面
		LABEL_SE_SWING,		// スイング
		LABEL_SE_IMPACT01,	// 衝撃
		LABEL_SE_DMG01,		// ダメージ
		LABEL_SE_COMBOIN,		// ダメージ
		LABEL_SE_TUTORIALWINDOW,		// ダメージ
		LABEL_SE_SPAWN,		// ダメージ
		LABEL_SE_SPAWNEN,		// ダメージ
		LABEL_SE_PLAYERDMG,		// ダメージ
		LABEL_SE_PLAYERDMG_BIG,		// ダメージ
		LABEL_SE_ENEMYDMG_SMALL,		// ダメージ
		LABEL_SE_ENEMYDMG_NORMAL,		// ダメージ
		LABEL_SE_ENEMYDMG_BIG,		// ダメージ
		LABEL_SE_COMBOKILL,		// ダメージ
		LABEL_SE_CONFUSION,		// ダメージ
		LABEL_SE_NUMBER,		// ダメージ
		LABEL_SE_NUMBERMOVE,		// ダメージ
		LABEL_SE_CURSOR,		// カーソル移動
		LABEL_SE_DONE,		// カーソル移動
		LABEL_SE_STEPCLEAR,		// カーソル移動
		LABEL_SE_SLASHHIT,		// 斬撃ヒット
		LABEL_SE_BOUNCE,		// 跳ね返し
		LABEL_SE_BOUNCEHIT,		// 跳ね返しヒット
		LABEL_SE_ENEMYEXPLOSION,		// 敵爆発
		LABEL_SE_TITLESLASH,		// タイトル斬撃
		LABEL_BGM_HOBARING,		// ホバリング
		LABEL_SE_ASSULT,		// 突進
		LABEL_SE_ASSULTEND,		// 突進終了
		LABEL_SE_STUN,			// スタン
		LABEL_SE_STUNHEAL,		// スタン回復
		LABEL_MAX,
	} LABEL;

	typedef struct
	{
		char *pFilename;	// ファイル名
		int nCntLoop;		// ループカウント
	} SOUNDINFO;

	CSound();
	~CSound();

	HRESULT Init(HWND hWnd);
	void Uninit(void);
	HRESULT PlaySound(LABEL label);
	void StopSound(void);
	void StopSound(LABEL label);
	void VolumeChange(float fVolume);
	int GetVolume(void);

private:
	HRESULT CheckChunk(HANDLE hFile, DWORD format, DWORD *pChunkSize, DWORD *pChunkDataPosition);
	HRESULT ReadChunkData(HANDLE hFile, void *pBuffer, DWORD dwBuffersize, DWORD dwBufferoffset);

	IXAudio2 *m_pXAudio2;								// XAudio2オブジェクトへのインターフェイス
	IXAudio2MasteringVoice *m_pMasteringVoice;			// マスターボイス
	IXAudio2SourceVoice *m_apSourceVoice[LABEL_MAX];	// ソースボイス
	BYTE *m_apDataAudio[LABEL_MAX];						// オーディオデータ
	DWORD m_aSizeAudio[LABEL_MAX];						// オーディオデータサイズ
	DSBUFFERDESC SetVolume;								// 音量調整
	float m_fVolume;									// 音量
	static SOUNDINFO m_aSoundInfo[LABEL_MAX];			// サウンドの情報
};


#endif
