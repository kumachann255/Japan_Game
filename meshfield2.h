//=============================================================================
//
// ���b�V���n�ʂ̏��� [meshfield2.h]
// Author : �G�i
//
//=============================================================================
#pragma once

#define FIELD_SPEED			(5.0f)		// �n�ʂ̓������x

//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
HRESULT InitMeshField2(XMFLOAT3 pos, XMFLOAT3 rot,
							int nNumBlockX, int nNumBlockZ, float nBlockSizeX, float nBlockSizeZ);
void UninitMeshField2(void);
void UpdateMeshField2(void);
void DrawMeshField2(void);

BOOL RayHitField2(XMFLOAT3 pos, XMFLOAT3 *HitPosition, XMFLOAT3 *Normal);

