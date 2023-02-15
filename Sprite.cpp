#include "Sprite.h"
#include "WinApp.h"

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
    {{  0.f, 100.f,0.0f},{0.0f,1.0f}},
    {{  0.f,   0.f,0.0f},{0.0f,0.0f}},
    {{100.f, 100.f,0.0f},{1.0f,1.0f}},
    {{100.f,   0.f,0.0f},{1.0f,0.0f}},
    };
    //verticesNum = _countof(vertices);
    //unsigned short indices[] = {
    //0,1,2,
    //1,2,3,
    //};
    UINT sizeVB = static_cast<UINT>(sizeof(vertices[0]) * _countof(vertices));

    // ���_�o�b�t�@�̐ݒ�
    D3D12_HEAP_PROPERTIES heapProp{};   // �q�[�v�ݒ�
    heapProp.Type = D3D12_HEAP_TYPE_UPLOAD; // GPU�ւ̓]���p
    // ���\�[�X�ݒ�
    D3D12_RESOURCE_DESC resDesc{};
    resDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
    resDesc.Width = sizeVB; // ���_�f�[�^�S�̂̃T�C�Y
    resDesc.Height = 1;
    resDesc.DepthOrArraySize = 1;
    resDesc.MipLevels = 1;
    resDesc.SampleDesc.Count = 1;
    resDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

    // ���_�o�b�t�@�̐���
    result = spriteCommon->GetDirectXCommon()->GetDevice()->CreateCommittedResource(
        &heapProp, // �q�[�v�ݒ�
        D3D12_HEAP_FLAG_NONE,
        &resDesc, // ���\�[�X�ݒ�
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
    {
        // ���_�o�b�t�@�̐ݒ�
        D3D12_HEAP_PROPERTIES heapProp{};   // �q�[�v�ݒ�
        heapProp.Type = D3D12_HEAP_TYPE_UPLOAD; // GPU�ւ̓]���p
        // ���\�[�X�ݒ�
        D3D12_RESOURCE_DESC resDesc{};
        resDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
        resDesc.Width = (sizeof(ConstBufferDataMaterial) + 0xff) & ~0xff; // ���_�f�[�^�S�̂̃T�C�Y
        resDesc.Height = 1;
        resDesc.DepthOrArraySize = 1;
        resDesc.MipLevels = 1;
        resDesc.SampleDesc.Count = 1;
        resDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

        // ���_�o�b�t�@�̐���
        result = spriteCommon->GetDirectXCommon()->GetDevice()->CreateCommittedResource(
            &heapProp, // �q�[�v�ݒ�
            D3D12_HEAP_FLAG_NONE,
            &resDesc, // ���\�[�X�ݒ�
            D3D12_RESOURCE_STATE_GENERIC_READ,
            nullptr,
            IID_PPV_ARGS(&constBuffMaterial));
        assert(SUCCEEDED(result));

        result = constBuffMaterial->Map(0, nullptr, (void**)&constMapMaterial);
        assert(SUCCEEDED(result));

        constMapMaterial->color = color;
    }


    {
        // ���_�o�b�t�@�̐ݒ�
        D3D12_HEAP_PROPERTIES heapProp{};   // �q�[�v�ݒ�
        heapProp.Type = D3D12_HEAP_TYPE_UPLOAD; // GPU�ւ̓]���p
        // ���\�[�X�ݒ�
        D3D12_RESOURCE_DESC resDesc{};
        resDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
        resDesc.Width = (sizeof(ConstBufferDataTransform) + 0xff) & ~0xff; // ���_�f�[�^�S�̂̃T�C�Y
        resDesc.Height = 1;
        resDesc.DepthOrArraySize = 1;
        resDesc.MipLevels = 1;
        resDesc.SampleDesc.Count = 1;
        resDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

        // ���_�o�b�t�@�̐���
        result = spriteCommon->GetDirectXCommon()->GetDevice()->CreateCommittedResource(
            &heapProp, // �q�[�v�ݒ�
            D3D12_HEAP_FLAG_NONE,
            &resDesc, // ���\�[�X�ݒ�
            D3D12_RESOURCE_STATE_GENERIC_READ,
            nullptr,
            IID_PPV_ARGS(&constBuffTransform));
        assert(SUCCEEDED(result));

        result = constBuffTransform->Map(0, nullptr, (void**)&constMapTransform);
        assert(SUCCEEDED(result));

        constMapTransform->mat = XMMatrixIdentity();
        constMapTransform->mat.r[0].m128_f32[0] = 2.0f / 1280.f;
        constMapTransform->mat.r[1].m128_f32[1] = -2.0f / 720.f;

        constMapTransform->mat.r[3].m128_f32[0] = -1.0f;
        constMapTransform->mat.r[3].m128_f32[1] = 1.0f;
    }

}
void Sprite::Draw()
{
    spriteCommon->GetDirectXCommon()->GetCommandList()->IASetVertexBuffers(0, 1, &vbView);

	spriteCommon->GetDirectXCommon()->GetCommandList()->SetGraphicsRootConstantBufferView(0, constBuffMaterial->GetGPUVirtualAddress());

	spriteCommon->GetDirectXCommon()->GetCommandList()->SetGraphicsRootConstantBufferView(2, constBuffTransform->GetGPUVirtualAddress());

	spriteCommon->GetDirectXCommon()->GetCommandList()->DrawInstanced(4, 1, 0, 0);
}
