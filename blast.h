//=============================================================================
//
// ���j�Ɠ����蔻�菈�� [blast.h]
// Author : 
//
//=============================================================================
#pragma once


//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define MAX_BLAST			(5)
#define	BLAST_SIZE			(50.0f)				// �����蔻��̑傫��

#define BLAST_DOWN			(20.0f)						// ���[�t�B���O��ɗ����鍂��
#define BLASE_DOWN_SPEED	(10.0f)						// �����鎞��

//*****************************************************************************
// �\���̒�`
//*****************************************************************************
struct BLAST
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
	int					life;				// ���j�̎c�莞��

	BOOL				shrink;				// �k�܂��Ă��邩

	BOOL				move;				// ���ֈړ����邩�t���O TRUE:�ړ�����
};

//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
HRESULT InitBlast(void);
void UninitBlast(void);
void UpdateBlast(void);
void DrawBlast(void);

BLAST *GetBlast(void);

void SetBlast(XMFLOAT3 pos);
BOOL GetCameraSwitch(void);
void SetCameraSwitch(BOOL data);
int GetMorphing(void);
int GetStopTime(void);
