//=============================================================================
//
// �p�[�e�B�N������ [particle.h]
// Author : 
//
//=============================================================================
#pragma once


//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define	MAX_PARTICLE				(1200)		// �p�[�e�B�N���ő吔
#define	PARTICLE_COLLISION_SIZE_X	(5.0f)		// �����蔻��̑傫��
#define	PARTICLE_COLLISION_SIZE_Y	(5.0f)		// �����蔻��̑傫��


//*****************************************************************************
// �\���̒�`
//*****************************************************************************

// �p�[�e�B�N���̎��
enum
{
	PARTICLE_TYPE_EXPLOSION,			// �q�b�g�E���j
	PARTICLE_TYPE_MAX,
};

struct PARTICLE_TYPE
{
	int texIdx;		// �e�N�X�`���ԍ�

	// [0]���ŏ��l�A[1]���ő�l
	int		cnt[2];		// ������

	int		life[2];	// ����

	float	sclX[2];	// �g�kX
	float	sclY[2];	// �g�kY
	float	sclZ[2];	// �g�kZ

	float	spdX[2];	// ���xX
	float	spdY[2];	// ���xY
	float	spdZ[2];	// ���xZ

	float	pop[2];		// �\���^�C�~���O

	int		digits;		// �����_�ȉ�
	XMFLOAT4	col;	// �F
};


// �p�[�e�B�N���\����
struct PARTICLE
{
	XMFLOAT3		pos;			// �ʒu
	XMFLOAT3		rot;			// ��]
	XMFLOAT3		scl;			// �g�k
	XMFLOAT3		spd;			// �ړ���

	MATERIAL		mat;			// �}�e���A��

	XMFLOAT2		size;			// �T�C�Y

	int				life;			// ����

	int				type;			// ���

	float			pop;			// �\���^�C�~���O

	BOOL			inUse;			// �g�p���t���O
};

//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
HRESULT InitParticle(void);
void UninitParticle(void);
void UpdateParticle(void);
void DrawParticle(void);

void SetParticle(int type, XMFLOAT3 pos);
PARTICLE *GetParticle();
