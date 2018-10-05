#pragma once

class CloudBuffer : public ShaderBuffer
{
public:
	CloudBuffer() : ShaderBuffer(&Data, sizeof(Struct))
	{
		Data.scale = 0.0f;
		Data.translation = 0.0f;
	}

	struct Struct
	{
		float translation;
		float scale;
		D3DXVECTOR2 pad;
	};

	Struct Data;
};

class PerlinNoise;
class Cloud {
public:
	Cloud();
	~Cloud();


	void Initialize();
	void Update();
	void Render();

	void MakeCloudPerlin();

	UINT getIndexCount() {
		return indexCount;
	}

	ID3D11ShaderResourceView* getDiffuseMap() {
		return diffuse;
	}

	ID3D11ShaderResourceView* getPerlinMap() {
		return perlin;
	}

	D3DXMATRIX getWorld() {
		return world;
	}


private:
	void CreateVertexData();
	void CreateIndexData();
	void CreateBuffer();

	ID3D11Buffer* vertexBuffer;
	ID3D11Buffer* indexBuffer;


	UINT vertexCount;
	UINT indexCount;
	VertexTexture* vertexData;
	UINT* indexData;


	UINT CloudResolution;
	float CloudWidth, CloudTop, CloudBottom,CloudDepth;


	ID3D11ShaderResourceView* diffuse;

	Shader* shader;
	class CloudBuffer cloudData;
	float speed;

	PerlinNoise* noise;
	PerlinNoise* perlinNoise;
	ID3D11ShaderResourceView* perlin;
	D3DXMATRIX world;


};