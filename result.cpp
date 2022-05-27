//=============================================================================
//
// リザルト画面処理 [result.cpp]
// Author : GP12B183 山田隆徳
//
//=============================================================================
#include "main.h"
#include "renderer.h"
#include "result.h"
#include "input.h"
#include "fade.h"
#include "sound.h"
#include "sprite.h"
#include "score.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define TEXTURE_WIDTH				(SCREEN_WIDTH)	// 背景サイズ
#define TEXTURE_HEIGHT				(SCREEN_HEIGHT)	// 
#define TEXTURE_MAX					(12)				// テクスチャの数

#define TEXTURE_WIDTH_LOGO			(480)			// ロゴサイズ
#define TEXTURE_HEIGHT_LOGO			(80)			// 

#define RESULT_MAX					(6)				// ON/OFFスイッチが必要なテクスチャ数

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************


//*****************************************************************************
// グローバル変数
//*****************************************************************************
static ID3D11Buffer				*g_VertexBuffer = NULL;		// 頂点情報
static ID3D11ShaderResourceView	*g_Texture[TEXTURE_MAX] = { NULL };	// テクスチャ情報

static char *g_TexturName[TEXTURE_MAX] = {
	"data/TEXTURE/result_bg.png",
	"data/TEXTURE/result_effect1.png",
	"data/TEXTURE/result_effect2.png",
	"data/TEXTURE/stage1.png",
	"data/TEXTURE/stage2.png",
	"data/TEXTURE/stage3.png",
	"data/TEXTURE/stage4.png",
	"data/TEXTURE/max_combo.png",
	"data/TEXTURE/totalscore.png",
	"data/TEXTURE/speech_boom.png",
	"data/TEXTURE/comboNum.png",
	"data/TEXTURE/time0.png",
};


static BOOL						g_Use[RESULT_MAX];						// TRUE:使っている  FALSE:未使用
static float					g_w, g_h;					// 幅と高さ
static XMFLOAT3					g_Pos;						// ポリゴンの座標
static int						g_TexNo[RESULT_MAX];					// テクスチャ番号

static int						g_time;

static int						g_totale;

static BOOL						g_Load = FALSE;

//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitResult(void)
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


	// 変数の初期化
	g_w     = TEXTURE_WIDTH;
	g_h     = TEXTURE_HEIGHT;
	g_Pos   = { g_w / 5.0f, 60.0f, 0.0f };

	g_time = 0;

	g_totale = 0;

	for (int i = 0; i < stage_max; i++)
	{
		g_totale += GetMainScore(i);
	}


	for (int i = 0; i < RESULT_MAX; i++)
	{
		g_Use[i] = FALSE;

		switch (i)
		{

		case 0:
			g_TexNo[i] = 3;
			break;

		case 1:
			g_TexNo[i] = 4;

			break;

		case 2:
			g_TexNo[i] = 5;
			break;

		case 3:
			g_TexNo[i] = 6;

			break;

		case 4:
			g_TexNo[i] = 8;

			break;

		case 5:
			g_TexNo[i] = 10;

			break;

		default:
			break;
		}
	}

	// BGM再生
	//PlaySound(SOUND_LABEL_BGM_sample002);

	g_Load = TRUE;
	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitResult(void)
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
void UpdateResult(void)
{

	if (GetKeyboardTrigger(DIK_RETURN))
	{// Enter押したら、ステージを切り替える
		SetFade(FADE_OUT, MODE_ENDROLL);
	}
	// ゲームパッドで入力処理
	else if (IsButtonTriggered(0, BUTTON_START))
	{
		SetFade(FADE_OUT, MODE_ENDROLL);
	}
	else if (IsButtonTriggered(0, BUTTON_B))
	{
		SetFade(FADE_OUT, MODE_ENDROLL);
	}

	g_time++;

	for (int i = 0; i < stage_max; i++)
	{
		if (g_time >= 45 * (i + 1))
		{
			g_Use[i] = TRUE;
		}
	}
	if (g_time >= 250 )
	{
		g_Use[4] = TRUE;

	}

	if (g_time >= 330)
	{
		g_Use[5] = TRUE;

	}



#ifdef _DEBUG	// デバッグ情報を表示する
	
#endif

}

//=============================================================================
// 描画処理
//=============================================================================
void DrawResult(void)
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

	// リザルトの背景を描画
	{
		// テクスチャ設定
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[0]);

		// １枚のポリゴンの頂点とテクスチャ座標を設定
		SetSpriteLeftTop(g_VertexBuffer, 0.0f, 0.0f, g_w, g_h, 0.0f, 0.0f, 1.0f, 1.0f);

		// ポリゴン描画
		GetDeviceContext()->Draw(4, 0);
	}


	// リザルトのエフェクトを描画
	{
		// テクスチャ設定
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[1]);

		// １枚のポリゴンの頂点とテクスチャ座標を設定
		SetSpriteLeftTop(g_VertexBuffer, 0.0f, 0.0f, g_w, g_h, 0.0f, 0.0f, 1.0f, 1.0f);

		// ポリゴン描画
		GetDeviceContext()->Draw(4, 0);
	}




	// ステージのロゴを描画
	{
		for (int i = 0; i < stage_max; i++)
		{

			if (g_Use[i] == TRUE)
			{
				GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_TexNo[i]]);

				// １枚のポリゴンの頂点とテクスチャ座標を設定
				SetSprite(g_VertexBuffer, g_Pos.x, g_Pos.y + (i * 65.0f), TEXTURE_WIDTH_LOGO / 2.0f, TEXTURE_HEIGHT_LOGO / 2.0f, 0.0f, 0.0f, 1.0f, 1.0f);

				// ポリゴン描画
				GetDeviceContext()->Draw(4, 0);

			}

		}
		// テクスチャ設定


		//// １枚のポリゴンの頂点とテクスチャ座標を設定
		//SetSprite(g_VertexBuffer, g_Pos.x, g_Pos.y, TEXTURE_WIDTH_LOGO, TEXTURE_HEIGHT_LOGO, 0.0f, 0.0f, 1.0f, 1.0f);

		//// ポリゴン描画
		//GetDeviceContext()->Draw(4, 0);
	}


	// 各ステージのスコア表示
	{
		// テクスチャ設定
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[11]);

		for (int m = 0; m < stage_max; m++)
		{

			// 桁数分処理する
			int number = GetMainScore(m);
			for (int i = 0; i < SCORE_DIGIT; i++)
			{
				// 今回表示する桁の数字
				float x = (float)(number % 10);

				// スコアの位置やテクスチャー座標を反映
				float pw = 32;			// スコアの表示幅
				float ph = 50;			// スコアの表示高さ
				float px = 500.0f - i * pw;	// スコアの表示位置X
				float py = 60.0f + (m * 65.0f);			// スコアの表示位置Y

				float tw = 1.0f / 10;		// テクスチャの幅
				float th = 1.0f / 1;		// テクスチャの高さ
				float tx = x * tw;			// テクスチャの左上X座標
				float ty = 0.0f;			// テクスチャの左上Y座標

				if (g_Use[m] == TRUE)
				{
					// １枚のポリゴンの頂点とテクスチャ座標を設定
					SetSpriteColor(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th,
						XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));

					// ポリゴン描画
					GetDeviceContext()->Draw(4, 0);

				}

				// 次の桁へ
				number /= 10;
			}
		}

	}


	// トータルロゴを描画
	if (g_Use[4] == TRUE)
	{
		// テクスチャ設定
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[8]);


		{

			// １枚のポリゴンの頂点とテクスチャ座標を設定
			SetSpriteLeftTop(g_VertexBuffer, 630.0f, 60.0f, g_w / 4, g_h / 5, 0.0f, 0.0f, 1.0f, 1.0f);

			// ポリゴン描画
			GetDeviceContext()->Draw(4, 0);
		}
	}


	// 合計スコア表示
	if (g_Use[5] == TRUE)
	{
		// テクスチャ設定
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[10]);

		// 桁数分処理する
		int number = g_totale;
		for (int i = 0; i < SCORE_DIGIT; i++)
		{
			// 今回表示する桁の数字
			float x = (float)(number % 10);

			// スコアの位置やテクスチャー座標を反映
			float pw = 80;			// スコアの表示幅
			float ph = 140;			// スコアの表示高さ
			float px = 900.0f - i * pw;	// スコアの表示位置X
			float py = 230.0f;			// スコアの表示位置Y

			float tw = 1.0f / 10;		// テクスチャの幅
			float th = 1.0f / 1;		// テクスチャの高さ
			float tx = x * tw;			// テクスチャの左上X座標
			float ty = 0.0f;			// テクスチャの左上Y座標

			// １枚のポリゴンの頂点とテクスチャ座標を設定
			SetSpriteColor(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th,
				XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));

			// ポリゴン描画
			GetDeviceContext()->Draw(4, 0);
				


			// 次の桁へ
			number /= 10;
		}
	 }

	// 最大コンボのロゴを描画
	{
		// テクスチャ設定
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[7]);

		// １枚のポリゴンの頂点とテクスチャ座標を設定
		SetSpriteLeftTop(g_VertexBuffer, 30.0f, 340.0f, g_w / 4, g_h / 5, 0.0f, 0.0f, 1.0f, 1.0f);

		// ポリゴン描画
		GetDeviceContext()->Draw(4, 0);
	}

}




