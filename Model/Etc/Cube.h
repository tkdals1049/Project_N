#pragma once

class Cube
{
public:
	static Cube* Get();
	static void Delete();

	void Update(D3DXMATRIX& world);
	void Render();
	void ColorChange(D3DXVECTOR4 color =D3DXVECTOR4(0,0,0,0));
	void GetLength(float& other){other=this->other;}

private:
	void CreateBuffer();

	static Cube* instance;

	Cube();
	~Cube();

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