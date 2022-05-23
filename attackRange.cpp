//=============================================================================
//
// �U���͈͏��� [attackRange.cpp]
// Author : �Q�[���̓f�B�i�[�̌�ł���[
//
//=============================================================================
#include "main.h"
#include "renderer.h"
#include "camera.h"
#include "input.h"
#include "model.h"
#include "attackRange.h"
#include "shadow.h"
#include "bom.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define	MODEL_ATTACKRANGE			"data/MODEL/red.obj"		// �ǂݍ��ރ��f����

#define	VALUE_MOVE			(5.0f)						// �ړ���
#define	VALUE_ROTATE		(XM_PI * 0.02f)				// ��]��



//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************


//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
static ATTACKRANGE		g_AttackR;				// �U���͈�

static BOOL				g_Load = FALSE;


//=============================================================================
// ����������
//=============================================================================
HRESULT InitAttackR(void)
{
	LoadModel(MODEL_ATTACKRANGE, &g_AttackR.model);
	g_AttackR.load = TRUE;

	g_AttackR.pos = XMFLOAT3(0.0f, 0.0f, 0.0f);
	g_AttackR.rot = XMFLOAT3(0.0f, 0.0f, 0.0f);
	g_AttackR.scl = XMFLOAT3(3.5f, 4.0f, 3.5f);

	g_AttackR.spd = 0.0f;			// �ړ��X�s�[�h�N���A

	// ���f���̃f�B�t���[�Y��ۑ����Ă����B�F�ς��Ή��ׁ̈B
	GetModelDiffuse(&g_AttackR.model, &g_AttackR.diffuse[0]);

	// �F�������ς���// ���a�g��Ȃ�
	XMFLOAT4 color = { 1.0f, 1.0f, 1.0f, 0.5f };

	// �F���Z�b�g
	SetModelDiffuse(&g_AttackR.model, 0, color);

	g_AttackR.use = TRUE;			// TRUE:�����Ă�


	g_Load = TRUE;
	return S_OK;
}

//=============================================================================
// �I������
//=============================================================================
void UninitAttackR(void)
{
	if (g_Load == FALSE) return;

	if (g_AttackR.load)
	{
		UnloadModel(&g_AttackR.model);
		g_AttackR.load = FALSE;
	}

	g_Load = FALSE;
}

//=============================================================================
// �X�V����
//=============================================================================
void UpdateAttackR(void)
{
	BOM *bom = GetBom();

	// �U���͈͂�\�����邩�ǂ���
	if (bom->use == FALSE)
	{
		g_AttackR.use = TRUE;
	}
	if (bom->use == TRUE)
	{
		g_AttackR.use = FALSE;
	}

	//if (g_AttackR.use == TRUE)			// ���̍U���͈͂��g���Ă���H
	{									// Yes

		// �ړ��������Ⴄ
		if (GetKeyboardPress(DIK_A))
		{	// ���ֈړ�
			g_AttackR.pos.x -= VALUE_MOVE;
		}
		if (GetKeyboardPress(DIK_D))
		{	// �E�ֈړ�
			g_AttackR.pos.x += VALUE_MOVE;
		}
		if (GetKeyboardPress(DIK_W))
		{	// ��ֈړ�
			g_AttackR.pos.z += VALUE_MOVE;
		}
		if (GetKeyboardPress(DIK_S))
		{	// ���ֈړ�
			g_AttackR.pos.z -= VALUE_MOVE;
		}

	}
}

//=============================================================================
// �`�揈��
//=============================================================================
void DrawAttackR(void)
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

	if (g_AttackR.use == TRUE)
	{
		// ���[���h�}�g���b�N�X�̏�����
		mtxWorld = XMMatrixIdentity();

		// �X�P�[���𔽉f
		mtxScl = XMMatrixScaling(g_AttackR.scl.x, g_AttackR.scl.y, g_AttackR.scl.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

		// ��]�𔽉f
		mtxRot = XMMatrixRotationRollPitchYaw(g_AttackR.rot.x, g_AttackR.rot.y + XM_PI, g_AttackR.rot.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);

		// �ړ��𔽉f
		mtxTranslate = XMMatrixTranslation(g_AttackR.pos.x, g_AttackR.pos.y, g_AttackR.pos.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

		// ���[���h�}�g���b�N�X�̐ݒ�
		SetWorldMatrix(&mtxWorld);

		XMStoreFloat4x4(&g_AttackR.mtxWorld, mtxWorld);


		// ���f���`��
		DrawModel(&g_AttackR.model);
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
// �U���͈͂̎擾
//=============================================================================
ATTACKRANGE *GetAttackR()
{
	return &g_AttackR;
}
