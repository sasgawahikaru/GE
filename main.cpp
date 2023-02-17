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
    spriteCommon->LoadTexture(0,"texture.png");
    spriteCommon->LoadTexture(1,"reimu.png");


#pragma endregion

#pragma region
    Sprite* sprite = nullptr;
    sprite = new Sprite();
    sprite->Initialize(spriteCommon);
    sprite->SetTextureIndex(1);

    //sprite->SetColor({ 0,1,1,1 });
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

        //DirectX::XMFLOAT2 size = sprite->GetSize();
        //size.y += 1.0f;
        //sprite->SetSize(size);
        sprite->Update();

#pragma endregion

        dxCommon->PreDraw();
#pragma region
        spriteCommon->PreDraw();
        sprite->Draw();
        spriteCommon->PostDraw();

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
