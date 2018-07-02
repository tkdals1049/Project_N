#pragma once

class PlaneBuffer : public ShaderBuffer
{
public:
	PlaneBuffer() : ShaderBuffer(&Data, sizeof(Struct))
	{
		Data.Color = D3DXCOLOR(0.0f, 0.05f, 0.6f, 1.0f);
		Data.Ambient = D3DXCOLOR(0.5f, 0.5f, 0.5f, 1.0f);
		Data.Diffuse = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
		Data.Size = D3DXVECTOR4(0.0f, 0.0f, 0.0f, 0.0f);
		Data.Point = D3DXVECTOR4(0.0f, 0.0f, 0.0f, 0.0f);
		Data.Thickness = 1.0f;
		Data.GridSpace = 4.0f;
	}

	struct Struct
	{
		D3DXCOLOR Color;
		D3DXCOLOR Ambient;
		D3DXCOLOR Diffuse;
		D3DXVECTOR4 Size;
		D3DXVECTOR4 Point;
		float Thickness;
		float GridSpace;
		float padding[2];
	};

	Struct Data;
};

class Plane
{
public:
	static Plane* Get();
	static void Delete();

	void Update();
	void Render();
	void PostRender(bool& isUse);

	D3DXVECTOR2 GetDot() { return dot; }
	void SetSample(int num);
	void SetTexture(int num, wstring file);

	void UpdatePointBuffer(D3DXVECTOR3 origin, D3DXVECTOR3 direction);
	void CreateNormalData();
	void CreateBuffer();
	void ChangeScale(UINT width, UINT height);

	void OpenMapDialog(wstring file = L"");
	void SaveMapDialog(wstring file = L"");
	void WriteMap(wstring file);
	void ReadMap(wstring file);

	void Frustum();

private:
	static Plane* instance;

	Plane();
	~Plane();

	struct Textures
	{	
		wstring file;
		ID3D11ShaderResourceView* texture;
		Textures()
		{
			file=L"";
			texture=NULL;
		}

	};
	friend class Brush;
	class Brush* brush;

	typedef VertexLerpColorTextureNormal VertexType;
	VertexType* vertex;
	UINT* index;

	Shader* shader;
	PlaneBuffer* planeBuffer;

	ID3D11Buffer* vertexBuffer;
	ID3D11Buffer* indexBuffer;

	UINT vertexCount;
	UINT indexCount;

	D3DXMATRIX world;
	WorldBuffer* worldBuffer;

	UINT width, height, number;
	D3DXVECTOR3 position;

	bool OnTextureList;
	D3DXVECTOR2 dot;
	Textures* textures[5];
	Textures* sample;

	bool isLoaded;
	thread* loadThread;
};