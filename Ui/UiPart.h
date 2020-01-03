#pragma once

class UiPart
{
public:
	UiPart(wstring file);
	~UiPart();

	void Update();
	void Render(D3DXVECTOR4 uv, D3DXVECTOR2 position, D3DXVECTOR2 size);
	
	void CreateBuffer();

private:
	typedef VertexTexture VertexType;
	Shader* shader;

	VertexType* vertex;
	UINT* index;

	ID3D11Buffer* vertexBuffer;
	ID3D11Buffer* indexBuffer;
	UINT vertexCount;
	UINT indexCount;

	WorldBuffer* worldBuffer;
	UINT width, height;
	UINT lookNum;
	wstring fileName;
	ID3D11ShaderResourceView* texture;

	D3DXMATRIX world, scale;
};