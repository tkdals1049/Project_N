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
		float isshadow;
		float padding;
	};

	Struct Data;
};

class ClipBuffer : public ShaderBuffer
{
public:
	ClipBuffer() : ShaderBuffer(&Data, sizeof(Struct))
	{
		Data.Clip=D3DXVECTOR4(0,1,0,1);
		D3DXMatrixIdentity(&Data.invWorld);
	}
	void SetDepth(float depth)
	{
		Data.Clip.w=depth;
	}

	void SetReverse()
	{
		Data.Clip.x*=-1;
		Data.Clip.y*=-1;
		Data.Clip.z*=-1;
		Data.Clip.w*=-1;
	}

	struct Struct
	{
		D3DXVECTOR4 Clip;
		D3DXMATRIX invWorld;
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
	void waterRender();
	void PostRender(bool& isUse);

	D3DXVECTOR2 GetDot() { return dot; }
	void SetSample(int num);
	void SetTexture(int num, wstring file);

	void UpdatePointBuffer(D3DXVECTOR3 origin, D3DXVECTOR3 direction);
	void LoadHeightMap();
	void CreateNormalData();
	void CreateBuffer();
	void ChangeScale(int num);

	void OpenMapDialog(wstring file = L"");
	void SaveMapDialog(wstring file = L"");
	void WriteMap(wstring file);
	void ReadMap(wstring file);

	void Frustum();
	UINT GetWidth() { return width; }
	float GetHeight(float x, float y);
	void ReverseClip(){clipBuffer->SetReverse();}

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
	class Water* water;
	class ZQuadTree* quadTree;

	typedef VertexLerpColorTextureNormal VertexType;
	VertexType* vertex;
	UINT* index;
	BYTE* heightData;

	Shader* shader;
	PlaneBuffer* planeBuffer;
	ClipBuffer* clipBuffer;

	ID3D11Buffer* vertexBuffer;
	ID3D11Buffer* indexBuffer;

	UINT vertexCount;
	UINT indexCount;

	D3DXMATRIX world;
	WorldBuffer* worldBuffer;

	UINT width, height, number;
	float size;
	D3DXVECTOR3 position;

	bool OnTextureList;
	D3DXVECTOR2 dot;
	Textures* textures[5];
	Textures* sample;

	bool isLoaded;
	thread* loadThread;

};