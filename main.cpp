
#include "Input.h"
#include "WinApp.h"
#include "DirectXCommon.h"

#include"SpriteCommon.h"
#include"Sprite.h"
// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

#pragma region WindowsAPI初期化処理

    WinApp* winApp = nullptr;
    winApp = new WinApp();
    winApp->Initialize();

    DirectXCommon* dxCommon = nullptr;
    dxCommon = new DirectXCommon();
    dxCommon->Initialize(winApp);

    Input* input = nullptr;
    input = new Input();
    input->Initialize(winApp);

    SpriteCommon* spriteCommon = nullptr;
    spriteCommon = new SpriteCommon();
    spriteCommon->Initialize(dxCommon);

#pragma endregion

#pragma region
    Sprite* sprite = nullptr;
    sprite = new Sprite();
    sprite->Initialize(spriteCommon);
#pragma endregion

    // ゲームループ
    while (true) {
#pragma region
        if (winApp->ProcessMessage() == true)
        {//ゲームループ
            break;
        }

        input->Update();
#pragma endregion

#pragma region
#pragma endregion

        dxCommon->PreDraw();
#pragma region
        spriteCommon->PreDraw();
        sprite->Draw();

#pragma endregion

        dxCommon->PostDraw();
    }
#pragma region
    delete sprite;
    sprite = nullptr;
#pragma endregion

#pragma region
    delete spriteCommon;
    spriteCommon = nullptr;

    delete input;
    input = nullptr;

    delete dxCommon;
    dxCommon = nullptr;

    winApp->Finalize();
    delete winApp;
    winApp = nullptr;
#pragma endregion
    return 0;
}
