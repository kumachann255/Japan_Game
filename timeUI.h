//=============================================================================
//
// タイム処理 [timeUI.h]
// Author : 
//
//=============================================================================
#pragma once


//*****************************************************************************
// マクロ定義
//*****************************************************************************
// #define TIME_MAX			(99999)		// タイムの最大値
#define TIME_DIGIT			(2)			// 桁数




//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
HRESULT InitTime(void);
void UninitTime(void);
void UpdateTime(void);
void DrawTime(void);

void AddTime(int add);
int GetTime(void);


