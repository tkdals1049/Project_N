#pragma once

class Test
{
public:
	Test();
	~Test();

	void PreUpdate();
	void Update();
	void Render();;
	void CreateBuffer();

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
	ID3D11ShaderResourceView* texture;
};