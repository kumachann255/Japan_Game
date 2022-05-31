//=============================================================================
//
// エネミーヘリモデル処理 [enemyHeli.h]
// Author : 大塚勝亮
//
//=============================================================================
#pragma once


//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define MAX_ENEMY_HELI		(1)			// エネミーヘリの数

#define HELI_PARTS_MAX		(2)			// エネミーヘリのパーツの数

#define	ENEMY_HELI_SIZE		(30.0f)		// 当たり判定の大きさ

#define ENEMY_HELI_OFFSET_Y	(40.0f)		// エネミーヘリの足元をあわせる

//*****************************************************************************
// 構造体定義
//*****************************************************************************
struct ENEMY_HELI
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
	int					shadowIdx;			// 影のインデックス番号

	float				zGoal;				// z座標のプレイヤーに近づく最大距離

	XMFLOAT3			hitPos;				// 爆発の座標
	XMFLOAT3			hitRot;				// 当たり判定後アニメーション用、毎回の移動量
	BOOL				isHit;				// 当たってるフラグ TRUE:当たっている
	BOOL				move;				// 奥へ移動するフラグ TRUE:移動する
	int					hitTime;			// 移動時間
	int					liveCount;			// ポップしてからの時間を管理

	BOOL				fuchi;				// リムライトのオンオフ

	float				speed;				// 移動スピード
	float				time;				// 時間
	BOOL				switchFlag;			// ベジェ用
	ENEMY_HELI			*parent;			// エネミーヘリのパーツ
};

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
HRESULT InitEnemyHeli(void);
void UninitEnemyHeli(void);
void UpdateEnemyHeli(void);
void DrawEnemyHeli(void);

ENEMY_HELI *GetEnemyHeli(void);

