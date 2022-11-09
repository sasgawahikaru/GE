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
    //�f�o�b�O���C���[���I����
    ComPtr<ID3D12Debug1> debugController;
    if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController))))
    {
        debugController->EnableDebugLayer();
        debugController->SetEnableGPUBasedValidation(TRUE);
    }
#endif

    // DXGI�t�@�N�g���[�̐���
    result = CreateDXGIFactory(IID_PPV_ARGS(&dxgiFactory));
    assert(SUCCEEDED(result));

    // �A�_�v�^�[�̗񋓗p
    std::vector<ComPtr<IDXGIAdapter4>> adapters;
    // �����ɓ���̖��O�����A�_�v�^�[�I�u�W�F�N�g������
    ComPtr<IDXGIAdapter4> tmpAdapter;

    // �p�t�H�[�}���X���������̂��珇�ɁA�S�ẴA�_�v�^�[��񋓂���
    for (UINT i = 0;
        dxgiFactory->EnumAdapterByGpuPreference(i, DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE, IID_PPV_ARGS(&tmpAdapter)) != DXGI_ERROR_NOT_FOUND;
        i++) {
        // ���I�z��ɒǉ�����
        adapters.push_back(tmpAdapter);
    }

    // �Ó��ȃA�_�v�^��I�ʂ���
    for (size_t i = 0; i < adapters.size(); i++) {
        DXGI_ADAPTER_DESC3 adapterDesc;
        // �A�_�v�^�[�̏����擾����
        adapters[i]->GetDesc3(&adapterDesc);

        // �\�t�g�E�F�A�f�o�C�X�����
        if (!(adapterDesc.Flags & DXGI_ADAPTER_FLAG3_SOFTWARE)) {
            // �f�o�C�X���̗p���ă��[�v�𔲂���
            tmpAdapter = adapters[i];
            break;
        }
    }

    // �Ή����x���̔z��
    D3D_FEATURE_LEVEL levels[] = {
        D3D_FEATURE_LEVEL_12_1,
        D3D_FEATURE_LEVEL_12_0,
        D3D_FEATURE_LEVEL_11_1,
        D3D_FEATURE_LEVEL_11_0,
    };

    D3D_FEATURE_LEVEL featureLevel;

    for (size_t i = 0; i < _countof(levels); i++) {
        // �̗p�����A�_�v�^�[�Ńf�o�C�X�𐶐�
        result = D3D12CreateDevice(tmpAdapter.Get(), levels[i], IID_PPV_ARGS(&device));
        if (result == S_OK) {
            // �f�o�C�X�𐶐��ł������_�Ń��[�v�𔲂���
            featureLevel = levels[i];
            break;
        }
    }

#ifdef _DEBUG
    Microsoft::WRL::ComPtr<ID3D12InfoQueue> infoQueue;
    if (SUCCEEDED(device->QueryInterface(IID_PPV_ARGS(&infoQueue)))) {
        // �}������G���[
        D3D12_MESSAGE_ID denyIds[] = {
            /*
             * Windows11�ł�DXGI�f�o�b�O���C���[��DX12�f�o�b�O���C���[�̑��ݍ�p�o�O�ɂ��G���[���b�Z�[�W
             * https://stackoverflow.com/questions/69805245/directx-12-application-is-crashing-in-windows-11
             */
            D3D12_MESSAGE_ID_RESOURCE_BARRIER_MISMATCHING_COMMAND_LIST_TYPE };
        // �}������\�����x��
        D3D12_MESSAGE_SEVERITY severities[] = { D3D12_MESSAGE_SEVERITY_INFO };
        D3D12_INFO_QUEUE_FILTER filter{};
        filter.DenyList.NumIDs = _countof(denyIds);
        filter.DenyList.pIDList = denyIds;
        filter.DenyList.NumSeverities = _countof(severities);
        filter.DenyList.pSeverityList = severities;
        // �w�肵���G���[�̕\����}������
        infoQueue->PushStorageFilter(&filter);
        // �G���[���Ƀu���[�N�𔭐�������
        infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_CORRUPTION, true);
        infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, true);
        infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_WARNING, true);
    }
}
void DirectXCommon::InitializeCommand()
{
    // �R�}���h�A���P�[�^�𐶐�
    result = device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&commandAllocator));
    assert(SUCCEEDED(result));

    // �R�}���h���X�g�𐶐�
    result = device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, commandAllocator.Get(), nullptr, IID_PPV_ARGS(&commandList));
    assert(SUCCEEDED(result));

    //�R�}���h�L���[�̐ݒ�
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

    // �o�b�N�o�b�t�@�̔ԍ����擾�i2�Ȃ̂�0�Ԃ�1�ԁj
    UINT bbIndex = swapchain->GetCurrentBackBufferIndex();

    cmdList->ResourceBarrier(1,
        &CD3DX12_RESOURCE_BARRIER::Transition(backBuffers[bbIndex].Get(),
            D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET));

    // �P�D���\�[�X�o���A�ŏ������݉\�ɕύX
    D3D12_RESOURCE_BARRIER barrierDesc{};
    barrierDesc.Transition.pResource = backBuffers[bbIndex].Get(); // �o�b�N�o�b�t�@���w��
    barrierDesc.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;      // �\����Ԃ���
    barrierDesc.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET; // �`���Ԃ�
    commandList->ResourceBarrier(1, &barrierDesc);

    // �Q�D�`���̕ύX
    // �����_�[�^�[�Q�b�g�r���[�̃n���h�����擾
    D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = rtvHeap->GetCPUDescriptorHandleForHeapStart();
    rtvHandle.ptr += bbIndex * device->GetDescriptorHandleIncrementSize(rtvHeapDesc.Type);
    // �[�x�X�e���V���r���[�p�f�X�N���v�^�q�[�v�̃n���h�����擾
    D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle = dsvHeap->GetCPUDescriptorHandleForHeapStart();
    commandList->OMSetRenderTargets(1, &rtvHandle, false, &dsvHandle);

    // �R�D��ʃN���A           R     G     B    A
    FLOAT clearColor[] = { 0.1f,0.25f, 0.5f,0.0f }; // ���ۂ��F
    commandList->ClearRenderTargetView(rtvHandle, clearColor, 0, nullptr);
    commandList->ClearDepthStencilView(dsvHandle, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);

    // �S�D�`��R�}���h��������
    // �r���[�|�[�g�ݒ�R�}���h
    D3D12_VIEWPORT viewport{};
    viewport.Width = WinApp::window_width;
    viewport.Height = WinApp::window_height;
    viewport.TopLeftX = 0;
    viewport.TopLeftY = 0;
    viewport.MinDepth = 0.0f;
    viewport.MaxDepth = 1.0f;
    // �r���[�|�[�g�ݒ�R�}���h���A�R�}���h���X�g�ɐς�
    commandList->RSSetViewports(1, &viewport);

    // �V�U�[��`
    D3D12_RECT scissorRect{};
    scissorRect.left = 0;                                       // �؂蔲�����W��
    scissorRect.right = scissorRect.left + WinApp::window_width;        // �؂蔲�����W�E
    scissorRect.top = 0;                                        // �؂蔲�����W��
    scissorRect.bottom = scissorRect.top + WinApp::window_height;       // �؂蔲�����W��
    // �V�U�[��`�ݒ�R�}���h���A�R�}���h���X�g�ɐς�
    commandList->RSSetScissorRects(1, &scissorRect);

    

}

void DirectXCommon::PostDraw()
{

}