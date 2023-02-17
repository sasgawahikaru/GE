#include "Sprite.h"
#include "WinApp.h"

using namespace DirectX;
using namespace Microsoft::WRL;
void Sprite::Initialize(SpriteCommon* _spriteCommon)
{
    HRESULT result{};
    assert(_spriteCommon);
    spriteCommon = _spriteCommon;

    float left = (0.0f - anchorPoint.x) * size.x;
    float right = (1.0f - anchorPoint.x) * size.x;
    float top = (0.0f - anchorPoint.y) * size.y;
    float bottom = (1.0f - anchorPoint.y) * size.y;

    if (IsFlipX) {
        left = -left;
        right = -right;
    }
    if (IsFlipY)
    {
        top = -top;
        bottom = -bottom;
    }

    vertices[LB] = { { left, bottom,0.0f},{0.0f,1.0f} };
    vertices[LT] = { { left,   top,0.0f},{0.0f,0.0f} } ;
    vertices[RB] = { {right, bottom,0.0f},{1.0f,1.0f} };
    vertices[RT] = { {right,   top,0.0f},{1.0f,0.0f} };

    UINT sizeVB = static_cast<UINT>(sizeof(vertices[0]) * _countof(vertices));

    // 頂点バッファの設定
    D3D12_HEAP_PROPERTIES heapProp{};   // ヒープ設定
    heapProp.Type = D3D12_HEAP_TYPE_UPLOAD; // GPUへの転送用
    // リソース設定
    D3D12_RESOURCE_DESC resDesc{};
    resDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
    resDesc.Width = sizeVB; // 頂点データ全体のサイズ
    resDesc.Height = 1;
    resDesc.DepthOrArraySize = 1;
    resDesc.MipLevels = 1;
    resDesc.SampleDesc.Count = 1;
    resDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

    // 頂点バッファの生成
    result = spriteCommon->GetDirectXCommon()->GetDevice()->CreateCommittedResource(
        &heapProp, // ヒープ設定
        D3D12_HEAP_FLAG_NONE,
        &resDesc, // リソース設定
        D3D12_RESOURCE_STATE_GENERIC_READ,
        nullptr,
        IID_PPV_ARGS(&vertBuff));
    assert(SUCCEEDED(result));

    Vertex* vertMap = nullptr;
    result = vertBuff->Map(0, nullptr, (void**)&vertMap);
    assert(SUCCEEDED(result));

    for (int i = 0; i < _countof(vertices); i++) {
        vertMap[i] = vertices[i];
    }
    vertBuff->Unmap(0, nullptr);

    vbView.BufferLocation = vertBuff->GetGPUVirtualAddress();
    vbView.SizeInBytes = sizeVB;
    vbView.StrideInBytes = sizeof(vertices[0]);
    {
        // 頂点バッファの設定
        D3D12_HEAP_PROPERTIES heapProp{};   // ヒープ設定
        heapProp.Type = D3D12_HEAP_TYPE_UPLOAD; // GPUへの転送用
        // リソース設定
        D3D12_RESOURCE_DESC resDesc{};
        resDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
        resDesc.Width = (sizeof(ConstBufferDataMaterial) + 0xff) & ~0xff; // 頂点データ全体のサイズ
        resDesc.Height = 1;
        resDesc.DepthOrArraySize = 1;
        resDesc.MipLevels = 1;
        resDesc.SampleDesc.Count = 1;
        resDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

        // 頂点バッファの生成
        result = spriteCommon->GetDirectXCommon()->GetDevice()->CreateCommittedResource(
            &heapProp, // ヒープ設定
            D3D12_HEAP_FLAG_NONE,
            &resDesc, // リソース設定
            D3D12_RESOURCE_STATE_GENERIC_READ,
            nullptr,
            IID_PPV_ARGS(&constBuffMaterial));
        assert(SUCCEEDED(result));

        result = constBuffMaterial->Map(0, nullptr, (void**)&constMapMaterial);
        assert(SUCCEEDED(result));
        constMapMaterial->color = color;
        
    }


    {
        // 頂点バッファの設定
        D3D12_HEAP_PROPERTIES heapProp{};   // ヒープ設定
        heapProp.Type = D3D12_HEAP_TYPE_UPLOAD; // GPUへの転送用
        // リソース設定
        D3D12_RESOURCE_DESC resDesc{};
        resDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
        resDesc.Width = (sizeof(ConstBufferDataTransform) + 0xff) & ~0xff; // 頂点データ全体のサイズ
        resDesc.Height = 1;
        resDesc.DepthOrArraySize = 1;
        resDesc.MipLevels = 1;
        resDesc.SampleDesc.Count = 1;
        resDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

        // 頂点バッファの生成
        result = spriteCommon->GetDirectXCommon()->GetDevice()->CreateCommittedResource(
            &heapProp, // ヒープ設定
            D3D12_HEAP_FLAG_NONE,
            &resDesc, // リソース設定
            D3D12_RESOURCE_STATE_GENERIC_READ,
            nullptr,
            IID_PPV_ARGS(&constBuffTransform));
        assert(SUCCEEDED(result));

        result = constBuffTransform->Map(0, nullptr, (void**)&constMapTransform);
        assert(SUCCEEDED(result));

        XMMATRIX matWorld;
        matWorld = XMMatrixIdentity();

        XMMATRIX matRot;
        matRot = XMMatrixIdentity();
        matRot *= XMMatrixRotationZ(XMConvertToRadians(rotationZ));
        matWorld *= matRot;

        XMMATRIX matTrans;
        matTrans = XMMatrixTranslation(position.x, position.y, 0.f);
        matWorld *= matTrans;

        XMMATRIX matProjection = XMMatrixOrthographicOffCenterLH(
            0.f, WinApp::window_width,
            WinApp::window_height, 0.f,
            0.0f, 1.0f
        );

        constMapTransform->mat = matWorld * matProjection;
    }

}
void Sprite::Update()
{

    float left = (0.0f - anchorPoint.x) * size.x;
    float right = (1.0f - anchorPoint.x) * size.x;
    float top = (0.0f - anchorPoint.y) * size.y;
    float bottom = (1.0f - anchorPoint.y) * size.y;

    if (IsFlipX) {
        left = -left;
        right = -right;
    }
    if (IsFlipY)
    {
        top = -top;
        bottom = -bottom;
    }

    vertices[LB] = { { left, bottom,0.0f},{0.0f,1.0f} };
    vertices[LT] = { { left,   top,0.0f},{0.0f,0.0f} };
    vertices[RB] = { {right, bottom,0.0f},{1.0f,1.0f} };
    vertices[RT] = { {right,   top,0.0f},{1.0f,0.0f} };

    Vertex* vertMap = nullptr;
    HRESULT result = vertBuff->Map(0, nullptr, (void**)&vertMap);
    assert(SUCCEEDED(result));

    for (int i = 0; i < _countof(vertices); i++) {
        vertMap[i] = vertices[i];
    }
    vertBuff->Unmap(0, nullptr);

    constMapMaterial->color = color;

    XMMATRIX matWorld;
    matWorld = XMMatrixIdentity();

    XMMATRIX matRot;
    matRot = XMMatrixIdentity();
    matRot *= XMMatrixRotationZ(XMConvertToRadians(rotationZ));
    matWorld *= matRot;

    XMMATRIX matTrans;
    matTrans = XMMatrixTranslation(position.x, position.y, 0.f);
    matWorld *= matTrans;

    XMMATRIX matProjection = XMMatrixOrthographicOffCenterLH(
        0.f, WinApp::window_width,
        WinApp::window_height, 0.f,
        0.0f, 1.0f
    );

    constMapTransform->mat = matWorld * matProjection;
}
void Sprite::Draw()
{
    if (IsInvisible)
    {
        return;
    }

    spriteCommon->GetDirectXCommon()->GetCommandList()->IASetVertexBuffers(0, 1, &vbView);

	spriteCommon->GetDirectXCommon()->GetCommandList()->SetGraphicsRootConstantBufferView(0, constBuffMaterial->GetGPUVirtualAddress());

	spriteCommon->GetDirectXCommon()->GetCommandList()->SetGraphicsRootConstantBufferView(2, constBuffTransform->GetGPUVirtualAddress());

	spriteCommon->GetDirectXCommon()->GetCommandList()->DrawInstanced(4, 1, 0, 0);
}
