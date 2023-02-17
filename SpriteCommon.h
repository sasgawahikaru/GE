#pragma once
#include <DirectXMath.h>
#include <d3dx12.h>
#include <wrl.h>
#include <array>
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

	void PostDraw();

    void LoadTexture(uint32_t index, const std::string& fileName);

    void SetTextureCommands(uint32_t index);

	DirectXCommon* GetDirectXCommon() { return dxCommon; }
private:
    // SRVの最大個数
    static const size_t kMaxSRVCount = 2056;

    static std::string kDefaultTextureDirectoryPath;
private:
	DirectXCommon* dxCommon;

	Microsoft::WRL::ComPtr<ID3D12PipelineState> pipelineState;
	Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature;

   // DirectX::XMFLOAT4* imageData;
    std::array<Microsoft::WRL::ComPtr<ID3D12Resource>, kMaxSRVCount> texBuff;
    Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> srvHeap;
};

