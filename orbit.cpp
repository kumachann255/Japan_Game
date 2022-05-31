//=============================================================================
//
// 軌道処理 [orbit.cpp]
// Author : 熊澤義弘
//
//=============================================================================
#include "main.h"
#include "renderer.h"
#include "input.h"
#include "camera.h"
#include "model.h"
#include "shadow.h"
#include "orbit.h"
#include "player.h"
#include "attackRange.h"
#include "bom.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define TEXTURE_MAX			(1)			// テクスチャの数

#define	ORBIT_SIZE_X		(2.0f)		// 頂点サイズ
#define	ORBIT_SIZE_Y		(2.0f)		// 頂点サイズ

#define	MAX_ORBIT			(32)		// パーティクル最大数

#define	DISP_SHADOW						// 影の表示
//#undef DISP_SHADOW

//*****************************************************************************
// 構造体定義
//*****************************************************************************
typedef struct
{
	XMFLOAT3		pos;			// 位置
	XMFLOAT3		rot;			// 回転
	XMFLOAT3		scale;			// スケール
	XMFLOAT3		move;			// 移動量
	MATERIAL		material;		// マテリアル
	float			fSizeX;			// 幅
	float			fSizeY;			// 高さ
	int				nIdxShadow;		// 影ID
	int				nLife;			// 寿命
	BOOL			bUse;			// 使用しているかどうか

} ORBIT;

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
HRESULT MakeVertexOrbit(void);

//*****************************************************************************
// グローバル変数
//*****************************************************************************
static ID3D11Buffer					*g_VertexBuffer = NULL;		// 頂点バッファ

static ID3D11ShaderResourceView		*g_Texture[TEXTURE_MAX] = { NULL };	// テクスチャ情報
static int							g_TexNo;					// テクスチャ番号

static ORBIT					g_Orbit[MAX_ORBIT];		// パーティクルワーク
static XMFLOAT3					g_posBase;						// ビルボード発生位置
static float					g_fWidthBase = 5.0f;			// 基準の幅
static float					g_fHeightBase = 10.0f;			// 基準の高さ
static float					g_roty = 0.0f;					// 移動方向
static float					g_spd = 0.0f;					// 移動スピード

static char *g_TextureName[TEXTURE_MAX] =
{
	"data/TEXTURE/effect000.jpg",
};

static BOOL						g_Load = FALSE;

//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitOrbit(void)
{
	// 頂点情報の作成
	MakeVertexOrbit();

	// テクスチャ生成
	for (int i = 0; i < TEXTURE_MAX; i++)
	{
		g_Texture[i] = NULL;
		D3DX11CreateShaderResourceViewFromFile(GetDevice(),
			g_TextureName[i],
			NULL,
			NULL,
			&g_Texture[i],
			NULL);
	}

	g_TexNo = 0;

	// パーティクルワークの初期化
	for(int nCntParticle = 0; nCntParticle < MAX_ORBIT; nCntParticle++)
	{
		g_Orbit[nCntParticle].pos = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_Orbit[nCntParticle].rot = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_Orbit[nCntParticle].scale = XMFLOAT3(1.0f, 1.0f, 1.0f);
		g_Orbit[nCntParticle].move = XMFLOAT3(1.0f, 1.0f, 1.0f);

		ZeroMemory(&g_Orbit[nCntParticle].material, sizeof(g_Orbit[nCntParticle].material));
		g_Orbit[nCntParticle].material.Diffuse = XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f);

		g_Orbit[nCntParticle].fSizeX = ORBIT_SIZE_X;
		g_Orbit[nCntParticle].fSizeY = ORBIT_SIZE_Y;
		g_Orbit[nCntParticle].nIdxShadow = -1;
		g_Orbit[nCntParticle].nLife = 0;
		g_Orbit[nCntParticle].bUse = TRUE;
	}

	g_posBase = XMFLOAT3(0.0f, 0.0f, 0.0f);
	g_roty = 0.0f;
	g_spd = 0.0f;

	g_Load = TRUE;
	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitOrbit(void)
{
	if (g_Load == FALSE) return;

	//テクスチャの解放
	for (int nCntTex = 0; nCntTex < TEXTURE_MAX; nCntTex++)
	{
		if (g_Texture[nCntTex] != NULL)
		{
			g_Texture[nCntTex]->Release();
			g_Texture[nCntTex] = NULL;
		}
	}

	// 頂点バッファの解放
	if (g_VertexBuffer != NULL)
	{
		g_VertexBuffer->Release();
		g_VertexBuffer = NULL;
	}

	g_Load = FALSE;
}

//=============================================================================
// 更新処理
//=============================================================================
void UpdateOrbit(void)
{
	PLAYER *pPlayer = GetPlayer();
	ATTACKRANGE *AttackR = GetAttackR();

	XMFLOAT3 control;	// 制御点
	float hight = 0.0f;	// 高さ調節

	// 制御点の算出
	control.x = (AttackR->pos.x - pPlayer->pos.x) / 2.0f + pPlayer->pos.x;
	control.z = (AttackR->pos.z - pPlayer->pos.z) / 2.0f + pPlayer->pos.z;
	control.y = (AttackR->pos.y - pPlayer->pos.y) / 2.0f + pPlayer->pos.y;

	hight = BOM_H;

	control.y += hight;

	float time = 1.0f;

	{
		for(int nCntParticle = 0; nCntParticle < MAX_ORBIT; nCntParticle++)
		{
			if(g_Orbit[nCntParticle].bUse)
			{// 使用中

				// 時間の初期化
				time = 1.0f;
				time /= MAX_ORBIT;
				time *= nCntParticle;

				// ベジェ曲線算出
				g_Orbit[nCntParticle].pos.x =	((1.0f - time) * (1.0f - time) * pPlayer->pos.x) + 
												(2 * time * (1.0f - time) * control.x) + 
												(time * time * AttackR->pos.x);

				g_Orbit[nCntParticle].pos.z = ((1.0f - time) * (1.0f - time) * pPlayer->pos.z) +
					(2 * time * (1.0f - time) * control.z) +
					(time * time * AttackR->pos.z);

				g_Orbit[nCntParticle].pos.y = ((1.0f - time) * (1.0f - time) * pPlayer->pos.y) +
					(2 * time * (1.0f - time) * control.y) +
					(time * time * AttackR->pos.y);

			}
		}
	}
}

//=============================================================================
// 描画処理
//=============================================================================
void DrawOrbit(void)
{
	XMMATRIX mtxScl, mtxTranslate, mtxWorld, mtxView;
	CAMERA *cam = GetCamera();

	// ライティングを無効に
	SetLightEnable(FALSE);

	// 加算合成に設定
	SetBlendState(BLEND_MODE_ADD);

	// Z比較無し
	SetDepthEnable(FALSE);

	// フォグ無効
	//SetFogEnable(FALSE);

	// 頂点バッファ設定
	UINT stride = sizeof(VERTEX_3D);
	UINT offset = 0;
	GetDeviceContext()->IASetVertexBuffers(0, 1, &g_VertexBuffer, &stride, &offset);

	// プリミティブトポロジ設定
	GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	// テクスチャ設定
	GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_TexNo]);

	for(int nCntParticle = 0; nCntParticle < MAX_ORBIT; nCntParticle++)
	{
		if(g_Orbit[nCntParticle].bUse)
		{
			// ワールドマトリックスの初期化
			mtxWorld = XMMatrixIdentity();

			// ビューマトリックスを取得
			mtxView = XMLoadFloat4x4(&cam->mtxView);

			// 処理が速いしお勧め
			mtxWorld.r[0].m128_f32[0] = mtxView.r[0].m128_f32[0];
			mtxWorld.r[0].m128_f32[1] = mtxView.r[1].m128_f32[0];
			mtxWorld.r[0].m128_f32[2] = mtxView.r[2].m128_f32[0];

			mtxWorld.r[1].m128_f32[0] = mtxView.r[0].m128_f32[1];
			mtxWorld.r[1].m128_f32[1] = mtxView.r[1].m128_f32[1];
			mtxWorld.r[1].m128_f32[2] = mtxView.r[2].m128_f32[1];

			mtxWorld.r[2].m128_f32[0] = mtxView.r[0].m128_f32[2];
			mtxWorld.r[2].m128_f32[1] = mtxView.r[1].m128_f32[2];
			mtxWorld.r[2].m128_f32[2] = mtxView.r[2].m128_f32[2];

			// スケールを反映
			mtxScl = XMMatrixScaling(g_Orbit[nCntParticle].scale.x, g_Orbit[nCntParticle].scale.y, g_Orbit[nCntParticle].scale.z);
			mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

			// 移動を反映
			mtxTranslate = XMMatrixTranslation(g_Orbit[nCntParticle].pos.x, g_Orbit[nCntParticle].pos.y, g_Orbit[nCntParticle].pos.z);
			mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

			// ワールドマトリックスの設定
			SetWorldMatrix(&mtxWorld);

			// マテリアル設定
			SetMaterial(g_Orbit[nCntParticle].material);

			// ポリゴンの描画
			GetDeviceContext()->Draw(4, 0);
		}
	}

	// ライティングを有効に
	SetLightEnable(TRUE);

	// 通常ブレンドに戻す
	SetBlendState(BLEND_MODE_ALPHABLEND);

	// Z比較有効
	SetDepthEnable(TRUE);

	// フォグ有効
	//SetFogEnable(TRUE);

}

//=============================================================================
// 頂点情報の作成
//=============================================================================
HRESULT MakeVertexOrbit(void)
{
	// 頂点バッファ生成
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.ByteWidth = sizeof(VERTEX_3D) * 4;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	GetDevice()->CreateBuffer(&bd, NULL, &g_VertexBuffer);

	{//頂点バッファの中身を埋める
		D3D11_MAPPED_SUBRESOURCE msr;
		GetDeviceContext()->Map(g_VertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &msr);

		VERTEX_3D* vertex = (VERTEX_3D*)msr.pData;

		// 頂点座標の設定
		vertex[0].Position = XMFLOAT3(-ORBIT_SIZE_X / 2,  ORBIT_SIZE_Y / 2, 0.0f);
		vertex[1].Position = XMFLOAT3( ORBIT_SIZE_X / 2,  ORBIT_SIZE_Y / 2, 0.0f);
		vertex[2].Position = XMFLOAT3(-ORBIT_SIZE_X / 2, -ORBIT_SIZE_Y / 2, 0.0f);
		vertex[3].Position = XMFLOAT3( ORBIT_SIZE_X / 2, -ORBIT_SIZE_Y / 2, 0.0f);

		// 法線の設定
		vertex[0].Normal = XMFLOAT3(0.0f, 0.0f, -1.0f);
		vertex[1].Normal = XMFLOAT3(0.0f, 0.0f, -1.0f);
		vertex[2].Normal = XMFLOAT3(0.0f, 0.0f, -1.0f);
		vertex[3].Normal = XMFLOAT3(0.0f, 0.0f, -1.0f);

		// 反射光の設定
		vertex[0].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
		vertex[1].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
		vertex[2].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
		vertex[3].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

		// テクスチャ座標の設定
		vertex[0].TexCoord = { 0.0f, 0.0f };
		vertex[1].TexCoord = { 1.0f, 0.0f };
		vertex[2].TexCoord = { 0.0f, 1.0f };
		vertex[3].TexCoord = { 1.0f, 1.0f };

		GetDeviceContext()->Unmap(g_VertexBuffer, 0);
	}

	return S_OK;
}

//=============================================================================
// マテリアルカラーの設定
//=============================================================================
void SetColorOrbit(int nIdxParticle, XMFLOAT4 col)
{
	g_Orbit[nIdxParticle].material.Diffuse = col;
}

//=============================================================================
// パーティクルの発生処理
//=============================================================================
int SetOrbit(XMFLOAT3 pos, XMFLOAT3 move, XMFLOAT4 col, float fSizeX, float fSizeY, int nLife)
{
	int nIdxParticle = -1;

	for(int nCntParticle = 0; nCntParticle < MAX_ORBIT; nCntParticle++)
	{
		if(!g_Orbit[nCntParticle].bUse)
		{
			g_Orbit[nCntParticle].pos = pos;
			g_Orbit[nCntParticle].rot   = { 0.0f, 0.0f, 0.0f };
			g_Orbit[nCntParticle].scale = { 1.0f, 1.0f, 1.0f };
			g_Orbit[nCntParticle].move = move;
			g_Orbit[nCntParticle].material.Diffuse = col;
			g_Orbit[nCntParticle].fSizeX = fSizeX;
			g_Orbit[nCntParticle].fSizeY = fSizeY;
			g_Orbit[nCntParticle].nLife = nLife;
			g_Orbit[nCntParticle].bUse = TRUE;

			nIdxParticle = nCntParticle;

#ifdef DISP_SHADOW
			// 影の設定
			g_Orbit[nCntParticle].nIdxShadow = CreateShadow(XMFLOAT3(pos.x, 0.1f, pos.z), 0.8f, 0.8f);		// 影の設定
			if(g_Orbit[nCntParticle].nIdxShadow != -1)
			{
				SetColorShadow(g_Orbit[nCntParticle].nIdxShadow, XMFLOAT4(1.0f, 1.0f, 1.0f, 0.5f));
			}
#endif

			break;
		}
	}

	return nIdxParticle;
}
