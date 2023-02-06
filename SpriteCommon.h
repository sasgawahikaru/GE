#pragma once
#include <DirectXMath.h>
#include <d3dx12.h>
#include <wrl.h>
#include "DirectXCommon.h"
class SpriteCommon
{
public:
	void Initialize(DirectXCommon* _dxCommon);

	void PreDraw();

	DirectXCommon* GetDirectXCommon() { return dxCommon; }
private:
	DirectXCommon* dxCommon;

	Microsoft::WRL::ComPtr<ID3D12PipelineState> pipelineState;
	Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature;
};

