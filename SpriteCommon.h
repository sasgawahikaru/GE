#pragma once
#include <DirectXMath.h>
#include <d3dx12.h>
#include <wrl.h>
#include "DirectXCommon.h"
class SpriteCommon
{
public:
    // 横方向ピクセル数
    const size_t textureWidth = 256;
    // 縦方向ピクセル数
    const size_t textureHeight = 256;
    // 配列の要素数
    const size_t imageDataCount = textureWidth * textureHeight;
    // 画像イメージデータ配列

public:
	void Initialize(DirectXCommon* _dxCommon);

	void PreDraw();

	DirectXCommon* GetDirectXCommon() { return dxCommon; }
private:
	DirectXCommon* dxCommon;

	Microsoft::WRL::ComPtr<ID3D12PipelineState> pipelineState;
	Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature;

    DirectX::XMFLOAT4* imageData;
    Microsoft::WRL::ComPtr<ID3D12Resource> texBuff;
    Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> srvHeap;
};

