#pragma once
#include <DirectXMath.h>
#include <d3dx12.h>
#include <wrl.h>
#include <array>
#include "DirectXCommon.h"
class SpriteCommon
{
public:
    // �������s�N�Z����
    const size_t textureWidth = 256;
    // �c�����s�N�Z����
    const size_t textureHeight = 256;
    // �z��̗v�f��
    const size_t imageDataCount = textureWidth * textureHeight;
    // �摜�C���[�W�f�[�^�z��

public:
	void Initialize(DirectXCommon* _dxCommon);

	void PreDraw();

	void PostDraw();

    void LoadTexture(uint32_t index, const std::string& fileName);

    void SetTextureCommands(uint32_t index);

	DirectXCommon* GetDirectXCommon() { return dxCommon; }
private:
    // SRV�̍ő��
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

