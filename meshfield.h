//=============================================================================
//
// ���b�V���n�ʂ̏��� [meshfield.h]
// Author : �F�V�`�O
//
//=============================================================================
#pragma once

#define FIELD_SPEED			(5.0f)		// �n�ʂ̓������x

//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
HRESULT InitMeshField(XMFLOAT3 pos, XMFLOAT3 rot,
							int nNumBlockX, int nNumBlockZ, float nBlockSizeX, float nBlockSizeZ);
void UninitMeshField(void);
void UpdateMeshField(void);
void DrawMeshField(void);

BOOL RayHitField(XMFLOAT3 pos, XMFLOAT3 *HitPosition, XMFLOAT3 *Normal);

