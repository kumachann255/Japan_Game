//=============================================================================
//
// 爆破と当たり判定処理 [blast.h]
// Author : 
//
//=============================================================================
#pragma once


//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define MAX_BLAST			(5)
#define	BLAST_SIZE			(30.0f)				// 当たり判定の大きさ

//*****************************************************************************
// 構造体定義
//*****************************************************************************
struct BLAST
{
	XMFLOAT4X4			mtxWorld;			// ワールドマトリックス
	XMFLOAT3			pos;				// モデルの位置
	XMFLOAT3			rot;				// モデルの向き(回転)
	XMFLOAT3			scl;				// モデルの大きさ(スケール)

	BOOL				use;
	BOOL				load;
	DX11_MODEL			model;				// モデル情報
	XMFLOAT4			diffuse[MODEL_MAX_MATERIAL];	// モデルの色

	float				spd;				// 移動スピード
	float				size;				// 当たり判定の大きさ
	int					life;				// 爆破の残り時間

	BOOL				move;				// 奥へ移動するかフラグ TRUE:移動する
};

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
HRESULT InitBlast(void);
void UninitBlast(void);
void UpdateBlast(void);
void DrawBlast(void);

BLAST *GetBlast(void);

void SetBlast(XMFLOAT3 pos);
BOOL GetCameraSwitch(void);
void SetCameraSwitch(BOOL data);
<<<<<<< HEAD
int GetMorphingNum(void);
=======
int GetMorphing(void);
>>>>>>> 3b80df470734f5703880fc331c20d482d12a10fc
