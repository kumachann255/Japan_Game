//=============================================================================
//
// �X�R�A���� [score.h]
// Author : 
//
//=============================================================================
#pragma once


//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define SCORE_MAX			(99999)		// �X�R�A�̍ő�l
#define SCORE_DIGIT			(5)			// �X�e�[�W����
#define COMBO_MAX_DIGIT		(3)			// �ő�R���{�̌���

//#define SCORE_STAGE0_BORDER	(2000)		// �X�e�[�W0�̃N���A�X�R�A
//#define SCORE_STAGE1_BORDER	(3000)		// �X�e�[�W1�̃N���A�X�R�A
//#define SCORE_STAGE2_BORDER	(4000)		// �X�e�[�W2�̃N���A�X�R�A
//#define SCORE_STAGE3_BORDER	(5000)		// �X�e�[�W3�̃N���A�X�R�A

#define SCORE_STAGE0_BORDER	(200)		// �X�e�[�W0�̃N���A�X�R�A
#define SCORE_STAGE1_BORDER	(200)		// �X�e�[�W1�̃N���A�X�R�A
#define SCORE_STAGE2_BORDER	(200)		// �X�e�[�W2�̃N���A�X�R�A
#define SCORE_STAGE3_BORDER	(200)		// �X�e�[�W3�̃N���A�X�R�A


//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
HRESULT InitScore(void);
void UninitScore(void);
void UpdateScore(void);
void DrawScore(void);

void AddScore(int add);
int GetScore(void);


