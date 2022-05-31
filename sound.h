//=============================================================================
//
// �T�E���h���� [sound.h]
// Author : ��ˏ���
//
//=============================================================================
#pragma once

#include <windows.h>
#include "xaudio2.h"						// �T�E���h�����ŕK�v

//*****************************************************************************
// �T�E���h�t�@�C��
//*****************************************************************************
enum 
{
	// BGM
	SOUND_LABEL_BGM_bgm_title,		// �^�C�g��BGM
	SOUND_LABEL_BGM_bgm_stage001,	// �X�e�[�WBGM1
	SOUND_LABEL_BGM_bgm_stage002,	// �X�e�[�WBGM2
	SOUND_LABEL_BGM_bgm_endroll,	// �G���h���[��BGM


	//// SE
	SOUND_LABEL_SE_booingSound01,		// �u�[�C���O��
	SOUND_LABEL_SE_carHorn01,			// �Ԃ̃N���N�V������
	SOUND_LABEL_SE_cheers00,			// ������0
	SOUND_LABEL_SE_cheers01,			// ������1
	SOUND_LABEL_SE_cheers02,			// ������2
	SOUND_LABEL_SE_cheers03,			// ������3
	SOUND_LABEL_SE_cheers04,			// ������4
	SOUND_LABEL_SE_cheers05,			// ������5
	SOUND_LABEL_SE_collisionSound01,	// �Ԃ̏Փˉ�
	SOUND_LABEL_SE_comboSound01,		// �R���{�̉��i�ł���Δ��e�j�􎞂̉��ƌ����j
	SOUND_LABEL_SE_countdownSound01,	// 10�b�O�J�E���g�_�E��
	SOUND_LABEL_SE_damagedSound01,		// �_���[�W��
	SOUND_LABEL_SE_explosionSound01,	// �S�����̔�����
	SOUND_LABEL_SE_gunShot01,			// �e��1
	SOUND_LABEL_SE_gunShot02,			// �e��2
	SOUND_LABEL_SE_hitSound01,			// �q�b�g��
	SOUND_LABEL_SE_propellerSound01,	// �v���y����
	SOUND_LABEL_SE_selectBomb01,		// �^�C�g���̑I����
	SOUND_LABEL_SE_shrinkingSound01,	// �S�����̎��k��
	SOUND_LABEL_SE_siren01,				// �p�g�J�[��
	SOUND_LABEL_SE_stickingSound01,		// �S�����̒���t����
	SOUND_LABEL_SE_stirringSound01,		// �S����������
	SOUND_LABEL_SE_throwingSound01,		// �S����������
	SOUND_LABEL_SE_titleClick01,		// �^�C�g�����[�h�I����1
	SOUND_LABEL_SE_titleClick02,		// �^�C�g�����[�h�I����2
	SOUND_LABEL_SE_titleClick03,		// �^�C�g�����[�h�I����3
	SOUND_LABEL_SE_titleClick04,		// �^�C�g�����[�h�I����4


	SOUND_LABEL_MAX,	// MAX
};

//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
BOOL InitSound(HWND hWnd);
void UninitSound(void);
void PlaySound(int label);
void StopSound(int label);
void StopSound(void);
void SetSouceVoice(int label, float volume);

void SetMasterVolume(float volume);
void SetSourceVolume(int label, float volume);
