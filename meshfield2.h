//=============================================================================
//
// メッシュ地面の処理 [meshfield2.h]
// Author : エナ
//
//=============================================================================
#pragma once

#define FIELD_SPEED			(5.0f)		// 地面の動く速度

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
HRESULT InitMeshField2(XMFLOAT3 pos, XMFLOAT3 rot,
							int nNumBlockX, int nNumBlockZ, float nBlockSizeX, float nBlockSizeZ);
void UninitMeshField2(void);
void UpdateMeshField2(void);
void DrawMeshField2(void);

BOOL RayHitField2(XMFLOAT3 pos, XMFLOAT3 *HitPosition, XMFLOAT3 *Normal);

