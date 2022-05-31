//=============================================================================
//
// メッシュ地面の処理 [meshfield.h]
// Author : 熊澤義弘
//
//=============================================================================
#pragma once

#define FIELD_SPEED			(5.0f)		// 地面の動く速度

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
HRESULT InitMeshField(XMFLOAT3 pos, XMFLOAT3 rot,
							int nNumBlockX, int nNumBlockZ, float nBlockSizeX, float nBlockSizeZ);
void UninitMeshField(void);
void UpdateMeshField(void);
void DrawMeshField(void);

BOOL RayHitField(XMFLOAT3 pos, XMFLOAT3 *HitPosition, XMFLOAT3 *Normal);

