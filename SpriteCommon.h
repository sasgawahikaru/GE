#pragma once
#include <DirectXMath.h>
#include <d3dx12.h>
#include <wrl.h>
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

	DirectXCommon* GetDirectXCommon() { return dxCommon; }
private:
	DirectXCommon* dxCommon;

	Microsoft::WRL::ComPtr<ID3D12PipelineState> pipelineState;
	Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature;

    DirectX::XMFLOAT4* imageData;
    Microsoft::WRL::ComPtr<ID3D12Resource> texBuff;
    Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> srvHeap;
};

