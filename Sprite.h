#pragma once
#include "SpriteCommon.h"
class Sprite
{
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
		DirectX::XMFLOAT4 mat;
	};
public:
	void Initialize(SpriteCommon* _spriteCommon);

	void Draw();
private:

	DirectX::XMFLOAT3 scale = { 1.f,1.f,1.f };
	DirectX::XMFLOAT3 rotation = { 0.f,0.f,0.f };
	DirectX::XMFLOAT3 position = { 0.f,0.f,0.f };

	SpriteCommon* spriteCommon = nullptr;

	Microsoft::WRL::ComPtr<ID3D12Resource>vertBuff;

	D3D12_VERTEX_BUFFER_VIEW vbView{};
};

