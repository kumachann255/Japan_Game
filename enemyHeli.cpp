//=============================================================================
//
// �G�l�~�[�w�����f������ [enemyHeli.cpp]
// Author : 
//
//=============================================================================
#include "main.h"
#include "input.h"
#include "camera.h"
#include "debugproc.h"
#include "model.h"
#include "enemyHeli.h"
#include "shadow.h"
#include "light.h"
#include "bullet.h"
#include "meshfield.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define	MODEL_HELI			"data/MODEL/heri.obj"			// �ǂݍ��ރ��f����
#define	MODEL_HELI_PARTS	"data/MODEL/puropera.obj"		// �ǂݍ��ރ��f����

#define	VALUE_MOVE			(2.0f)							// �ړ���
#define	VALUE_ROTATE		(XM_PI * 0.02f)					// ��]��

#define HELI_SHADOW_SIZE	(1.0f)							// �e�̑傫��
#define HELI_OFFSET_Y		(50.0f)						// �G�l�~�[�w���̑��������킹��

#define HELI_PARTS_MAX		(2)								// �G�l�~�[�w���̃p�[�c�̐�
#define ROT_ADDDTION		(0.5f)							// �G�l�~�[�w���̃p�[�c�̐�

#define HELI_UPPER_ADDTION	(0.2f)							// �G�l�~�[�w���̏㏸���Z�l
#define HELI_UNDER_ADDTION	(0.2f)							// �G�l�~�[�w���̉��~���Z�l

#define HELI_SPEED			(0.005f)						// �w���̑��x


//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************


//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
static HELI		g_Heli[MAX_HELI];					// �G�l�~�[�w��

static HELI		g_Parts[HELI_PARTS_MAX];			// �G�l�~�[�w���̃p�[�c�p

static XMFLOAT3		control0, control1, control2;	// ����

static BOOL		g_Load = FALSE;

//=============================================================================
// ����������
//=============================================================================
HRESULT InitHeli(void)
{

	for (int i = 0; i < MAX_HELI; i++)
	{
		LoadModel(MODEL_HELI, &g_Heli[i].model);
		g_Heli[i].load = TRUE;

		g_Heli[i].pos = { 0.0f, HELI_OFFSET_Y, 80.0f };
		g_Heli[i].rot = { 0.314f, 0.0f, 0.0f };
		g_Heli[i].scl = { 1.0f, 1.0f, 1.0f };

		g_Heli[i].spd = 0.0f;			// �ړ��X�s�[�h�N���A
		g_Heli[i].size = HELI_SIZE;		// �����蔻��̑傫��

		g_Heli[i].use = TRUE;
		g_Heli[i].switchFlag = TO_GO;
		g_Heli[i].patternFlag = PATTERN_02; // �����Ń��[�h�����炤
		g_Heli[i].speed = HELI_SPEED;
		g_Heli[i].time = 0.0f;			// ���ԃN���A

		// �󂯎���������œ����̃p�^�[���̍��W���Z�b�g
		switch (g_Heli[i].patternFlag)
		{
		case PATTERN_01:
			control0.x = -50.0f;
			control0.y =  0.0f;

			control1.z = 100.0f;
			control1.y = HELI_OFFSET_Y;

			control2.x = 50.0f;
			control2.y = 0.0f;
			break;

		case PATTERN_02:
			control0.x = -50.0f;
			control0.y = HELI_OFFSET_Y;

			control1.z = 100.0f;
			control1.y = -20.0f;

			control2.x = 50.0f;
			control2.y = HELI_OFFSET_Y;
			break;
		}
		// �����ŃG�l�~�[�w���p�̉e���쐬���Ă���
		XMFLOAT3 pos = g_Heli[i].pos;
		pos.y -= (HELI_OFFSET_Y - 0.1f);
		g_Heli[i].shadowIdx = CreateShadow(pos, HELI_SHADOW_SIZE, HELI_SHADOW_SIZE);
		//          ��
		//        ���̃����o�[�ϐ������������e��Index�ԍ�

		// �K�w�A�j���[�V�����p�̏���������
		g_Heli[i].parent = NULL;			// �{�́i�e�j�Ȃ̂�NULL������

			// �p�[�c�̏�����
		for (int i = 0; i < HELI_PARTS_MAX; i++)
		{
			g_Parts[i].use = FALSE;

			// �e�q�֌W
			g_Parts[i].parent = &g_Heli[i];		// �� �����ɐe�̃A�h���X������

			// �p�[�c�̓ǂݍ��݂͂܂����Ă��Ȃ�
			g_Parts[i].load = 0;
		}

		// �ʒu�E��]�E�X�P�[���̏����ݒ�
		g_Parts[0].pos = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_Parts[1].pos = XMFLOAT3(7.5f, 0.0f, -32.5f);

		g_Parts[0].rot = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_Parts[1].rot = XMFLOAT3(0.0f, 0.0f, 1.57f);

		g_Parts[0].scl = XMFLOAT3(1.0f, 1.0f, 1.0f);
		g_Parts[1].scl = XMFLOAT3(0.25f, 1.0f, 0.25f);


		g_Parts[0].use = TRUE;
		g_Parts[1].use = TRUE;

		g_Parts[0].parent = &g_Heli[i];		// �e���Z�b�g
		g_Parts[0].load = 1;
		LoadModel(MODEL_HELI_PARTS, &g_Parts[0].model);

		g_Parts[1].parent = &g_Heli[i];		// �e���Z�b�g
		g_Parts[1].load = 1;
		LoadModel(MODEL_HELI_PARTS, &g_Parts[1].model);


	}

	g_Load = TRUE;
	return S_OK;
}

//=============================================================================
// �I������
//=============================================================================
void UninitHeli(void)
{
	if (g_Load == FALSE) return;

	for (int i = 0; i < MAX_HELI; i++)
	{
		// ���f���̉������
		if (g_Heli[i].load)
		{
			UnloadModel(&g_Heli[i].model);
			g_Heli[i].load = FALSE;
		}
	}
	g_Load = FALSE;
}

//=============================================================================
// �X�V����
//=============================================================================
void UpdateHeli(void)
{

	for (int i = 0; i < MAX_HELI; i++)
	{

		if (g_Heli[i].use == TRUE)
		{
			// �K�w�A�j���[�V����
			for (int i = 0; i < HELI_PARTS_MAX; i++)
			{
				// �g���Ă���Ȃ珈������
				if ((g_Parts[i].use == TRUE))
				{
					// ��]����
					g_Parts[0].rot.y += 0.5f;
					g_Parts[1].rot.x += 0.5f;
				}
			}

			g_Heli[i].pos.x = ((1.0f - g_Heli[i].time) * (1.0f - g_Heli[i].time) * control0.x) +
				(2 * g_Heli[i].time * (1.0f - g_Heli[i].time) * control1.x) +
				(g_Heli[i].time * g_Heli[i].time * control2.x);

			g_Heli[i].pos.z = ((1.0f - g_Heli[i].time) * (1.0f - g_Heli[i].time) * control0.z) +
				(2 * g_Heli[i].time * (1.0f - g_Heli[i].time) * control1.z) +
				(g_Heli[i].time * g_Heli[i].time * control2.z);

			g_Heli[i].pos.y = ((1.0f - g_Heli[i].time) * (1.0f - g_Heli[i].time) * control0.y) +
				(2 * g_Heli[i].time * (1.0f - g_Heli[i].time) * control1.y) +
				(g_Heli[i].time * g_Heli[i].time * control2.y);

			// �w���̋����p�^�[���؂�ւ�
			switch (g_Heli[i].switchFlag)
			{
			case TO_GO:
				g_Heli[i].time += g_Heli[i].speed;

				if (1.0f <= g_Heli[i].time)
				{
					g_Heli[i].switchFlag = RETURN;
				}

				break;

			case RETURN:
				g_Heli[i].time -= g_Heli[i].speed;

				if (0.0f >= g_Heli[i].time)
				{
					g_Heli[i].switchFlag = TO_GO;
				}
				break;
			}

			// �e���G�l�~�[�w���̈ʒu�ɍ��킹��
			XMFLOAT3 pos = g_Heli[i].pos;
			pos.y = HELI_OFFSET_Y;
			SetPositionShadow(g_Heli[i].shadowIdx, pos);
		}
	}

#ifdef _DEBUG	// �f�o�b�O����\������
	//PrintDebugProc("HELI:�� �� �� ���@Space\n");
	//PrintDebugProc("HELI:X:%f Y:%f Z:%f\n", g_Heli[i].pos.x, g_Heli[i].pos.y, g_Heli[i].pos.z);
#endif
}

//=============================================================================
// �`�揈��
//=============================================================================
void DrawHeli(void)
{
	// �J�����O����
	SetCullingMode(CULL_MODE_NONE);

	XMMATRIX mtxScl, mtxRot, mtxTranslate, mtxWorld;

	for (int i = 0; i < MAX_HELI; i++)
	{

		// ���[���h�}�g���b�N�X�̏�����
		mtxWorld = XMMatrixIdentity();

		// �X�P�[���𔽉f
		mtxScl = XMMatrixScaling(g_Heli[i].scl.x, g_Heli[i].scl.y, g_Heli[i].scl.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

		// ��]�𔽉f
		mtxRot = XMMatrixRotationRollPitchYaw(g_Heli[i].rot.x, g_Heli[i].rot.y + XM_PI, g_Heli[i].rot.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);

		//// �N�H�[�^�j�I���𔽉f
		//XMMATRIX quatMatrix = XMMatrixRotationQuaternion(XMLoadFloat4(&g_Heli[i].quaternion));
		//mtxWorld = XMMatrixMultiply(mtxWorld, quatMatrix);

		// �ړ��𔽉f
		mtxTranslate = XMMatrixTranslation(g_Heli[i].pos.x, g_Heli[i].pos.y, g_Heli[i].pos.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

		// ���[���h�}�g���b�N�X�̐ݒ�
		SetWorldMatrix(&mtxWorld);

		XMStoreFloat4x4(&g_Heli[i].mtxWorld, mtxWorld);

		// ���f���`��
		DrawModel(&g_Heli[i].model);


		// �p�[�c�̊K�w�A�j���[�V����
		for (int i = 0; i < HELI_PARTS_MAX; i++)
		{
			// ���[���h�}�g���b�N�X�̏�����
			mtxWorld = XMMatrixIdentity();

			// �X�P�[���𔽉f
			mtxScl = XMMatrixScaling(g_Parts[i].scl.x, g_Parts[i].scl.y, g_Parts[i].scl.z);
			mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

			// ��]�𔽉f
			mtxRot = XMMatrixRotationRollPitchYaw(g_Parts[i].rot.x, g_Parts[i].rot.y, g_Parts[i].rot.z);
			mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);

			// �ړ��𔽉f
			mtxTranslate = XMMatrixTranslation(g_Parts[i].pos.x, g_Parts[i].pos.y, g_Parts[i].pos.z);
			mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

			if (g_Parts[i].parent != NULL)	// �q����������e�ƌ�������
			{
				mtxWorld = XMMatrixMultiply(mtxWorld, XMLoadFloat4x4(&g_Parts[i].parent->mtxWorld));
				// ��
				// g_Heli[i].mtxWorld���w���Ă���
			}

			XMStoreFloat4x4(&g_Parts[i].mtxWorld, mtxWorld);

			// �g���Ă���Ȃ珈������B�����܂ŏ������Ă��闝�R�͑��̃p�[�c�����̃p�[�c���Q�Ƃ��Ă���\�������邩��B
			if (g_Parts[i].use == FALSE) continue;

			// ���[���h�}�g���b�N�X�̐ݒ�
			SetWorldMatrix(&mtxWorld);


			// ���f���`��
			DrawModel(&g_Parts[i].model);
		}
	}
	// �J�����O�ݒ��߂�
	SetCullingMode(CULL_MODE_BACK);
}


//=============================================================================
// �G�l�~�[�w�������擾
//=============================================================================
HELI *GetHeli(void)
{
	return &g_Heli[0];
}

////=============================================================================
//// �G�l�~�[�w���Z�b�g
////=============================================================================
//void SetHeli(void)
//{
//	for (int i = 0; i < MAX_HELI; i++)
//	{
//		if (g_Heli[i].use == FALSE)
//		{
//			g_Heli[i].use = TRUE;
//			g_Heli[i].time = 0.0f;
//
//			PLAYER *pPlayer = GetPlayer();
//			ATTACKRANGE *AttackR = GetAttackR();
//
//			control0 = pPlayer->pos;
//			control2 = AttackR->pos;
//
//			float hight = 0.0f;	// ��������
//
//			// ����_�̎Z�o
//			control1.x = (control2.x - control0.x) / 2.0f + control0.x;
//			control1.z = (control2.z - control0.z) / 2.0f + control0.z;
//			control1.y = (control2.y - control0.y) / 2.0f + control0.y;
//
//			hight = BOM_H_MAX;
//
//			control1.y += hight;
//			control2.y += BOM_H;
//
//			// ��]���x�̎Z�o
//			rot.x = (float)(rand() % 50) / 1000.0f;
//			rot.y = (float)(rand() % 50) / 1000.0f;
//			rot.z = (float)(rand() % 50) / 1000.0f;
//
//			// ��]�̏�����
//			g_Heli[i].rot = XMFLOAT3(0.0f, 0.0f, 0.0f);
//		}
//	}
//}
