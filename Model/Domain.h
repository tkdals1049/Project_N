#pragma once


class Domain
{
public:
	static Domain* Get();
	static void Delete();

	void Render(D3DXMATRIX* world);
	void ColorChange(D3DXVECTOR4 color =D3DXVECTOR4(0,0,0,0));
	void GetLength(float& other){other=this->other;}

private:
	void CreateBuffer();

	static Domain* instance;

	Domain();
	~Domain();

	Shader* shader;

	VertexColor* vertex;

	ID3D11Buffer* vertexBuffer;
	ID3D11Buffer* indexBuffer;
	D3DXMATRIX world;

	UINT vertexCount;
	UINT indexCount;

	float other;
	WorldBuffer* worldBuffer;
};