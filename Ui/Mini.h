#pragma once

class Mini
{
public:
	Mini();
	~Mini();

	void SetTranslation(D3DXVECTOR2& translation)
	{
		this->translation = translation;

		D3DXMatrixTranslation(&T, translation.x, translation.y, 0);
		world = S * T;
	}

	void SetScale(D3DXVECTOR2& scale)
	{
		this->scale = scale;

		D3DXMatrixScaling(&S, scale.x, scale.y, 1);
		//S = invCenter * S * center;

		world = S * T;
	}

	void Update();
	void Render(D3DXVECTOR2 scale, D3DXVECTOR2 position, RenderMode mode);

	void CreateBlock();
	void CreateCircle();
	
private:

	typedef VertexTexture VertexType;
	Shader* shader;
	WorldBuffer* worldBuffer;

	D3DXVECTOR2 translation;
	D3DXVECTOR2 scale;

	D3DXMATRIX center, invCenter;
	D3DXMATRIX S, T;

	D3DXMATRIX world;

	ID3D11Buffer* vertexBuffer;
	ID3D11Buffer* indexBuffer;
	ID3D11Buffer* vertexBuffer2;
	ID3D11Buffer* indexBuffer2;
};