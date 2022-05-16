//=============================================================================
//
// �G�l�~�[���f������ [enemy.h]
// Author : 
//
//=============================================================================
#pragma once


//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define MAX_ENEMY		(50)					// �G�l�~�[�̐�

#define	ENEMY_SIZE		(30.0f)				// �����蔻��̑傫��
#define ENEMY_OFFSET_Y		(7.0f)			// �G�l�~�[�̑��������킹��


//*****************************************************************************
// �\���̒�`
//*****************************************************************************
struct ENEMY
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
	
	INTERPOLATION_DATA	*tbl_adr;			// �A�j���f�[�^�̃e�[�u���擪�A�h���X
	int					tbl_size;			// �o�^�����e�[�u���̃��R�[�h����
	float				move_time;			// ���s����

	float				zGoal;				// z���W�̃v���C���[�ɋ߂Â��ő勗��
	
	XMFLOAT3			hitPos;				// �����̍��W
	XMFLOAT3			hitSpd;				// �����蔻���A�j���[�V�����p�A����̈ړ���
	BOOL				isHit;				// �������Ă�t���O
	float				hitMove;			// �����蔻���A�j���[�V�����p�A�ړ��X�s�[�h

	int					liveCount;			// �|�b�v���Ă���̎��Ԃ��Ǘ�

};

//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
HRESULT InitEnemy(void);
void UninitEnemy(void);
void UpdateEnemy(void);
void DrawEnemy(void);

ENEMY *GetEnemy(void);

