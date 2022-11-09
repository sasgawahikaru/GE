#include"DirectXCommon.h"
#include <cassert>
#include <vector>

#pragma comment(lib,"d3d12.lib")
#pragma comment(lib,"dxgi.lib")

using namespace Microsoft::WRL;

void DirectXCommon::Initialize()
{
    assert(winApp);
    this->winApp = winApp;
}
void DirectXCommon::InitializeDevice()
{
	HRESULT result;
	ComPtr<IDXGIFactory7>dxgiFactory;
#ifdef _DEBUG
    //デバッグレイヤーをオンに
    ComPtr<ID3D12Debug1> debugController;
    if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController))))
    {
        debugController->EnableDebugLayer();
        debugController->SetEnableGPUBasedValidation(TRUE);
    }
#endif

    // DXGIファクトリーの生成
    result = CreateDXGIFactory(IID_PPV_ARGS(&dxgiFactory));
    assert(SUCCEEDED(result));

    // アダプターの列挙用
    std::vector<ComPtr<IDXGIAdapter4>> adapters;
    // ここに特定の名前を持つアダプターオブジェクトが入る
    ComPtr<IDXGIAdapter4> tmpAdapter;

    // パフォーマンスが高いものから順に、全てのアダプターを列挙する
    for (UINT i = 0;
        dxgiFactory->EnumAdapterByGpuPreference(i, DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE, IID_PPV_ARGS(&tmpAdapter)) != DXGI_ERROR_NOT_FOUND;
        i++) {
        // 動的配列に追加する
        adapters.push_back(tmpAdapter);
    }

    // 妥当なアダプタを選別する
    for (size_t i = 0; i < adapters.size(); i++) {
        DXGI_ADAPTER_DESC3 adapterDesc;
        // アダプターの情報を取得する
        adapters[i]->GetDesc3(&adapterDesc);

        // ソフトウェアデバイスを回避
        if (!(adapterDesc.Flags & DXGI_ADAPTER_FLAG3_SOFTWARE)) {
            // デバイスを採用してループを抜ける
            tmpAdapter = adapters[i];
            break;
        }
    }

    // 対応レベルの配列
    D3D_FEATURE_LEVEL levels[] = {
        D3D_FEATURE_LEVEL_12_1,
        D3D_FEATURE_LEVEL_12_0,
        D3D_FEATURE_LEVEL_11_1,
        D3D_FEATURE_LEVEL_11_0,
    };

    D3D_FEATURE_LEVEL featureLevel;

    for (size_t i = 0; i < _countof(levels); i++) {
        // 採用したアダプターでデバイスを生成
        result = D3D12CreateDevice(tmpAdapter.Get(), levels[i], IID_PPV_ARGS(&device));
        if (result == S_OK) {
            // デバイスを生成できた時点でループを抜ける
            featureLevel = levels[i];
            break;
        }
    }

#ifdef _DEBUG
    Microsoft::WRL::ComPtr<ID3D12InfoQueue> infoQueue;
    if (SUCCEEDED(device->QueryInterface(IID_PPV_ARGS(&infoQueue)))) {
        // 抑制するエラー
        D3D12_MESSAGE_ID denyIds[] = {
            /*
             * Windows11でのDXGIデバッグレイヤーとDX12デバッグレイヤーの相互作用バグによるエラーメッセージ
             * https://stackoverflow.com/questions/69805245/directx-12-application-is-crashing-in-windows-11
             */
            D3D12_MESSAGE_ID_RESOURCE_BARRIER_MISMATCHING_COMMAND_LIST_TYPE };
        // 抑制する表示レベル
        D3D12_MESSAGE_SEVERITY severities[] = { D3D12_MESSAGE_SEVERITY_INFO };
        D3D12_INFO_QUEUE_FILTER filter{};
        filter.DenyList.NumIDs = _countof(denyIds);
        filter.DenyList.pIDList = denyIds;
        filter.DenyList.NumSeverities = _countof(severities);
        filter.DenyList.pSeverityList = severities;
        // 指定したエラーの表示を抑制する
        infoQueue->PushStorageFilter(&filter);
        // エラー時にブレークを発生させる
        infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_CORRUPTION, true);
        infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, true);
        infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_WARNING, true);
    }
}
void DirectXCommon::InitializeCommand()
{
    // コマンドアロケータを生成
    result = device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&commandAllocator));
    assert(SUCCEEDED(result));

    // コマンドリストを生成
    result = device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, commandAllocator.Get(), nullptr, IID_PPV_ARGS(&commandList));
    assert(SUCCEEDED(result));

    //コマンドキューの設定
    D3D12_COMMAND_QUEUE_DESC commandQueueDesc{};
}
void DirectXCommon::InitializeRenderTargetView()
{
    device->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(&rtvHeap));

    for (int i = 0; i < 2; i++)
    {
        device->CreateRenderTargetView(backBuffers[i].Get(), &rtvDesc, rtvHandle);
    }
}
void DirectXCommon::InitializeDepthBuffer()
{

}
void DirectXCommon::InitializeFence()
{

}

void DirectXCommon::PreDraw()
{

    // バックバッファの番号を取得（2つなので0番か1番）
    UINT bbIndex = swapchain->GetCurrentBackBufferIndex();

    cmdList->ResourceBarrier(1,
        &CD3DX12_RESOURCE_BARRIER::Transition(backBuffers[bbIndex].Get(),
            D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET));

    // １．リソースバリアで書き込み可能に変更
    D3D12_RESOURCE_BARRIER barrierDesc{};
    barrierDesc.Transition.pResource = backBuffers[bbIndex].Get(); // バックバッファを指定
    barrierDesc.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;      // 表示状態から
    barrierDesc.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET; // 描画状態へ
    commandList->ResourceBarrier(1, &barrierDesc);

    // ２．描画先の変更
    // レンダーターゲットビューのハンドルを取得
    D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = rtvHeap->GetCPUDescriptorHandleForHeapStart();
    rtvHandle.ptr += bbIndex * device->GetDescriptorHandleIncrementSize(rtvHeapDesc.Type);
    // 深度ステンシルビュー用デスクリプタヒープのハンドルを取得
    D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle = dsvHeap->GetCPUDescriptorHandleForHeapStart();
    commandList->OMSetRenderTargets(1, &rtvHandle, false, &dsvHandle);

    // ３．画面クリア           R     G     B    A
    FLOAT clearColor[] = { 0.1f,0.25f, 0.5f,0.0f }; // 青っぽい色
    commandList->ClearRenderTargetView(rtvHandle, clearColor, 0, nullptr);
    commandList->ClearDepthStencilView(dsvHandle, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);

    // ４．描画コマンドここから
    // ビューポート設定コマンド
    D3D12_VIEWPORT viewport{};
    viewport.Width = WinApp::window_width;
    viewport.Height = WinApp::window_height;
    viewport.TopLeftX = 0;
    viewport.TopLeftY = 0;
    viewport.MinDepth = 0.0f;
    viewport.MaxDepth = 1.0f;
    // ビューポート設定コマンドを、コマンドリストに積む
    commandList->RSSetViewports(1, &viewport);

    // シザー矩形
    D3D12_RECT scissorRect{};
    scissorRect.left = 0;                                       // 切り抜き座標左
    scissorRect.right = scissorRect.left + WinApp::window_width;        // 切り抜き座標右
    scissorRect.top = 0;                                        // 切り抜き座標上
    scissorRect.bottom = scissorRect.top + WinApp::window_height;       // 切り抜き座標下
    // シザー矩形設定コマンドを、コマンドリストに積む
    commandList->RSSetScissorRects(1, &scissorRect);

    

}

void DirectXCommon::PostDraw()
{

}