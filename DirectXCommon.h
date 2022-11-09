#include <d3d12.h>
#include <dxgi1_6.h>
#include <wrl.h>
#include "WinApp.h"
#pragma once

class DirectXCommon
{
	void PreDraw();
	void PostDraw();

	void Initialize();
	void InitializeDevice();
	void InitializeCommand();
	void InitializeRenderTargetView();
	void InitializeDepthBuffer();
	void InitializeFence();

private:
	Microsoft::WRL::ComPtr < ID3D12Device>device;
	Microsoft::WRL::ComPtr <IDXGIFactory7> dxgiFactory;
	std::vector<Microsoft::WRL::ComPtr<ID3D12Resource>>backBuffers;
	WinApp* winApp = nullptr;
};