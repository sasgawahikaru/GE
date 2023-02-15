#include "Sprite.h"
#include "WinApp.h"
#include<DirectXTex.h>

using namespace DirectX;
using namespace Microsoft::WRL;
void Sprite::Initialize(SpriteCommon* _spriteCommon)
{
    HRESULT result{};
    assert(_spriteCommon);
    spriteCommon = _spriteCommon;

    //XMFLOAT3 vertices[] = {
    //   {  -0.5f,-0.5f,0.0f},
    //   {  -0.5f,+0.5f,0.0f},
    //   {  +0.5f,-0.5f,0.0f},
    //   {  +0.5f,+0.5f,0.0f},
    //};
    Vertex vertices[] = {
    {{ -0.4f, -0.7f,0.0f},{0.0f,1.0f}},
    {{ -0.4f, +0.7f,0.0f},{0.0f,0.0f}},
    {{ +0.4f, -0.7f,0.0f},{1.0f,1.0f}},
    {{ +0.4f, +0.7f,0.0f},{1.0f,0.0f}},
    };
    //verticesNum = _countof(vertices);
    unsigned short indices[] = {
    0,1,2,
    1,2,3,
    };
    UINT sizeVB = static_cast<UINT>(sizeof(vertices[0]) * _countof(vertices));

	//// 横方向ピクセル数
	//const size_t textureWidth = 256;
	//// 縦方向ピクセル数
	//const size_t textureHeight = 256;
	//// 配列の要素数
	//const size_t imageDataCount = textureWidth * textureHeight;
	//// 画像イメージデータ配列
	//XMFLOAT4* imageData = new XMFLOAT4[imageDataCount]; // ※必ず後で解放する

	//// 全ピクセルの色を初期化
	//for (size_t i = 0; i < imageDataCount; i++) {
	//	imageData[i].x = 1.0f;    // R
	//	imageData[i].y = 0.0f;    // G
	//	imageData[i].z = 0.0f;    // B
	//	imageData[i].w = 1.0f;    // A
	//}

	//// ヒープ設定
	//D3D12_HEAP_PROPERTIES textureHeapProp{};
	//textureHeapProp.Type = D3D12_HEAP_TYPE_CUSTOM;
	//textureHeapProp.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_WRITE_BACK;
	//textureHeapProp.MemoryPoolPreference = D3D12_MEMORY_POOL_L0;
	//// リソース設定
	//D3D12_RESOURCE_DESC textureResourceDesc{};
	//textureResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D; textureResourceDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT; textureResourceDesc.Width = textureWidth;  // 幅
	//textureResourceDesc.Height = textureWidth; // 高さ
	//textureResourceDesc.DepthOrArraySize = 1;
	//textureResourceDesc.MipLevels = 1;
	//textureResourceDesc.SampleDesc.Count = 1;

	//// テクスチャバッファの生成
	//ID3D12Resource* texBuff = nullptr;
	//result = device->CreateCommittedResource(
	//	&textureHeapProp,
	//	D3D12_HEAP_FLAG_NONE,
	//	&textureResourceDesc,
	//	D3D12_RESOURCE_STATE_GENERIC_READ,
	//	nullptr,
	//	IID_PPV_ARGS(&texBuff));

	//// テクスチャバッファにデータ転送
	//result = texBuff->WriteToSubresource(
	//	0,
	//	nullptr, // 全領域へコピー
	//	imageData,    // 元データアドレス
	//	sizeof(XMFLOAT4) * textureWidth, // 1ラインサイズ
	//	sizeof(XMFLOAT4) * imageDataCount // 全サイズ
	//);

	//// 元データ解放
	//delete[] imageData;

	//// SRVの最大個数
	//const size_t kMaxSRVCount = 2056;

	//// デスクリプタヒープの設定
	//D3D12_DESCRIPTOR_HEAP_DESC srvHeapDesc = {};
	//srvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	//srvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;//シェーダから見えるように
	//srvHeapDesc.NumDescriptors = kMaxSRVCount;

	//// 設定を元にSRV用デスクリプタヒープを生成
	//ID3D12DescriptorHeap* srvHeap = nullptr;
	//result = device->CreateDescriptorHeap(&srvHeapDesc, IID_PPV_ARGS(&srvHeap));
	//assert(SUCCEEDED(result));

	////SRVヒープの先頭ハンドルを取得
	//D3D12_CPU_DESCRIPTOR_HANDLE srvHandle = srvHeap->GetCPUDescriptorHandleForHeapStart();

	//// シェーダリソースビュー設定
	//D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{}; // 設定構造体
	//srvDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;//RGBA float
	//srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	//srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;//2Dテクスチャ
	//srvDesc.Texture2D.MipLevels = 1;

	//// ハンドルの指す位置にシェーダーリソースビュー作成
	//device->CreateShaderResourceView(texBuff, &srvDesc, srvHandle);

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

}
void Sprite::Draw()
{
    spriteCommon->GetDirectXCommon()->GetCommandList()->IASetVertexBuffers(0, 1, &vbView);

	//spriteCommon->GetDirectXCommon()->GetCommandList()->SetGraphicsRootConstantBufferView(0, constBuffMaterial->GetGPUVirtualAddress());

	spriteCommon->GetDirectXCommon()->GetCommandList()->DrawInstanced(4, 1, 0, 0);
}
