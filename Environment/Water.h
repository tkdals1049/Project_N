#pragma once

class ReflectBuffer : public ShaderBuffer
{
public:
	ReflectBuffer() : ShaderBuffer(&Data, sizeof(Struct))
	{
		D3DXMatrixIdentity(&Data.reflect);
	}
	void SetMatrix(D3DXMATRIX mat)
	{
		Data.reflect = mat;
		D3DXMatrixTranspose(&Data.reflect, &Data.reflect);
	}
	struct Struct
	{
		D3DXMATRIX reflect;
	};
	Struct Data;
};

class WaveBuffer;
class Water
{
public:
	Water(UINT width, UINT height, float depth=-2.0f);
	~Water();

	void CreateBuffer();

	void Update();
	void Render();
	float GetDepth(){return depth;}
	void SetSize(UINT width,UINT height);

private:
	typedef VertexTexture VertexType;
	Shader* shader;

	WorldBuffer* worldBuffer;
	WaveBuffer* waveBuffer;
	ReflectBuffer* reflectBuffer;
	ID3D11ShaderResourceView* normalMap;

	ID3D11Buffer* vertexBuffer;
	ID3D11Buffer* indexBuffer;

	UINT width, height;
	float depth, wave;
	D3DXVECTOR3 normal,size;
	D3DXMATRIX world;
	VertexType* vertex;
	UINT* index;

	UINT vertexCount;
	UINT indexCount;
};