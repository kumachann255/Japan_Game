//=============================================================================
//
// �G�l�~�[�w�����f������ [enemyHeli.h]
// Author : �ύX
//
//=============================================================================
#pragma once


//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define MAX_ENEMY_HELI		(1)			// �G�l�~�[�w���̐�

#define HELI_PARTS_MAX		(2)			// �G�l�~�[�w���̃p�[�c�̐�

#define	ENEMY_HELI_SIZE		(30.0f)		// �����蔻��̑傫��

#define ENEMY_HELI_OFFSET_Y	(40.0f)		// �G�l�~�[�w���̑��������킹��

//*****************************************************************************
// �\���̒�`
//*****************************************************************************
struct ENEMY_HELI
{
	XMFLOAT4X4			mtxWorld;			// ���[���h�}�g���b�N�X
	XMFLOAT3			pos;				// ���f���̈ʒu
	XMFLOAT3			rot;				// ���f���̌���(��])
	XMFLOAT3			scl;				// ���f���̑傫��(�X�P�[��)

	BOOL				use;
	BOOL				load;
	DX11_MODEL			model;				// ���f�����
	XMFLOAT4			diffuse[MODEL_MAX_MATERIAL];	// ���f���̐F

	float				spd;				// �ړ��X�s�[�h
	float				size;				// �����蔻��̑傫��
	int					shadowIdx;			// �e�̃C���f�b�N�X�ԍ�

	float				zGoal;				// z���W�̃v���C���[�ɋ߂Â��ő勗��

	XMFLOAT3			hitPos;				// �����̍��W
	XMFLOAT3			hitRot;				// �����蔻���A�j���[�V�����p�A����̈ړ���
	BOOL				isHit;				// �������Ă�t���O TRUE:�������Ă���
	BOOL				move;				// ���ֈړ�����t���O TRUE:�ړ�����
	int					hitTime;			// �ړ�����
	int					liveCount;			// �|�b�v���Ă���̎��Ԃ��Ǘ�

	BOOL				fuchi;				// �������C�g�̃I���I�t

	float				speed;				// �ړ��X�s�[�h
	float				time;				// ����
	BOOL				switchFlag;			// �x�W�F�p
	ENEMY_HELI			*parent;			// �G�l�~�[�w���̃p�[�c
};

//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
HRESULT InitEnemyHeli(void);
void UninitEnemyHeli(void);
void UpdateEnemyHeli(void);
void DrawEnemyHeli(void);

ENEMY_HELI *GetEnemyHeli(void);

