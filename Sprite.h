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
		DirectX::XMMATRIX mat;
	};

public:
	enum VertexNumber
	{
		LB, LT, RB, RT, 
	};

public:
	void Initialize(SpriteCommon* _spriteCommon, uint32_t textureIndex = UINT32_MAX);
	void Update();
	void Draw();


	const DirectX::XMFLOAT4 GetColor()const { return color; }

	const uint32_t GetTextureIndex()const { return textureIndex; }

	const DirectX::XMFLOAT2& GetPosition()const { return position;}
	const float& GetRotationZ()const { return rotationZ; }
	const DirectX::XMFLOAT2& GetSize()const { return size; }
	const DirectX::XMFLOAT2& GetAnchorPoint()const { return anchorPoint; }

	const DirectX::XMFLOAT2& GetTextureLeftTop()const { return textureLeftTop; }
	const DirectX::XMFLOAT2& GetTextureSize()const { return textureSize; }


	const bool& GetIsFlipX() const { return IsFlipX; }
	const bool& GetIsFlipY() const { return IsFlipY; }
	const bool& GetInvisible() const { return IsInvisible; }

	void SetColor(DirectX::XMFLOAT4 color) { this->color = color; }

	void SetTextureIndex(uint32_t index) { this->textureIndex = index; }

	void SetPosition(const DirectX::XMFLOAT2& position) { this->position = position; }
	void SetRotationZ(const float& rotationZ) { this->rotationZ = rotationZ; }
	void SetSize(const DirectX::XMFLOAT2& size) {this->size = size; }
	void SetAncharPoint(const DirectX::XMFLOAT2& anchorPoint) { this->anchorPoint = anchorPoint; }

	void SetTextureLeftTop(const DirectX::XMFLOAT2 leftTop) { this->textureLeftTop = leftTop; }
	void SetTextureSize(const DirectX::XMFLOAT2 size) { this->textureSize = size; }

	void SetIsFlipX(const bool& isFlipX) { this->IsFlipX = isFlipX; }
	void SetIsFlipY(const bool& isFlipY) { this->IsFlipY = isFlipY; }
	void SetInvisible(const bool& IsInvisible) { this->IsFlipY = IsInvisible; }

private:
	void AdjustTextureSize();
private:

	//DirectX::XMFLOAT3 scale = { 1.f,1.f,1.f };
	//DirectX::XMFLOAT3 rotation = { 0.f,0.f,0.f };
	//DirectX::XMFLOAT3 position = { 0.f,0.f,0.f };

	SpriteCommon* spriteCommon = nullptr;

	uint32_t textureIndex = 0;

	DirectX::XMFLOAT4 color = { 1,1,1,1 };

	DirectX::XMFLOAT2 position{ 0.f,0.f };
	float rotationZ = 0.f;
	DirectX::XMFLOAT2 size = { 100.f,100.f };
	DirectX::XMFLOAT2 anchorPoint = { 0.f,0.f };

	DirectX::XMFLOAT2 textureLeftTop = { 0.f,0.f };
	DirectX::XMFLOAT2 textureSize = { 100.f,100.f };

	bool IsFlipX = false;
	bool IsFlipY = false;

	bool IsInvisible = false;


	Vertex vertices[4];

	Microsoft::WRL::ComPtr<ID3D12Resource>vertBuff;

	D3D12_VERTEX_BUFFER_VIEW vbView{};

	Microsoft::WRL::ComPtr<ID3D12Resource>constBuffMaterial;
	ConstBufferDataMaterial* constMapMaterial = nullptr;

	Microsoft::WRL::ComPtr<ID3D12Resource>constBuffTransform;
	ConstBufferDataTransform* constMapTransform = nullptr;
};

