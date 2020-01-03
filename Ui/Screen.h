#pragma once

class ScreenBuffer : public ShaderBuffer
{
public:
	ScreenBuffer() : ShaderBuffer(&data, sizeof(Data))
	{
		data.size = D3DXVECTOR2(0, 0);
		data.mode = 0.0f;

		float weight[5];
		float normalization;

		// 각 이웃 픽셀이 흐림에 기여할 가중치를 만듭니다.
		weight[0] = 1.0f;
		weight[1] = 0.9f;
		weight[2] = 0.55f;
		weight[3] = 0.18f;
		weight[4] = 0.1f;

		// 가중치를 평균화하는 정규화 된 값을 만듭니다.
		normalization = (weight[0] + 2.0f * (weight[1] + weight[2] + weight[3] + weight[4]));
		for(int i=0;i<5;i++)weight[i]=weight[i] /normalization;
		
		data.weight = D3DXVECTOR4(weight[1] , weight[2] , weight[3] , weight[4]);
		data.normalization = weight[0];
	}
	void SetSize(D3DXVECTOR2 size)
	{
		float scale=1;
		data.size = D3DXVECTOR2(scale/size.x,scale/size.y);
	}
	void SetMode(float mode)
	{
		data.mode = mode;
	}
	float GetMode()
	{
		return data.mode;
	}

	struct Data
	{
		D3DXVECTOR4 weight;
		D3DXVECTOR2 size;
		float normalization;
		float mode;
	};
private:
	Data data;
};

class Screen
{
public:
	Screen();
	~Screen();

	void Update();
	void Render(D3DXVECTOR2 position, D3DXVECTOR2 size);

	void CreateBuffer();
	void SetMode();
private:

	typedef VertexTexture VertexType;
	Shader* shader;
	class UiPart* part;

	ID3D11Buffer* vertexBuffer;
	ID3D11Buffer* indexBuffer;

	WorldBuffer* worldBuffer;
	ScreenBuffer* screenbuffer;
	D3DXMATRIX scale,world;
};