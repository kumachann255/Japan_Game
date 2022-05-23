//=============================================================================
//
// ���j�Ɠ����蔻�菈�� [blast.cpp]
// Author : a
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
#include "meshfield.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define	MODEL_BLAST				"data/MODEL/bak00.obj"		// �ǂݍ��ރ��f����
#define	MODEL_BLAST_1			"data/MODEL/bak02.obj"		// �ǂݍ��ރ��f����


#define	VALUE_MOVE			(5.0f)						// �ړ���
#define	VALUE_ROTATE		(XM_PI * 0.02f)				// ��]��

#define BLAST_LIFE			(150)						// ���j�I�u�W�F�N�g�̕\������
#define BLAST_MOVE_TIME_0	(3)							// �˂΂˂΂��L���鎞��
#define BLAST_MOVE_TIME_1	(10)						// �L��������ԂŎ~�܂鎞��
#define BLAST_MOVE_TIME_2	(10)						// �˂΂˂΂��k�܂鎞��
#define BLAST_MOVE_TIME_3	(20)						// �˂΂˂΂��󒆂Ŏ~�܂鎞��


#define BLASE_ROT			(3.14f)						// ��]�̍ő�l

#define MAX_BLAST_MOVE		(2)							// ���[�t�B���O�̐�



//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************


//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
static BLAST			g_Blast[MAX_BLAST];				// ���j�I�u�W�F�N�g

static BOOL				g_Load = FALSE;

static MODEL			g_Blast_Vertex[MAX_BLAST_MOVE];		// ���[�t�B���O�p���f���̒��_��񂪓������f�[�^�z��
static VERTEX_3D		*g_BlastVertex = NULL;				// �r���o�߂��L�^����ꏊ

static float			g_time;							// ���[�t�B���O�̌o�ߎ���
static int				g_downCount;					// �����n�߂Ăǂ̂��炢���Ԃ��o�߂�����

static int				g_morphingNum;					// ���[�t�B���O�̔ԍ�

static BOOL				g_cameraOn;						// �J�����̃X�C�b�`

static int				g_stopTime;						// ���e���󒆂Ŏ~�܂鎞��


//=============================================================================
// ����������
//=============================================================================
HRESULT InitBlast(void)
{
	for (int i = 0; i < MAX_BLAST; i++)
	{
		LoadModelMorphing(MODEL_BLAST, &g_Blast[i].model);
		g_Blast[i].load = TRUE;

		g_Blast[i].pos = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_Blast[i].rot = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_Blast[i].scl = XMFLOAT3(1.1f, 1.1f, 1.1f);
		g_Blast[i].size = BLAST_SIZE;
		g_Blast[i].life = 0;
		g_Blast[i].spd = 0.0f;			// �ړ��X�s�[�h�N���A

		// ���f���̃f�B�t���[�Y��ۑ����Ă����B�F�ς��Ή��ׁ̈B
		GetModelDiffuse(&g_Blast[i].model, &g_Blast[i].diffuse[0]);

		g_Blast[i].use = FALSE;			// TRUE:�����Ă�
		g_Blast[i].move = FALSE;		// TRUE:���ֈړ�����
		g_Blast[i].shrink = FALSE;		// �k�܂�n�߂Ă��邩
	
	}

	// ���[�t�B���O����I�u�W�F�N�g�̓ǂݍ���
	LoadObj(MODEL_BLAST, &g_Blast_Vertex[0]);
	LoadObj(MODEL_BLAST_1, &g_Blast_Vertex[1]);

	// ���g��z��Ƃ��Ďg�p�ł���悤�Ɏd�l�ύX
	g_BlastVertex = new VERTEX_3D[g_Blast_Vertex[0].VertexNum];

	// ����(�r���o��)�̏�����
	for (int i = 0; i < g_Blast_Vertex[0].VertexNum; i++)
	{
		g_BlastVertex[i].Position = g_Blast_Vertex[0].VertexArray[i].Position;
		g_BlastVertex[i].Diffuse  = g_Blast_Vertex[0].VertexArray[i].Diffuse;
		g_BlastVertex[i].Normal   = g_Blast_Vertex[0].VertexArray[i].Normal;
		g_BlastVertex[i].TexCoord = g_Blast_Vertex[0].VertexArray[i].TexCoord;
	}

	g_morphingNum = 0;
	g_time = 0.0f;
	g_downCount = 0;
	g_cameraOn = FALSE;
	g_stopTime = 0;

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

			// �˂΂˂΂̃��[�t�B���O�̏���
			{
				int after, brfore;

				// �L����I������k�܂�����փ��[�t�B���O
				if (g_Blast[i].life < BLAST_LIFE - BLAST_MOVE_TIME_0)
				{
					g_morphingNum = 1;
				}
				else
				{
					g_morphingNum = 0;
				}


				// ���[�t�B���O���f���̔ԍ�����
				switch (g_morphingNum)
				{
				case 0:
					after = 1;
					brfore = 0;

					break;

				case 1:
					after = 0;
					brfore = 1;

					break;
				}


				// ���[�t�B���O����
				for (int p = 0; p < g_Blast_Vertex[0].VertexNum; p++)
				{
					g_BlastVertex[p].Position.x = g_Blast_Vertex[after].VertexArray[p].Position.x - g_Blast_Vertex[brfore].VertexArray[p].Position.x;
					g_BlastVertex[p].Position.y = g_Blast_Vertex[after].VertexArray[p].Position.y - g_Blast_Vertex[brfore].VertexArray[p].Position.y;
					g_BlastVertex[p].Position.z = g_Blast_Vertex[after].VertexArray[p].Position.z - g_Blast_Vertex[brfore].VertexArray[p].Position.z;

					g_BlastVertex[p].Position.x *= g_time;
					g_BlastVertex[p].Position.y *= g_time;
					g_BlastVertex[p].Position.z *= g_time;

					g_BlastVertex[p].Position.x += g_Blast_Vertex[brfore].VertexArray[p].Position.x;
					g_BlastVertex[p].Position.y += g_Blast_Vertex[brfore].VertexArray[p].Position.y;
					g_BlastVertex[p].Position.z += g_Blast_Vertex[brfore].VertexArray[p].Position.z;
				}

				// ���Ԃ�i�߂�
				switch (g_morphingNum)
				{
				case 0:
					if (g_time < 1.0f) g_time += 1.0f / BLAST_MOVE_TIME_0;

					break;

				case 1:
					if ((g_time < 1.0f) && (g_Blast[i].life < BLAST_LIFE - BLAST_MOVE_TIME_0 - BLAST_MOVE_TIME_1))
					{
						g_time += 1.0f / BLAST_MOVE_TIME_2;
						g_Blast[i].shrink = TRUE;
					}
					break;
				}

				// ��ꃂ�[�t�B���O�����������玟�̃��[�t�B���O��
				if ((g_time >= 1.0f) && (g_morphingNum == 0))
				{
					g_time = 0.0f;
					g_morphingNum++;
				}

				// ��񃂁[�t�B���O���I�������͂�����Ǝ~�܂�
				if ((g_time >= 1.0f) && (g_morphingNum == 1) && (g_downCount < BLASE_DOWN_SPEED))
				{
					g_stopTime++;
				}

				if ((g_stopTime >= BLAST_MOVE_TIME_3) && (g_downCount < BLASE_DOWN_SPEED))
				{
					g_Blast[i].pos.y -= BLAST_DOWN / BLASE_DOWN_SPEED;
					g_downCount++;

					g_cameraOn = FALSE;
				}
				
				// ��������͏��ƈꏏ�ɉ��ֈړ�����
				if (g_downCount >= BLASE_DOWN_SPEED)
				{
					g_Blast[i].pos.z += FIELD_SPEED;
					g_Blast[i].move = TRUE;
				}
					

				D3D11_SUBRESOURCE_DATA sd;
				ZeroMemory(&sd, sizeof(sd));
				sd.pSysMem = g_BlastVertex;

				// ���_�o�b�t�@�ɒl���Z�b�g����
				D3D11_MAPPED_SUBRESOURCE msr;
				GetDeviceContext()->Map(g_Blast[0].model.VertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &msr);
				VERTEX_3D* pVtx = (VERTEX_3D*)msr.pData;

				// �S���_���𖈉�㏑�����Ă���̂�DX11�ł͂��̕�����������ł�
				memcpy(pVtx, g_BlastVertex, sizeof(VERTEX_3D)*g_Blast_Vertex[0].VertexNum);

				GetDeviceContext()->Unmap(g_Blast[0].model.VertexBuffer, 0);
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
	//SetBlendState(BLEND_MODE_ADD);

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
			g_Blast[i].move = FALSE;
			g_Blast[i].shrink = FALSE;

			g_Blast[i].pos = pos;
			g_Blast[i].life = BLAST_LIFE;

			g_cameraOn = TRUE;

			g_time = 0.0f;
			g_morphingNum = 0;
			g_downCount = 0;
			g_stopTime = 0;

			g_Blast[i].rot.y = RamdomFloat(2, BLASE_ROT, -BLASE_ROT);

			// ������
			// PlaySound(SOUND_LABEL_SE_shot000);

			return;
		}
	}
}


// �J������؂�ւ��邩�ǂ����̃X�C�b�`��m��֐�
BOOL GetCameraSwitch(void)
{
	return g_cameraOn;
}


// �J������؂�ւ��邩�ǂ����̃X�C�b�`
void SetCameraSwitch(BOOL data)
{
	g_cameraOn = data;
}


// ���݂̃��[�t�B���O�ԍ����擾
int GetMorphing(void)
{
	return g_morphingNum;
}


int GetStopTime(void)
{
	return g_stopTime;
}