//=============================================================================
//
// ���j�Ɠ����蔻�菈�� [blast.cpp]
// Author : 
//
//=============================================================================
#include "main.h"
#include "renderer.h"
#include "camera.h"
#include "input.h"
#include "model.h"
#include "attackRange.h"
#include "shadow.h"
#include "blast.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define	MODEL_BLAST			"data/MODEL/blast.obj"		// �ǂݍ��ރ��f����

#define	VALUE_MOVE			(5.0f)						// �ړ���
#define	VALUE_ROTATE		(XM_PI * 0.02f)				// ��]��

#define BLAST_LIFE			(30)						// ���j�I�u�W�F�N�g�̕\������

//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************


//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
static BLAST			g_Blast[MAX_BLAST];				// ���j�I�u�W�F�N�g

static BOOL				g_Load = FALSE;


//=============================================================================
// ����������
//=============================================================================
HRESULT InitBlast(void)
{
	for (int i = 0; i < MAX_BLAST; i++)
	{
		LoadModel(MODEL_BLAST, &g_Blast[i].model);
		g_Blast[i].load = TRUE;

		g_Blast[i].pos = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_Blast[i].rot = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_Blast[i].scl = XMFLOAT3(2.0f, 2.0f, 2.0f);
		g_Blast[i].size = BLAST_SIZE;
		g_Blast[i].life = 0;
		g_Blast[i].spd = 0.0f;			// �ړ��X�s�[�h�N���A

		// ���f���̃f�B�t���[�Y��ۑ����Ă����B�F�ς��Ή��ׁ̈B
		GetModelDiffuse(&g_Blast[i].model, &g_Blast[i].diffuse[0]);

		// �F�������ς���
		XMFLOAT4 color = { 1.0f, 1.0f, 1.0f, 0.5f };

		// �F���Z�b�g
		SetModelDiffuse(&g_Blast[i].model, 0, color);

		g_Blast[i].use = FALSE;			// TRUE:�����Ă�
	}


	g_Load = TRUE;
	return S_OK;
}

//=============================================================================
// �I������
//=============================================================================
void UninitBlast(void)
{
	if (g_Load == FALSE) return;

	for (int i = 0; i < MAX_BLAST; i++)
	{
		if (g_Blast[i].load)
		{
			UnloadModel(&g_Blast[i].model);
			g_Blast[i].load = FALSE;
		}
	}

	g_Load = FALSE;
}

//=============================================================================
// �X�V����
//=============================================================================
void UpdateBlast(void)
{
	for (int i = 0; i < MAX_BLAST; i++)
	{
		if (g_Blast[i].use == TRUE)			// ���̍U���͈͂��g���Ă���H
		{									// Yes
			// ���������炷
			g_Blast[i].life--;

			// �������s�����疢�g�p��
			if (g_Blast[i].life < 0)
			{
				g_Blast[i].use = FALSE;
			}
		}
	}
}

//=============================================================================
// �`�揈��
//=============================================================================
void DrawBlast(void)
{
	XMMATRIX mtxScl, mtxRot, mtxTranslate, mtxWorld;


	// �J�����O����
	SetCullingMode(CULL_MODE_NONE);

	// ���e�X�g��L����
	SetAlphaTestEnable(TRUE);

	// ���Z�����ɐݒ�
	SetBlendState(BLEND_MODE_ADD);

	//�t�H�O�𖳌���
	SetFogEnable(FALSE);

	for (int i = 0; i < MAX_BLAST; i++)
	{
		if (g_Blast[i].use == TRUE)
		{
			// ���[���h�}�g���b�N�X�̏�����
			mtxWorld = XMMatrixIdentity();

			// �X�P�[���𔽉f
			mtxScl = XMMatrixScaling(g_Blast[i].scl.x, g_Blast[i].scl.y, g_Blast[i].scl.z);
			mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

			// ��]�𔽉f
			mtxRot = XMMatrixRotationRollPitchYaw(g_Blast[i].rot.x, g_Blast[i].rot.y + XM_PI, g_Blast[i].rot.z);
			mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);

			// �ړ��𔽉f
			mtxTranslate = XMMatrixTranslation(g_Blast[i].pos.x, g_Blast[i].pos.y, g_Blast[i].pos.z);
			mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

			// ���[���h�}�g���b�N�X�̐ݒ�
			SetWorldMatrix(&mtxWorld);

			XMStoreFloat4x4(&g_Blast[i].mtxWorld, mtxWorld);


			// ���f���`��
			DrawModel(&g_Blast[i].model);
		}
	}

	// �ʏ�u�����h�ɖ߂�
	SetBlendState(BLEND_MODE_ALPHABLEND);

	// �J�����O�ݒ��߂�
	SetCullingMode(CULL_MODE_BACK);

	// ���e�X�g�𖳌���
	SetAlphaTestEnable(FALSE);

	// �t�H�O��L����
	SetFogEnable(TRUE);

}

//=============================================================================
// ���j�̎擾
//=============================================================================
BLAST *GetBlast()
{
	return &g_Blast[0];
}


//=============================================================================
// ���j�̔���
//=============================================================================
void SetBlast(XMFLOAT3 pos)
{
	for (int i = 0; i < MAX_BLAST; i++)
	{
		if (g_Blast[i].use == FALSE)
		{
			g_Blast[i].use = TRUE;
			g_Blast[i].pos = pos;
			g_Blast[i].life = BLAST_LIFE;


			return;
		}
	}
}
