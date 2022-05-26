//=============================================================================
//
// ���f������ [player.cpp]
// Author :GP12B183 �R�c����
//
//=============================================================================
#include "main.h"
#include "input.h"
#include "camera.h"
#include "debugproc.h"
#include "model.h"
#include "player.h"
#include "shadow.h"
#include "light.h"
#include "bullet.h"
#include "meshfield.h"
#include "bom.h"
#include "speech.h"


//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define	MODEL_CAR					"data/MODEL/car2.obj"			// �ǂݍ��ރ��f����

#define	MODEL_PLAYER_PARTS_H		"data/MODEL/head.obj"			// �ǂݍ��ރ��f����
#define	MODEL_PLAYER_PARTS_B		"data/MODEL/body.obj"			// �ǂݍ��ރ��f����
#define	MODEL_PLAYER_PARTS_R_HAND	"data/MODEL/R_hand.obj"			// �ǂݍ��ރ��f����
#define	MODEL_PLAYER_PARTS_R_REG	"data/MODEL/R_reg.obj"			// �ǂݍ��ރ��f����
#define	MODEL_PLAYER_PARTS_L_HAND	"data/MODEL/L_hand.obj"			// �ǂݍ��ރ��f����
#define	MODEL_PLAYER_PARTS_L_REG	"data/MODEL/L_reg.obj"			// �ǂݍ��ރ��f����
#define	MODEL_PLAYER_PARTS_STICK	"data/MODEL/stick.obj"			// �ǂݍ��ރ��f����
#define	MODEL_PLAYER_PARTS_NABE		"data/MODEL/nabe.obj"			// �ǂݍ��ރ��f����


#define	VALUE_MOVE			(2.0f)							// �ړ���
#define	VALUE_ROTATE		(XM_PI * 0.02f)					// ��]��

#define PLAYER_SHADOW_SIZE	(1.0f)							// �e�̑傫��
#define PLAYER_OFFSET_Y		(13.0f)							// �v���C���[�̑��������킹��

#define PLAYER_PARTS_MAX	(8)								// �v���C���[�̃p�[�c�̐�


//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************


//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
static PLAYER		g_Player;						// �v���C���[

static PLAYER		g_Parts[PLAYER_PARTS_MAX];		// �v���C���[�̃p�[�c�p

static BOOL			g_Load = FALSE;

static char *g_ModelName[PLAYER_PARTS_MAX] = {
#define	MODEL_PLAYER_PARTS_H		"data/MODEL/head.obj"			// �ǂݍ��ރ��f����
#define	MODEL_PLAYER_PARTS_B		"data/MODEL/body.obj"			// �ǂݍ��ރ��f����
#define	MODEL_PLAYER_PARTS_R_HAND	"data/MODEL/R_hand.obj"			// �ǂݍ��ރ��f����
#define	MODEL_PLAYER_PARTS_R_REG	"data/MODEL/R_reg.obj"			// �ǂݍ��ރ��f����
#define	MODEL_PLAYER_PARTS_L_HAND	"data/MODEL/L_hand.obj"			// �ǂݍ��ރ��f����
#define	MODEL_PLAYER_PARTS_L_REG	"data/MODEL/L_reg.obj"			// �ǂݍ��ރ��f����
#define	MODEL_PLAYER_PARTS_STICK	"data/MODEL/stick.obj"			// �ǂݍ��ރ��f����
#define	MODEL_PLAYER_PARTS_NABE		"data/MODEL/nabe.obj"			// �ǂݍ��ރ��f����

};

//*****************************************************************************
// �Ԃ̃��[�V�����e�[�u��
//*****************************************************************************
static INTERPOLATION_DATA car[] = {
	// pos,								rot,						 scl,						 frame
	{ XMFLOAT3(0.0f, PLAYER_OFFSET_Y, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(1.0f, 1.0f, 1.0f), 60 * 0.6f },
	{ XMFLOAT3(0.0f, PLAYER_OFFSET_Y + 0.8f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(1.0f, 1.0f, 1.0f), 60 * 0.6f },
	{ XMFLOAT3(0.0f, PLAYER_OFFSET_Y, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(1.0f, 1.0f, 1.0f), 60 * 0.6f },


};



//*****************************************************************************
// �ҋ@���̃��[�V�����e�[�u��
//*****************************************************************************

static INTERPOLATION_DATA s_head[] = {
	// pos,								rot,						 scl,						 frame
	{ XMFLOAT3(0.0f, 20.0f, 0.0f), XMFLOAT3(0.0f, XM_PI, 0.0f), XMFLOAT3(0.9f, 0.9f, 0.9f), 60 * 1.0f },
};

static INTERPOLATION_DATA s_body[] = {
	// pos,								rot,						 scl,						 frame
	{ XMFLOAT3(4.0f, 8.0f, -30.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.5f, 0.5f, 0.5f), 60 * 1.0f },
};

static INTERPOLATION_DATA s_R_hand[] = {
	// pos,								rot,						 scl,						 frame
	{ XMFLOAT3(-16.0f, 14.0f,  0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.7f, 0.7f, 0.7f), 60 * 0.3f },
	{ XMFLOAT3(-17.5f, 14.0f, -3.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.7f, 0.7f, 0.7f), 60 * 0.2f },
	{ XMFLOAT3(-19.0f, 14.0f, -5.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.7f, 0.7f, 0.7f), 60 * 0.3f },
	{ XMFLOAT3(-20.5f, 14.0f, -3.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.7f, 0.7f, 0.7f), 60 * 0.2f },
	{ XMFLOAT3(-22.0f, 14.0f,  0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.7f, 0.7f, 0.7f), 60 * 0.3f },
	{ XMFLOAT3(-20.5f, 14.0f,  3.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.7f, 0.7f, 0.7f), 60 * 0.2f },
	{ XMFLOAT3(-19.0f, 14.0f,  5.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.7f, 0.7f, 0.7f), 60 * 0.3f },
	{ XMFLOAT3(-17.5f, 14.0f,  3.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.7f, 0.7f, 0.7f), 60 * 0.2f },
	{ XMFLOAT3(-16.0f, 14.0f,  0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.7f, 0.7f, 0.7f), 60 * 0.3f },

};

static INTERPOLATION_DATA s_R_reg[] = {
	// pos,								rot,						 scl,						 frame
	{ XMFLOAT3(7.0f, -13.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(1.0f, 1.0f, 1.0f), 60 * 1.0f },
};

static INTERPOLATION_DATA s_L_hand[] = {
	// pos,								rot,						 scl,						 frame
	{ XMFLOAT3(-16.0f, 8.0f,  0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.7f, 0.7f, 0.7f), 60 * 0.3f },
	{ XMFLOAT3(-17.5f, 8.0f, -3.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.7f, 0.7f, 0.7f), 60 * 0.2f },
	{ XMFLOAT3(-19.0f, 8.0f, -5.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.7f, 0.7f, 0.7f), 60 * 0.3f },
	{ XMFLOAT3(-20.5f, 8.0f, -3.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.7f, 0.7f, 0.7f), 60 * 0.2f },
	{ XMFLOAT3(-22.0f, 8.0f,  0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.7f, 0.7f, 0.7f), 60 * 0.3f },
	{ XMFLOAT3(-20.5f, 8.0f,  3.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.7f, 0.7f, 0.7f), 60 * 0.2f },
	{ XMFLOAT3(-19.0f, 8.0f,  5.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.7f, 0.7f, 0.7f), 60 * 0.3f },
	{ XMFLOAT3(-17.5f, 8.0f,  3.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.7f, 0.7f, 0.7f), 60 * 0.2f },
	{ XMFLOAT3(-16.0f, 8.0f,  0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.7f, 0.7f, 0.7f), 60 * 0.3f },
};

static INTERPOLATION_DATA s_L_reg[] = {
	// pos,								rot,						 scl,						 frame
	{ XMFLOAT3(-7.0f, -13.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(1.0f, 1.0f, 1.0f), 60 * 1.0f },
};

static INTERPOLATION_DATA s_stick[] = {
	// pos,								rot,						 scl,						 frame
	{ XMFLOAT3(0.0f, -8.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, XM_PI), XMFLOAT3(0.7f, 0.7f, 0.7f), 60 * 1.0f },
};

static INTERPOLATION_DATA s_nabe[] = {
	// pos,								rot,						 scl,						 frame
	{ XMFLOAT3(-19.0f, -7.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(2.0f, 2.0f, 2.0f), 60 * 1.0f },
};

//*****************************************************************************
// �������̃��[�V�����e�[�u��
//*****************************************************************************
static INTERPOLATION_DATA m_head[] = {
	// pos,								rot,						 scl,						 frame
	{ XMFLOAT3(0.0f, 20.0f, 0.0f), XMFLOAT3(0.0f, XM_PI, 0.0f), XMFLOAT3(0.9f, 0.9f, 0.9f), 60 * 1.0f },
};

static INTERPOLATION_DATA m_body[] = {
	// pos,								rot,						 scl,						 frame
	{ XMFLOAT3(4.0f, 8.0f, -30.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.5f, 0.5f, 0.5f), 60 * 1.0f },
};

static INTERPOLATION_DATA m_R_hand[] = {
	// pos,								rot,						 scl,						 frame
	{ XMFLOAT3(7.0f, 0.0f, -15.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.7f, 0.7f, 0.7f), 60 * 0.2f },
	{ XMFLOAT3(7.0f, 0.0f,	 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.7f, 0.7f, 0.7f), 60 * 0.2f },
	{ XMFLOAT3(7.0f, 0.0f,  15.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.7f, 0.7f, 0.7f), 60 * 0.2f },


};

static INTERPOLATION_DATA m_R_reg[] = {
	// pos,								rot,						 scl,						 frame
	{ XMFLOAT3(7.0f, -13.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(1.0f, 1.0f, 1.0f), 60 * 1.0f },
};

static INTERPOLATION_DATA m_L_hand[] = {
	// pos,								rot,						 scl,						 frame
	{ XMFLOAT3(-12.0f, 18.0f, 15.0f), XMFLOAT3(0.0f, 0.0f, 0.0f),			 XMFLOAT3(0.7f, 0.7f, 0.7f), 60 * 0.1f },
	{ XMFLOAT3(-12.0f, 17.5f,  7.5f), XMFLOAT3(-XM_PI * 0.125f, 0.0f, 0.0f), XMFLOAT3(0.7f, 0.7f, 0.7f), 60 * 0.1f },
	{ XMFLOAT3(-12.0f, 16.0f,  0.0f), XMFLOAT3(-XM_PI * 0.25f, 0.0f, 0.0f),  XMFLOAT3(0.7f, 0.7f, 0.7f), 60 * 0.1f },
	{ XMFLOAT3(-12.0f, 10.0f, -7.5f), XMFLOAT3(-XM_PI * 0.375f, 0.0f, 0.0f), XMFLOAT3(0.7f, 0.7f, 0.7f), 60 * 0.1f },
	{ XMFLOAT3(-12.0f, 0.0f, -15.0f), XMFLOAT3(-XM_PI * 0.5f, 0.0f, 0.0f),	 XMFLOAT3(0.7f, 0.7f, 0.7f), 60 * 0.1f },
};

static INTERPOLATION_DATA m_L_reg[] = {
	// pos,								rot,						 scl,						 frame
	{ XMFLOAT3(-7.0f, -13.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(1.0f, 1.0f, 1.0f), 60 * 1.0f },
};

static INTERPOLATION_DATA m_stick[] = {
	// pos,								rot,						 scl,						 frame
	{ XMFLOAT3(0.0f, 15.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.7f, 0.7f, 0.7f), 60 * 1.0f },
};

static INTERPOLATION_DATA m_nabe[] = {
	// pos,								rot,						 scl,						 frame
	{ XMFLOAT3(-19.0f, -7.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(2.0f, 2.0f, 2.0f), 60 * 1.0f },
};




//=============================================================================
// ����������
//=============================================================================
HRESULT InitPlayer(void)
{
	LoadModel(MODEL_CAR, &g_Player.model);
	g_Player.load = TRUE;

	g_Player.pos = { 0.0f, PLAYER_OFFSET_Y, 200.0f };
	g_Player.rot = { 0.0f, 0.0f, 0.0f };
	g_Player.scl = { 1.0f, 1.0f, 1.0f };

	g_Player.spd = 0.0f;			// �ړ��X�s�[�h�N���A
	g_Player.size = PLAYER_SIZE;	// �����蔻��̑傫��

	g_Player.hp = PLAYER_MAX_HP;

	g_Player.action = FALSE;

	g_Player.use = TRUE;

	// �����Ńv���C���[�p�̉e���쐬���Ă���
	XMFLOAT3 pos = g_Player.pos;
	pos.y -= (PLAYER_OFFSET_Y - 0.1f);
	g_Player.shadowIdx = CreateShadow(pos, PLAYER_SHADOW_SIZE, PLAYER_SHADOW_SIZE);
	//          ��
	//        ���̃����o�[�ϐ������������e��Index�ԍ�

	// �K�w�A�j���[�V�����p�̏���������
	g_Player.parent = NULL;			// �{�́i�e�j�Ȃ̂�NULL������




	// �p�[�c�̏�����
	for (int i = 0; i < PLAYER_PARTS_MAX; i++)
	{
		g_Parts[i].use = TRUE;

		// �ʒu�E��]�E�X�P�[���̏����ݒ�
		g_Parts[i].pos = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_Parts[i].rot = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_Parts[i].scl = XMFLOAT3(1.0f, 1.0f, 1.0f);

		// �e�q�֌W
		g_Parts[i].parent = &g_Player;		// �� �����ɐe�̃A�h���X������
//	//	g_Parts[�r].parent= &g_Player;		// �r��������e�͖{�́i�v���C���[�j
//	//	g_Parts[��].parent= &g_Paerts[�r];	// �w���r�̎q���������ꍇ�̗�

		// �K�w�A�j���[�V�����p�̃����o�[�ϐ��̏�����
		g_Parts[i].tbl_adr = NULL;		// �Đ�����A�j���f�[�^�̐擪�A�h���X���Z�b�g
		g_Parts[i].move_time = 0.0f;	// ���s���Ԃ��N���A
		g_Parts[i].tbl_size = 0;		// �Đ�����A�j���f�[�^�̃��R�[�h�����Z�b�g

		// �p�[�c�̓ǂݍ��݂͂܂����Ă��Ȃ�
		g_Parts[i].load = 0;
	}
//
	g_Parts[0].use = TRUE;

	//========================================================================
	// �e�q�֌W(��ɖ��[�p�[�c�Ƃ̃y�A�����O)
	//
	//�쐬��
	//	g_Parts[�r].parent= &g_Player;		// �r��������e�͖{�́i�v���C���[�j
	//	g_Parts[��].parent= &g_Paerts[�r];	// �w���r�̎q���������ꍇ�̗�
	//========================================================================
	g_Parts[0].parent = &g_Parts[1];						//	�����{�f�B��
	g_Parts[2].parent = &g_Parts[1];						//	�E����{�f�B��
	g_Parts[3].parent = &g_Parts[1];						//	�E�����{�f�B��
	g_Parts[4].parent = &g_Parts[1];						//	������{�f�B��
	g_Parts[5].parent = &g_Parts[1];						//	�������{�f�B��
	g_Parts[6].parent = &g_Parts[4];						//	�X�e�B�b�N���E���
	g_Parts[7].parent = &g_Parts[1];						//	�X�e�B�b�N���E���


	SetStayMotion();

	g_Parts[0].load = TRUE;


	

	//=============================================================================
// �����Ŋe�p�[�c�̓ǂݍ���
//=============================================================================
	LoadModel(MODEL_PLAYER_PARTS_H,		 &g_Parts[0].model);	 // �O�Ԗڂ̃��f����ǂݍ���
	LoadModel(MODEL_PLAYER_PARTS_B,		 &g_Parts[1].model);	 // �P�Ԗڂ̃��f����ǂݍ���
	LoadModel(MODEL_PLAYER_PARTS_R_HAND, &g_Parts[2].model);	 // �Q�Ԗڂ̃��f����ǂݍ���
	LoadModel(MODEL_PLAYER_PARTS_R_REG,  &g_Parts[3].model);	 // �R�Ԗڂ̃��f����ǂݍ���
	LoadModel(MODEL_PLAYER_PARTS_L_HAND, &g_Parts[4].model);	 // �S�Ԗڂ̃��f����ǂݍ���
	LoadModel(MODEL_PLAYER_PARTS_L_REG,	 &g_Parts[5].model);	 // �T�Ԗڂ̃��f����ǂݍ���
	LoadModel(MODEL_PLAYER_PARTS_STICK,  &g_Parts[6].model);	 // �U�Ԗڂ̃��f����ǂݍ���
	LoadModel(MODEL_PLAYER_PARTS_NABE,   &g_Parts[7].model);	 // �V�Ԗڂ̃��f����ǂݍ���


	g_Load = TRUE;
	return S_OK;
}

//=============================================================================
// �I������
//=============================================================================
void UninitPlayer(void)
{
	if (g_Load == FALSE) return;

	// ���f���̉������
	if (g_Player.load)
	{
		UnloadModel(&g_Player.model);
		g_Player.load = FALSE;
	}

	g_Load = FALSE;
}

//=============================================================================
// �X�V����
//=============================================================================
void UpdatePlayer(void)
{
	CAMERA *cam = GetCamera();

	//// �ړ��������Ⴄ
	//if (GetKeyboardPress(DIK_LEFT))
	//{	// ���ֈړ�
	//	g_Player.spd = VALUE_MOVE;
	//	g_Player.dir = XM_PI / 2;
	//}
	//if (GetKeyboardPress(DIK_RIGHT))
	//{	// �E�ֈړ�
	//	g_Player.spd = VALUE_MOVE;
	//	g_Player.dir = -XM_PI / 2;
	//}
	//if (GetKeyboardPress(DIK_UP))
	//{	// ��ֈړ�
	//	g_Player.spd = VALUE_MOVE;
	//	g_Player.dir = XM_PI;
	//}
	//if (GetKeyboardPress(DIK_DOWN))
	//{	// ���ֈړ�
	//	g_Player.spd = VALUE_MOVE;
	//	g_Player.dir = 0.0f;
	//}

	g_Player.pos.z -= 2.0f;

	if (g_Player.pos.z <= 0.0f)
	{
		g_Player.pos.z = 0.0f;
	}
	if (g_Player.pos.z == 0.0f)
	{
		SetCarMotion();
	}

#ifdef _DEBUG
	if (GetKeyboardPress(DIK_R))
	{
		g_Player.pos.z = g_Player.pos.x = 0.0f;
		g_Player.rot.y = g_Player.dir = 0.0f;
		g_Player.spd = 0.0f;
	}
#endif


	//	// Key���͂���������ړ���������
	if (g_Player.spd > 0.0f)
	{
		g_Player.rot.y = g_Player.dir + cam->rot.y;

		// ���͂̂����������փv���C���[���������Ĉړ�������
		g_Player.pos.x -= sinf(g_Player.rot.y) * g_Player.spd;
		g_Player.pos.z -= cosf(g_Player.rot.y) * g_Player.spd;
	}



	// ���C�L���X�g���đ����̍��������߂�
	XMFLOAT3 normal = { 0.0f, 1.0f, 0.0f };				// �Ԃ������|���S���̖@���x�N�g���i�����j
	XMFLOAT3 hitPosition;								// ��_
	hitPosition.y = g_Player.pos.y - PLAYER_OFFSET_Y;	// �O�ꂽ���p�ɏ��������Ă���
	bool ans = RayHitField(g_Player.pos, &hitPosition, &normal);
	g_Player.pos.y = hitPosition.y + PLAYER_OFFSET_Y;
	g_Player.pos.y = PLAYER_OFFSET_Y;


	// �e���v���C���[�̈ʒu�ɍ��킹��
	XMFLOAT3 pos = g_Player.pos;
	pos.y -= (PLAYER_OFFSET_Y - 0.1f);
	SetPositionShadow(g_Player.shadowIdx, pos);

	// �e���ˏ���
	if (GetKeyboardTrigger(DIK_SPACE))
	{
		g_Player.action = TRUE;
		SetBomthrowMotion();

		SetBom();
		
	}


	g_Player.spd *= 0.5f;


	// �K�w�A�j���[�V����
			// �g���Ă���Ȃ珈������
	if ((g_Player.use == TRUE) && (g_Player.tbl_adr != NULL))
	{
		// �ړ�����
		int		index = (int)g_Player.move_time;
		float	time = g_Player.move_time - index;
		int		size = g_Player.tbl_size;

		float dt = 1.0f / g_Player.tbl_adr[index].frame;	// 1�t���[���Ői�߂鎞��
		g_Player.move_time += dt;					// �A�j���[�V�����̍��v���Ԃɑ���

		//if ((g_Player.action == TRUE) && (g_Player.move_time >= size - 1) && (i == 4))
		//{
		//	SetStayMotion();
		//	g_Player.action = FALSE;

		//}
		if (index > (size - 2))	// �S�[�����I�[�o�[���Ă�����A�ŏ��֖߂�
		{
			g_Player.move_time = 0.0f;
			index = 0;
		}

		// ���W�����߂�	X = StartX + (EndX - StartX) * ���̎���
		XMVECTOR p1 = XMLoadFloat3(&g_Player.tbl_adr[index + 1].pos);	// ���̏ꏊ
		XMVECTOR p0 = XMLoadFloat3(&g_Player.tbl_adr[index + 0].pos);	// ���݂̏ꏊ
		XMVECTOR vec = p1 - p0;
		XMStoreFloat3(&g_Player.pos, p0 + vec * time);

		// ��]�����߂�	R = StartX + (EndX - StartX) * ���̎���
		XMVECTOR r1 = XMLoadFloat3(&g_Player.tbl_adr[index + 1].rot);	// ���̊p�x
		XMVECTOR r0 = XMLoadFloat3(&g_Player.tbl_adr[index + 0].rot);	// ���݂̊p�x
		XMVECTOR rot = r1 - r0;
		XMStoreFloat3(&g_Player.rot, r0 + rot * time);

		// scale�����߂� S = StartX + (EndX - StartX) * ���̎���
		XMVECTOR s1 = XMLoadFloat3(&g_Player.tbl_adr[index + 1].scl);	// ����Scale
		XMVECTOR s0 = XMLoadFloat3(&g_Player.tbl_adr[index + 0].scl);	// ���݂�Scale
		XMVECTOR scl = s1 - s0;
		XMStoreFloat3(&g_Player.scl, s0 + scl * time);

	}


	for (int i = 0; i < PLAYER_PARTS_MAX; i++)
	{
		// �g���Ă���Ȃ珈������
		if ((g_Parts[i].use == TRUE)&&(g_Parts[i].tbl_adr != NULL))
		{
			// �ړ�����
			int		index = (int)g_Parts[i].move_time;
			float	time = g_Parts[i].move_time - index;
			int		size = g_Parts[i].tbl_size;

			float dt = 1.0f / g_Parts[i].tbl_adr[index].frame;	// 1�t���[���Ői�߂鎞��
			g_Parts[i].move_time += dt;					// �A�j���[�V�����̍��v���Ԃɑ���

			if ((g_Player.action == TRUE) && (g_Parts[i].move_time >= size - 1) && (i == 4))
			{
				SetStayMotion();
				g_Player.action = FALSE;

			}
			if (index > (size - 2))	// �S�[�����I�[�o�[���Ă�����A�ŏ��֖߂�
			{
				g_Parts[i].move_time = 0.0f;
				index = 0;
			}

			// ���W�����߂�	X = StartX + (EndX - StartX) * ���̎���
			XMVECTOR p1 = XMLoadFloat3(&g_Parts[i].tbl_adr[index + 1].pos);	// ���̏ꏊ
			XMVECTOR p0 = XMLoadFloat3(&g_Parts[i].tbl_adr[index + 0].pos);	// ���݂̏ꏊ
			XMVECTOR vec = p1 - p0;
			XMStoreFloat3(&g_Parts[i].pos, p0 + vec * time);

			// ��]�����߂�	R = StartX + (EndX - StartX) * ���̎���
			XMVECTOR r1 = XMLoadFloat3(&g_Parts[i].tbl_adr[index + 1].rot);	// ���̊p�x
			XMVECTOR r0 = XMLoadFloat3(&g_Parts[i].tbl_adr[index + 0].rot);	// ���݂̊p�x
			XMVECTOR rot = r1 - r0;
			XMStoreFloat3(&g_Parts[i].rot, r0 + rot * time);

			// scale�����߂� S = StartX + (EndX - StartX) * ���̎���
			XMVECTOR s1 = XMLoadFloat3(&g_Parts[i].tbl_adr[index + 1].scl);	// ����Scale
			XMVECTOR s0 = XMLoadFloat3(&g_Parts[i].tbl_adr[index + 0].scl);	// ���݂�Scale
			XMVECTOR scl = s1 - s0;
			XMStoreFloat3(&g_Parts[i].scl, s0 + scl * time);

		}
	}





	{	// �|�C���g���C�g�̃e�X�g
		LIGHT *light = GetLightData(1);
		XMFLOAT3 pos = g_Player.pos;
		pos.y += 20.0f;

		light->Position = pos;
		light->Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
		light->Ambient = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
		light->Type = LIGHT_TYPE_POINT;
		light->Enable = TRUE;
		SetLightData(1, light);
	}


	//////////////////////////////////////////////////////////////////////
	// �p������
	//////////////////////////////////////////////////////////////////////

	XMVECTOR vx, nvx, up;
	XMVECTOR quat;
	float len, angle;

	// �Q�̃x�N�g���̊O�ς�����ĔC�ӂ̉�]�������߂�
	g_Player.upVector = normal;
	up = { 0.0f, 1.0f, 0.0f, 0.0f };
	vx = XMVector3Cross(up, XMLoadFloat3(&g_Player.upVector));

	// ���߂���]������N�H�[�^�j�I�������o��
	nvx = XMVector3Length(vx);
	XMStoreFloat(&len, nvx);
	nvx = XMVector3Normalize(vx);
	angle = asinf(len);
	quat = XMQuaternionRotationNormal(nvx, angle);

	// �O��̃N�H�[�^�j�I�����獡��̃N�H�[�^�j�I���܂ł̉�]�����炩�ɂ���
	quat = XMQuaternionSlerp(XMLoadFloat4(&g_Player.quaternion), quat, 0.05f);

	// ����̃N�H�[�^�j�I���̌��ʂ�ۑ�����
	XMStoreFloat4(&g_Player.quaternion, quat);



#ifdef _DEBUG	// �f�o�b�O����\������
	PrintDebugProc("Player:�� �� �� ���@Space\n");
	PrintDebugProc("Player:X:%f Y:%f Z:%f\n", g_Player.pos.x, g_Player.pos.y, g_Player.pos.z);
#endif
}

//=============================================================================
// �`�揈��
//=============================================================================
void DrawPlayer(void)
{
	// �J�����O����
	SetCullingMode(CULL_MODE_NONE);

	XMMATRIX mtxScl, mtxRot, mtxTranslate, mtxWorld;

	// ���[���h�}�g���b�N�X�̏�����
	mtxWorld = XMMatrixIdentity();

	// �X�P�[���𔽉f
	mtxScl = XMMatrixScaling(g_Player.scl.x, g_Player.scl.y, g_Player.scl.z);
	mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

	// ��]�𔽉f
	mtxRot = XMMatrixRotationRollPitchYaw(g_Player.rot.x, g_Player.rot.y + XM_PI, g_Player.rot.z);
	mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);

	//// �N�H�[�^�j�I���𔽉f
	//XMMATRIX quatMatrix = XMMatrixRotationQuaternion(XMLoadFloat4(&g_Player.quaternion));
	//mtxWorld = XMMatrixMultiply(mtxWorld, quatMatrix);
	
	// �ړ��𔽉f
	mtxTranslate = XMMatrixTranslation(g_Player.pos.x, g_Player.pos.y, g_Player.pos.z);
	mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

	// ���[���h�}�g���b�N�X�̐ݒ�
	SetWorldMatrix(&mtxWorld);

	XMStoreFloat4x4(&g_Player.mtxWorld, mtxWorld);


	// ���f���`��
	DrawModel(&g_Player.model);



	// �p�[�c�̊K�w�A�j���[�V����
	for (int i = 0; i < PLAYER_PARTS_MAX; i++)
	{
		//if (i != 2) return;
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
			// g_Player.mtxWorld���w���Ă���
		}

		XMStoreFloat4x4(&g_Parts[i].mtxWorld, mtxWorld);

		// �g���Ă���Ȃ珈������B�����܂ŏ������Ă��闝�R�͑��̃p�[�c�����̃p�[�c���Q�Ƃ��Ă���\�������邩��B
		if (g_Parts[i].use == FALSE) continue;

		// ���[���h�}�g���b�N�X�̐ݒ�
		SetWorldMatrix(&mtxWorld);


		// ���f���`��
		DrawModel(&g_Parts[i].model);

	}



	// �J�����O�ݒ��߂�
	SetCullingMode(CULL_MODE_BACK);
}


//=============================================================================
// �v���C���[�����擾
//=============================================================================
PLAYER *GetPlayer(void)
{
	return &g_Player;
}

void SetPlayerDamage(int damage)
{
	g_Player.hp -= damage;

	SetDamageSpeech();

}


void SetCarMotion(void)
{
	g_Player.tbl_adr = car;	// �Đ�����A�j���f�[�^�̐擪�A�h���X���Z�b�g
	g_Player.tbl_size = sizeof(car) / sizeof(INTERPOLATION_DATA);		// �Đ�����A�j���f�[�^�̃��R�[�h�����Z�b�g


}




void SetStayMotion(void)
{
//*****************************************************************************
// �ҋ@���̃��[�V�����e�[�u��
//*****************************************************************************
	//g_Player.tbl_adr = s_car;	// �Đ�����A�j���f�[�^�̐擪�A�h���X���Z�b�g
	//g_Player.tbl_size = sizeof(s_car) / sizeof(INTERPOLATION_DATA);		// �Đ�����A�j���f�[�^�̃��R�[�h�����Z�b�g


	g_Parts[0].tbl_adr = s_head;	// �Đ�����A�j���f�[�^�̐擪�A�h���X���Z�b�g
	g_Parts[0].tbl_size = sizeof(s_head) / sizeof(INTERPOLATION_DATA);		// �Đ�����A�j���f�[�^�̃��R�[�h�����Z�b�g
	g_Parts[0].load = 1;

	g_Parts[1].tbl_adr = s_body;	// �Đ�����A�j���f�[�^�̐擪�A�h���X���Z�b�g
	g_Parts[1].tbl_size = sizeof(s_body) / sizeof(INTERPOLATION_DATA);		// �Đ�����A�j���f�[�^�̃��R�[�h�����Z�b�g
	g_Parts[1].load = 1;

	g_Parts[2].tbl_adr = s_R_hand;	// �Đ�����A�j���f�[�^�̐擪�A�h���X���Z�b�g
	g_Parts[2].tbl_size = sizeof(s_R_hand) / sizeof(INTERPOLATION_DATA);		// �Đ�����A�j���f�[�^�̃��R�[�h�����Z�b�g
	g_Parts[2].load = 1;

	g_Parts[3].tbl_adr = s_R_reg;	// �Đ�����A�j���f�[�^�̐擪�A�h���X���Z�b�g
	g_Parts[3].tbl_size = sizeof(s_R_reg) / sizeof(INTERPOLATION_DATA);		// �Đ�����A�j���f�[�^�̃��R�[�h�����Z�b�g
	g_Parts[3].load = 1;

	g_Parts[4].tbl_adr = s_L_hand;	// �Đ�����A�j���f�[�^�̐擪�A�h���X���Z�b�g
	g_Parts[4].tbl_size = sizeof(s_L_hand) / sizeof(INTERPOLATION_DATA);		// �Đ�����A�j���f�[�^�̃��R�[�h�����Z�b�g
	g_Parts[4].load = 1;

	g_Parts[5].tbl_adr = s_L_reg;	// �Đ�����A�j���f�[�^�̐擪�A�h���X���Z�b�g
	g_Parts[5].tbl_size = sizeof(s_L_reg) / sizeof(INTERPOLATION_DATA);		// �Đ�����A�j���f�[�^�̃��R�[�h�����Z�b�g
	g_Parts[5].load = 1;

	g_Parts[6].tbl_adr = s_stick;	// �Đ�����A�j���f�[�^�̐擪�A�h���X���Z�b�g
	g_Parts[6].tbl_size = sizeof(s_stick) / sizeof(INTERPOLATION_DATA);		// �Đ�����A�j���f�[�^�̃��R�[�h�����Z�b�g
	g_Parts[6].load = 1;

	g_Parts[7].tbl_adr = s_nabe;	// �Đ�����A�j���f�[�^�̐擪�A�h���X���Z�b�g
	g_Parts[7].tbl_size = sizeof(s_nabe) / sizeof(INTERPOLATION_DATA);		// �Đ�����A�j���f�[�^�̃��R�[�h�����Z�b�g
	g_Parts[7].load = 1;


	for (int i = 0; i < 8; i++)
	{
		g_Parts[i].move_time = 0.0f;
		g_Parts[6].use = TRUE;
	}

}


void SetBomthrowMotion(void)
{
//*****************************************************************************
// �������̃��[�V�����e�[�u��
//*****************************************************************************
	g_Parts[0].tbl_adr = m_head;	// �Đ�����A�j���f�[�^�̐擪�A�h���X���Z�b�g
	g_Parts[0].tbl_size = sizeof(m_head) / sizeof(INTERPOLATION_DATA);		// �Đ�����A�j���f�[�^�̃��R�[�h�����Z�b�g
	g_Parts[0].load = 1;

	g_Parts[1].tbl_adr = m_body;	// �Đ�����A�j���f�[�^�̐擪�A�h���X���Z�b�g
	g_Parts[1].tbl_size = sizeof(m_body) / sizeof(INTERPOLATION_DATA);		// �Đ�����A�j���f�[�^�̃��R�[�h�����Z�b�g
	g_Parts[1].load = 1;

	g_Parts[2].tbl_adr = m_R_hand;	// �Đ�����A�j���f�[�^�̐擪�A�h���X���Z�b�g
	g_Parts[2].tbl_size = sizeof(m_R_hand) / sizeof(INTERPOLATION_DATA);		// �Đ�����A�j���f�[�^�̃��R�[�h�����Z�b�g
	g_Parts[2].load = 1;

	g_Parts[3].tbl_adr = m_R_reg;	// �Đ�����A�j���f�[�^�̐擪�A�h���X���Z�b�g
	g_Parts[3].tbl_size = sizeof(m_R_reg) / sizeof(INTERPOLATION_DATA);		// �Đ�����A�j���f�[�^�̃��R�[�h�����Z�b�g
	g_Parts[3].load = 1;

	g_Parts[4].tbl_adr = m_L_hand;	// �Đ�����A�j���f�[�^�̐擪�A�h���X���Z�b�g
	g_Parts[4].tbl_size = sizeof(m_L_hand) / sizeof(INTERPOLATION_DATA);		// �Đ�����A�j���f�[�^�̃��R�[�h�����Z�b�g
	g_Parts[4].load = 1;

	g_Parts[5].tbl_adr = m_L_reg;	// �Đ�����A�j���f�[�^�̐擪�A�h���X���Z�b�g
	g_Parts[5].tbl_size = sizeof(m_L_reg) / sizeof(INTERPOLATION_DATA);		// �Đ�����A�j���f�[�^�̃��R�[�h�����Z�b�g
	g_Parts[5].load = 1;

	g_Parts[6].tbl_adr = m_stick;	// �Đ�����A�j���f�[�^�̐擪�A�h���X���Z�b�g
	g_Parts[6].tbl_size = sizeof(m_stick) / sizeof(INTERPOLATION_DATA);		// �Đ�����A�j���f�[�^�̃��R�[�h�����Z�b�g
	g_Parts[6].load = 1;

	g_Parts[7].tbl_adr = m_nabe;	// �Đ�����A�j���f�[�^�̐擪�A�h���X���Z�b�g
	g_Parts[7].tbl_size = sizeof(m_nabe) / sizeof(INTERPOLATION_DATA);		// �Đ�����A�j���f�[�^�̃��R�[�h�����Z�b�g
	g_Parts[7].load = 1;


	for (int i = 0; i < 8; i++)
	{
		g_Parts[i].move_time = 0.0f;
		g_Parts[6].use = FALSE;
	}


}

