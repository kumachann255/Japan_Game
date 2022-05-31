//=============================================================================
//
// �X�J�C�h�[������ [Sky.cpp]
// Author : �G�i
//
//=============================================================================
#include "main.h"
#include "renderer.h"
#include "camera.h"
#include "input.h"
#include "model.h"
#include "sky.h"
#include "debugproc.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define	MODEL_SKY				"data/MODEL/sky.obj"			// �ǂݍ��ރ��f����
#define	SKY_OFFSET_Y			(XM_PI/15000)							// ��]���x


//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************


//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
static SKY				g_Sky[MAX_SKY];						// �w�i
static float			g_rot_y;

static BOOL				g_Load = FALSE;

//=============================================================================
// ����������
//=============================================================================
HRESULT InitSky(void)
{
	for (int i = 0; i < MAX_SKY; i++)
	{
		LoadModel(MODEL_SKY, &g_Sky[i].model);
		g_Sky[i].load = TRUE;

		g_Sky[i].use = TRUE;

		g_Sky[i].pos = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_Sky[i].rot = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_Sky[i].scl = XMFLOAT3(80.0f, 30.0f, 90.0f);

		// ���f���̃f�B�t���[�Y��ۑ����Ă����B�F�ς��Ή��ׁ̈B
		GetModelDiffuse(&g_Sky[0].model, &g_Sky[0].diffuse[0]);
	}
	
	g_rot_y = SKY_OFFSET_Y;

	g_Load = TRUE;
	return S_OK;
}

//=============================================================================
// �I������
//=============================================================================
void UninitSky(void)
{
	if (g_Load == FALSE) return;

	for (int i = 0; i < MAX_SKY; i++)
	{
		if (g_Sky[i].load)
		{
			UnloadModel(&g_Sky[i].model);
			g_Sky[i].load = FALSE;
		}
	}

	g_Load = FALSE;
}

//=============================================================================
// �X�V����
//=============================================================================
void UpdateSky(void)
{
	for (int i = 0; i < MAX_SKY; i++)
	{
		g_Sky[i].rot.y += g_rot_y;

		if (g_Sky[i].rot.y > XM_PI)
		{
			g_Sky[i].rot.y = 0.0f;
		}
	}
}

//=============================================================================
// �`�揈��
//=============================================================================
void DrawSky(void)
{
	XMMATRIX mtxScl, mtxRot, mtxTranslate, mtxWorld;

	// �J�����O����
	SetCullingMode(CULL_MODE_NONE);

	for (int i = 0; i < MAX_SKY; i++)
	{
		if (g_Sky[i].use == FALSE) continue;

		// ���[���h�}�g���b�N�X�̏�����
		mtxWorld = XMMatrixIdentity();

		// �X�P�[���𔽉f
		mtxScl = XMMatrixScaling(g_Sky[i].scl.x, g_Sky[i].scl.y, g_Sky[i].scl.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

		// ��]�𔽉f
		mtxRot = XMMatrixRotationRollPitchYaw(g_Sky[i].rot.x, g_Sky[i].rot.y, g_Sky[i].rot.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);

		// �ړ��𔽉f
		mtxTranslate = XMMatrixTranslation(g_Sky[i].pos.x, g_Sky[i].pos.y, g_Sky[i].pos.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

		// ���[���h�}�g���b�N�X�̐ݒ�
		SetWorldMatrix(&mtxWorld);

		XMStoreFloat4x4(&g_Sky[i].mtxWorld, mtxWorld);

		// ���f���`��
		DrawModel(&g_Sky[i].model);
	
	}

	// �J�����O�ݒ��߂�
	SetCullingMode(CULL_MODE_BACK);
}

//=============================================================================
// �G�l�~�[�̎擾
//=============================================================================
SKY *GetSky()
{
	return &g_Sky[0];
}
