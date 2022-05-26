//=============================================================================
//
// �ӂ��������� [speech.h]
// Author : 
//
//=============================================================================
#pragma once


//*****************************************************************************
// �}�N����`
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
// �v���g�^�C�v�錾
//*****************************************************************************
HRESULT InitSpeech(void);
void UninitSpeech(void);
void UpdateSpeech(void);
void DrawSpeech(void);

void SetStartSpeech(void);
void SetBomSpeech(void);
void SetComboSpeech(void);
void SetDamageSpeech(void);

