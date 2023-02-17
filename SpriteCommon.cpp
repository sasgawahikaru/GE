#include "SpriteCommon.h"
#include "Sprite.h"
#include <DirectXTex.h>
#include <d3dcompiler.h>
#include <cassert>
#include <d3dx12.h>

#pragma comment(lib,"d3dcompiler.lib")

using namespace DirectX;
using namespace Microsoft::WRL;

std::string SpriteCommon::kDefaultTextureDirectoryPath = "Resources/";

void SpriteCommon::Initialize(DirectXCommon* _dxCommon)
{
 //   ID3D12GraphicsCommandList* commandList = nullptr;
 //   ID3D12Resource* constBuffMaterial = nullptr;
    HRESULT result{};
    assert(_dxCommon);
    dxCommon = _dxCommon;


    // デスクリプタヒープの設定
    D3D12_DESCRIPTOR_HEAP_DESC srvHeapDesc = {};
    srvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
    srvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;//シェーダから見えるように
    srvHeapDesc.NumDescriptors = kMaxSRVCount;

    // 設定を元にSRV用デスクリプタヒープを生成
    result = dxCommon->GetDevice()->CreateDescriptorHeap(&srvHeapDesc, IID_PPV_ARGS(&srvHeap));
    assert(SUCCEEDED(result));

    ComPtr<ID3DBlob> vsBlob;
    ComPtr<ID3DBlob> psBlob;
    ComPtr<ID3DBlob> errorBlob;

    // 頂点シェーダの読み込みとコンパイル
    result = D3DCompileFromFile(
        L"Resources/shaders/SpriteVS.hlsl",  // シェーダファイル名
        nullptr,
        D3D_COMPILE_STANDARD_FILE_INCLUDE, // インクルード可能にする
        "main", "vs_5_0", // エントリーポイント名、シェーダーモデル指定
        D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, // デバッグ用設定
        0,
        &vsBlob, &errorBlob);
    // エラーなら
    if (FAILED(result)) {
        // errorBlobからエラー内容をstring型にコピー
        std::string error;
        error.resize(errorBlob->GetBufferSize());

        std::copy_n((char*)errorBlob->GetBufferPointer(),
            errorBlob->GetBufferSize(),
            error.begin());
        error += "\n";
        // エラー内容を出力ウィンドウに表示
        OutputDebugStringA(error.c_str());
        assert(0);
    }

    // ピクセルシェーダの読み込みとコンパイル
    result = D3DCompileFromFile(
        L"Resources/shaders/SpritePS.hlsl",   // シェーダファイル名
        nullptr,
        D3D_COMPILE_STANDARD_FILE_INCLUDE, // インクルード可能にする
        "main", "ps_5_0", // エントリーポイント名、シェーダーモデル指定
        D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, // デバッグ用設定
        0,
        &psBlob, &errorBlob);
    // エラーなら
    if (FAILED(result)) {
        // errorBlobからエラー内容をstring型にコピー
        std::string error;
        error.resize(errorBlob->GetBufferSize());

        std::copy_n((char*)errorBlob->GetBufferPointer(),
            errorBlob->GetBufferSize(),
            error.begin());
        error += "\n";
        // エラー内容を出力ウィンドウに表示
        OutputDebugStringA(error.c_str());
        assert(0);
    }
    // 頂点レイアウト
    D3D12_INPUT_ELEMENT_DESC inputLayout[] = {
        { // xyz座標(1行で書いたほうが見やすい)
            "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,
            D3D12_APPEND_ALIGNED_ELEMENT,
            D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
        },
        {
            "TEXCOORD",0,DXGI_FORMAT_R32G32_FLOAT,0,
            D3D12_APPEND_ALIGNED_ELEMENT,
            D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0
        },
    };
    // グラフィックスパイプライン設定
    D3D12_GRAPHICS_PIPELINE_STATE_DESC pipelineDesc{};
    // シェーダーの設定
    pipelineDesc.VS.pShaderBytecode = vsBlob->GetBufferPointer();
    pipelineDesc.VS.BytecodeLength = vsBlob->GetBufferSize();
    pipelineDesc.PS.pShaderBytecode = psBlob->GetBufferPointer();
    pipelineDesc.PS.BytecodeLength = psBlob->GetBufferSize();
    // サンプルマスクの設定
    pipelineDesc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK; // 標準設定
    // ラスタライザの設定
    pipelineDesc.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;  // カリングしない
    pipelineDesc.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID; // ポリゴン内塗りつぶし
    pipelineDesc.RasterizerState.DepthClipEnable = true; // 深度クリッピングを有効に
    // ブレンドステート
    //pipelineDesc.BlendState.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;  // RBGA全てのチャンネルを描画
    D3D12_RENDER_TARGET_BLEND_DESC& blenddesc = pipelineDesc.BlendState.RenderTarget[0];
    blenddesc.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

    blenddesc.BlendEnable = true;
    blenddesc.BlendOpAlpha = D3D12_BLEND_OP_ADD;
    blenddesc.SrcBlendAlpha = D3D12_BLEND_ONE;
    blenddesc.DestBlendAlpha = D3D12_BLEND_ZERO;

 /* blenddesc.BlendOp = D3D12_BLEND_OP_ADD;
    blenddesc.SrcBlend = D3D12_BLEND_ONE;
    blenddesc.DestBlend = D3D12_BLEND_ONE;*/

    // 半透明合成
    blenddesc.BlendOp = D3D12_BLEND_OP_ADD;             // 加算
    blenddesc.SrcBlend = D3D12_BLEND_SRC_ALPHA;         // ソースのアルファ値
    blenddesc.DestBlend = D3D12_BLEND_INV_SRC_ALPHA;    // 1.0f-ソースのアルファ値

    pipelineDesc.InputLayout.pInputElementDescs = inputLayout;
    pipelineDesc.InputLayout.NumElements = _countof(inputLayout);

    pipelineDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
    pipelineDesc.NumRenderTargets = 1;
    pipelineDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
    pipelineDesc.SampleDesc.Count = 1;

    // デスクリプタレンジの設定
    D3D12_DESCRIPTOR_RANGE descriptorRange{};
    descriptorRange.NumDescriptors = 1;         //一度の描画に使うテクスチャが1枚なので1
    descriptorRange.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
    descriptorRange.BaseShaderRegister = 0;     //テクスチャレジスタ0番
    descriptorRange.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

    // ルートパラメータの設定
    D3D12_ROOT_PARAMETER rootParam[3] = {};
    // 定数バッファ0番
    rootParam[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;   // 種類
    rootParam[0].Descriptor.ShaderRegister = 0;                   // 定数バッファ番号
    rootParam[0].Descriptor.RegisterSpace = 0;                    // デフォルト値
    rootParam[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;  // 全てのシェーダから見える
    // テクスチャレジスタ0番
    rootParam[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;   //種類
    rootParam[1].DescriptorTable.pDescriptorRanges = &descriptorRange;		  //デスクリプタレンジ
    rootParam[1].DescriptorTable.NumDescriptorRanges = 1;              		  //デスクリプタレンジ数
    rootParam[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

    rootParam[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;   //種類
    rootParam[2].Descriptor.ShaderRegister = 1;    	  //デスクリプタレンジ
    rootParam[2].Descriptor.RegisterSpace = 0;             		  //デスクリプタレンジ数
    rootParam[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
    
    // テクスチャサンプラーの設定
    D3D12_STATIC_SAMPLER_DESC samplerDesc{};
    samplerDesc.AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;                 //横繰り返し（タイリング）
    samplerDesc.AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;                 //縦繰り返し（タイリング）
    samplerDesc.AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;                 //奥行繰り返し（タイリング）
    samplerDesc.BorderColor = D3D12_STATIC_BORDER_COLOR_TRANSPARENT_BLACK;  //ボーダーの時は黒
    samplerDesc.Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;                   //全てリニア補間
    samplerDesc.MaxLOD = D3D12_FLOAT32_MAX;                                 //ミップマップ最大値
    samplerDesc.MinLOD = 0.f;                                              //ミップマップ最小値
    samplerDesc.ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
    samplerDesc.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;           //ピクセルシェーダからのみ使用可能
    
    // ルートシグネチャの設定
    D3D12_ROOT_SIGNATURE_DESC rootSignatureDesc{};
    rootSignatureDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
    rootSignatureDesc.pParameters = rootParam; //ルートパラメータの先頭アドレス
    rootSignatureDesc.NumParameters = _countof(rootParam);        //ルートパラメータ数
    rootSignatureDesc.pStaticSamplers = &samplerDesc;
    rootSignatureDesc.NumStaticSamplers = 1;
    // ルートシグネチャのシリアライズ
    ID3DBlob* rootSigBlob = nullptr;
    result = D3D12SerializeRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1_0, &rootSigBlob, &errorBlob);
    assert(SUCCEEDED(result));
    result = dxCommon->GetDevice()->CreateRootSignature(0, rootSigBlob->GetBufferPointer(), rootSigBlob->GetBufferSize(), IID_PPV_ARGS(&rootSignature));
    assert(SUCCEEDED(result));
    rootSigBlob->Release();
    // パイプラインにルートシグネチャをセット
    pipelineDesc.pRootSignature = rootSignature.Get();
    // パイプランステートの生成
    result = dxCommon->GetDevice()->CreateGraphicsPipelineState(&pipelineDesc, IID_PPV_ARGS(&pipelineState));
    assert(SUCCEEDED(result));


}
void SpriteCommon::PreDraw()
{
    dxCommon->GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

    dxCommon->GetCommandList()->SetPipelineState(pipelineState.Get());
    dxCommon->GetCommandList()->SetGraphicsRootSignature(rootSignature.Get());

    //// 定数バッファビュー(CBV)の設定コマンド
    ID3D12DescriptorHeap* ppHeaps[] = { srvHeap.Get() };
    //// SRVヒープの設定コマンド
    dxCommon->GetCommandList()->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);
}

void SpriteCommon::PostDraw()
{

}

void SpriteCommon::LoadTexture(uint32_t index, const std::string& fileName)
{

    std::string fullPath = kDefaultTextureDirectoryPath + fileName;
    
    int filePathBufferSize = MultiByteToWideChar(CP_ACP, 0, fullPath.c_str(), -1, nullptr, 0);
    
    std::vector<wchar_t> wfilePath(filePathBufferSize);
    MultiByteToWideChar(CP_ACP, 0, fullPath.c_str(), -1, wfilePath.data(), filePathBufferSize);

    TexMetadata metadata{};
    ScratchImage scrachImg{};

   HRESULT result = LoadFromWICFile(
       wfilePath.data(),
        WIC_FLAGS_NONE,
        &metadata, scrachImg
    );
    assert(SUCCEEDED(result));

    ScratchImage mipChain{};
    result = GenerateMipMaps(
        scrachImg.GetImages(), scrachImg.GetImageCount(), scrachImg.GetMetadata(),
        TEX_FILTER_DEFAULT, 0, mipChain);
    if (SUCCEEDED(result))
    {
        scrachImg = std::move(mipChain);
        metadata = scrachImg.GetMetadata();
    }

    metadata.format = MakeSRGB(metadata.format);

    // ヒープ設定
    D3D12_HEAP_PROPERTIES textureHeapProp{};
    textureHeapProp.Type = D3D12_HEAP_TYPE_CUSTOM;
    textureHeapProp.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_WRITE_BACK;
    textureHeapProp.MemoryPoolPreference = D3D12_MEMORY_POOL_L0;
    // リソース設定
    D3D12_RESOURCE_DESC textureResourceDesc{};
    textureResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
    textureResourceDesc.Format = metadata.format;
    textureResourceDesc.Width = metadata.width;  // 幅
    textureResourceDesc.Height = (UINT)metadata.height; // 高さ
    textureResourceDesc.DepthOrArraySize = (UINT16)metadata.arraySize;
    textureResourceDesc.MipLevels = (UINT16)metadata.mipLevels;
    textureResourceDesc.SampleDesc.Count = 1;

    // テクスチャバッファの生成
    result = dxCommon->GetDevice()->CreateCommittedResource(
        &textureHeapProp,
        D3D12_HEAP_FLAG_NONE,
        &textureResourceDesc,
        D3D12_RESOURCE_STATE_GENERIC_READ,
        nullptr,
        IID_PPV_ARGS(&texBuff[index]));

    // テクスチャバッファにデータ転送
    for (size_t i = 0; i < metadata.mipLevels; i++)
    {
        const Image* img = scrachImg.GetImage(i, 0, 0);

        result = texBuff[index]->WriteToSubresource(
            (UINT)i,
            nullptr, // 全領域へコピー
            img->pixels,    // 元データアドレス
            (UINT)img->rowPitch, // 1ラインサイズ
            (UINT)img->slicePitch // 全サイズ
        );
        assert(SUCCEEDED(result));
    }
    //SRVヒープの先頭ハンドルを取得
    D3D12_CPU_DESCRIPTOR_HANDLE srvHandle = srvHeap->GetCPUDescriptorHandleForHeapStart();

    // シェーダリソースビュー設定
    D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{}; // 設定構造体
    srvDesc.Format = textureResourceDesc.Format;//RGBA float
    srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
    srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;//2Dテクスチャ
    srvDesc.Texture2D.MipLevels = textureResourceDesc.MipLevels;

    // ハンドルの指す位置にシェーダーリソースビュー作成
    //dxCommon->GetDevice()->CreateShaderResourceView(texBuff[index].Get(), &srvDesc, srvHandle);

    UINT descriptorhandleIncrementSize = dxCommon->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
    dxCommon->GetDevice()->CreateShaderResourceView
    (
        texBuff[index].Get(),
        &srvDesc,
        CD3DX12_CPU_DESCRIPTOR_HANDLE(
            srvHeap->GetCPUDescriptorHandleForHeapStart(),
            index,
            descriptorhandleIncrementSize)
    );

}

void SpriteCommon::SetTextureCommands(uint32_t index)
{
    // SRVヒープの先頭ハンドルを取得（SRVを指しているはず）
    //D3D12_GPU_DESCRIPTOR_HANDLE srvGpuHandle = srvHeap->GetGPUDescriptorHandleForHeapStart();
    UINT descriptorhandleIncrementSize = dxCommon->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
    // SRVヒープの先頭にあるSRVをルートパラメータ1番に設定
    dxCommon->GetCommandList()->SetGraphicsRootDescriptorTable(
        1,
        CD3DX12_GPU_DESCRIPTOR_HANDLE(
        srvHeap->GetGPUDescriptorHandleForHeapStart(),
        index,
        descriptorhandleIncrementSize)
    );
    

}
