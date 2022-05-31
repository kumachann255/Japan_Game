//=============================================================================
//
// スコア処理 [score.h]
// Author : 熊澤義弘
//
//=============================================================================
#pragma once


//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define SCORE_MAX			(99999)		// スコアの最大値
#define SCORE_DIGIT			(5)			// ステージ桁数
#define COMBO_MAX_DIGIT		(3)			// 最大コンボの桁数

#define SCORE_STAGE0_BORDER	(4000)		// ステージ0のクリアスコア
#define SCORE_STAGE1_BORDER	(8000)		// ステージ1のクリアスコア
#define SCORE_STAGE2_BORDER	(16000)		// ステージ2のクリアスコア
#define SCORE_STAGE3_BORDER	(32000)		// ステージ3のクリアスコア

//#define SCORE_STAGE0_BORDER	(200)		// ステージ0のクリアスコア
//#define SCORE_STAGE1_BORDER	(200)		// ステージ1のクリアスコア
//#define SCORE_STAGE2_BORDER	(200)		// ステージ2のクリアスコア
//#define SCORE_STAGE3_BORDER	(200)		// ステージ3のクリアスコア


//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
HRESULT InitScore(void);
void UninitScore(void);
void UpdateScore(void);
void DrawScore(void);

void AddScore(int add);
int GetScore(void);


