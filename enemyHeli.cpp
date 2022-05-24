//=============================================================================
//
// �w���G�l�~�[���f������ [enemyHeli.cpp]
// Author : hennkousitayo
//
//=============================================================================
#include "main.h"
#include "renderer.h"
#include "camera.h"
#include "input.h"
#include "model.h"
#include "enemyHeli.h"
#include "blast.h"
#include "meshfield.h"
#include "shadow.h"
#include "collision.h"
#include "damageEF.h"
#include "debugproc.h"
#include "player.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define	MODEL_ENEMY_HELI		"data/MODEL/heri.obj"		// �ǂݍ��ރ��f����
#define	MODEL_HELI_PARTS		"data/MODEL/puropera.obj"	// �ǂݍ��ރ��f����

#define	VALUE_MOVE				(3.0f)						// �ړ���
#define	VALUE_ROTATE			(0.5f)						// ��]��
#define ENEMY_HELI_SPEED		(0.0025f)					// �_�ł̊Ԋu

#define ENEMY_HELI_SHADOW_SIZE	(0.4f)						// �e�̑傫��

#define ENEMY_HELI_POP_Z		(500.0f)					// �w���G�l�~�[�̏����|�b�v�ʒu(z���W)
#define ENEMY_HELI_POP_X		(250)						// �w���G�l�~�[�̏����|�b�v�͈̔�(x���W)

#define ENEMY_HELI_GOAL_Z		(70.0f)						// �w���G�l�~�[�̃S�[����ʒu(z���W)
#define ENEMY_HELI_GOAL_Z_OFFSET (60)						// �w���G�l�~�[�̃S�[���ʒu�̗���

#define POP_COUNT				(300)						// �w���G�l�~�[�̃|�b�v�Ԋu
#define MAX_POP					(2)							// �ő�A��ɉ��̃w���G�l�~�[���o����

#define ENEMY_HELI_HIT_MOVE		(5.0f)						// �����蔻���A�j���[�V�����p�ړ���

#define ENEMY_HELI_ATTACK_0		(300)						// �w���G�l�~�[���_�ł���܂ł̎���
#define ENEMY_HELI_ATTACK_1		(120 + ENEMY_HELI_ATTACK_0)	// �_�ł������Ȃ�܂ł̎���
#define ENEMY_HELI_ATTACK_2		(120 + ENEMY_HELI_ATTACK_1)	// �U������܂ł̎���

#define ENEMY_HELI_BLINKING_0	(50)						// �_�ł̊Ԋu
#define ENEMY_HELI_BLINKING_1	(14)						// �_�ł̊Ԋu

//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
void SetEnemyHeli(void);


//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
static ENEMY_HELI	g_EnemyHeli[MAX_ENEMY_HELI];	// �w���G�l�~�[

static ENEMY_HELI	g_Parts[HELI_PARTS_MAX];		// �w���G�l�~�[

static XMFLOAT3		control0, control1, control2;	// �w���G�l�~�[�̋�������

static BOOL			g_Load = FALSE;

static int			count = 0;						// �|�b�v�J�E���g



//=============================================================================
// ����������
//=============================================================================
HRESULT InitEnemyHeli(void)
{

	for (int i = 0; i < MAX_ENEMY_HELI; i++)
	{
		g_EnemyHeli[i].load = TRUE;
		LoadModel(MODEL_ENEMY_HELI, &g_EnemyHeli[i].model);

		g_EnemyHeli[i].pos = XMFLOAT3(0.0f, ENEMY_HELI_OFFSET_Y, 20.0f);
		g_EnemyHeli[i].rot = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_EnemyHeli[i].scl = XMFLOAT3(1.0f, 1.0f, 1.0f);

		g_EnemyHeli[i].spd = 0.0f;			// �ړ��X�s�[�h�N���A
		g_EnemyHeli[i].size = ENEMY_HELI_SIZE;	// �����蔻��̑傫��

		// ���f���̃f�B�t���[�Y��ۑ����Ă����B�F�ς��Ή��ׁ̈B
		GetModelDiffuse(&g_EnemyHeli[i].model, &g_EnemyHeli[i].diffuse[0]);

		XMFLOAT3 pos = g_EnemyHeli[i].pos;
		pos.y -= (ENEMY_HELI_OFFSET_Y - 0.1f);
		g_EnemyHeli[i].shadowIdx = CreateShadow(pos, ENEMY_HELI_SHADOW_SIZE, ENEMY_HELI_SHADOW_SIZE);

		g_EnemyHeli[i].use = FALSE;			// TRUE:�����Ă�
		g_EnemyHeli[i].switchFlag = TRUE;	// �x�W�F�Ȑ��t���O TRUE = time+ / FALSE = time-
		g_EnemyHeli[i].time = 0.0f;			// �x�W�F�Ȑ��p���ԃ��Z�b�g
		g_EnemyHeli[i].speed = ENEMY_HELI_SPEED;


		g_EnemyHeli[i].hitPos = XMFLOAT3(0.0f, ENEMY_HELI_OFFSET_Y, 0.0f);	// �����̒��S
		g_EnemyHeli[i].hitRot = XMFLOAT3(0.0f, 0.0f, 0.0f);				// �����蔻���A�j���[�V�����p�X�s�[�h
		g_EnemyHeli[i].isHit = FALSE;					// TRUE:�������Ă�
		g_EnemyHeli[i].move = FALSE;					// ���ֈړ�����t���O TRUE:�ړ�����
		g_EnemyHeli[i].hitTime = 0;						// ���ֈړ�����t���O TRUE:�ړ�����

		g_EnemyHeli[i].liveCount = 0;		// �������Ԃ����Z�b�g

		g_EnemyHeli[i].fuchi = FALSE;

		// �G�l�~�[�w���̃p�[�c
		// �p�[�c�̏�����
		for (int i = 0; i < HELI_PARTS_MAX; i++)
		{
			g_Parts[i].use = FALSE;

			// �e�q�֌W
			g_Parts[i].parent = &g_EnemyHeli[i];		// �� �����ɐe�̃A�h���X������

			// �p�[�c�̓ǂݍ��݂͂܂����Ă��Ȃ�
			g_Parts[i].load = 0;
		}

		g_Parts[0].pos = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_Parts[1].pos = XMFLOAT3(7.5f, 0.0f, -32.5f);

		g_Parts[0].rot = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_Parts[1].rot = XMFLOAT3(0.0f, 0.0f, 1.57f);

		g_Parts[0].scl = XMFLOAT3(1.0f, 1.0f, 1.0f);
		g_Parts[1].scl = XMFLOAT3(0.25f, 1.0f, 0.25f);


		g_Parts[0].use = TRUE;
		g_Parts[1].use = TRUE;

		g_Parts[0].parent = &g_EnemyHeli[i];		// �e���Z�b�g
		g_Parts[0].load = 1;
		LoadModel(MODEL_HELI_PARTS, &g_Parts[0].model);

		g_Parts[1].parent = &g_EnemyHeli[i];		// �e���Z�b�g
		g_Parts[1].load = 1;
		LoadModel(MODEL_HELI_PARTS, &g_Parts[1].model);

		//g_Parts[2].pos = XMFLOAT3(0.0f, 0.0f, 0.0f);
		//g_Parts[3].pos = XMFLOAT3(7.5f, 0.0f, -32.5f);

		//g_Parts[2].rot = XMFLOAT3(0.0f, 0.0f, 0.0f);
		//g_Parts[3].rot = XMFLOAT3(0.0f, 0.0f, 1.57f);

		//g_Parts[2].scl = XMFLOAT3(1.0f, 1.0f, 1.0f);
		//g_Parts[3].scl = XMFLOAT3(0.25f, 1.0f, 0.25f);


		//g_Parts[2].use = TRUE;
		//g_Parts[3].use = TRUE;

		//g_Parts[2].parent = &g_EnemyHeli[1];		// �e���Z�b�g
		//g_Parts[2].load = 1;
		//LoadModel(MODEL_HELI_PARTS, &g_Parts[2].model);

		//g_Parts[3].parent = &g_EnemyHeli[1];		// �e���Z�b�g
		//g_Parts[3].load = 1;
		//LoadModel(MODEL_HELI_PARTS, &g_Parts[3].model);


	}


	g_Load = TRUE;
	return S_OK;
}

//=============================================================================
// �I������
//=============================================================================
void UninitEnemyHeli(void)
{
	if (g_Load == FALSE) return;

	for (int i = 0; i < MAX_ENEMY_HELI; i++)
	{
		if (g_EnemyHeli[i].load)
		{
			UnloadModel(&g_EnemyHeli[i].model);
			g_EnemyHeli[i].load = FALSE;
		}
	}

	g_Load = FALSE;
}

//=============================================================================
// �X�V����
//=============================================================================
void UpdateEnemyHeli(void)
{
	// pop����
	{
		count++;
		int useCount = 0;

		// �����̏o�����Ă��邩���m�F
		for (int i = 0; i < MAX_ENEMY_HELI; i++)
		{
			if (g_EnemyHeli[i].use == TRUE) useCount++;
		}

		// ���Ԍo�߂ƃw���G�l�~�[�̏o��������Ń|�b�v���邩���f
		if ((count % POP_COUNT == 0) && (useCount < MAX_POP))
		{

			SetEnemyHeli();
		}
	}


	// �w���G�l�~�[�𓮂����ꍇ�́A�e�����킹�ē���������Y��Ȃ��悤�ɂˁI
	for (int i = 0; i < MAX_ENEMY_HELI; i++)
	{
		if (g_EnemyHeli[i].use == TRUE)			// ���̃w���G�l�~�[���g���Ă���H
		{									// Yes
			// �������Ԃ��J�E���g
			g_EnemyHeli[i].liveCount++;

			// �U����H����Ă��Ȃ���΍U������
			if (g_EnemyHeli[i].isHit == FALSE)
			{
				// �v���y����]����
				{
					g_Parts[0].rot.y += VALUE_ROTATE;
					g_Parts[1].rot.x += VALUE_ROTATE;
				}

				// �x�W�F�Ȑ��ł̈ړ�
				{
					g_EnemyHeli[i].pos.x = ((1.0f - g_EnemyHeli[i].time) * (1.0f - g_EnemyHeli[i].time) * control0.x) +
						(2 * g_EnemyHeli[i].time * (1.0f - g_EnemyHeli[i].time) * control1.x) +
						(g_EnemyHeli[i].time * g_EnemyHeli[i].time * control2.x);

					//g_EnemyHeli[i].pos.z = ((1.0f - g_EnemyHeli[i].time) * (1.0f - g_EnemyHeli[i].time) * control0.z) +
					//	(2 * g_EnemyHeli[i].time * (1.0f - g_EnemyHeli[i].time) * control1.z) +
					//	(g_EnemyHeli[i].time * g_EnemyHeli[i].time * control2.z);

					g_EnemyHeli[i].pos.y = ((1.0f - g_EnemyHeli[i].time) * (1.0f - g_EnemyHeli[i].time) * control0.y) +
						(2 * g_EnemyHeli[i].time * (1.0f - g_EnemyHeli[i].time) * control1.y) +
						(g_EnemyHeli[i].time * g_EnemyHeli[i].time * control2.y);
				}

				// �w���̋����p�^�[���؂�ւ�
				switch (g_EnemyHeli[i].switchFlag)
				{
				case TRUE:
					g_EnemyHeli[i].time += g_EnemyHeli[i].speed;

					if (1.0f <= g_EnemyHeli[i].time)
					{
						g_EnemyHeli[i].switchFlag = FALSE;
					}

					break;

				case FALSE:
					g_EnemyHeli[i].time -= g_EnemyHeli[i].speed;

					if (0.0f >= g_EnemyHeli[i].time)
					{
						g_EnemyHeli[i].switchFlag = TRUE;
					}
					break;
				}

				// �U������
				if (g_EnemyHeli[i].liveCount > ENEMY_HELI_ATTACK_2)
				{	// �U�����s��
					// �������Ԃ����Z�b�g
					g_EnemyHeli[i].liveCount = 0;

					// �������C�g�I�t
					g_EnemyHeli[i].fuchi = FALSE;


					// �U��
					SetDamageEF(TRUE);
					SetCameraShake(20);
					SetPlayerDamage(1);

				}
				else if (g_EnemyHeli[i].liveCount > ENEMY_HELI_ATTACK_1)
				{	// �Ԃ��_�ł������Ȃ�

					if (g_EnemyHeli[i].liveCount % ENEMY_HELI_BLINKING_1 < ENEMY_HELI_BLINKING_1 / 2)
					{	// �I�u�W�F�N�g��Ԃ�����

						// �������C�g�I��
						g_EnemyHeli[i].fuchi = TRUE;
					}
					else
					{	// �I�u�W�F�N�g�̐F��߂�

						// �������C�g�I�t
						g_EnemyHeli[i].fuchi = FALSE;
					}


				}
				else if (g_EnemyHeli[i].liveCount > ENEMY_HELI_ATTACK_0)
				{	// �Ԃ��_�ł���

					if (g_EnemyHeli[i].liveCount % ENEMY_HELI_BLINKING_0 < ENEMY_HELI_BLINKING_0 / 2)
					{	// �I�u�W�F�N�g��Ԃ�����

						// �������C�g�I��
						g_EnemyHeli[i].fuchi = TRUE;
					}
					else
					{	// �I�u�W�F�N�g�̐F��߂�

						// �������C�g�I�t
						g_EnemyHeli[i].fuchi = FALSE;
					}

				}
			}


			// �ڕW�n�_�܂œ��B���Ă��Ȃ��ꍇ�Ɉړ�����
			if (g_EnemyHeli[i].pos.z > g_EnemyHeli[i].zGoal)
			{
				BOOL ans = TRUE;
				// ���̃p�g�J�[�Ɠ������Ă��Ȃ������m�F
				for (int p = 0; p < MAX_ENEMY_HELI; p++)
				{
					//�G�̗L���t���O���`�F�b�N����
					if ((g_EnemyHeli[p].use == FALSE) || (i == p)) continue;

					//BC�̓����蔻��
					if (CollisionBC(g_EnemyHeli[i].pos, g_EnemyHeli[p].pos, g_EnemyHeli[p].size / 5.0f, g_EnemyHeli[p].size / 5.0f) &&
						(g_EnemyHeli[i].pos.z > g_EnemyHeli[p].pos.z))
					{	// �������Ă��Ȃ��ꍇ�Ɉړ�

						ans = FALSE;
						break;
					}
				}

				if (ans)
				{
					g_EnemyHeli[i].pos.z -= VALUE_MOVE;
				}
			}



			// �w���G�l�~�[�̏����A�j���[�V����
			if (g_EnemyHeli[i].isHit == TRUE)				// �U�����������Ă邩�H
			{											// Yes

				//g_EnemyHeli[i].pos_old.x = g_EnemyHeli[i].pos.x;
				//g_EnemyHeli[i].pos_old.y = g_EnemyHeli[i].pos.y;
				//g_EnemyHeli[i].pos_old.z = g_EnemyHeli[i].pos.z;

				//BOOL ans = TRUE;

				//// �ق��̃p�g�J�[�ƂԂ����ĂȂ����H
				//for (int j = 0; j < MAX_ENEMY; j++)
				//{
				//	if ((g_EnemyHeli[j].isHit == FALSE) || (i == j)) continue;	// �U���ɓ������ĂȂ��z�ɂ͓����蔻��̃`�F�b�N���X�L�b�v

				//	if (CollisionBC(g_EnemyHeli[i].pos, g_EnemyHeli[j].pos, g_EnemyHeli[i].size - 20.0f, g_EnemyHeli[j].size - 20.0f))
				//	{
				//		ans = FALSE;
				//		break;
				//	}
				//
				//}

				//// ���܂����ʒu�܂ŗ��Ă������H
				//if (CollisionBC(g_EnemyHeli[i].pos, g_EnemyHeli[i].hitPos, g_EnemyHeli[i].size, g_EnemyHeli[i].size))
				//{
				//	ans = FALSE;
				//}


				//XMFLOAT3 temp = XMFLOAT3(g_EnemyHeli[i].pos.x - g_EnemyHeli[i].hitPos.x, g_EnemyHeli[i].pos.y - g_EnemyHeli[i].hitPos.y, g_EnemyHeli[i].pos.z - g_EnemyHeli[i].hitPos.z);
				//float lenSq = (temp.x * temp.x) + (temp.y * temp.y) + (temp.z * temp.z);

				//if (lenSq > 5000)
				//{
				//	g_EnemyHeli[i].pos.x -= g_EnemyHeli[i].hitSpd.x;
				//	g_EnemyHeli[i].pos.y -= g_EnemyHeli[i].hitSpd.y;
				//	g_EnemyHeli[i].pos.z -= g_EnemyHeli[i].hitSpd.z;
				//}
				BLAST *blast = GetBlast();		// ���j�I�u�W�F�N�g�̏�����

				// �k�܂鏈��
				if ((blast[0].shrink) && (g_EnemyHeli[i].hitTime > 0))
				{
					g_EnemyHeli[i].pos.x += (g_EnemyHeli[i].hitPos.x - g_EnemyHeli[i].pos.x) / ENEMY_HELI_HIT_MOVE;
					g_EnemyHeli[i].pos.y += (g_EnemyHeli[i].hitPos.y - g_EnemyHeli[i].pos.y) / ENEMY_HELI_HIT_MOVE;
					g_EnemyHeli[i].pos.z += (g_EnemyHeli[i].hitPos.z - g_EnemyHeli[i].pos.z) / ENEMY_HELI_HIT_MOVE;

					// �����_���ɉ�]������
					g_EnemyHeli[i].hitRot.x = RamdomFloat(2, 6.28f, -6.28f);
					g_EnemyHeli[i].hitRot.y = RamdomFloat(2, 6.28f, -6.28f);
					g_EnemyHeli[i].hitRot.z = RamdomFloat(2, 6.28f, -6.28f);

					if (g_EnemyHeli[i].hitTime == 15)
					{
						g_EnemyHeli[i].rot.x += g_EnemyHeli[i].hitRot.x;
						g_EnemyHeli[i].rot.y += g_EnemyHeli[i].hitRot.y;
						g_EnemyHeli[i].rot.z += g_EnemyHeli[i].hitRot.z;
					}

					g_EnemyHeli[i].hitTime--;
				}



				// ���e�ƈꏏ�ɗ�������
				BOOL camera = GetCameraSwitch();

				if (camera == FALSE && blast[0].move == FALSE)
				{
					g_EnemyHeli[i].pos.y -= BLAST_DOWN / BLASE_DOWN_SPEED;

					if (g_EnemyHeli[i].pos.y < ENEMY_HELI_OFFSET_Y)
					{
						g_EnemyHeli[i].pos.y = ENEMY_HELI_OFFSET_Y;
					}
				}

				//���e�ƈꏏ�ɉ��ֈړ�����
				if (blast[0].move == TRUE) /*&& (g_EnemyHeli[i].move == TRUE)*/ //&& (g_EnemyHeli[i].hitTime == 0))
				{
					g_EnemyHeli[i].pos.z += FIELD_SPEED + 2.0f;
				}


				if (blast[0].use == FALSE)
				{
					g_EnemyHeli[i].use = FALSE;
				}
			}


			// �e���v���C���[�̈ʒu�ɍ��킹��
			XMFLOAT3 pos = g_EnemyHeli[i].pos;
			pos.y -= (ENEMY_HELI_OFFSET_Y - 0.1f);
			SetPositionShadow(g_EnemyHeli[i].shadowIdx, pos);
		}



	}


#ifdef _DEBUG	// �f�o�b�O����\������
	//PrintDebugProc("enmey spd x:%f y:%f z:%f \n ", g_EnemyHeli[0].hitSpd.x, g_EnemyHeli[0].hitSpd.y, g_EnemyHeli[0].hitSpd.z);
#endif

}

//=============================================================================
// �`�揈��
//=============================================================================
void DrawEnemyHeli(void)
{
	XMMATRIX mtxScl, mtxRot, mtxTranslate, mtxWorld;

	// �J�����O����
	SetCullingMode(CULL_MODE_NONE);

	for (int i = 0; i < MAX_ENEMY_HELI; i++)
	{
		if (g_EnemyHeli[i].use == FALSE) continue;

		// �������C�g�̐ݒ�
		SetFuchi(g_EnemyHeli[i].fuchi);

		// ���[���h�}�g���b�N�X�̏�����
		mtxWorld = XMMatrixIdentity();

		// �X�P�[���𔽉f
		mtxScl = XMMatrixScaling(g_EnemyHeli[i].scl.x, g_EnemyHeli[i].scl.y, g_EnemyHeli[i].scl.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

		// ��]�𔽉f
		mtxRot = XMMatrixRotationRollPitchYaw(g_EnemyHeli[i].rot.x, g_EnemyHeli[i].rot.y + XM_PI, g_EnemyHeli[i].rot.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);

		// �ړ��𔽉f
		mtxTranslate = XMMatrixTranslation(g_EnemyHeli[i].pos.x, g_EnemyHeli[i].pos.y, g_EnemyHeli[i].pos.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

		// ���[���h�}�g���b�N�X�̐ݒ�
		SetWorldMatrix(&mtxWorld);

		XMStoreFloat4x4(&g_EnemyHeli[i].mtxWorld, mtxWorld);


		// ���f���`��
		DrawModel(&g_EnemyHeli[i].model);

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
				// g_EnemyHeli[i].mtxWorld���w���Ă���
			}

			XMStoreFloat4x4(&g_Parts[i].mtxWorld, mtxWorld);

			// �g���Ă���Ȃ珈������B�����܂ŏ������Ă��闝�R�͑��̃p�[�c�����̃p�[�c���Q�Ƃ��Ă���\�������邩��B
			if (g_Parts[i].use == FALSE) continue;

			// ���[���h�}�g���b�N�X�̐ݒ�
			SetWorldMatrix(&mtxWorld);


			// ���f���`��
			DrawModel(&g_Parts[i].model);
		}

		// �������C�g�̐ݒ�
		SetFuchi(FALSE);
	}

	// �J�����O�ݒ��߂�
	SetCullingMode(CULL_MODE_BACK);
}

//=============================================================================
// �w���G�l�~�[�̎擾
//=============================================================================
ENEMY_HELI *GetEnemyHeli()
{
	return &g_EnemyHeli[0];
}


//=============================================================================
// �w���G�l�~�[�̔���
//=============================================================================
void SetEnemyHeli(void)
{
	for (int i = 0; i < MAX_ENEMY_HELI; i++)
	{
		if (g_EnemyHeli[i].use == FALSE)
		{
			g_EnemyHeli[i].use = TRUE;
			g_EnemyHeli[i].pos.z = ENEMY_HELI_POP_Z;
			g_EnemyHeli[i].pos.y = ENEMY_HELI_OFFSET_Y;
			g_EnemyHeli[i].isHit = FALSE;
			g_EnemyHeli[i].move = FALSE;
			g_EnemyHeli[i].rot = XMFLOAT3(0.0f, 0.0f, 0.0f);

			// ���B�n�_�������_��
			g_EnemyHeli[i].zGoal = (float)(rand() % ENEMY_HELI_GOAL_Z_OFFSET) + ENEMY_HELI_GOAL_Z;

			{
				control0.x = -50.0f;
				control0.y = ENEMY_HELI_OFFSET_Y;
				control0.z = g_EnemyHeli[i].zGoal;

				control1.x = 0.0f;
				control1.y = ENEMY_HELI_OFFSET_Y + 75.0f;
				control1.z = g_EnemyHeli[i].zGoal;

				control2.x = 50.0f;
				control2.y = ENEMY_HELI_OFFSET_Y;
				control2.z = g_EnemyHeli[i].zGoal;
			}


			g_EnemyHeli[i].liveCount = 0;

			// �������C�g�I�t
			g_EnemyHeli[i].fuchi = FALSE;

			return;
		}
	}
}



void SetEnemyHeliMove(void)
{

}