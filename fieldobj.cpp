//=============================================================================
//
// �t�B�[���h�I�u�W�F�N�g�̏��� [fieldobj.cpp]
// Author : �F�V�`�O
//
//=============================================================================
#include "main.h"
#include "renderer.h"
#include "camera.h"
#include "input.h"
#include "debugproc.h"
#include "model.h"
#include "fieldobj.h"
#include "meshfield.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define	MODEL_BUILDING			"data/MODEL/biru.obj"		// �ǂݍ��ރ��f����
#define	MODEL_GUARDRAIL			"data/MODEL/guardrail.obj"	// �ǂݍ��ރ��f����
#define	MODEL_POLE				"data/MODEL/denchu.obj"		// �ǂݍ��ރ��f����
#define	MODEL_HOUSE				"data/MODEL/house.obj"		// �ǂݍ��ރ��f����
#define	MODEL_SIGN00			"data/MODEL/sign00.obj"		// �ǂݍ��ރ��f����
#define	MODEL_SIGN01			"data/MODEL/sign01.obj"		// �ǂݍ��ރ��f����

#define	VALUE_ROTATE			(XM_PI * 0.02f)				// ��]��

#define FOBJ_X					(430.0f)					// ���ɒu���ꏊ
#define FOBJ_X_HOUSE			(400.0f)					// ���ɒu���ꏊ(�Ƃ̏ꍇ
#define FOBJ_X_GUAD				(180.0f)					// ���ɒu���ꏊ(�K�[�h���[���̏ꍇ
#define FOBJ_X_POLE				(210.0f)					// ���ɒu���ꏊ(�d���̏ꍇ
#define FOBJ_X_SIGN				(210.0f)					// ���ɒu���ꏊ(�d���̏ꍇ
#define FOBF_Z_MAX				(2000.0f)					// �I�u�W�F�N�g������z���W
#define FOBF_Z_START			(-500.0f)					// �I�u�W�F�N�g���o��z���W
#define FOBJ_Y					(50)						// �����̒���(�����_�����E)
#define FOBJ_Y_Pole				(70)						// �d���̍����̒���
#define FOBJ_Y_SIGN				(10)						// �Ŕ̍����̒���

#define FOBJ_DISTANCE			(500.0f)					// �I�u�W�F�N�g�̊Ԋu
#define FOBJ_DISTANCE_GUAD		(100.0f)					// �K�[�h���[���̊Ԋu
#define FOBJ_DISTANCE_SIGN		(1200.0f)					// �Ŕ̊Ԋu

enum {
	left,
	right,
};

//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
void SetBilding(BOOL LR);
void SetHouse(BOOL LR);
void SetGuardrail(BOOL LR);
void SetPole(BOOL LR);
void SetSign0(BOOL LR);
void SetSign1(BOOL LR);


//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
static FOBJ				g_Bilding[MAX_FOBJ];			// �r��
static FOBJ				g_House[MAX_FOBJ];				// ��
static FOBJ				g_Guardrail[MAX_FOBJ_GUAD];		// �K�[�h���[��
static FOBJ				g_Pole[MAX_FOBJ];				// �d��
static FOBJ				g_Sign0[MAX_FOBJ_SIGN];			// �Ŕi���{
static FOBJ				g_Sign1[MAX_FOBJ_SIGN];			// �Ŕi�p��

static BOOL				g_Load = FALSE;


//=============================================================================
// ����������
//=============================================================================
HRESULT InitTree(void)
{
	LoadModel(MODEL_BUILDING, &g_Bilding[0].model);
	LoadModel(MODEL_HOUSE, &g_House[0].model);
	LoadModel(MODEL_GUARDRAIL, &g_Guardrail[0].model);
	LoadModel(MODEL_POLE, &g_Pole[0].model);
	LoadModel(MODEL_SIGN00, &g_Sign0[0].model);
	LoadModel(MODEL_SIGN01, &g_Sign1[0].model);

	for (int i = 0; i < MAX_FOBJ; i++)
	{
		g_Bilding[i].load = TRUE;
		g_Bilding[i].pos = XMFLOAT3(-370.0f, 0.0f, 0.0f);
		g_Bilding[i].rot = XMFLOAT3(0.0f, 1.57f, 0.0f);
		g_Bilding[i].scl = XMFLOAT3(18.0f, 18.0f, 18.0f);
		g_Bilding[i].use = FALSE;			// TRUE:�����Ă�
		// ���f���̃f�B�t���[�Y��ۑ����Ă����B�F�ς��Ή��ׁ̈B
		GetModelDiffuse(&g_Bilding[i].model, &g_Bilding[i].diffuse[0]);
	}

	for (int i = 0; i < MAX_FOBJ; i++)
	{
		g_House[i].load = TRUE;
		g_House[i].pos = XMFLOAT3(-370.0f, 0.0f, 0.0f);
		g_House[i].rot = XMFLOAT3(0.0f, 1.57f, 0.0f);
		g_House[i].scl = XMFLOAT3(11.0f, 11.0f, 11.0f);
		g_House[i].use = FALSE;			// TRUE:�����Ă�
		// ���f���̃f�B�t���[�Y��ۑ����Ă����B�F�ς��Ή��ׁ̈B
		GetModelDiffuse(&g_House[i].model, &g_House[i].diffuse[0]);
	}

	for (int i = 0; i < MAX_FOBJ_GUAD; i++)
	{
		g_Guardrail[i].load = TRUE;
		g_Guardrail[i].pos = XMFLOAT3(-370.0f, 0.0f, 0.0f);
		g_Guardrail[i].rot = XMFLOAT3(0.0f, 1.57f, 0.0f);
		g_Guardrail[i].scl = XMFLOAT3(1.0f, 1.0f, 1.0f);
		g_Guardrail[i].use = FALSE;			// TRUE:�����Ă�
		// ���f���̃f�B�t���[�Y��ۑ����Ă����B�F�ς��Ή��ׁ̈B
		GetModelDiffuse(&g_Guardrail[i].model, &g_Guardrail[i].diffuse[0]);
	}

	for (int i = 0; i < MAX_FOBJ; i++)
	{
		g_Pole[i].load = TRUE;
		g_Pole[i].pos = XMFLOAT3(-370.0f, FOBJ_Y_Pole, 0.0f);
		g_Pole[i].rot = XMFLOAT3(0.0f, 1.57f, 0.0f);
		g_Pole[i].scl = XMFLOAT3(2.5f, 2.5f, 2.5f);
		g_Pole[i].use = FALSE;			// TRUE:�����Ă�
		// ���f���̃f�B�t���[�Y��ۑ����Ă����B�F�ς��Ή��ׁ̈B
		GetModelDiffuse(&g_Pole[i].model, &g_Pole[i].diffuse[0]);
	}

	for (int i = 0; i < MAX_FOBJ_SIGN; i++)
	{
		g_Sign0[i].load = TRUE;
		g_Sign0[i].pos = XMFLOAT3(-FOBJ_X_SIGN, FOBJ_Y_SIGN, 0.0f);
		g_Sign0[i].rot = XMFLOAT3(0.0f, 1.57f, 0.0f);
		g_Sign0[i].scl = XMFLOAT3(1.5f, 1.5f, 1.5f);
		g_Sign0[i].use = FALSE;			// TRUE:�����Ă�
		// ���f���̃f�B�t���[�Y��ۑ����Ă����B�F�ς��Ή��ׁ̈B
		GetModelDiffuse(&g_Sign0[i].model, &g_Sign0[i].diffuse[0]);
	}

	for (int i = 0; i < MAX_FOBJ_SIGN; i++)
	{
		g_Sign1[i].load = TRUE;
		g_Sign1[i].pos = XMFLOAT3(-FOBJ_X_SIGN, FOBJ_Y_SIGN, 0.0f);
		g_Sign1[i].rot = XMFLOAT3(0.0f, 1.57f, 0.0f);
		g_Sign1[i].scl = XMFLOAT3(1.5f, 1.5f, 1.5f);
		g_Sign1[i].use = FALSE;			// TRUE:�����Ă�
		// ���f���̃f�B�t���[�Y��ۑ����Ă����B�F�ς��Ή��ׁ̈B
		GetModelDiffuse(&g_Sign1[i].model, &g_Sign1[i].diffuse[0]);
	}


	// �����̃t�B�[���h�I�u�W�F�N�g�̏����z�u
	// �r��
	for (int i = 0; i < 5; i++)
	{
		g_Bilding[i].use = TRUE;
		g_Bilding[i].pos = XMFLOAT3(-FOBJ_X_HOUSE, 0.0f, FOBF_Z_START + FOBJ_DISTANCE * i);
		g_Bilding[i].rot = XMFLOAT3(0.0f, -1.57f, 0.0f);
	}

	// ��
	for (int i = 0; i < 5; i++)
	{
		g_House[i].use = TRUE;
		g_House[i].pos = XMFLOAT3(-FOBJ_X_HOUSE, 0.0f, FOBF_Z_START + FOBJ_DISTANCE * i + FOBJ_DISTANCE / 2.0f);
		g_House[i].rot = XMFLOAT3(0.0f, -1.57f, 0.0f);

	}

	// �K�[�h���[��
	for (int i = 0; i < 25; i++)
	{
		g_Guardrail[i].use = TRUE;
		g_Guardrail[i].pos = XMFLOAT3(-FOBJ_X_GUAD, 0.0f, FOBF_Z_START + FOBJ_DISTANCE_GUAD * i);
		g_Guardrail[i].rot = XMFLOAT3(0.0f, 3.14f, 0.0f);

	}

	// �d��
	for (int i = 0; i < 5; i++)
	{
		g_Pole[i].use = TRUE;
		g_Pole[i].pos = XMFLOAT3(-FOBJ_X_POLE, FOBJ_Y_Pole, FOBF_Z_START + FOBJ_DISTANCE * i);
		g_Pole[i].rot = XMFLOAT3(0.0f, 3.14f, 0.0f);

	}

	// �Ŕ�
	for (int i = 0; i < 2; i++)
	{
		g_Sign1[i].use = TRUE;
		g_Sign1[i].pos = XMFLOAT3(-FOBJ_X_SIGN, FOBJ_Y_SIGN, FOBF_Z_START + FOBJ_DISTANCE_SIGN * (i + 1) - FOBJ_DISTANCE_SIGN / 2.0f);
		g_Sign1[i].rot = XMFLOAT3(0.0f, 0.0f, 0.0f);

	}


	// �E���̃t�B�[���h�I�u�W�F�N�g�̏����z�u
	// �r��
	for (int i = 0; i < 5; i++)
	{
		g_Bilding[i + 5].use = TRUE;
		g_Bilding[i + 5].pos = XMFLOAT3(FOBJ_X_HOUSE, 0.0f, FOBF_Z_START + FOBJ_DISTANCE * i + FOBJ_DISTANCE / 2.0f);
		g_Bilding[i + 5].rot = XMFLOAT3(0.0f, 1.57f, 0.0f);
	}

	// ��
	for (int i = 0; i < 5; i++)
	{
		g_House[i + 5].use = TRUE;
		g_House[i + 5].pos = XMFLOAT3(FOBJ_X_HOUSE, 0.0f, FOBF_Z_START + FOBJ_DISTANCE * i);
		g_House[i + 5].rot = XMFLOAT3(0.0f, 1.57f, 0.0f);

	}

	// �K�[�h���[��
	for (int i = 0; i < 25; i++)
	{
		g_Guardrail[i + 25].use = TRUE;
		g_Guardrail[i + 25].pos = XMFLOAT3(FOBJ_X_GUAD, 0.0f, FOBF_Z_START + FOBJ_DISTANCE_GUAD * i);
		g_Guardrail[i + 25].rot = XMFLOAT3(0.0f, 0.0f, 0.0f);

	}

	// �d��
	for (int i = 0; i < 5; i++)
	{
		g_Pole[i + 5].use = TRUE;
		g_Pole[i + 5].pos = XMFLOAT3(FOBJ_X_POLE, FOBJ_Y_Pole, FOBF_Z_START + FOBJ_DISTANCE * i);
		g_Pole[i + 5].rot = XMFLOAT3(0.0f, 0.0f, 0.0f);

	}

	// �Ŕ�
	for (int i = 0; i < 2; i++)
	{
		g_Sign0[i].use = TRUE;
		g_Sign0[i].pos = XMFLOAT3(FOBJ_X_SIGN, FOBJ_Y_SIGN, FOBF_Z_START + FOBJ_DISTANCE_SIGN * (i + 1));
		g_Sign0[i].rot = XMFLOAT3(0.0f, 0.0f, 0.0f);

	}


	g_Load = TRUE;
	return S_OK;
}

//=============================================================================
// �I������
//=============================================================================
void UninitTree(void)
{
	if (g_Load == FALSE) return;

	for (int i = 0; i < MAX_FOBJ; i++)
	{
		if (g_Bilding[i].load)
		{
			UnloadModel(&g_Bilding[i].model);
			g_Bilding[i].load = FALSE;
		}
	}

	for (int i = 0; i < MAX_FOBJ; i++)
	{
		if (g_House[i].load)
		{
			UnloadModel(&g_House[i].model);
			g_House[i].load = FALSE;
		}
	}


	g_Load = FALSE;
}

//=============================================================================
// �X�V����
//=============================================================================
void UpdateTree(void)
{
	// �r���̏���
	for (int i = 0; i < MAX_FOBJ; i++)
	{
		if (g_Bilding[i].use == TRUE)			// ���̃G�l�~�[���g���Ă���H
		{									// Yes
			// �n�ʂƈꏏ�ɓ���
			g_Bilding[i].pos.z += FIELD_SPEED;

			if (g_Bilding[i].pos.z >= FOBF_Z_MAX)
			{
				g_Bilding[i].use = FALSE;
				g_Bilding[i].pos.z = 0;

				// ���E�ǂ����ɕ\�����邩
				BOOL LR = left;
				if(g_Bilding[i].pos.x > 0) LR = right;

				// �����_���Ńr�����Ƃ�ݒu
				int data = 0;
				data = rand() % 2;

				switch (data)
				{
				case 0:
					SetBilding(LR);
					break;

				case 1:
					SetHouse(LR);
					break;
				}
			}
		}
	}

	// �Ƃ̏���
	for (int i = 0; i < MAX_FOBJ; i++)
	{
		if (g_House[i].use == TRUE)			// ���̃G�l�~�[���g���Ă���H
		{									// Yes
			// �n�ʂƈꏏ�ɓ���
			g_House[i].pos.z += FIELD_SPEED;

			if (g_House[i].pos.z >= FOBF_Z_MAX)
			{
				g_House[i].use = FALSE;

				// ���E�ǂ����ɕ\�����邩
				BOOL LR = left;
				if (g_House[i].pos.x > 0) LR = right;

				// �����_���Ńr�����Ƃ�ݒu
				int data = 0;
				data = rand() % 2;

				switch (data)
				{
				case 0:
					SetBilding(LR);
					break;

				case 1:
					SetHouse(LR);
					break;
				}
			}
		}
	}

	// �K�[�h���[���̏���
	for (int i = 0; i < MAX_FOBJ_GUAD; i++)
	{
		if (g_Guardrail[i].use == TRUE)			// ���̃G�l�~�[���g���Ă���H
		{									// Yes
			// �n�ʂƈꏏ�ɓ���
			g_Guardrail[i].pos.z += FIELD_SPEED;

			if (g_Guardrail[i].pos.z >= FOBF_Z_MAX)
			{
				g_Guardrail[i].use = FALSE;

				// ���E�ǂ����ɕ\�����邩
				BOOL LR = left;
				if (g_Guardrail[i].pos.x > 0) LR = right;

				// �K�[�h���[���ݒu
				SetGuardrail(LR);
			}
		}
	}

	// �d���̏���
	for (int i = 0; i < MAX_FOBJ; i++)
	{
		if (g_Pole[i].use == TRUE)			// ���̃G�l�~�[���g���Ă���H
		{									// Yes
			// �n�ʂƈꏏ�ɓ���
			g_Pole[i].pos.z += FIELD_SPEED;

			if (g_Pole[i].pos.z >= FOBF_Z_MAX)
			{
				g_Pole[i].use = FALSE;

				// ���E�ǂ����ɕ\�����邩
				BOOL LR = left;
				if (g_Pole[i].pos.x > 0) LR = right;

				// �d���ݒu
				SetPole(LR);
			}
		}
	}

	// �Ŕ�(���{)�̏���
	for (int i = 0; i < MAX_FOBJ_SIGN; i++)
	{
		if (g_Sign0[i].use == TRUE)			// ���̃G�l�~�[���g���Ă���H
		{									// Yes
			// �n�ʂƈꏏ�ɓ���
			g_Sign0[i].pos.z += FIELD_SPEED;

			if (g_Sign0[i].pos.z >= FOBF_Z_MAX)
			{
				g_Sign0[i].use = FALSE;

				// ���E�ǂ����ɕ\�����邩
				BOOL LR = left;
				if (g_Sign0[i].pos.x > 0) LR = right;

				// �����_���ŊŔ�ݒu
				int data = 0;
				data = rand() % 2;

				// �Ŕݒu
				switch (data)
				{
				case 0:
					SetSign0(LR);
					break;

				case 1:
					SetSign1(LR);
					break;
				}
			}
		}
	}

	// �Ŕ�(�A�����J)�̏���
	for (int i = 0; i < MAX_FOBJ_SIGN; i++)
	{
		if (g_Sign1[i].use == TRUE)			// ���̃G�l�~�[���g���Ă���H
		{									// Yes
			// �n�ʂƈꏏ�ɓ���
			g_Sign1[i].pos.z += FIELD_SPEED;

			if (g_Sign1[i].pos.z >= FOBF_Z_MAX)
			{
				g_Sign1[i].use = FALSE;

				// ���E�ǂ����ɕ\�����邩
				BOOL LR = left;
				if (g_Sign1[i].pos.x > 0) LR = right;

				// �����_���ŊŔ�ݒu
				int data = 0;
				data = rand() % 2;

				// �Ŕݒu
				switch (data)
				{
				case 0:
					SetSign0(LR);
					break;

				case 1:
					SetSign1(LR);
					break;
				}
			}
		}
	}


}

//=============================================================================
// �`�揈��
//=============================================================================
void DrawTree(void)
{
	XMMATRIX mtxScl, mtxRot, mtxTranslate, mtxWorld;

	// �r���̕`��
	for (int i = 0; i < MAX_FOBJ; i++)
	{
		if (g_Bilding[i].use == FALSE) continue;

		// ���[���h�}�g���b�N�X�̏�����
		mtxWorld = XMMatrixIdentity();

		// �X�P�[���𔽉f
		mtxScl = XMMatrixScaling(g_Bilding[i].scl.x, g_Bilding[i].scl.y, g_Bilding[i].scl.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

		// ��]�𔽉f
		mtxRot = XMMatrixRotationRollPitchYaw(g_Bilding[i].rot.x, g_Bilding[i].rot.y + XM_PI, g_Bilding[i].rot.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);

		// �ړ��𔽉f
		mtxTranslate = XMMatrixTranslation(g_Bilding[i].pos.x, g_Bilding[i].pos.y, g_Bilding[i].pos.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

		// ���[���h�}�g���b�N�X�̐ݒ�
		SetWorldMatrix(&mtxWorld);

		XMStoreFloat4x4(&g_Bilding[i].mtxWorld, mtxWorld);

		// ���f���`��
		DrawModel(&g_Bilding[0].model);

	}

	// �Ƃ̕`��
	for (int i = 0; i < MAX_FOBJ; i++)
	{
		if (g_House[i].use == FALSE) continue;

		// ���[���h�}�g���b�N�X�̏�����
		mtxWorld = XMMatrixIdentity();

		// �X�P�[���𔽉f
		mtxScl = XMMatrixScaling(g_House[i].scl.x, g_House[i].scl.y, g_House[i].scl.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

		// ��]�𔽉f
		mtxRot = XMMatrixRotationRollPitchYaw(g_House[i].rot.x, g_House[i].rot.y + XM_PI, g_House[i].rot.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);

		// �ړ��𔽉f
		mtxTranslate = XMMatrixTranslation(g_House[i].pos.x, g_House[i].pos.y, g_House[i].pos.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

		// ���[���h�}�g���b�N�X�̐ݒ�
		SetWorldMatrix(&mtxWorld);

		XMStoreFloat4x4(&g_House[i].mtxWorld, mtxWorld);

		// ���f���`��
		DrawModel(&g_House[0].model);

	}

	// �K�[�h���[���̕`��
	for (int i = 0; i < MAX_FOBJ_GUAD; i++)
	{
		if (g_Guardrail[i].use == FALSE) continue;

		// ���[���h�}�g���b�N�X�̏�����
		mtxWorld = XMMatrixIdentity();

		// �X�P�[���𔽉f
		mtxScl = XMMatrixScaling(g_Guardrail[i].scl.x, g_Guardrail[i].scl.y, g_Guardrail[i].scl.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

		// ��]�𔽉f
		mtxRot = XMMatrixRotationRollPitchYaw(g_Guardrail[i].rot.x, g_Guardrail[i].rot.y + XM_PI, g_Guardrail[i].rot.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);

		// �ړ��𔽉f
		mtxTranslate = XMMatrixTranslation(g_Guardrail[i].pos.x, g_Guardrail[i].pos.y, g_Guardrail[i].pos.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

		// ���[���h�}�g���b�N�X�̐ݒ�
		SetWorldMatrix(&mtxWorld);

		XMStoreFloat4x4(&g_Guardrail[i].mtxWorld, mtxWorld);

		// ���f���`��
		DrawModel(&g_Guardrail[0].model);

	}

	// �d���̕`��
	for (int i = 0; i < MAX_FOBJ_GUAD; i++)
	{
		if (g_Pole[i].use == FALSE) continue;

		// ���[���h�}�g���b�N�X�̏�����
		mtxWorld = XMMatrixIdentity();

		// �X�P�[���𔽉f
		mtxScl = XMMatrixScaling(g_Pole[i].scl.x, g_Pole[i].scl.y, g_Pole[i].scl.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

		// ��]�𔽉f
		mtxRot = XMMatrixRotationRollPitchYaw(g_Pole[i].rot.x, g_Pole[i].rot.y + XM_PI, g_Pole[i].rot.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);

		// �ړ��𔽉f
		mtxTranslate = XMMatrixTranslation(g_Pole[i].pos.x, g_Pole[i].pos.y, g_Pole[i].pos.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

		// ���[���h�}�g���b�N�X�̐ݒ�
		SetWorldMatrix(&mtxWorld);

		XMStoreFloat4x4(&g_Pole[i].mtxWorld, mtxWorld);

		// ���f���`��
		DrawModel(&g_Pole[0].model);

	}

	// �Ŕ�(���{)�̕`��
	for (int i = 0; i < MAX_FOBJ_SIGN; i++)
	{
		if (g_Sign0[i].use == FALSE) continue;

		// ���[���h�}�g���b�N�X�̏�����
		mtxWorld = XMMatrixIdentity();

		// �X�P�[���𔽉f
		mtxScl = XMMatrixScaling(g_Sign0[i].scl.x, g_Sign0[i].scl.y, g_Sign0[i].scl.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

		// ��]�𔽉f
		mtxRot = XMMatrixRotationRollPitchYaw(g_Sign0[i].rot.x, g_Sign0[i].rot.y + XM_PI, g_Sign0[i].rot.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);

		// �ړ��𔽉f
		mtxTranslate = XMMatrixTranslation(g_Sign0[i].pos.x, g_Sign0[i].pos.y, g_Sign0[i].pos.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

		// ���[���h�}�g���b�N�X�̐ݒ�
		SetWorldMatrix(&mtxWorld);

		XMStoreFloat4x4(&g_Sign0[i].mtxWorld, mtxWorld);

		// ���f���`��
		DrawModel(&g_Sign0[0].model);

	}

	// �Ŕ�(�A�����J)�̕`��
	for (int i = 0; i < MAX_FOBJ_SIGN; i++)
	{
		if (g_Sign1[i].use == FALSE) continue;

		// ���[���h�}�g���b�N�X�̏�����
		mtxWorld = XMMatrixIdentity();

		// �X�P�[���𔽉f
		mtxScl = XMMatrixScaling(g_Sign1[i].scl.x, g_Sign1[i].scl.y, g_Sign1[i].scl.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

		// ��]�𔽉f
		mtxRot = XMMatrixRotationRollPitchYaw(g_Sign1[i].rot.x, g_Sign1[i].rot.y + XM_PI, g_Sign1[i].rot.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);

		// �ړ��𔽉f
		mtxTranslate = XMMatrixTranslation(g_Sign1[i].pos.x, g_Sign1[i].pos.y, g_Sign1[i].pos.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

		// ���[���h�}�g���b�N�X�̐ݒ�
		SetWorldMatrix(&mtxWorld);

		XMStoreFloat4x4(&g_Sign1[i].mtxWorld, mtxWorld);

		// ���f���`��
		DrawModel(&g_Sign1[0].model);

	}
}

//=============================================================================
// �G�l�~�[�̎擾
//=============================================================================
FOBJ *GetBilding()
{
	return &g_Bilding[0];
}


//=============================================================================
// �r����ݒu
// LR : L == 0,R == 1
//=============================================================================
void SetBilding(BOOL LR)
{
	for (int i = 0; i < MAX_FOBJ; i++)
	{
		if (g_Bilding[i].use == FALSE)
		{
			g_Bilding[i].use = TRUE;

			g_Bilding[i].pos.z = FOBF_Z_START;

			if (LR)
			{	// �E�ɐݒu
				g_Bilding[i].pos.x = FOBJ_X;
				g_Bilding[i].rot = XMFLOAT3(0.0f, 1.57f, 0.0f);
			}
			else
			{	// ���ɐݒu
				g_Bilding[i].pos.x = -FOBJ_X;
				g_Bilding[i].rot = XMFLOAT3(0.0f, -1.57f, 0.0f);
			}

			g_Bilding[i].pos.y -= (float)(rand() % FOBJ_Y);

			return;
		}
	}
}


//=============================================================================
// �Ƃ�ݒu
// LR : L == 0,R == 1
//=============================================================================
void SetHouse(BOOL LR)
{
	for (int i = 0; i < MAX_FOBJ; i++)
	{
		if (g_House[i].use == FALSE)
		{
			g_House[i].use = TRUE;

			g_House[i].pos.z = FOBF_Z_START;

			if (LR)
			{	// �E�ɐݒu
				g_House[i].pos.x = FOBJ_X_HOUSE;
				g_House[i].rot = XMFLOAT3(0.0f, 1.57f, 0.0f);
			}
			else
			{	// ���ɐݒu
				g_House[i].pos.x = -FOBJ_X_HOUSE;
				g_House[i].rot = XMFLOAT3(0.0f, -1.57f, 0.0f);
			}

			return;
		}
	}
}


//=============================================================================
// �K�[�h���[����ݒu
// LR : L == 0,R == 1
//=============================================================================
void SetGuardrail(BOOL LR)
{
	for (int i = 0; i < MAX_FOBJ_GUAD; i++)
	{
		if (g_Guardrail[i].use == FALSE)
		{
			g_Guardrail[i].use = TRUE;

			g_Guardrail[i].pos.z = FOBF_Z_START;

			if (LR)
			{	// �E�ɐݒu
				g_Guardrail[i].pos.x = FOBJ_X_GUAD;
				g_Guardrail[i].rot = XMFLOAT3(0.0f, 0.0f, 0.0f);
			}
			else
			{	// ���ɐݒu
				g_Guardrail[i].pos.x = -FOBJ_X_GUAD;
				g_Guardrail[i].rot = XMFLOAT3(0.0f, 3.14f, 0.0f);
			}

			return;
		}
	}
}


//=============================================================================
// �d����ݒu
// LR : L == 0,R == 1
//=============================================================================
void SetPole(BOOL LR)
{
	for (int i = 0; i < MAX_FOBJ_GUAD; i++)
	{
		if (g_Pole[i].use == FALSE)
		{
			g_Pole[i].use = TRUE;

			g_Pole[i].pos.z = FOBF_Z_START;

			if (LR)
			{	// �E�ɐݒu
				g_Pole[i].pos.x = FOBJ_X_POLE;
				g_Pole[i].rot = XMFLOAT3(0.0f, 0.0f, 0.0f);
			}
			else
			{	// ���ɐݒu
				g_Pole[i].pos.x = -FOBJ_X_POLE;
				g_Pole[i].rot = XMFLOAT3(0.0f, 3.14f, 0.0f);
			}

			return;
		}
	}
}


//=============================================================================
// �Ŕ�(���{)��ݒu
// LR : L == 0,R == 1
//=============================================================================
void SetSign0(BOOL LR)
{
	for (int i = 0; i < MAX_FOBJ_SIGN; i++)
	{
		if (g_Sign0[i].use == FALSE)
		{
			g_Sign0[i].use = TRUE;

			g_Sign0[i].pos.z = FOBF_Z_START;

			if (LR)
			{	// �E�ɐݒu
				g_Sign0[i].pos.x = FOBJ_X_SIGN;
				g_Sign0[i].rot = XMFLOAT3(0.0f, 0.0f, 0.0f);
			}
			else
			{	// ���ɐݒu
				g_Sign0[i].pos.x = -FOBJ_X_SIGN;
				g_Sign0[i].rot = XMFLOAT3(0.0f, 0.0f, 0.0f);
			}

			return;
		}
	}
}


//=============================================================================
// �Ŕ�(�A�����J)��ݒu
// LR : L == 0,R == 1
//=============================================================================
void SetSign1(BOOL LR)
{
	for (int i = 0; i < MAX_FOBJ_SIGN; i++)
	{
		if (g_Sign1[i].use == FALSE)
		{
			g_Sign1[i].use = TRUE;

			g_Sign1[i].pos.z = FOBF_Z_START;

			if (LR)
			{	// �E�ɐݒu
				g_Sign1[i].pos.x = FOBJ_X_SIGN;
				g_Sign1[i].rot = XMFLOAT3(0.0f, 0.0f, 0.0f);
			}
			else
			{	// ���ɐݒu
				g_Sign1[i].pos.x = -FOBJ_X_SIGN;
				g_Sign1[i].rot = XMFLOAT3(0.0f, 0.0f, 0.0f);
			}

			return;
		}
	}
}