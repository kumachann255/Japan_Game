//=============================================================================
//
// �Q�[����ʏ��� [game.cpp]
// Author : 
//
//=============================================================================
#include "main.h"
#include "renderer.h"
#include "model.h"
#include "camera.h"
#include "input.h"
#include "sound.h"
#include "fade.h"
#include "game.h"

#include "player.h"
#include "enemy.h"
#include "attackRange.h"
#include "bom.h"
#include "blast.h"
#include "meshfield.h"
#include "meshwall.h"
#include "shadow.h"
#include "fieldobj.h"
#include "bullet.h"
#include "score.h"
#include "particle.h"
#include "orbit.h"
#include "collision.h"
#include "debugproc.h"
#include "timeUI.h"
#include "damageEF.h"
#include "enemyHeli.h"
#include "combo.h"
#include "playerHP.h"


//*****************************************************************************
// �}�N����`
//*****************************************************************************



//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
void CheckHit(void);



//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
static int	g_ViewPortType_Game = TYPE_FULL_SCREEN;

static BOOL	g_bPause = TRUE;	// �|�[�YON/OFF


//=============================================================================
// ����������
//=============================================================================
HRESULT InitGame(void)
{
	g_ViewPortType_Game = TYPE_FULL_SCREEN;

	// �t�B�[���h�̏�����
	InitMeshField(XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), 1, 7, 700.0f, 400.0f);

	// ���C�g��L����	// �e�̏���������
	InitShadow();

	// �v���C���[�̏�����
	InitPlayer();

	// �G�l�~�[�̏�����
	InitEnemy();

	// �G�l�~�[�w���̏�����
	InitEnemyHeli();

	// �U���͈͂̏�����
	InitAttackR();

	// �_�C�i�}�C�g�̏�����
	InitBom();

	// ���j�I�u�W�F�N�g�̏�����
	InitBlast();

	//// �ǂ̏�����
	//InitMeshWall(XMFLOAT3(0.0f, 0.0f, MAP_TOP), XMFLOAT3(0.0f, 0.0f, 0.0f),
	//	XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), 16, 2, 80.0f, 80.0f);
	//InitMeshWall(XMFLOAT3(MAP_LEFT, 0.0f, 0.0f), XMFLOAT3(0.0f, -XM_PI * 0.50f, 0.0f),
	//	XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), 16, 2, 80.0f, 80.0f);
	//InitMeshWall(XMFLOAT3(MAP_RIGHT, 0.0f, 0.0f), XMFLOAT3(0.0f, XM_PI * 0.50f, 0.0f),
	//	XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), 16, 2, 80.0f, 80.0f);
	//InitMeshWall(XMFLOAT3(0.0f, 0.0f, MAP_DOWN), XMFLOAT3(0.0f,  XM_PI, 0.0f),
	//	XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), 16, 2, 80.0f, 80.0f);

	//// ��(�����p�̔�����)
	//InitMeshWall(XMFLOAT3(0.0f, 0.0f, MAP_TOP), XMFLOAT3(0.0f,    XM_PI, 0.0f),
	//	XMFLOAT4(1.0f, 1.0f, 1.0f, 0.25f), 16, 2, 80.0f, 80.0f);
	//InitMeshWall(XMFLOAT3(MAP_LEFT, 0.0f, 0.0f), XMFLOAT3(0.0f,   XM_PI * 0.50f, 0.0f),
	//	XMFLOAT4(1.0f, 1.0f, 1.0f, 0.25f), 16, 2, 80.0f, 80.0f);
	//InitMeshWall(XMFLOAT3(MAP_RIGHT, 0.0f, 0.0f), XMFLOAT3(0.0f, -XM_PI * 0.50f, 0.0f),
	//	XMFLOAT4(1.0f, 1.0f, 1.0f, 0.25f), 16, 2, 80.0f, 80.0f);
	//InitMeshWall(XMFLOAT3(0.0f, 0.0f, MAP_DOWN), XMFLOAT3(0.0f, 0.0f, 0.0f),
	//	XMFLOAT4(1.0f, 1.0f, 1.0f, 0.25f), 16, 2, 80.0f, 80.0f);

	// �؂𐶂₷
	InitTree();

	// �e�̏�����
	InitBullet();

	// �X�R�A�̏�����
	InitScore();

	// �R���{�̏�����
	InitCombo();

	// �������Ԃ̏�����
	InitTime();

	// �v���C���[��HP�̏�����
	InitPlayerHP();

	// �p�[�e�B�N���̏�����
	InitParticle();

	// �O�Ղ̏�����
	InitOrbit();

	// �_���[�W�G�t�F�N�g�̏�����
	InitDamageEF();

	// BGM�Đ�
	//PlaySound(SOUND_LABEL_BGM_sample001);

	return S_OK;
}

//=============================================================================
// �I������
//=============================================================================
void UninitGame(void)
{
	// �_���[�W�G�t�F�N�g�̏I������
	UninitDamageEF();

	// �O�Ղ̏I������
	UninitOrbit();

	// �p�[�e�B�N���̏I������
	UninitParticle();

	// �v���C���[��HP�̏I������
	UninitPlayerHP();

	// ���Ԃ̏I������
	UninitTime();

	// �R���{�̏I������
	UninitCombo();

	// �X�R�A�̏I������
	UninitScore();

	// �e�̏I������
	UninitBullet();

	// �؂̏I������
	UninitTree();

	// �ǂ̏I������
	UninitMeshWall();

	// �n�ʂ̏I������
	UninitMeshField();

	// ���j�I�u�W�F�N�g�̏I������
	UninitBlast();

	// �_�C�i�}�C�g�̏I������
	UninitBom();

	// �U���͈͂̏I������
	UninitAttackR();

	// �G�l�~�[�w���̏I������
	UninitEnemyHeli();

	// �G�l�~�[�̏I������
	UninitEnemy();

	// �v���C���[�̏I������
	UninitPlayer();

	// �e�̏I������
	UninitShadow();

}

//=============================================================================
// �X�V����
//=============================================================================
void UpdateGame(void)
{
#ifdef _DEBUG
	if (GetKeyboardTrigger(DIK_V))
	{
		g_ViewPortType_Game = (g_ViewPortType_Game + 1) % TYPE_NONE;
		SetViewPort(g_ViewPortType_Game);
	}

	if (GetKeyboardTrigger(DIK_P))
	{
		g_bPause = g_bPause ? FALSE : TRUE;
	}


#endif

	if(g_bPause == FALSE)
		return;

	// �n�ʏ����̍X�V
	UpdateMeshField();

	// �v���C���[�̍X�V����
	UpdatePlayer();

	// �G�l�~�[�̍X�V����
	UpdateEnemy();

	// �G�l�~�[�w���̍X�V����
	UpdateEnemyHeli();

	// �U���͈͂̍X�V����
	UpdateAttackR();

	// �_�C�i�}�C�g�̍X�V����
	UpdateBom();

	// ���j�I�u�W�F�N�g�̍X�V����
	UpdateBlast();

	// �Ǐ����̍X�V
	UpdateMeshWall();

	// �؂̍X�V����
	UpdateTree();

	// �e�̍X�V����
	UpdateBullet();

	// �p�[�e�B�N���̍X�V����
	UpdateParticle();

	// �O�Ղ̍X�V����
	UpdateOrbit();

	// �e�̍X�V����
	UpdateShadow();

	// �����蔻�菈��
	CheckHit();

	// �X�R�A�̍X�V����
	UpdateScore();

	// �R���{�̍X�V����
	UpdateCombo();

	// ���Ԃ̍X�V����
	UpdateTime();

	// �v���C���[��HP�̍X�V����
	UpdatePlayerHP();

	// �_���[�W�G�t�F�N�g�̍X�V����
	UpdateDamageEF();
}

//=============================================================================
// �`�揈��
//=============================================================================
void DrawGame0(void)
{
	// 3D�̕���`�悷�鏈��
	// �n�ʂ̕`�揈��
	DrawMeshField();

	// �e�̕`�揈��
	DrawShadow();

	// �G�l�~�[�w���̕`�揈��
	DrawEnemyHeli();

	// �G�l�~�[�̕`�揈��
	DrawEnemy();

	// �v���C���[�̕`�揈��
	DrawPlayer();

	// �_�C�i�}�C�g�̕`�揈��
	DrawBom();

	// ���j�I�u�W�F�N�g�̕`�揈��
	DrawBlast();

	// �e�̕`�揈��
	DrawBullet();

	// �ǂ̕`�揈��
	DrawMeshWall();

	// �؂̕`�揈��
	//DrawTree();

	// �p�[�e�B�N���̕`�揈��
	DrawParticle();

	// �O�Ղ̕`�揈��
	DrawOrbit();

	// �U���͈͂̕`�揈��
	DrawAttackR();



	// 2D�̕���`�悷�鏈��
	// Z��r�Ȃ�
	SetDepthEnable(FALSE);

	// ���C�e�B���O�𖳌�
	SetLightEnable(FALSE);

	// �X�R�A�̕`�揈��
	DrawScore();

	// �R���{�̕`�揈��
	DrawCombo();

	// ���Ԃ̕`�揈��
	DrawTime();

	// �v���C���[��HP�̕`�揈��
	DrawPlayerHP();

	// �_���[�W�G�t�F�N�g�̕`�揈��
	DrawDamageEF();


	// ���C�e�B���O��L����
	SetLightEnable(TRUE);

	// Z��r����
	SetDepthEnable(TRUE);
}


void DrawGame(void)
{
	XMFLOAT3 pos;


#ifdef _DEBUG
	// �f�o�b�O�\��
	PrintDebugProc("ViewPortType:%d\n", g_ViewPortType_Game);

#endif

	// �v���C���[���_
	pos = GetPlayer()->pos;
	pos.y = 0.0f;			// �J����������h�����߂ɃN���A���Ă���
	SetCameraAT(pos);
	SetCamera();

	switch(g_ViewPortType_Game)
	{
	case TYPE_FULL_SCREEN:
		SetViewPort(TYPE_FULL_SCREEN);
		DrawGame0();
		break;

	case TYPE_LEFT_HALF_SCREEN:
	case TYPE_RIGHT_HALF_SCREEN:
		SetViewPort(TYPE_LEFT_HALF_SCREEN);
		DrawGame0();

		// �G�l�~�[���_
		pos = GetEnemy()->pos;
		pos.y = 0.0f;
		SetCameraAT(pos);
		SetCamera();
		SetViewPort(TYPE_RIGHT_HALF_SCREEN);
		DrawGame0();
		break;

	case TYPE_UP_HALF_SCREEN:
	case TYPE_DOWN_HALF_SCREEN:
		SetViewPort(TYPE_UP_HALF_SCREEN);
		DrawGame0();

		// �G�l�~�[���_
		pos = GetEnemy()->pos;
		pos.y = 0.0f;
		SetCameraAT(pos);
		SetCamera();
		SetViewPort(TYPE_DOWN_HALF_SCREEN);
		DrawGame0();
		break;

	}

}


//=============================================================================
// �����蔻�菈��
//=============================================================================
void CheckHit(void)
{
	ENEMY *enemy = GetEnemy();		// �G�l�~�[�̃|�C���^�[��������
	PLAYER *player = GetPlayer();	// �v���C���[�̃|�C���^�[��������
	BULLET *bullet = GetBullet();	// �e�̃|�C���^�[��������
	BLAST *blast = GetBlast();		// ���j�I�u�W�F�N�g�̏�����

	float offsetX;
	float offsetY;
	float offsetZ;

	// �G�Ɣ��j�I�u�W�F�N�g
	for (int i = 0; i < MAX_ENEMY; i++)
	{
		//�G�̗L���t���O���`�F�b�N����
		if (enemy[i].use == FALSE || enemy[i].isHit == TRUE)
			continue;


		for (int p = 0; p < MAX_BLAST; p++)
		{
			//���j�I�u�W�F�N�g�̗L���t���O���`�F�b�N����
			if (blast[p].use == FALSE)
				continue;

			//BC�̓����蔻��
			float size = blast[p].size;
			if (GetMorphing() == 1) size /= 4.0f;

			if (CollisionBC(blast[p].pos, enemy[i].pos, size, enemy[i].size))
			{
				if (enemy[i].isHit == TRUE) break;

				// �G�L�����N�^�[�͓|�����
				enemy[i].isHit = TRUE;
				enemy[i].hitTime = 15;

				offsetX = RamdomFloat(0, 30.0f, -30.0f);
				offsetY = RamdomFloat(0, 30.0f, ENEMY_OFFSET_Y);
				offsetZ = RamdomFloat(0, -10.0f, -40.0f);

				enemy[i].hitPos.x = blast[p].pos.x + offsetX;
				enemy[i].hitPos.y = blast[p].pos.y + offsetY;
				enemy[i].hitPos.z = blast[p].pos.z + offsetZ;

				ReleaseShadow(enemy[i].shadowIdx);

				// �X�R�A�𑫂�
				AddScore(100);

				// �R���{�𑫂�
				AddCombo(1);
				ResetComboTime();
			}
		}
	}


	//// �v���C���[�̒e�ƓG
	//for (int i = 0; i < MAX_BULLET; i++)
	//{
	//	//�e�̗L���t���O���`�F�b�N����
	//	if (bullet[i].use == FALSE)
	//		continue;

	//	// �G�Ɠ������Ă邩���ׂ�
	//	for (int j = 0; j < MAX_ENEMY; j++)
	//	{
	//		//�G�̗L���t���O���`�F�b�N����
	//		if (enemy[j].use == FALSE)
	//			continue;

	//		//BC�̓����蔻��
	//		if (CollisionBC(bullet[i].pos, enemy[j].pos, bullet[i].fWidth, enemy[j].size))
	//		{
	//			// �����������疢�g�p�ɖ߂�
	//			bullet[i].use = FALSE;
	//			ReleaseShadow(bullet[i].shadowIdx);

	//			// �G�L�����N�^�[�͓|�����
	//			enemy[j].use = FALSE;
	//			ReleaseShadow(enemy[j].shadowIdx);

	//			// �X�R�A�𑫂�
	//			AddScore(10);
	//		}
	//	}

	//}


	// �v���C���[��HP��0�ŃQ�[���I�[�o�[
	// ���U���g��ʂ֑J��
	// �ŏI�I�ɂ̓Q�[���I�[�o�[��ʂ��烊�U���g��ʂ֑J��
	if (player->hp <= 0)
	{
		SetFade(FADE_OUT, MODE_RESULT);




	}

	// �G�l�~�[���S�����S�������ԑJ��
	//int enemy_count = 0;
	//for (int i = 0; i < MAX_ENEMY; i++)
	//{
	//	if (enemy[i].use == FALSE) continue;
	//	enemy_count++;
	//}

	//// �G�l�~�[���O�C�H
	//if (enemy_count == 0)
	//{
	//	SetFade(FADE_OUT, MODE_RESULT);
	//}

}


