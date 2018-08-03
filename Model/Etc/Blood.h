#pragma once

class Blood
{
public:
	Blood();
	~Blood();

	void PreUpdate();
	void Update();
	void Render();;
	void CreateBuffer();

	void SetPosition(const D3DXVECTOR3 another)
	{
		position.x= another.x;
		position.y = another.y;
		position.z = another.z;
		isblood = true;
	}
	void isBlood(){isblood=true;}
private:
	//π´±‚ ¿Ã∆Â∆Æ Ω¶¿Ã¥ı
	typedef VertexTexture VertexType;
	Shader* shader;

	VertexType* vertex;
	UINT* index;
	
	ID3D11Buffer* vertexBuffer;
	ID3D11Buffer* indexBuffer;
	UINT vertexCount;
	UINT indexCount;

	WorldBuffer* worldBuffer;
	UINT width,height;
	UINT lookNum;
	wstring fileName;
	ID3D11ShaderResourceView* texture;

	ID3D11BlendState* linearState;
	ID3D11BlendState* offState;
	ID3D11DepthStencilState* offMaskState;
	ID3D11DepthStencilState* onMaskZeroState;

	D3DXVECTOR3 position,rotate;
	float time;
	bool isblood;

};