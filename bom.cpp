//=============================================================================
//
// �_�C�i�}�C�g�g���� [bom.cpp]
// Author : bo
// �_�C�i�}�C�g���� [bom.cpp]
// Author : �Q�[���̓f�B�i�[�̌��
//
//=============================================================================
#include "main.h"
#include "renderer.h"
#include "camera.h"
#include "input.h"
#include "model.h"
#include "bom.h"
#include "shadow.h"
#include "player.h"
#include "attackRange.h"
#include "blast.h"

#include "sound.h"


//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define	MODEL_BOM			"data/MODEL/stick00.obj"		// �ǂݍ��ރ��f����

#define	VALUE_MOVE			(5.0f)						// �ړ���
#define	VALUE_ROTATE		(XM_PI * 0.02f)				// ��]��

#define BOM_SPEED			(0.015f)						// �{���̑��x

#define BOM_COOL			(180)						// �N�[���^�C��


//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************


//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
static BOM				g_Bom;				// �U���͈�

static BOOL				g_Load = FALSE;

static XMFLOAT3			control0, control1, control2;	// ����_

static int				coolTime;		// �N�[���^�C��

static XMFLOAT3			rot;	// ��]

//=============================================================================
// ����������
//=============================================================================
HRESULT InitBom(void)
{
	LoadModel(MODEL_BOM, &g_Bom.model);
	g_Bom.load = TRUE;

	g_Bom.pos = XMFLOAT3(0.0f, 0.0f, 0.0f);
	g_Bom.rot = XMFLOAT3(0.0f, 0.0f, 0.0f);
	g_Bom.scl = XMFLOAT3(0.5f, 0.5f, 0.5f);

	g_Bom.speed = BOM_SPEED;			// �ړ��X�s�[�h�N���A
	g_Bom.time = 0.0f;

	// ���f���̃f�B�t���[�Y��ۑ����Ă����B�F�ς��Ή��ׁ̈B
	GetModelDiffuse(&g_Bom.model, &g_Bom.diffuse[0]);

	// �F�������ς���
	XMFLOAT4 color = { 1.0f, 1.0f, 1.0f, 0.5f };

	// �F���Z�b�g
	SetModelDiffuse(&g_Bom.model, 0, color);

	g_Bom.use = FALSE;			// TRUE:�����Ă�

	coolTime = BOM_COOL;

	g_Load = TRUE;
	return S_OK;
}

//=============================================================================
// �I������
//=============================================================================
void UninitBom(void)
{
	if (g_Load == FALSE) return;

	if (g_Bom.load)
	{
		UnloadModel(&g_Bom.model);
		g_Bom.load = FALSE;
	}

	g_Load = FALSE;
}

//=============================================================================
// �X�V����
//=============================================================================
void UpdateBom(void)
{
	if (g_Bom.use == TRUE)			// ���̍U���͈͂��g���Ă���H
	{									// Yes
		// �ړ�����
		// �x�W�F�Ȑ��Z�o
		g_Bom.pos.x = ((1.0f - g_Bom.time) * (1.0f - g_Bom.time) * control0.x) +
			(2 * g_Bom.time * (1.0f - g_Bom.time) * control1.x) +
			(g_Bom.time * g_Bom.time * control2.x);

		g_Bom.pos.z = ((1.0f - g_Bom.time) * (1.0f - g_Bom.time) * control0.z) +
			(2 * g_Bom.time * (1.0f - g_Bom.time) * control1.z) +
			(g_Bom.time * g_Bom.time * control2.z);

		g_Bom.pos.y = ((1.0f - g_Bom.time) * (1.0f - g_Bom.time) * control0.y) +
			(2 * g_Bom.time * (1.0f - g_Bom.time) * control1.y) +
			(g_Bom.time * g_Bom.time * control2.y);


		// ���Ԃ�i�߂�
		if (g_Bom.time < 1.0f)
		{
			g_Bom.time += g_Bom.speed;
		}
		else
		{	// �ړI�n�ɒ����������
			g_Bom.use = FALSE;

			// ���j�I�u�W�F�N�g�𔭐�
			SetBlast(g_Bom.pos);
		}


		// ��]����
		g_Bom.rot.x += rot.x;
		g_Bom.rot.y += rot.y;
		//g_Bom.rot.z += rot.z;

	}

	// �N�[���^�C���Ǘ�
	if (coolTime > 0)
	{
		coolTime--;
	}

}

//=============================================================================
// �`�揈��
//=============================================================================
void DrawBom(void)
{
	XMMATRIX mtxScl, mtxRot, mtxTranslate, mtxWorld;

	// �J�����O����
	SetCullingMode(CULL_MODE_NONE);

	if (g_Bom.use == FALSE) return;

	// ���[���h�}�g���b�N�X�̏�����
	mtxWorld = XMMatrixIdentity();

	// �X�P�[���𔽉f
	mtxScl = XMMatrixScaling(g_Bom.scl.x, g_Bom.scl.y, g_Bom.scl.z);
	mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

	// ��]�𔽉f
	mtxRot = XMMatrixRotationRollPitchYaw(g_Bom.rot.x, g_Bom.rot.y + XM_PI, g_Bom.rot.z);
	mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);

	// �ړ��𔽉f
	mtxTranslate = XMMatrixTranslation(g_Bom.pos.x, g_Bom.pos.y, g_Bom.pos.z);
	mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

	// ���[���h�}�g���b�N�X�̐ݒ�
	SetWorldMatrix(&mtxWorld);

	XMStoreFloat4x4(&g_Bom.mtxWorld, mtxWorld);


	// ���f���`��
	DrawModel(&g_Bom.model);

	// �J�����O�ݒ��߂�
	SetCullingMode(CULL_MODE_BACK);
}

//=============================================================================
// �_�C�i�}�C�g�̎擾
//=============================================================================
BOM *GetBom()
{
	return &g_Bom;
}


//=============================================================================
// �_�C�i�}�C�g�̔��ˏ���
//=============================================================================
void SetBom(void)
{

	if ((g_Bom.use == FALSE) && (coolTime <= 0))
	{
		// SE�̃Z�b�g
		PlaySound(SOUND_LABEL_SE_throwingSound01);

		g_Bom.use = TRUE;
		g_Bom.time = 0.0f;

		PLAYER *pPlayer = GetPlayer();
		ATTACKRANGE *AttackR = GetAttackR();

		control0 = pPlayer->pos;
		control2 = AttackR->pos;

		float hight = 0.0f;	// ��������

		// ����_�̎Z�o
		control1.x = (control2.x - control0.x) / 2.0f + control0.x;
		control1.z = (control2.z - control0.z) / 2.0f + control0.z;
		control1.y = (control2.y - control0.y) / 2.0f + control0.y;

		hight = BOM_H_MAX;

		control1.y += hight;
		control2.y += BOM_H;

		// ��]���x�̎Z�o
		rot.x = (float)(rand() % 50) / 1000.0f;
		rot.y = (float)(rand() % 50) / 1000.0f;
		rot.z = (float)(rand() % 50) / 1000.0f;

		// ��]�̏�����
		g_Bom.rot = XMFLOAT3(0.0f, 0.0f, 0.0f);

		coolTime = BOM_COOL;

	}
}


BOOL GetBomUse(void)
{
	return g_Bom.use;
}


// ���e�̐i�񂾎��Ԃ��擾
float GetBomTime(void)
{
	return g_Bom.time;
}

// �N�[���^�C����Ԃ��֐�
int GetCoolTime(void)
{
	return coolTime;
}