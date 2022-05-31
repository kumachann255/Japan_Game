//=============================================================================
//
// ふきだし処理 [speech.cpp]
// Author : 熊澤義弘
//
//=============================================================================
#include "main.h"
#include "renderer.h"
#include "speech.h"
#include "sprite.h"
#include "combo.h"

#include "sound.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define GO_WIDTH					(450.0f)	// キャラサイズ
#define GO_HEIGHT					(450.0f)	// 

#define BOM_WIDTH					(300.0f)	// キャラサイズ
#define BOM_HEIGHT					(300.0f)	// 

#define COMBO_WIDTH					(150.0f)	// キャラサイズ
#define COMBO_HEIGHT				(150.0f)	// 

#define DAMAGE_WIDTH				(300.0f)	// キャラサイズ
#define DAMAGE_HEIGHT				(300.0f)	// 

#define TEXTURE_MAX					(9)		// テクスチャの数

#define SPEECH_LIFE					(60)	// 何フレーム吹き出しを出しておくか
#define SPEECH_OFFSET_Y				(50.0f)		// 最初の表示で中心から少しずらす

#define SPEECH_RAND_MAX				(300.0f)	// ランダム値

#define SPEECH_COMBO_X				(800.0f)	// コンボの表示座標
#define SPEECH_COMBO_Y				(250.0f)	// コンボの表示座標

#define SPEECH_BOM_X				(SCREEN_CENTER_X / 2)	// コンボの表示座標
#define SPEECH_BOM_Y				(SCREEN_CENTER_Y / 2)	// コンボの表示座標

#define SCALE_BIG_TIME				(10)		// 吹き出しが大きくなる時間
#define SCALE_SMOLL_TIME			(5)			// 吹き出しが小さくなる時間
#define SCALE_SPEED					(1.01f)		// 吹き出しが大きくなる速度
#define SCALE_DOWN					(0.8f)		// 吹き出しが小さくなる速度


enum {
	go,
	bam,
	boom,
	yeah,
	crash,
	smash,
	party,
	oops,
	ouch,

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
	"data/TEXTURE/speech_go.png",
	"data/TEXTURE/speech_bam.png",
	"data/TEXTURE/speech_boom.png",
	"data/TEXTURE/speech_yeah.png",
	"data/TEXTURE/speech_crash.png",
	"data/TEXTURE/speech_smash.png",
	"data/TEXTURE/speech_party.png",
	"data/TEXTURE/speech_oops.png",
	"data/TEXTURE/speech_ouch.png",
};

static SPEECH					g_Speech[TEXTURE_MAX];		// ふきだし

static int						g_Score;					// スコア

static BOOL						g_Border;					// 目標スコアを超えているかどうか
static int						g_BorderScore;				// 目標スコア

static float					g_time;

static BOOL						g_Load = FALSE;


//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitSpeech(void)
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
		g_Speech[i].use = FALSE;
		g_Speech[i].pos = { 0.0f,0.0f,0.0f };
		g_Speech[i].texNo = i;
		g_Speech[i].w = GO_WIDTH;
		g_Speech[i].h = GO_HEIGHT;
		g_Speech[i].life = SPEECH_LIFE;
		g_Speech[i].combo = FALSE;
	}

	// 最初に吹き出しを表示
	SetStartSpeech();

	g_Load = TRUE;
	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitSpeech(void)
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
void UpdateSpeech(void)
{
	for (int i = 0; i < TEXTURE_MAX; i++)
	{
		if (!g_Speech[i].use) continue;

		// 寿命を減らす
		g_Speech[i].life--;

		// 寿命が尽きたら非表示に
		if (g_Speech[i].life <= 0)
		{
			g_Speech[i].use = FALSE;
		}


		// コンボの吹き出しは発生時に大きくなる
		if ((g_Speech[i].life > SPEECH_LIFE - SCALE_BIG_TIME))
			{
			g_Speech[i].w *= SCALE_SPEED;
			g_Speech[i].h *= SCALE_SPEED;

		}
		else if (g_Speech[i].life <= SCALE_SMOLL_TIME)
		{
			g_Speech[i].w *= SCALE_DOWN;
			g_Speech[i].h *= SCALE_DOWN;
		}


	}
}

//=============================================================================
// 描画処理
//=============================================================================
void DrawSpeech(void)
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
		if (!g_Speech[i].use) continue;

		// テクスチャ設定
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_Speech[i].texNo]);

		// １枚のポリゴンの頂点とテクスチャ座標を設定
		SetSprite(g_VertexBuffer, g_Speech[i].pos.x, g_Speech[i].pos.y, g_Speech[i].w, g_Speech[i].h, 0.0f, 0.0f, 1.0f, 1.0f);

		// ポリゴン描画
		GetDeviceContext()->Draw(4, 0);
	}
}


//=============================================================================
// スタート時の吹き出し
//=============================================================================
void SetStartSpeech(void)
{
	for (int i = 0; i < TEXTURE_MAX; i++)
	{
		if (!g_Speech[i].use)
		{
			// SEのセット
			PlaySound(SOUND_LABEL_SE_carHorn01);

			g_Speech[i].use = TRUE;
			g_Speech[i].life = SPEECH_LIFE;
			g_Speech[i].combo = FALSE;
			g_Speech[i].texNo = go;
			g_Speech[i].w = GO_WIDTH;
			g_Speech[i].h = GO_HEIGHT;

			g_Speech[i].pos = { SCREEN_CENTER_X , SCREEN_CENTER_Y - SPEECH_OFFSET_Y , 0.0f };
			return;
		}
	}
}


//=============================================================================
// 爆弾が爆発した際の吹き出し
//=============================================================================
void SetBomSpeech(void)
{
	for (int i = 0; i < TEXTURE_MAX; i++)
	{
		if (!g_Speech[i].use)
		{
			g_Speech[i].use = TRUE;
			g_Speech[i].life = SPEECH_LIFE;
			g_Speech[i].combo = FALSE;
			g_Speech[i].w = BOM_WIDTH;
			g_Speech[i].h = BOM_HEIGHT;

			int data = rand() % 2;

			g_Speech[i].texNo = data + bam;

			float x = RamdomFloat(1, SPEECH_BOM_X / 2, -SPEECH_BOM_X / 2);
			float y = RamdomFloat(1, SPEECH_BOM_Y / 2, -SPEECH_BOM_Y / 2);

			if (x > 0)
			{
				g_Speech[i].pos.x = SCREEN_CENTER_X + SPEECH_BOM_X / 2 + x;
			}
			else
			{
				g_Speech[i].pos.x = SCREEN_CENTER_X - SPEECH_BOM_X / 2 + x;
			}

			if (y > 0)
			{
				g_Speech[i].pos.y = SCREEN_CENTER_Y + SPEECH_BOM_Y / 2 + x;
			}
			else
			{
				g_Speech[i].pos.y = SCREEN_CENTER_Y - SPEECH_BOM_Y / 2 + x;
			}

			return;
		}
	}
}


//=============================================================================
// コンボ数が増えた時の吹き出し
//=============================================================================
void SetComboSpeech(void)
{
	// SEの挿入
	PlaySound(SOUND_LABEL_SE_shrinkingSound01);


	// コンボの吹き出しをまず消す
	for (int i = 0; i < TEXTURE_MAX; i++)
	{
		if (g_Speech[i].combo)
		{
			g_Speech[i].use = FALSE;
		}
	}

	// 発生
	for (int i = 0; i < TEXTURE_MAX; i++)
	{
		if (!g_Speech[i].use)
		{
			g_Speech[i].use = TRUE;
			g_Speech[i].life = SPEECH_LIFE;
			g_Speech[i].combo = TRUE;
			g_Speech[i].w = COMBO_WIDTH;
			g_Speech[i].h = COMBO_HEIGHT;

			// コンボ数に応じてテクスチャを変更
			if (GetCombo() > COMBO_BONUS_2)
			{
				g_Speech[i].texNo = crash;
			}
			else if (GetCombo() > COMBO_BONUS_1)
			{
				g_Speech[i].texNo = smash;
			}
			else if (GetCombo() > COMBO_BONUS_0)
			{
				g_Speech[i].texNo = party;
			}
			else
			{
				g_Speech[i].texNo = yeah;
			}

			g_Speech[i].pos = { SPEECH_COMBO_X , SPEECH_COMBO_Y, 0.0f };

			return;
		}
	}


}


//=============================================================================
// ダメージを受けた時の吹き出し
//=============================================================================
void SetDamageSpeech(void)
{
	// SEの挿入（破壊されている段階でブーイングを入れる処理）
	PlaySound(SOUND_LABEL_SE_booingSound01);

	for (int i = 0; i < TEXTURE_MAX; i++)
	{
		if (!g_Speech[i].use)
		{
			g_Speech[i].use = TRUE;
			g_Speech[i].life = SPEECH_LIFE;
			g_Speech[i].combo = FALSE;
			g_Speech[i].w = DAMAGE_WIDTH;
			g_Speech[i].h = DAMAGE_HEIGHT;

			int data = rand() % 2;

			g_Speech[i].texNo = data + oops;

			g_Speech[i].pos.x = SCREEN_CENTER_X + RamdomFloat(1, SPEECH_RAND_MAX, -SPEECH_RAND_MAX);
			g_Speech[i].pos.y = SCREEN_CENTER_Y + RamdomFloat(1, SPEECH_RAND_MAX / 3.0f, -SPEECH_RAND_MAX / 3.0f);

			return;
		}
	}
}