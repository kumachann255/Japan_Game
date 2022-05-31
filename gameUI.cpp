//=============================================================================
//
// ゲームUI処理 [gameUI.cpp]
// Author : 熊澤義弘
//
//=============================================================================
#include "main.h"
#include "renderer.h"
#include "gameUI.h"
#include "sprite.h"
#include "score.h"

#include "sound.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define HOWTO_WIDTH					(330.0f)	// キャラサイズ
#define HOWTO_HEIGHT				(120.0f)	// 

#define SCORE0_WIDTH				(100.0f)	// キャラサイズ
#define SCORE0_HEIGHT				(50.0f)		// 

#define SCORE1_WIDTH				(120.0f)	// キャラサイズ
#define SCORE1_HEIGHT				(40.0f)		// 

#define TEXTURE_MAX					(6)			// テクスチャの数

#define MOVE_VOLUME					(20.0f)		// カーブの半径
#define MOVE_SPEED					(0.1f)		// カーブの速度


enum {
	HowTo,
	score0,
	score1,
	under,
	up,
	right,


};


//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************


//*****************************************************************************
// グローバル変数
//*****************************************************************************
static ID3D11Buffer				*g_VertexBuffer = NULL;		// 頂点情報
static ID3D11ShaderResourceView	*g_Texture[TEXTURE_MAX] = { NULL };	// テクスチャ情報


static char *g_TexturName[TEXTURE_MAX] = {
	"data/TEXTURE/hoetouse_ui.png",
	"data/TEXTURE/score_00.png",
	"data/TEXTURE/score_01.png",
	"data/TEXTURE/partypeople_down.png",
	"data/TEXTURE/partypeople_up.png",
	"data/TEXTURE/partypeople_right.png",
};

static UI						g_UI[TEXTURE_MAX];			// UI

static int						g_Score;					// スコア

static BOOL						g_Border;					// 目標スコアを超えているかどうか
static int						g_BorderScore;				// 目標スコア

static float					g_time;
static int						g_Stage;

static BOOL						g_SoundFlag;
static BOOL						g_Load = FALSE;


//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitGameUI(void)
{
	ID3D11Device *pDevice = GetDevice();

	//テクスチャ生成
	for (int i = 0; i < TEXTURE_MAX; i++)
	{
		g_Texture[i] = NULL;
		D3DX11CreateShaderResourceViewFromFile(GetDevice(),
			g_TexturName[i],
			NULL,
			NULL,
			&g_Texture[i],
			NULL);
	}


	// 頂点バッファ生成
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.ByteWidth = sizeof(VERTEX_3D) * 4;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	GetDevice()->CreateBuffer(&bd, NULL, &g_VertexBuffer);


	// 初期化
	for (int i = 0; i < TEXTURE_MAX; i++)
	{
		g_UI[i].use = TRUE;
		g_UI[i].texNo = i;

		// お祝いテクスチャは最初非表示
		if (i >= under)
		{
			g_UI[i].use = FALSE;
			g_UI[i].pos = { SCREEN_WIDTH / 2 ,SCREEN_HEIGHT / 2 , 0.0f };
			g_UI[i].w = SCREEN_WIDTH;
			g_UI[i].h = SCREEN_HEIGHT;
		}

		switch (i)
		{
		case HowTo:
			g_UI[i].pos = { 170.0f , 480.0f , 0.0f };
			g_UI[i].w = HOWTO_WIDTH;
			g_UI[i].h = HOWTO_HEIGHT;

			break;

		case score0:
			g_UI[i].pos = { 700.0f , 50.0f , 0.0f };
			g_UI[i].w = SCORE0_WIDTH;
			g_UI[i].h = SCORE0_HEIGHT;

			break;

		case score1:
			g_UI[i].pos = { 750.0f , 120.0f , 0.0f };
			g_UI[i].w = SCORE1_WIDTH;
			g_UI[i].h = SCORE1_HEIGHT;

			break;

		}
	}

	g_Stage = GetStage();

	// 目標スコアの初期化
	switch (g_Stage)
	{
	case stage0:
	case tutorial:
		g_BorderScore = SCORE_STAGE0_BORDER;

		break;

	case stage1:
		g_BorderScore = SCORE_STAGE1_BORDER;

		break;

	case stage2:
		g_BorderScore = SCORE_STAGE2_BORDER;

		break;

	case stage3:
		g_BorderScore = SCORE_STAGE3_BORDER;

		break;

	}


	g_Score = 0;	// スコアの初期化
	g_time = 0.0f;
	g_Border = FALSE;
	g_SoundFlag = FALSE;

	g_Load = TRUE;
	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitGameUI(void)
{
	if (g_Load == FALSE) return;

	if (g_VertexBuffer)
	{
		g_VertexBuffer->Release();
		g_VertexBuffer = NULL;
	}

	for (int i = 0; i < TEXTURE_MAX; i++)
	{
		if (g_Texture[i])
		{
			g_Texture[i]->Release();
			g_Texture[i] = NULL;
		}
	}

	g_Load = FALSE;
}

//=============================================================================
// 更新処理
//=============================================================================
void UpdateGameUI(void)
{
	// 目標スコアを超えたらお祝いテクスチャを表示
	if (g_BorderScore <= GetScore())
	{
		for (int i = under; i < under + 3; i++)
		{
			g_UI[i].use = TRUE;

		}

		if (!g_SoundFlag)
		{
			PlaySound(SOUND_LABEL_SE_cheers00);
			g_SoundFlag = TRUE;
		}
	}

	// お祝いテクスチャの処理
	for (int i = under; i < under + 3; i++)
	{
		switch (i)
		{
		case under:
			g_UI[i].pos.y = SCREEN_HEIGHT / 2 + (cosf(g_time) * MOVE_VOLUME) + MOVE_VOLUME;

			break;

		case up:
			g_UI[i].pos.y = SCREEN_HEIGHT / 2 - (cosf(g_time) * MOVE_VOLUME) - MOVE_VOLUME;

			break;

		case right:
			g_UI[i].pos.x = SCREEN_WIDTH / 2 - (sinf(g_time) * MOVE_VOLUME) + MOVE_VOLUME;
			break;
		}
	}

	g_time += MOVE_SPEED;

}

//=============================================================================
// 描画処理
//=============================================================================
void DrawGameUI(void)
{
	// 頂点バッファ設定
	UINT stride = sizeof(VERTEX_3D);
	UINT offset = 0;
	GetDeviceContext()->IASetVertexBuffers(0, 1, &g_VertexBuffer, &stride, &offset);

	// マトリクス設定
	SetWorldViewProjection2D();

	// プリミティブトポロジ設定
	GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	// マテリアル設定
	MATERIAL material;
	ZeroMemory(&material, sizeof(material));
	material.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	SetMaterial(material);

	for (int i = 0; i < TEXTURE_MAX; i++)
	{
		if (!g_UI[i].use) continue;

		// テクスチャ設定
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[i]);

		// １枚のポリゴンの頂点とテクスチャ座標を設定
		SetSprite(g_VertexBuffer, g_UI[i].pos.x, g_UI[i].pos.y, g_UI[i].w, g_UI[i].h, 0.0f, 0.0f, 1.0f, 1.0f);

		// ポリゴン描画
		GetDeviceContext()->Draw(4, 0);
	}
}



//=============================================================================
// 目標スコアを返す
//=============================================================================
int GetBorderScore(void)
{
	return g_BorderScore;
}


//=============================================================================
// お祝いテクスチャが表示されているかを返す
//=============================================================================
BOOL GetCelebration(void)
{
	return g_UI[under].use;
}