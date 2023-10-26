//=============================================================================
// 
//  ステージヘッダー [stage.h]
//  Author : 相馬靜雅
// 
//=============================================================================

#ifndef _STAGE_H_
#define _STAGE_H_	// 二重インクルード防止

#include "main.h"
#include "object.h"

//==========================================================================
// クラス定義
//==========================================================================
// ステージクラス定義
class CStage
{
public:

	CStage();
	~CStage();

	HRESULT Init(void);
	void Uninit(void);
	void Release(void);
	void Update(void);
	void Draw(void);

	CObjectX *GetObj(int nIdx);
	int GetNumAll(void);		// 総数取得
	static CStage *Create(const char *pTextFile);

private:

	// メンバ関数
	HRESULT ReadXFile(const char *pTextFile);	// もでる読み込み処理
	HRESULT ReadText(const char *pTextFile);		// 外部ファイル読み込み処理
	HRESULT SaveText(void);		// 外部ファイル書き込み処理

	// メンバ変数
	int m_nNumAll;
	std::vector<std::string> ModelFile;		// モデルファイル名
	CObjectX *m_pObjX[mylib_const::MAX_STAGE];	// オブジェクトXのポインタ

};


#endif