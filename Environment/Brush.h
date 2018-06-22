#pragma once

class Plane;
class Brush
{
public:
	Brush(Plane* plane);
	~Brush();

	void Update(D3DXVECTOR3 origin, D3DXVECTOR3 direction);
	void Render();
	D3DXVECTOR2 CheckMouseIndex(D3DXVECTOR3 origin, D3DXVECTOR3 direction);
	UINT mode;
	UINT type;
	UINT size;
	float floor;
	D3DXCOLOR color;
	UINT num, tabpage;
private:
	struct Box
	{
		float left;
		float top;
		float right;
		float bottom;
		Box() { left = top = right = bottom = 0; }
	};
	friend Plane;
	Plane* plane;

	void AdjustHeight(Box* box);
	void ControlHeight(Box* box);
	void Coloring(Box * box);
	void Splatting(Box* box);
	ID3D11Buffer* vertexBuffer;

	typedef VertexLerpColorTextureNormal VertexType;
	VertexType* vertex;

	UINT vertexCount;
	UINT width;
	UINT height;
	UINT brushOn;

	Camera* camera;
	D3DXVECTOR2 position;
	float way;
};

