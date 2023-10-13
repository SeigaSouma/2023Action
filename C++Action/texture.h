//=============================================================================
// 
// テクスチャヘッダー [texture.h]
//  Author : 相馬靜雅
// 
//=============================================================================

#ifndef _TEXTURE_H_
#define _TEXTURE_H_	// 二重インクルード防止

#include "main.h"
#include "object.h"
#include "constans.h"

class CObjectX;

//==========================================================================
// クラス定義
//==========================================================================
//テクスチャクラス定義
class CTexture
{
public:

	CTexture();
	~CTexture();

private:
	struct STexture
	{
		LPDIRECT3DTEXTURE9 pTexture;	// テクスチャのポインタ
		D3DXIMAGE_INFO imageInfo;		// テクスチャ素材情報
		char acFilename[mylib_const::MAX_STRING];	// ファイル名
		int nFileNameLen;				// ファイル名の文字数
	};

public:

	static HRESULT LoadAll(void);

	HRESULT Load(void);
	void Unload(void);
	int Regist(const char *pFileName);
	LPDIRECT3DTEXTURE9 GetAdress(int nIdx);
	D3DXVECTOR2 GetImageSize(int nIdx);		// テクスチャ素材のサイズ取得

	static int GetNumAll(void);		// 読み込んだ総数
	STexture GetTextureInfo(const char *pFileName);	// テクスチャ情報取得
	STexture GetTextureInfo(int nIdxTex);	// テクスチャ情報取得


private:
	HRESULT LoadTex(const char *pFileName);

	static int m_nNumAll;		// 読み込んだ総数
	STexture m_pTexInfo[mylib_const::MAX_OBJ];	// テクスチャの情報
};



#endif