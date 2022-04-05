//=============================================================================
//
// �O������ [orbit.h]
// Author : 
//
//=============================================================================
#pragma once


//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
HRESULT InitOrbit(void);
void UninitOrbit(void);
void UpdateOrbit(void);
void DrawOrbit(void);

int SetOrbit(XMFLOAT3 pos, XMFLOAT3 move, XMFLOAT4 col, float fSizeX, float fSizeY, int nLife);
void SetOrbit(int nIdxParticle, XMFLOAT4 col);

