//=============================================================================
//
// メイン処理 [main.cpp]
// Author : 追加したぞおおお
//
//=============================================================================
#include "main.h"
#include "renderer.h"
#include "input.h"
#include "camera.h"
#include "debugproc.h"
#include "model.h"
#include "player.h"
#include "enemy.h"
#include "shadow.h"
#include "light.h"
#include "meshfield.h"
#include "meshwall.h"
#include "fieldobj.h"
#include "collision.h"
#include "bullet.h"
#include "score.h"
#include "sound.h"
#include "particle.h"

#include "title.h"
#include "game.h"
#include "result.h"
#include "fade.h"

#include "timeUI.h"

#include "logo.h"
#include "logoD.h"
#include "countdown.h"


//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define CLASS_NAME		"AppClass"			// ウインドウのクラス名
#define WINDOW_NAME		"メッシュ表示"		// ウインドウのキャプション名

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
HRESULT Init(HINSTANCE hInstance, HWND hWnd, BOOL bWindow);
void Uninit(void);
void Update(void);
void Draw(void);


//*****************************************************************************
// グローバル変数:
//*****************************************************************************
long g_MouseX = 0;
long g_MouseY = 0;


#ifdef _DEBUG
int		g_CountFPS;							// FPSカウンタ
char	g_DebugStr[2048] = WINDOW_NAME;		// デバッグ文字表示用

#endif

int	g_Mode = MODE_TITLE_LOGO;					// 起動時の画面を設定

int g_Stage = stage0;							// 現在のステージ

//=============================================================================
// メイン関数
//=============================================================================
int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);	// 無くても良いけど、警告が出る（未使用宣言）
	UNREFERENCED_PARAMETER(lpCmdLine);		// 無くても良いけど、警告が出る（未使用宣言）

	// 時間計測用
	DWORD dwExecLastTime;
	DWORD dwFPSLastTime;
	DWORD dwCurrentTime;
	DWORD dwFrameCount;

	WNDCLASSEX	wcex = {
		sizeof(WNDCLASSEX),
		CS_CLASSDC,
		WndProc,
		0,
		0,
		hInstance,
		NULL,
		LoadCursor(NULL, IDC_ARROW),
		(HBRUSH)(COLOR_WINDOW + 1),
		NULL,
		CLASS_NAME,
		NULL
	};
	HWND		hWnd;
	MSG			msg;
	
	// ウィンドウクラスの登録
	RegisterClassEx(&wcex);

	// ウィンドウの作成
	hWnd = CreateWindow(CLASS_NAME,
		WINDOW_NAME,
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,																		// ウィンドウの左座標
		CW_USEDEFAULT,																		// ウィンドウの上座標
		SCREEN_WIDTH + GetSystemMetrics(SM_CXDLGFRAME) * 2,									// ウィンドウ横幅
		SCREEN_HEIGHT + GetSystemMetrics(SM_CXDLGFRAME) * 2 + GetSystemMetrics(SM_CYCAPTION),	// ウィンドウ縦幅
		NULL,
		NULL,
		hInstance,
		NULL);

	// 初期化処理(ウィンドウを作成してから行う)
	if(FAILED(Init(hInstance, hWnd, TRUE)))
	{
		return -1;
	}

	// フレームカウント初期化
	timeBeginPeriod(1);	// 分解能を設定
	dwExecLastTime = dwFPSLastTime = timeGetTime();	// システム時刻をミリ秒単位で取得
	dwCurrentTime = dwFrameCount = 0;

	// ウインドウの表示(初期化処理の後に呼ばないと駄目)
	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);
	
	// メッセージループ
	while(1)
	{
		if(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if(msg.message == WM_QUIT)
			{// PostQuitMessage()が呼ばれたらループ終了
				break;
			}
			else
			{
				// メッセージの翻訳と送出
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
        }
		else
		{
			dwCurrentTime = timeGetTime();

			if ((dwCurrentTime - dwFPSLastTime) >= 1000)	// 1秒ごとに実行
			{
#ifdef _DEBUG
				g_CountFPS = dwFrameCount;
#endif
				dwFPSLastTime = dwCurrentTime;				// FPSを測定した時刻を保存
				dwFrameCount = 0;							// カウントをクリア
			}

			if ((dwCurrentTime - dwExecLastTime) >= (1000 / 60))	// 1/60秒ごとに実行
			{
				dwExecLastTime = dwCurrentTime;	// 処理した時刻を保存

#ifdef _DEBUG	// デバッグ版の時だけFPSを表示する
				wsprintf(g_DebugStr, WINDOW_NAME);
				wsprintf(&g_DebugStr[strlen(g_DebugStr)], " FPS:%d", g_CountFPS);
#endif

				Update();			// 更新処理
				Draw();				// 描画処理

#ifdef _DEBUG	// デバッグ版の時だけ表示する
				wsprintf(&g_DebugStr[strlen(g_DebugStr)], " MX:%d MY:%d", GetMousePosX(), GetMousePosY());
				SetWindowText(hWnd, g_DebugStr);
#endif

				dwFrameCount++;
			}
		}
	}

	timeEndPeriod(1);				// 分解能を戻す

	// ウィンドウクラスの登録を解除
	UnregisterClass(CLASS_NAME, wcex.hInstance);

	// 終了処理
	Uninit();

	return (int)msg.wParam;
}

//=============================================================================
// プロシージャ
//=============================================================================
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch(message)
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	case WM_KEYDOWN:
		switch(wParam)
		{
		case VK_ESCAPE:
			DestroyWindow(hWnd);
			break;
		}
		break;

	case WM_MOUSEMOVE:
		g_MouseX = LOWORD(lParam);
		g_MouseY = HIWORD(lParam);
		break;

	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}

	return 0;
}

//=============================================================================
// 初期化処理
//=============================================================================
HRESULT Init(HINSTANCE hInstance, HWND hWnd, BOOL bWindow)
{
	InitRenderer(hInstance, hWnd, bWindow);

	InitLight();

	InitCamera();

	// 入力処理の初期化
	InitInput(hInstance, hWnd);

	// サウンドの初期化
	InitSound(hWnd);


	// ライトを有効化
	SetLightEnable(TRUE);

	// 背面ポリゴンをカリング
	SetCullingMode(CULL_MODE_BACK);


	// フェードの初期化
	InitFade();

	// 最初のモードをセット
	SetMode(g_Mode);	// ここはSetModeのままで！

	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void Uninit(void)
{
	// 終了のモードをセット
	SetMode(MODE_MAX);


	// サウンド終了処理
	UninitSound();

	// カメラの終了処理
	UninitCamera();

	//入力の終了処理
	UninitInput();

	// レンダラーの終了処理
	UninitRenderer();
}

//=============================================================================
// 更新処理
//=============================================================================
void Update(void)
{
	// 入力の更新処理
	UpdateInput();

	// ライトの更新処理
	UpdateLight();

	// カメラ更新
	UpdateCamera();

	// モードによって処理を分ける
	switch (g_Mode)
	{
	case MODE_TITLE_LOGO:	// ロゴ画面の更新
		UpdateLogo();
		break;

	case MODE_TITLE_DirectX:// DirectXで作ってますアピ画面の更新
		UpdateLogoD();
		break;

	case MODE_TITLE:		// タイトル画面の更新
		UpdateTitle();
		break;

	case MODE_GAME:			// ゲーム画面の更新
		UpdateGame();
		break;

	case MODE_GAME_COUNT:	// 次のステージまでのカウント画面の更新
		UpdateCountDown();
		break;

	case MODE_RESULT:		// リザルト画面の更新
		UpdateResult();
		break;

	case MODE_ENDROLL:		// エンドロール画面の更新

		break;
	}

	// フェード処理の更新
	UpdateFade();


}

//=============================================================================
// 描画処理
//=============================================================================
void Draw(void)
{
	// バックバッファクリア
	Clear();

	SetCamera();

	// モードによって処理を分ける
	switch (g_Mode)
	{
	case MODE_TITLE_LOGO:	// ロゴ画面の描画
		// 2Dの物を描画する処理
		// Z比較なし
		SetDepthEnable(FALSE);

		// ライティングを無効
		SetLightEnable(FALSE);

		DrawLogo();

		// ライティングを有効に
		SetLightEnable(TRUE);

		// Z比較あり
		SetDepthEnable(TRUE);
		break;

	case MODE_TITLE_DirectX:// DirectXで作ってますアピ画面の描画
		// 2Dの物を描画する処理
		// Z比較なし
		SetDepthEnable(FALSE);

		// ライティングを無効
		SetLightEnable(FALSE);

		DrawLogoD();

		// ライティングを有効に
		SetLightEnable(TRUE);

		// Z比較あり
		SetDepthEnable(TRUE);
		break;

	case MODE_TITLE:		// タイトル画面の描画
		SetViewPort(TYPE_FULL_SCREEN);

		// 2Dの物を描画する処理
		// Z比較なし
		SetDepthEnable(FALSE);

		// ライティングを無効
		SetLightEnable(FALSE);

		DrawTitle();

		// ライティングを有効に
		SetLightEnable(TRUE);

		// Z比較あり
		SetDepthEnable(TRUE);
		break;

	case MODE_GAME:			// ゲーム画面の描画
		DrawGame();
		break;

	case MODE_GAME_COUNT:	// 次のステージまでのカウント画面の描画
		// 2Dの物を描画する処理
		// Z比較なし
		SetDepthEnable(FALSE);

		// ライティングを無効
		SetLightEnable(FALSE);

		DrawCountDown();

		// ライティングを有効に
		SetLightEnable(TRUE);

		// Z比較あり
		SetDepthEnable(TRUE);
		break;

	case MODE_RESULT:		// リザルト画面の描画
		SetViewPort(TYPE_FULL_SCREEN);

		// 2Dの物を描画する処理
		// Z比較なし
		SetDepthEnable(FALSE);

		// ライティングを無効
		SetLightEnable(FALSE);

		DrawResult();

		// ライティングを有効に
		SetLightEnable(TRUE);

		// Z比較あり
		SetDepthEnable(TRUE);
		break;

	case MODE_ENDROLL:		// エンドロール画面の描画
		// 2Dの物を描画する処理
		// Z比較なし
		SetDepthEnable(FALSE);


		// Z比較あり
		SetDepthEnable(TRUE);
		break;
	}

	// フェード描画
	DrawFade();

#ifdef _DEBUG
	// デバッグ表示
	DrawDebugProc();
#endif

	// バックバッファ、フロントバッファ入れ替え
	Present();
}


long GetMousePosX(void)
{
	return g_MouseX;
}


long GetMousePosY(void)
{
	return g_MouseY;
}


#ifdef _DEBUG
char* GetDebugStr(void)
{
	return g_DebugStr;
}
#endif



//=============================================================================
// モードの設定
//=============================================================================
void SetMode(int mode)
{
	// モードを変える前に全部メモリを解放しちゃう

	// ロゴ画面の終了処理
	UninitLogo();

	// DirectXで作ってますアピ画面の終了処理
	UninitLogoD();

	// タイトル画面の終了処理
	UninitTitle();

	// ゲーム画面の終了処理
	UninitGame();

	// 次のステージまでのカウント画面の終了処理
	UninitCountDown();

	// リザルト画面の終了処理
	UninitResult();


	g_Mode = mode;	// 次のモードをセットしている

	switch (g_Mode)
	{
	case MODE_TITLE_LOGO:	// ロゴ画面の初期化
		InitLogo();
		break;

	case MODE_TITLE_DirectX:// DirectXで作ってますアピ画面の初期化
		InitLogoD();
		break;

	case MODE_TITLE:
		// タイトル画面の初期化
		InitTitle();
		break;

	case MODE_GAME:
		// ゲーム画面の初期化
		InitGame();
		break;

	case MODE_GAME_COUNT:
		InitCountDown();
		break;

	case MODE_RESULT:
		// リザルト画面の初期化
		InitResult();
		break;

		// ゲーム終了時の処理
	case MODE_MAX:
		// エネミーの終了処理
		UninitEnemy();

		// プレイヤーの終了処理
		UninitPlayer();
		break;
	}
}

//=============================================================================
// モードの取得
//=============================================================================
int GetMode(void)
{
	return g_Mode;
}


//=============================================================================
// float型で負数にも対応しているランダムの値を返す関数
// digits:小数点以下の桁数(0.01f → 2), max:欲しい最大値, min:欲しい最小値
//=============================================================================
float RamdomFloat(int digits, float max, float min)
{
	// 小数点を消すための変数を作成
	int l_digits = 1;
	for (int i = 0; i < digits; i++)
	{
		l_digits *= 10;
	}

	// 剰余算するための値を作成
	float surplus = (max - min) * l_digits;

	int random = rand() % (int)surplus;

	float ans;

	// 整数のランダムの値をfloat型へキャストと負数の処理
	ans = (float)random / l_digits + min;

	return ans;
}



// 目標物との角度を調べる方法
//{
//	底辺 = enemyX - PlayerX
//	高さ = enemyY - PlayerY
//
//	angle = atan2(高さ, 底辺)
//}


int GetStage(void)
{
	return g_Stage;
}


void SetStage(int stage)
{
	g_Stage = stage;
}