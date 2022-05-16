//=============================================================================
//
// パーティクル処理 [particle.cpp]
// Author : 朱暁テイ
//
//=============================================================================
#include "main.h"
#include "renderer.h"
#include "input.h"
#include "camera.h"
#include "model.h"
#include "shadow.h"
#include "particle.h"
#include "player.h"
#include "customFunc.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define TEXTURE_MAX					(2)			// テクスチャの数
#define	MAX_EXPLOSION				(50)		// 爆破用のパーティクル
#define PARTICLE_BASE_SIZE			(10.0f)		// パーティクルのベースサイズ


//*****************************************************************************
// 構造体定義
//*****************************************************************************


//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
HRESULT MakeVertexParticle(void);
void SetExplosion(XMFLOAT3 pos, float pop);


//*****************************************************************************
// グローバル変数
//*****************************************************************************
static BOOL isLoad = FALSE;		// ロード済みフラグ

static ID3D11Buffer					*vertexBuffer = NULL;				// 頂点バッファ

static ID3D11ShaderResourceView		*textures[TEXTURE_MAX] = { NULL };	// テクスチャ情報
static char *textureNames[TEXTURE_MAX] =
{
	"data/TEXTURE/effect000.jpg",
	"data/TEXTURE/effect000.jpg",
};

static PARTICLE			particles[MAX_PARTICLE];		// パーティクル構造体
static PARTICLE_TYPE	types[PARTICLE_TYPE_MAX];		// パーティクルタイプ構造体


//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitParticle(void)
{
	// 頂点情報の作成
	MakeVertexParticle();

	// テクスチャ生成
	for (int i = 0; i < TEXTURE_MAX; i++)
	{
		textures[i] = NULL;
		D3DX11CreateShaderResourceViewFromFile(GetDevice(), textureNames[i], NULL, NULL, &textures[i], NULL);
	}

	// 構造体初期化
	for (int i = 0; i < MAX_PARTICLE; i++)
	{
		particles[i].pos = XMFLOAT3(0.0f, 0.0f, 0.0f);
		particles[i].rot = XMFLOAT3(0.0f, 0.0f, 0.0f);
		particles[i].scl = XMFLOAT3(1.0f, 1.0f, 1.0f);
		particles[i].spd = XMFLOAT3(1.0f, 1.0f, 1.0f);

		ZeroMemory(&particles[i].mat, sizeof(particles[i].mat));
		particles[i].mat.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

		particles[i].size = XMFLOAT2(PARTICLE_COLLISION_SIZE_X, PARTICLE_COLLISION_SIZE_Y);

		particles[i].life = 0;
		particles[i].inUse = FALSE;
	}

	// 爆発の初期化
	{
		types[PARTICLE_TYPE_EXPLOSION].texIdx = 1;

		types[PARTICLE_TYPE_EXPLOSION].life[0] = 45;
		types[PARTICLE_TYPE_EXPLOSION].life[1] = 50;

		types[PARTICLE_TYPE_EXPLOSION].cnt[0] = 5;
		types[PARTICLE_TYPE_EXPLOSION].cnt[1] = 10;

		types[PARTICLE_TYPE_EXPLOSION].sclX[0] = 0.5f;
		types[PARTICLE_TYPE_EXPLOSION].sclX[1] = 0.5f;

		types[PARTICLE_TYPE_EXPLOSION].sclY[0] = 0.5f;
		types[PARTICLE_TYPE_EXPLOSION].sclY[1] = 0.5f;

		types[PARTICLE_TYPE_EXPLOSION].sclZ[0] = 0.5f;
		types[PARTICLE_TYPE_EXPLOSION].sclZ[1] = 0.5f;

		types[PARTICLE_TYPE_EXPLOSION].spdX[0] = -0.8f;
		types[PARTICLE_TYPE_EXPLOSION].spdX[1] = 0.8f;

		types[PARTICLE_TYPE_EXPLOSION].spdY[0] = 0.5f;
		types[PARTICLE_TYPE_EXPLOSION].spdY[1] = 1.5f;

		types[PARTICLE_TYPE_EXPLOSION].spdZ[0] = -0.8f;
		types[PARTICLE_TYPE_EXPLOSION].spdZ[1] = 0.8f;

		types[PARTICLE_TYPE_EXPLOSION].pop[0] = 0.0f;
		types[PARTICLE_TYPE_EXPLOSION].pop[1] = 0.0f;

		types[PARTICLE_TYPE_EXPLOSION].col = XMFLOAT4(1.0f, 0.7f, 0.0f, 1.0f);
	}

	isLoad = TRUE;
	return S_OK;
}


//=============================================================================
// 終了処理
//=============================================================================
void UninitParticle(void)
{
	if (isLoad == FALSE) return;

	//テクスチャの解放
	for (int i = 0; i < TEXTURE_MAX; i++)
	{
		if (textures[i] != NULL)
		{
			textures[i]->Release();
			textures[i] = NULL;
		}
	}

	// 頂点バッファの解放
	if (vertexBuffer != NULL)
	{
		vertexBuffer->Release();
		vertexBuffer = NULL;
	}

	isLoad = FALSE;
}


//=============================================================================
// 更新処理
//=============================================================================
void UpdateParticle(void)
{
	for (int i = 0; i < MAX_PARTICLE; i++)
	{
		if (particles[i].inUse == FALSE) continue;

		// 移動処理
		particles[i].pos.x += particles[i].spd.x;
		particles[i].pos.y += particles[i].spd.y;
		particles[i].pos.z += particles[i].spd.z;

		// 寿命の処理
		particles[i].life--;
		if (particles[i].life <= 10)
		{	// 寿命が３０フレーム切ったら段々透明になっていく
			particles[i].mat.Diffuse.w -= 0.1f;
			if (particles[i].mat.Diffuse.w < 0.0f)
			{
				particles[i].mat.Diffuse.w = 0.0f;
			}
		}
		// 寿命切ったら消す
		if (particles[i].life <= 0)
		{
			particles[i].inUse = FALSE;
		}
		// 地面の下に行かせないように
		if (particles[i].pos.y < 0)
		{
			particles[i].pos.y = 0.0f;
			particles[i].life = 10;
			particles[i].spd = XMFLOAT3(0.0f, 0.0f, 0.0f);
		}


		// 表示タイミングの処理
		if (particles[i].pop > 0.0f)
		{
			particles[i].pop--;
		}
	}

	//爆破
	if (GetKeyboardTrigger(DIK_3))
	{
		PLAYER *player = GetPlayer();

		//XMFLOAT3 pos = player->pos;
		XMFLOAT3 pos = XMFLOAT3(0.0f, 0.0f, 100.0f);

		types[PARTICLE_TYPE_EXPLOSION].pop[0] = 0.0f;
		types[PARTICLE_TYPE_EXPLOSION].pop[1] = 0.0f;

		SetExplosion(pos, 0.0f);

	}
}


// 爆破の組立処理
void SetExplosion(XMFLOAT3 pos, float pop)
{
	//XMFLOAT3 temp = pos;
	XMFLOAT3 move = XMFLOAT3(0.0f, 0.0f, 0.0f);

	for (int i = 0; i < MAX_EXPLOSION; i++)
	{

		int col = rand() & 3;
		switch (col)
		{
		case 0:
			types[PARTICLE_TYPE_EXPLOSION].col = XMFLOAT4(0.7f, 0.4f, 0.0f, 1.0f);
			break;

		case 1:
			types[PARTICLE_TYPE_EXPLOSION].col = XMFLOAT4(0.4f, 0.2f, 0.0f, 1.0f);
			break;

		case 2:
			types[PARTICLE_TYPE_EXPLOSION].col = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
			break;
		}

		SetParticle(PARTICLE_TYPE_EXPLOSION, pos);

		move.x = RandomFloat(2, 2.0f, -2.0f);
		move.y = RandomFloat(2, 1.0f, 0.0f);
		move.z = RandomFloat(2, 2.0f, -2.0f);

		pos.x = pos.x + move.x;
		pos.y = pos.y + move.y;
		pos.z = pos.z + move.z;

		// タイミングをずらして表示させる
		types[PARTICLE_TYPE_EXPLOSION].pop[0] = pop + 0.1f * i;
		types[PARTICLE_TYPE_EXPLOSION].pop[1] = pop + 0.1f * i;
	}
}


//=============================================================================
// 描画処理
//=============================================================================
void DrawParticle(void)
{
	CAMERA *cam = GetCamera();
	XMMATRIX mtxScl, mtxTranslate, mtxWorld, mtxView;

	// ライティングを無効に
	SetLightEnable(false);

	// 加算合成に設定
	SetBlendState(BLEND_MODE_ADD);

	// Z比較無し
	SetDepthEnable(false);

	// フォグ無効
	SetFogEnable(false);

	int state = rand() % 4;

	// 頂点バッファ設定
	UINT stride = sizeof(VERTEX_3D);
	UINT offset = 0;
	GetDeviceContext()->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);

	// プリミティブトポロジ設定
	GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	for (int i = 0; i < MAX_PARTICLE; i++)
	{
		// ワールドマトリックスの初期化
		mtxWorld = XMMatrixIdentity();

		// ビューマトリックスを取得
		mtxView = XMLoadFloat4x4(&cam->mtxView);

		// ビルボード処理
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
		mtxScl = XMMatrixScaling(particles[i].scl.x, particles[i].scl.y, particles[i].scl.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

		// 移動を反映
		mtxTranslate = XMMatrixTranslation(particles[i].pos.x, particles[i].pos.y, particles[i].pos.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

		// ワールドマトリックスの設定
		SetWorldMatrix(&mtxWorld);

		// テクスチャ設定
		int texIndex = types[particles[i].type].texIdx;
		GetDeviceContext()->PSSetShaderResources(0, 1, &textures[texIndex]);

		// マテリアルを設定
		SetMaterial(particles[i].mat);

		// ポリゴンの描画
		GetDeviceContext()->Draw(4, 0);
	}


	// ライティングを有効に
	SetLightEnable(true);

	// 通常ブレンドに戻す
	SetBlendState(BLEND_MODE_ALPHABLEND);

	// Z比較有効
	SetDepthEnable(TRUE);

}


//=============================================================================
// 頂点情報の作成
//=============================================================================
HRESULT MakeVertexParticle(void)
{
	float a = PARTICLE_BASE_SIZE;

	VERTEX_3D vertexArray[4] =
	{
		{ XMFLOAT3(-5.0f, +5.0f, 0.0f),  XMFLOAT3(0.0f, 0.0f, -1.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), XMFLOAT2(0.0f, 0.0f) },
		{ XMFLOAT3(+5.0f, +5.0f, 0.0f),  XMFLOAT3(0.0f, 0.0f, -1.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), XMFLOAT2(1.0f, 0.0f) },
		{ XMFLOAT3(-5.0f, -5.0f, 0.0f),  XMFLOAT3(0.0f, 0.0f, -1.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), XMFLOAT2(0.0f, 1.0f) },
		{ XMFLOAT3(+5.0f, -5.0f, 0.0f),  XMFLOAT3(0.0f, 0.0f, -1.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), XMFLOAT2(1.0f, 1.0f) },
	};

	// 頂点バッファ生成
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(VERTEX_3D) * 4;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = 0;

	D3D11_SUBRESOURCE_DATA sd;
	ZeroMemory(&sd, sizeof(sd));
	sd.pSysMem = vertexArray;

	GetDevice()->CreateBuffer(&bd, &sd, &vertexBuffer);

	return S_OK;
}


//=============================================================================
// パーティクルの発生処理
//=============================================================================
void SetParticle(int type, XMFLOAT3 pos)
{
	// 生成するエフェクトを選択
	PARTICLE_TYPE *curType = &types[type];

	// 生成数を決定
	int cnt = RandomRange(curType->cnt[0], curType->cnt[1]);

	for (int i = 0; i < cnt; i++)
	{
		for (int j = 0; j < MAX_PARTICLE; j++)
		{
			// パーティクル生成
			if (particles[j].inUse == FALSE)
			{
				particles[j].type = type;

				particles[j].pos = pos;

				particles[j].life = RandomRange(curType->life[0], curType->life[1]);

				particles[j].scl.x = (float)(RandomRange((int)(curType->sclX[0] * 10), (int)(curType->sclX[1] * 10))) * 0.1f;
				particles[j].scl.y = (float)(RandomRange((int)(curType->sclY[0] * 10), (int)(curType->sclY[1] * 10))) * 0.1f;
				particles[j].scl.z = (float)(RandomRange((int)(curType->sclZ[0] * 10), (int)(curType->sclZ[1] * 10))) * 0.1f;

				particles[j].spd.x = (float)(RandomRange((int)(curType->spdX[0] * 10), (int)(curType->spdX[1] * 10))) * 0.1f;
				particles[j].spd.y = (float)(RandomRange((int)(curType->spdY[0] * 10), (int)(curType->spdY[1] * 10))) * 0.1f;
				particles[j].spd.z = (float)(RandomRange((int)(curType->spdZ[0] * 10), (int)(curType->spdZ[1] * 10))) * 0.1f;

				particles[j].pop = (float)(RandomRange((int)(curType->pop[0] * 10), (int)(curType->pop[1] * 10))) * 0.1f;

				particles[j].mat.Diffuse = curType->col;


				particles[j].inUse = TRUE;
				break;
			}
		}
	}
}


//=============================================================================
// パーティクル情報の取得
//=============================================================================
PARTICLE *GetParticle()
{
	return &particles[0];
}
