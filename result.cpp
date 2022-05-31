//=============================================================================
//
// リザルト画面処理 [result.cpp]
// Author : 山田隆徳
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
#define TEXTURE_MAX					(16)				// テクスチャの数

#define TEXTURE_WIDTH_LOGO			(480)			// ロゴサイズ
#define TEXTURE_HEIGHT_LOGO			(80)			// 

#define RESULT_MAX					(17)				// ON/OFFスイッチが必要なテクスチャ数

#define RANK_C_BORDER				(15000)
#define RANK_B_BORDER				(30000)
#define RANK_A_BORDER				(50000)
#define RANK_S_BORDER				(75000)

#define MOVE_VOLUME					(20.0f)		// カーブの半径
#define MOVE_SPEED					(0.1f)		// カーブの速度

//enum 
//{
//	result_bg,
//	result_effect,
//	rank_effect,
//	stage1,
//	stage2,
//	stage3,
//	stage4,
//	MaxCombo,
//	TotalScore,
//	BomEffect,
//	ComboNum,
//	FinalScore,
//	RankA,
//	RankB,
//	RankC,
//	RankS
//};
//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************


//*****************************************************************************
// グローバル変数
//*****************************************************************************
static ID3D11Buffer				*g_VertexBuffer = NULL;		// 頂点情報
static ID3D11ShaderResourceView	*g_Texture[TEXTURE_MAX] = { NULL };	// テクスチャ情報

static char *g_TexturName[TEXTURE_MAX] = {
	"data/TEXTURE/result_bg2.png",
	"data/TEXTURE/result_effect.png",
	"data/TEXTURE/rank_effect.png",
	"data/TEXTURE/stage1.png",
	"data/TEXTURE/stage2.png",
	"data/TEXTURE/stage3.png",
	"data/TEXTURE/stage4.png",
	"data/TEXTURE/max_combo.png",
	"data/TEXTURE/totalscore.png",
	"data/TEXTURE/speech_boom.png",
	"data/TEXTURE/comboNum.png",
	"data/TEXTURE/time0.png",
	"data/TEXTURE/rankA.png",
	"data/TEXTURE/rankB.png",
	"data/TEXTURE/rankC.png",
	"data/TEXTURE/rankS.png",

};

//static BOOL						g_Use;						// TRUE:使っている  FALSE:未使用	デバッグ用

static BOOL						g_Use[RESULT_MAX];						// TRUE:使っている  FALSE:未使用
static float					g_w, g_h;					// 幅と高さ
static XMFLOAT3					g_Pos;						// ポリゴンの座標
static int						g_TexNo[RESULT_MAX];					// テクスチャ番号

static int						g_time;

static int						g_totale;

static XMFLOAT4					g_Colar;

static int						g_timeRank;

static BOOL						g_soundFlag;

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
	g_Pos   = { g_w / 3.6f, 130.0f, 0.0f };

	g_time = 0;

	g_totale = 0;

	g_Colar.w = 0.0f;

	g_timeRank = 0;

	g_soundFlag = FALSE;

	for (int i = 0; i < stage_max; i++)
	{
		g_totale += GetMainScore(i);
	}

	// 最大を超えていた場合99999で固定
	if (g_totale > SCORE_MAX)
	{
		g_totale = SCORE_MAX;
	}

	//g_Use = TRUE;		// デバッグ用

	for (int i = 0; i < RESULT_MAX; i++)
	{
		g_Use[i] = FALSE;

		switch (i)
		{

		case 0:
			g_TexNo[i] = 3;	// ステージ１ロゴ
			break;

		case 1:
			g_TexNo[i] = 4;	// ステージ2ロゴ

			break;

		case 2:
			g_TexNo[i] = 5;	// ステージ3ロゴ
			break;

		case 3:
			g_TexNo[i] = 6;	// ステージ4ロゴ

			break;

		case 4:
			g_TexNo[i] = 8;	//トータルロゴ

			break;

		case 5:
			g_TexNo[i] = 10; // トータルスコア
			break;

		case 6:
			g_TexNo[i] = 7;	// 最大コンボロゴ
			break;

		case 7:
			g_TexNo[i] = 10; // 最大コンボ数
			break;

		case 8:
			g_TexNo[i] = 12;	// rankA
			break;

		case 9:
			g_TexNo[i] = 13;	// rankB
			break;

		case 10:
			g_TexNo[i] = 14;	// rankC
			break;

		case 11:
			g_TexNo[i] = 15;	// rankS
			break;

		case 12:
			g_TexNo[i] = 9;		// ランク表示エフェクト
			break;

		case 13:
			g_TexNo[i] = 2;		// ランクSエフェクト
			break;

		case 14:
			g_TexNo[i] = 9;		// ランクS表示エフェクト
			break;

		default:
			break;
		}
	}

	// BGM再生
	PlaySound(SOUND_LABEL_BGM_bgm_stage001);

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
		if (g_time == 45 * (i + 1))
		{
			g_Use[i] = TRUE;
			PlaySound(SOUND_LABEL_SE_comboSound01);
		}
	}

	if (g_time == 250 )
	{
		g_Use[4] = TRUE;
		PlaySound(SOUND_LABEL_SE_stickingSound01);
	}

	if (g_time == 330)
	{
		g_Use[5] = TRUE;
		PlaySound(SOUND_LABEL_SE_stickingSound01);
	}

	if (g_time == 390)
	{
		g_Use[6] = TRUE;
		g_Use[7] = TRUE;
		PlaySound(SOUND_LABEL_SE_stickingSound01);
	}

	if (g_time >= 440)
	{

		if (g_timeRank == 0)
		{
			g_Colar.w += 0.04f;

		}

		if (g_Colar.w >= 1.0f)
		{
			g_Colar.w = 1.0f;
			g_timeRank++;


			if (0 <= g_totale && g_totale < RANK_C_BORDER)
			{
				g_Use[10] = TRUE;
				g_Use[12] = TRUE;


			}
			if (RANK_C_BORDER <= g_totale && g_totale < RANK_B_BORDER)
			{
				g_Use[9] = TRUE;
				g_Use[12] = TRUE;


			}
			if (RANK_B_BORDER <= g_totale && g_totale < RANK_A_BORDER)
			{
				g_Use[8] = TRUE;
				g_Use[12] = TRUE;


			}
			if ( g_totale >= RANK_S_BORDER)
			{
				g_Use[11] = TRUE;
				g_Use[13] = TRUE;
				g_Use[14] = TRUE;


			}
		}

		if ((g_timeRank >60) && (g_Colar.w > 0.0f))
		{
			g_Colar.w -= 0.04f;

		}
	}

	if ((g_Use[13]) && (!g_soundFlag))
	{
		PlaySound(SOUND_LABEL_SE_selectBomb01);
		g_soundFlag = TRUE;
	}
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
				SetSprite(g_VertexBuffer, g_Pos.x, g_Pos.y + (i * 55.0f), TEXTURE_WIDTH_LOGO / 3.5f, TEXTURE_HEIGHT_LOGO / 3.0f, 0.0f, 0.0f, 1.0f, 1.0f);

				// ポリゴン描画
				GetDeviceContext()->Draw(4, 0);

			}
		}
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
				float pw = 22;			// スコアの表示幅
				float ph = 40;			// スコアの表示高さ
				float px = 450.0f - i * pw;	// スコアの表示位置X
				float py = 130.0f + (m * 55.0f);			// スコアの表示位置Y

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
			SetSpriteLeftTop(g_VertexBuffer, 510.0f, 120.0f, g_w / 4, g_h / 8, 0.0f, 0.0f, 1.0f, 1.0f);

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
			float pw = 60;			// スコアの表示幅
			float ph = 130;			// スコアの表示高さ
			float px = 750.0f - i * pw;	// スコアの表示位置X
			float py = 250.0f;			// スコアの表示位置Y

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
	if(g_Use[6] == TRUE)
	{
		// テクスチャ設定
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[7]);

		// １枚のポリゴンの頂点とテクスチャ座標を設定
		SetSpriteLeftTop(g_VertexBuffer, 200.0f, 350.0f, g_w / 6, g_h / 10, 0.0f, 0.0f, 1.0f, 1.0f);

		// ポリゴン描画
		GetDeviceContext()->Draw(4, 0);
	}


	// 最大コンボ数を描画
	if(g_Use[7] == TRUE)
	{
		// テクスチャ設定
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[10]);

		// 桁数分処理する
		int number = GetComboMax();
		for (int i = 0; i < COMBO_MAX_DIGIT; i++)
		{
			// 今回表示する桁の数字
			float x = (float)(number % 10);

			// スコアの位置やテクスチャー座標を反映
			float pw = 40;			// スコアの表示幅
			float ph = 110;			// スコアの表示高さ
			float px = 470.0f - i * pw;	// スコアの表示位置X
			float py = 380.0f;			// スコアの表示位置Y

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



	// ランクを描画
	if (g_Use[8] == TRUE)
	{
		// テクスチャ設定
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_TexNo[8]]);


		// １枚のポリゴンの頂点とテクスチャ座標を設定
		SetSpriteLeftTop(g_VertexBuffer, 490.0f, 280.0f, g_w / 4, g_h / 3, 0.0f, 0.0f, 1.0f, 1.0f);

		// ポリゴン描画
		GetDeviceContext()->Draw(4, 0);

	}

	if (g_Use[9] == TRUE)
	{
		// テクスチャ設定
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[13]);


		// １枚のポリゴンの頂点とテクスチャ座標を設定
		SetSpriteLeftTop(g_VertexBuffer, 490.0f, 280.0f, g_w / 4, g_h / 3, 0.0f, 0.0f, 1.0f, 1.0f);

		// ポリゴン描画
		GetDeviceContext()->Draw(4, 0);

	}

	if (g_Use[10] == TRUE)
	{
		// テクスチャ設定
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_TexNo[10]]);


		// １枚のポリゴンの頂点とテクスチャ座標を設定
		SetSpriteLeftTop(g_VertexBuffer, 490.0f, 280.0f, g_w / 4, g_h / 3, 0.0f, 0.0f, 1.0f, 1.0f);

		// ポリゴン描画
		GetDeviceContext()->Draw(4, 0);

	}

	if (g_Use[11] == TRUE)
	{
		// テクスチャ設定
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[15]);


		// １枚のポリゴンの頂点とテクスチャ座標を設定
		SetSpriteLeftTop(g_VertexBuffer, 320, 130 , g_w / 3, g_h / 2 , 0.0f, 0.0f, 1.0f, 1.0f);

		// ポリゴン描画
		GetDeviceContext()->Draw(4, 0);
		
	}


	// ランクSエフェクトを描画
	if (g_Use[13] == TRUE)
	{
		// テクスチャ設定
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[2]);

		// １枚のポリゴンの頂点とテクスチャ座標を設定
		SetSpriteLeftTop(g_VertexBuffer, 0.0f, 0.0f, g_w, g_h, 0.0f, 0.0f, 1.0f, 1.0f);

		// ポリゴン描画
		GetDeviceContext()->Draw(4, 0);
	}


	// ランク表示前のエフェクトを描画
	if (g_Use[12] == TRUE)
	{
		// テクスチャ設定
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[9]);

		// スコアの位置やテクスチャー座標を反映
		float pw = 270;			// スコアの表示幅
		float ph = 270;			// スコアの表示高さ
		float px = 610.0f;		// スコアの表示位置X
		float py = 360.0f;			// スコアの表示位置Y

		float tw = 1.0f;			// テクスチャの幅
		float th = 1.0f;			// テクスチャの高さ
		float tx = 0.0f;			// テクスチャの左上X座標
		float ty = 0.0f;			// テクスチャの左上Y座標

		// １枚のポリゴンの頂点とテクスチャ座標を設定
		SetSpriteColor(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th,
			XMFLOAT4(1.0f, 1.0f, 1.0f, g_Colar.w));


		// ポリゴン描画
		GetDeviceContext()->Draw(4, 0);

	}

	// ランクS表示前のエフェクトを描画
	if (g_Use[14] == TRUE)
	{
		// テクスチャ設定
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[9]);

		// スコアの位置やテクスチャー座標を反映
		float pw = g_w / 1.5f ;			// スコアの表示幅
		float ph = g_h ;			// スコアの表示高さ
		float px = 480.0f;		// スコアの表示位置X
		float py = 260.0f;			// スコアの表示位置Y

		float tw = 1.0f;			// テクスチャの幅
		float th = 1.0f;			// テクスチャの高さ
		float tx = 0.0f;			// テクスチャの左上X座標
		float ty = 0.0f;			// テクスチャの左上Y座標

		// １枚のポリゴンの頂点とテクスチャ座標を設定
		SetSpriteColor(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th,
			XMFLOAT4(1.0f, 1.0f, 1.0f, g_Colar.w));


		// ポリゴン描画
		GetDeviceContext()->Draw(4, 0);

	}

}




