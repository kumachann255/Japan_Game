//=============================================================================
//
// �R���{���� [combo.h]
// Author : 
//
//=============================================================================
#pragma once


//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define COMBO_MAX			(99)		// �X�R�A�̍ő�l
#define COMBO_DIGIT			(2)			// ����




//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
HRESULT InitCombo(void);
void UninitCombo(void);
void UpdateCombo(void);
void DrawCombo(void);

void AddCombo(int add);
int GetCombo(void);
void ResetComboTime(void);

