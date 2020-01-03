#pragma once
class BarBuffer : public ShaderBuffer
{
public:
	BarBuffer() : ShaderBuffer(&data, sizeof(Data))
	{
		data.gage = D3DXVECTOR2(0.9f,0.9f);
		data.color = D3DXCOLOR(0, 0, 0, 0);
	}
	void SetGage(D3DXVECTOR2 gage)
	{
		data.gage=gage;
	}
	void SetGageX(float value)
	{
		data.gage.x += value;
	}
	void SetGageY(float value)
	{
		data.gage.y += value;
	}
	bool GageCompare()
	{
		return data.gage.x >= data.gage.y;
	}

	void SetColor(D3DXCOLOR color)
	{
		data.color = color;
	}

	struct Data
	{
		D3DXCOLOR color;
		D3DXVECTOR2 gage;
		float padding[2];
	};
private:
	Data data;
};
class Bar
{
public:
	Bar(D3DXCOLOR color=D3DXCOLOR(0,1,0,1));
	~Bar();

	void Update();
	void Render(D3DXVECTOR2 position, D3DXVECTOR2 size);

	void CreateBuffer();
	void Damage(UINT dmg);

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
	BarBuffer* barbuffer;

	UINT dmg;

	D3DXMATRIX world, scale;
};