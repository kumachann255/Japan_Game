//=============================================================================
//
// チュートリアル処理 [tutorial.cpp]
// Author : 
//
//=============================================================================
#include "main.h"
#include "renderer.h"
#include "tutorial.h"
#include "sprite.h"
#include "score.h"
#include "input.h"
#include "fade.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define TEXTURE_WIDTH				(500.0f)	// キャラサイズ
#define TEXTURE_HEIGHT				(300.0f)	// 

#define TEXTURE_MAX					(10)			// テクスチャの数

#define TEXTURE_OFFSET_Y			(60.0f)		// 表示位置調整
#define TEXTURE_OFFSET_X			(180.0f)	// 表示位置調整

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************


//*****************************************************************************
// グローバル変数
//*****************************************************************************
static ID3D11Buffer				*g_VertexBuffer = NULL;		// 頂点情報
static ID3D11ShaderResourceView	*g_Texture[TEXTURE_MAX] = { NULL };	// テクスチャ情報


static char *g_TexturName[TEXTURE_MAX] = {
	"data/TEXTURE/fade_black.png",
	"data/TEXTURE/tutorial00.png",
	"data/TEXTURE/tutorial01.png",
	"data/TEXTURE/tutorial02.png",
	"data/TEXTURE/tutorial03.png",
	"data/TEXTURE/tutorial04.png",
	"data/TEXTURE/tutorial05.png",
	"data/TEXTURE/tutorial06.png",
	"data/TEXTURE/tutorial07.png",
	"data/TEXTURE/tutorial08.png",
};

enum {
	tutorial00 = 1,
	tutorial01,
	tutorial02,
	tutorial03,
	tutorial04,
	tutorial05,
	tutorial06,
	tutorial07,
	tutorial08,
};

static TUTORIAL					g_Tutorial[2];	// 0：少しくらい背景　1：テキスト

static float					g_time;

static BOOL						g_EnemyDead;

static int						g_Stage;

static BOOL						g_Load = FALSE;


//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitTutorial(void)
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

	g_Stage = GetStage();

	// 初期化
	for (int i = 0; i < 2; i++)
	{
		g_Tutorial[i].use = FALSE;
		g_Tutorial[i].pos = { SCREEN_WIDTH / 2 ,SCREEN_HEIGHT / 2 , 0.0f };
		g_Tutorial[i].texNo = i;

		if (i == 0)
		{	// 背景
			g_Tutorial[i].w = SCREEN_WIDTH;
			g_Tutorial[i].h = SCREEN_HEIGHT;
		}
		else
		{	// テキスト
			g_Tutorial[i].w = TEXTURE_WIDTH;
			g_Tutorial[i].h = TEXTURE_HEIGHT;

			g_Tutorial[i].pos.y -= TEXTURE_OFFSET_Y;
			g_Tutorial[i].pos.x -= TEXTURE_OFFSET_X;

		}
	}

	// チュートリアル以外は使わない
	if (g_Stage == tutorial)
	{
		for (int i = 0; i < 2; i++)
		{
			g_Tutorial[i].use = TRUE;
		}
	}


	g_time = 0.0f;
	g_EnemyDead = FALSE;
	g_Load = TRUE;
	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitTutorial(void)
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
void UpdateTutorial(void)
{
	// チュートリアル以外は使わない
	if (g_Stage != tutorial) return;

	{
		if ((GetKeyboardTrigger(DIK_RETURN)) || (GetKeyboardTrigger(DIK_SPACE)))
		{
			// チュートリアルテキストが最後まで言っていたらチュートリアル終了
			if (g_Tutorial[1].texNo == tutorial08)
			{
				SetStage(stage0);
				SetFade(FADE_OUT, MODE_GAME);
			}

			// 敵を倒す前までのチュートリアル
			if (!g_EnemyDead)
			{
				if (g_Tutorial[1].texNo < tutorial05) g_Tutorial[1].texNo++;
				else if (g_Tutorial[1].texNo == tutorial05)
				{	// 敵を倒すフェーズに行ったらチュートリアルを消す
					for (int i = 0; i < 2; i++)
					{
						g_Tutorial[i].use = FALSE;
					}
				}
			}
			else
			{	// 敵を倒した後のチュートリアル
				if (g_Tutorial[1].texNo < tutorial08) g_Tutorial[1].texNo++;
			}
		}

		// 敵が倒されていたら再度チュートリアルを表示
		if (g_EnemyDead)
		{
			for (int i = 0; i < 2; i++)
			{
				g_Tutorial[i].use = TRUE;
			}
		}

	}
#ifdef _DEBUG	// デバッグ情報を表示する
	//char *str = GetDebugStr();
	//sprintf(&str[strlen(str)], " PX:%.2f PY:%.2f", g_Pos.x, g_Pos.y);
	
#endif

}

//=============================================================================
// 描画処理
//=============================================================================
void DrawTutorial(void)
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

	XMFLOAT4 color = { 1.0f, 1.0f, 1.0f, 1.0f };

	for (int i = 0; i < 2; i++)
	{
		if (!g_Tutorial[i].use) continue;

		if (i == 0)
		{
			color.w = 0.3f;
		}
		else
		{
			color.w = 1.0f;
		}

		// テクスチャ設定
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_Tutorial[i].texNo]);

		// １枚のポリゴンの頂点とテクスチャ座標を設定
		SetSpriteColor(g_VertexBuffer, g_Tutorial[i].pos.x, g_Tutorial[i].pos.y, g_Tutorial[i].w, g_Tutorial[i].h, 0.0f, 0.0f, 1.0f, 1.0f, color);

		// ポリゴン描画
		GetDeviceContext()->Draw(4, 0);
	}
}


// 敵を倒したかどうかを確認する関数
void SetTutorialEnemy(BOOL data)
{
	g_EnemyDead = data;
	if (g_Tutorial[1].texNo < tutorial06) g_Tutorial[1].texNo++;
}