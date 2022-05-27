//=============================================================================
//
// �G�l�~�[���f������ [enemy.cpp]
// Author : aaa
//
//=============================================================================
#include "main.h"
#include "renderer.h"
#include "camera.h"
#include "input.h"
#include "model.h"
#include "enemy.h"
#include "blast.h"
#include "meshfield.h"
#include "shadow.h"
#include "collision.h"
#include "damageEF.h"
#include "debugproc.h"
#include "player.h"
#include "fade.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define	MODEL_ENEMY			"data/MODEL/patoka-.obj"		// �ǂݍ��ރ��f����
#define	MODEL_ENEMY_01		"data/MODEL/sirobai.obj"		// �ǂݍ��ރ��f����

#define ENEMY_TYPE_MAX		(2)							// �G�l�~�[�^�C�v�̍ő吔

#define	VALUE_MOVE			(3.0f)						// �ړ���
#define	VALUE_ROTATE		(XM_PI * 0.02f)				// ��]��

#define ENEMY_SHADOW_SIZE	(0.4f)						// �e�̑傫��

#define ENEMY_POP_Z			(500.0f)					// �G�l�~�[�̏����|�b�v�ʒu(z���W)
#define ENEMY_POP_X			(250)						// �G�l�~�[�̏����|�b�v�͈̔�(x���W)

#define ENEMY_GOAL_Z		(70.0f)						// �G�l�~�[�̃S�[����ʒu(z���W)
#define ENEMY_GOAL_Z_OFFSET	(60)						// �G�l�~�[�̃S�[���ʒu�̗���

#define STAGE0_POP_COUNT			(100)				// �G�l�~�[�̃|�b�v�Ԋu
#define STAGE0_MAX_POP				(20)				// �ő�A��ɉ��̃G�l�~�[���o����

#define STAGE1_POP_COUNT			(70)				// �G�l�~�[�̃|�b�v�Ԋu
#define STAGE1_MAX_POP				(25)				// �ő�A��ɉ��̃G�l�~�[���o����

#define STAGE2_POP_COUNT			(50)				// �G�l�~�[�̃|�b�v�Ԋu
#define STAGE2_MAX_POP				(30)				// �ő�A��ɉ��̃G�l�~�[���o����

#define STAGE3_POP_COUNT			(30)				// �G�l�~�[�̃|�b�v�Ԋu
#define STAGE3_MAX_POP				(45)				// �ő�A��ɉ��̃G�l�~�[���o����


#define ENEMY_HIT_MOVE		(5.0f)						// �����蔻���A�j���[�V�����p�ړ���

#define ENEMY_ATTACK_0		(300)						// �G�l�~�[���_�ł���܂ł̎���
#define ENEMY_ATTACK_1		(120 + ENEMY_ATTACK_0)		// �_�ł������Ȃ�܂ł̎���
#define ENEMY_ATTACK_2		(120 + ENEMY_ATTACK_1)		// �U������܂ł̎���

#define ENEMY_BLINKING0		(50)						// �_�ł̊Ԋu
#define ENEMY_BLINKING1		(14)						// �_�ł̊Ԋu

//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
void SetEnemy(void);


//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
static ENEMY			g_Enemy[MAX_ENEMY];				// �G�l�~�[

static BOOL				g_Load = FALSE;

static int				count = 0;		// �|�b�v�J�E���g



//=============================================================================
// ����������
//=============================================================================
HRESULT InitEnemy(void)
{
	LoadModel(MODEL_ENEMY, &g_Enemy[0].model);
	LoadModel(MODEL_ENEMY_01, &g_Enemy[1].model);

	for (int i = 0; i < MAX_ENEMY; i++)
	{	
		g_Enemy[i].load = TRUE;

		g_Enemy[i].pos = XMFLOAT3(-50.0f + i * 30.0f, ENEMY_OFFSET_Y, 20.0f);
		g_Enemy[i].rot = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_Enemy[i].scl = XMFLOAT3(1.0f, 1.0f, 1.0f);

		g_Enemy[i].spd = 0.0f;			// �ړ��X�s�[�h�N���A
		g_Enemy[i].size = ENEMY_SIZE;	// �����蔻��̑傫��

			// ���f���̃f�B�t���[�Y��ۑ����Ă����B�F�ς��Ή��ׁ̈B
		GetModelDiffuse(&g_Enemy[i].model, &g_Enemy[i].diffuse[0]);

		XMFLOAT3 pos = g_Enemy[i].pos;
		pos.y -= (ENEMY_OFFSET_Y - 0.1f);
		g_Enemy[i].shadowIdx = CreateShadow(pos, ENEMY_SHADOW_SIZE, ENEMY_SHADOW_SIZE);

		g_Enemy[i].use = FALSE;			// TRUE:�����Ă�

		g_Enemy[i].hitPos = XMFLOAT3(0.0f, ENEMY_OFFSET_Y, 0.0f);	// �����̒��S
		g_Enemy[i].hitRot = XMFLOAT3(0.0f, 0.0f, 0.0f);				// �����蔻���A�j���[�V�����p�X�s�[�h
		g_Enemy[i].isHit = FALSE;									// TRUE:�������Ă�
		g_Enemy[i].hitTime = 0;										// �^�C�~���O�Ǘ��p
		g_Enemy[i].liveCount = 0;		// �������Ԃ����Z�b�g
		g_Enemy[i].type = 0;			// �G�l�~�[�^�C�v

		g_Enemy[i].fuchi = FALSE;

	}


	g_Load = TRUE;
	return S_OK;
}

//=============================================================================
// �I������
//=============================================================================
void UninitEnemy(void)
{
	if (g_Load == FALSE) return;

	for (int i = 0; i < MAX_ENEMY; i++)
	{
		if (g_Enemy[i].load)
		{
			UnloadModel(&g_Enemy[i].model);
			g_Enemy[i].load = FALSE;
		}
	}

	g_Load = FALSE;
}

//=============================================================================
// �X�V����
//=============================================================================
void UpdateEnemy(void)
{
	if (GetStage() == tutorial)
	{	// �`���[�g���A���l��1�̏o��
		count++;

		// ���ԂɂȂ�����1�̏o��
		if (count == STAGE0_POP_COUNT)
		{
			SetEnemy();
		}

		// �G�l�~�[���|���ꂽ��
		if ((g_Enemy[0].use == FALSE) && (count > STAGE0_POP_COUNT))
		{
			// �`���[�g���A�������̃e���b�v�\��


			// Enter����������X�e�[�W�J��
			if (GetKeyboardTrigger(DIK_RETURN))
			{
				SetStage(stage0);
				SetFade(FADE_OUT, MODE_GAME);
			}
		}
	}
	else
	{	// pop����
		count++;
		int useCount = 0;

		switch (GetStage())
		{
		case stage0:
			// �����̏o�����Ă��邩���m�F
			for (int i = 0; i < MAX_ENEMY; i++)
			{
				if (g_Enemy[i].use == TRUE) useCount++;
			}

			// ���Ԍo�߂ƃG�l�~�[�̏o��������Ń|�b�v���邩���f
			if ((count % STAGE0_POP_COUNT == 0) && (useCount < STAGE0_MAX_POP))
			{

				SetEnemy();
			}

			break;

		case stage1:
			// �����̏o�����Ă��邩���m�F
			for (int i = 0; i < MAX_ENEMY; i++)
			{
				if (g_Enemy[i].use == TRUE) useCount++;
			}

			// ���Ԍo�߂ƃG�l�~�[�̏o��������Ń|�b�v���邩���f
			if ((count % STAGE1_POP_COUNT == 0) && (useCount < STAGE1_MAX_POP))
			{

				SetEnemy();
			}

			break;

		case stage2:
			// �����̏o�����Ă��邩���m�F
			for (int i = 0; i < MAX_ENEMY; i++)
			{
				if (g_Enemy[i].use == TRUE) useCount++;
			}

			// ���Ԍo�߂ƃG�l�~�[�̏o��������Ń|�b�v���邩���f
			if ((count % STAGE2_POP_COUNT == 0) && (useCount < STAGE2_MAX_POP))
			{

				SetEnemy();
			}

			break;

		case stage3:
			// �����̏o�����Ă��邩���m�F
			for (int i = 0; i < MAX_ENEMY; i++)
			{
				if (g_Enemy[i].use == TRUE) useCount++;
			}

			// ���Ԍo�߂ƃG�l�~�[�̏o��������Ń|�b�v���邩���f
			if ((count % STAGE3_POP_COUNT == 0) && (useCount < STAGE3_MAX_POP))
			{

				SetEnemy();
			}

			break;

		}
	}


	// �G�l�~�[�𓮂����ꍇ�́A�e�����킹�ē���������Y��Ȃ��悤�ɂˁI
	for (int i = 0; i < MAX_ENEMY; i++)
	{
		if (g_Enemy[i].use == TRUE)			// ���̃G�l�~�[���g���Ă���H
		{									// Yes
			// �������Ԃ��J�E���g
			if(GetStage() != tutorial) g_Enemy[i].liveCount++;

			// �U����H����Ă��Ȃ���΍U������
			if (g_Enemy[i].isHit == FALSE)
			{
				// �U������
				if (g_Enemy[i].liveCount > ENEMY_ATTACK_2)
				{	// �U�����s��
					// �������Ԃ����Z�b�g
					g_Enemy[i].liveCount = 0;

					// �������C�g�I�t
					g_Enemy[i].fuchi = FALSE;


					// �U��
					SetDamageEF(TRUE);
					SetCameraShake(20);
					SetPlayerDamage(1);

				}
				else if(g_Enemy[i].liveCount > ENEMY_ATTACK_1)
				{	// �Ԃ��_�ł������Ȃ�
				
					if (g_Enemy[i].liveCount % ENEMY_BLINKING1 < ENEMY_BLINKING1 / 2)
					{	// �I�u�W�F�N�g��Ԃ�����

						// �������C�g�I��
						g_Enemy[i].fuchi = TRUE;
					}
					else
					{	// �I�u�W�F�N�g�̐F��߂�

						// �������C�g�I�t
						g_Enemy[i].fuchi = FALSE;
					}


				}
				else if (g_Enemy[i].liveCount > ENEMY_ATTACK_0)
				{	// �Ԃ��_�ł���

					if (g_Enemy[i].liveCount % ENEMY_BLINKING0 < ENEMY_BLINKING0 / 2)
					{	// �I�u�W�F�N�g��Ԃ�����

						// �������C�g�I��
						g_Enemy[i].fuchi = TRUE;
					}
					else
					{	// �I�u�W�F�N�g�̐F��߂�

						// �������C�g�I�t
						g_Enemy[i].fuchi = FALSE;
					}

				}
			}


			// �ڕW�n�_�܂œ��B���Ă��Ȃ��ꍇ�Ɉړ�����
			if (g_Enemy[i].pos.z > g_Enemy[i].zGoal)
			{
				BOOL ans = TRUE;
				// ���̃p�g�J�[�Ɠ������Ă��Ȃ������m�F
				for (int p = 0; p < MAX_ENEMY; p++)
				{
					//�G�̗L���t���O���`�F�b�N����
					if ((g_Enemy[p].use == FALSE) || (i == p) || (g_Enemy[p].isHit == TRUE)) continue;

					//BC�̓����蔻��
					if (CollisionBC(g_Enemy[i].pos, g_Enemy[p].pos, g_Enemy[p].size, g_Enemy[p].size) &&
						(g_Enemy[i].pos.z > g_Enemy[p].pos.z))
					{	// �������Ă��Ȃ��ꍇ�Ɉړ�

						ans = FALSE;
						break;
					}
				}

				if (ans)
				{
					g_Enemy[i].pos.z -= VALUE_MOVE;
				}
			}



			// �G�l�~�[�̏����A�j���[�V����
			if (g_Enemy[i].isHit == TRUE)				// �U�����������Ă邩�H
			{											// Yes
				//BOOL ans = TRUE;

				//// �ق��̃p�g�J�[�ƂԂ����ĂȂ����H
				//for (int j = 0; j < MAX_ENEMY; j++)
				//{
				//	if ((g_Enemy[j].isHit == FALSE) || (i == j)) continue;	// �U���ɓ������ĂȂ��z�ɂ͓����蔻��̃`�F�b�N���X�L�b�v

				//	if (CollisionBC(g_Enemy[i].pos, g_Enemy[j].pos, g_Enemy[i].size - 20.0f, g_Enemy[j].size - 20.0f))
				//	{
				//		ans = FALSE;
				//		break;
				//	}
				//
				//}

				//// ���܂����ʒu�܂ŗ��Ă������H
				//if (CollisionBC(g_Enemy[i].pos, g_Enemy[i].hitPos, g_Enemy[i].size, g_Enemy[i].size))
				//{
				//	ans = FALSE;
				//}


				BLAST *blast = GetBlast();		// ���j�I�u�W�F�N�g�̏�����

				// �k�܂鏈��
				if ((blast[0].shrink) && (g_Enemy[i].hitTime > 0))
				{
					g_Enemy[i].pos.x += (g_Enemy[i].hitPos.x - g_Enemy[i].pos.x) / ENEMY_HIT_MOVE;
					g_Enemy[i].pos.y += (g_Enemy[i].hitPos.y - g_Enemy[i].pos.y) / ENEMY_HIT_MOVE;
					g_Enemy[i].pos.z += (g_Enemy[i].hitPos.z - g_Enemy[i].pos.z) / ENEMY_HIT_MOVE;

					// �����_���ɉ�]������
					g_Enemy[i].hitRot.x = RamdomFloat(2, 6.28f, -6.28f);
					g_Enemy[i].hitRot.y = RamdomFloat(2, 6.28f, -6.28f);
					g_Enemy[i].hitRot.z = RamdomFloat(2, 6.28f, -6.28f);

					if (g_Enemy[i].hitTime == 15)
					{
						g_Enemy[i].rot.x += g_Enemy[i].hitRot.x;
						g_Enemy[i].rot.y += g_Enemy[i].hitRot.y;
						g_Enemy[i].rot.z += g_Enemy[i].hitRot.z;
					}

					g_Enemy[i].hitTime--;
				}
				


				//if (ans)
				//{
				//	g_Enemy[i].pos.x += (g_Enemy[i].hitPos.x - g_Enemy[i].pos.x) / ENEMY_HIT_MOVE;
				//	g_Enemy[i].pos.y += (g_Enemy[i].hitPos.y - g_Enemy[i].pos.y) / ENEMY_HIT_MOVE;
				//	g_Enemy[i].pos.z += (g_Enemy[i].hitPos.z - g_Enemy[i].pos.z) / ENEMY_HIT_MOVE;
				//}


				// ���e�ƈꏏ�ɗ�������
				BOOL camera = GetCameraSwitch();

				if (camera == FALSE && blast[0].move == FALSE)
				{
					g_Enemy[i].pos.y -= BLAST_DOWN / BLASE_DOWN_SPEED;

					if (g_Enemy[i].pos.y < ENEMY_OFFSET_Y)
					{
						g_Enemy[i].pos.y = ENEMY_OFFSET_Y;
					}
				}
				
				//���e�ƈꏏ�ɉ��ֈړ�����
				if (blast[0].move == TRUE) /*&& (g_Enemy[i].move == TRUE)*/ //&& (g_Enemy[i].hitTime == 0))
				{
					g_Enemy[i].pos.z += FIELD_SPEED+ 2.0f;
				}


				if (blast[0].use == FALSE)
				{
					g_Enemy[i].use = FALSE;
				}
			}


			// �e���v���C���[�̈ʒu�ɍ��킹��
			XMFLOAT3 pos = g_Enemy[i].pos;
			pos.y -= (ENEMY_OFFSET_Y - 0.1f);
			SetPositionShadow(g_Enemy[i].shadowIdx, pos);
		}



	}


#ifdef _DEBUG	// �f�o�b�O����\������
	//PrintDebugProc("enmey spd x:%f y:%f z:%f \n ", g_Enemy[0].hitSpd.x, g_Enemy[0].hitSpd.y, g_Enemy[0].hitSpd.z);
#endif

}

//=============================================================================
// �`�揈��
//=============================================================================
void DrawEnemy(void)
{
	XMMATRIX mtxScl, mtxRot, mtxTranslate, mtxWorld;

	// �J�����O����
	SetCullingMode(CULL_MODE_NONE);

	for (int i = 0; i < MAX_ENEMY; i++)
	{
		if (g_Enemy[i].use == FALSE) continue;

		// �������C�g�̐ݒ�
		SetFuchi(g_Enemy[i].fuchi);

		// ���[���h�}�g���b�N�X�̏�����
		mtxWorld = XMMatrixIdentity();

		// �X�P�[���𔽉f
		mtxScl = XMMatrixScaling(g_Enemy[i].scl.x, g_Enemy[i].scl.y, g_Enemy[i].scl.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

		// ��]�𔽉f
		mtxRot = XMMatrixRotationRollPitchYaw(g_Enemy[i].rot.x, g_Enemy[i].rot.y + XM_PI, g_Enemy[i].rot.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);

		// �ړ��𔽉f
		mtxTranslate = XMMatrixTranslation(g_Enemy[i].pos.x, g_Enemy[i].pos.y, g_Enemy[i].pos.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

		// ���[���h�}�g���b�N�X�̐ݒ�
		SetWorldMatrix(&mtxWorld);

		XMStoreFloat4x4(&g_Enemy[i].mtxWorld, mtxWorld);


		// ���f���`��
		switch (g_Enemy[i].type)
		{
		case 0:
			DrawModel(&g_Enemy[0].model);
			break;

		case 1:
			DrawModel(&g_Enemy[1].model);
			break;
		}

		// �������C�g�̐ݒ�
		SetFuchi(FALSE);
	}

	// �J�����O�ݒ��߂�
	SetCullingMode(CULL_MODE_BACK);
}

//=============================================================================
// �G�l�~�[�̎擾
//=============================================================================
ENEMY *GetEnemy()
{
	return &g_Enemy[0];
}


//=============================================================================
// �G�l�~�[�̔���
//=============================================================================
void SetEnemy(void)
{
	for (int i = 0; i < MAX_ENEMY; i++)
	{
		if (g_Enemy[i].use == FALSE)
		{
			g_Enemy[i].use = TRUE;
			g_Enemy[i].pos.z = ENEMY_POP_Z;
			g_Enemy[i].pos.y = ENEMY_OFFSET_Y;
			g_Enemy[i].isHit = FALSE;
			g_Enemy[i].rot = XMFLOAT3(0.0f, 0.0f, 0.0f);

			// x���W�̓����_��
			g_Enemy[i].pos.x = (float)(rand() % ENEMY_POP_X) - ((float)ENEMY_POP_X / 2.0f);
			if (GetStage() == tutorial) g_Enemy[i].pos.x = 0.0f;

			// ���B�n�_�������_��
			g_Enemy[i].zGoal = (float)(rand() % ENEMY_GOAL_Z_OFFSET) + ENEMY_GOAL_Z;
			if (GetStage() == tutorial) g_Enemy[i].zGoal = 150.0f;

			g_Enemy[i].liveCount = 0;

			// �G�l�~�[�̃^�C�v�������_����
			g_Enemy[i].type = rand() % ENEMY_TYPE_MAX;
			// �������C�g�I�t
			g_Enemy[i].fuchi = FALSE;

			return;
		}
	}
}



void SetEnemyMove(void)
{

}