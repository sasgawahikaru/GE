#pragma once
#include <d3d12.h>
#include <dxgi1_6.h>
#include <wrl.h>
#include "WinApp.h"
#include <vector>

class DirectXCommon
{
	//void PreDraw();
	//void PostDraw();

	void Initialize(WinApp* winApp);
	

private:
	void InitializeDevice();

	void InitializeCommand();
	
	void InitializeSwapchain();

	void InitializeRenderTargetView();

	void InitializeDepthBuffer();

	void InitializeFence();
private:
	WinApp* winApp = nullptr;
	
	//デバイス
	Microsoft::WRL::ComPtr<ID3D12Device> device;
	Microsoft::WRL::ComPtr<IDXGIFactory7> dxgiFactory;
	//コマンド
	Microsoft::WRL::ComPtr<ID3D12CommandAllocator> commandAllocator;
	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> commandList;
	Microsoft::WRL::ComPtr<ID3D12CommandQueue> commandQueue;

	Microsoft::WRL::ComPtr<IDXGISwapChain4> swapChain;
	DXGI_SWAP_CHAIN_DESC1 swapChainDesc{};

	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> rtvHeap;
	std::vector<Microsoft::WRL::ComPtr<ID3D12Resource>>backBuffers;

	Microsoft::WRL::ComPtr<ID3D12Resource> depthBuff;
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> dsvHeap;

	Microsoft::WRL::ComPtr<ID3D12Fence> fence;
};