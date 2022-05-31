//=============================================================================
//
// ふきだし処理 [speech.h]
// Author : 熊澤義弘
//
//=============================================================================
#pragma once


//*****************************************************************************
// マクロ定義
//*****************************************************************************


struct SPEECH
{
	BOOL		use;
	XMFLOAT3	pos;
	float		w;
	float		h;
	int			texNo;
	int			life;
	BOOL		combo;

};

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
HRESULT InitSpeech(void);
void UninitSpeech(void);
void UpdateSpeech(void);
void DrawSpeech(void);

void SetStartSpeech(void);
void SetBomSpeech(void);
void SetComboSpeech(void);
void SetDamageSpeech(void);

