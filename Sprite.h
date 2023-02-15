#pragma once
#include "SpriteCommon.h"
class Sprite
{
private:
	const int window_width = 1280;
	const int window_height = 720;
public:
	struct Vertex
	{
		DirectX::XMFLOAT3 pos;
		DirectX::XMFLOAT2 uv;
	};

	struct ConstBufferDataMaterial
	{
		DirectX::XMFLOAT4 color;
	};

	struct ConstBufferDataTransform
	{
		DirectX::XMMATRIX mat;
	};
public:
	void Initialize(SpriteCommon* _spriteCommon);

	void Draw();
private:

	//DirectX::XMFLOAT3 scale = { 1.f,1.f,1.f };
	//DirectX::XMFLOAT3 rotation = { 0.f,0.f,0.f };
	//DirectX::XMFLOAT3 position = { 0.f,0.f,0.f };

	SpriteCommon* spriteCommon = nullptr;

	DirectX::XMFLOAT4 color = { 1,1,1,1.0f };
	Microsoft::WRL::ComPtr<ID3D12Resource>vertBuff;

	D3D12_VERTEX_BUFFER_VIEW vbView{};

	Microsoft::WRL::ComPtr<ID3D12Resource>constBuffMaterial;
	ConstBufferDataMaterial* constMapMaterial = nullptr;

	Microsoft::WRL::ComPtr<ID3D12Resource>constBuffTransform;
	ConstBufferDataTransform* constMapTransform = nullptr;
};

