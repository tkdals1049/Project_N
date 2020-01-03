#pragma once

class LookBuffer : public ShaderBuffer
{
public:
	LookBuffer() : ShaderBuffer(&Data, sizeof(Struct))
	{
		Data.color = D3DXCOLOR(0, 0, 0, 1);
	}
	void SetColor(D3DXCOLOR& color)
	{
		Data.color = color;
	}
	struct Struct
	{
		D3DXCOLOR color;
	};
	Struct Data;
};

#define PLANE_EPSILON	5.0f
class Look
{
public:
	Look();
	~Look();
	/// 카메라(view) * 프로젝션(projection)행렬을 입력받아 6개의 평면을 만든다.
	BOOL	Make();

	/// 한점 v가 프러스텀안에 있으면 TRUE를 반환, 아니면 FALSE를 반환한다.
	BOOL	IsIn(const D3DXVECTOR3* pv);

	/** 중심(v)와 반지름(radius)를 갖는 경계구(bounding sphere)가 프러스텀안에 있으면
	 *  TRUE를 반환, 아니면 FALSE를 반환한다.
	 */
	BOOL	IsInSphere(const D3DXVECTOR3* pv, float radius);

	void PreUpdate();
	void Update();
	void Render();;
	void CreateBuffer();

	void ChangeColor();

	void SetWorld(D3DXMATRIX world);
private:
	//무기 이펙트 쉐이더
	typedef Vertex VertexType;
	Shader* shader;

	VertexType* vertex;
	UINT* index;
	
	ID3D11Buffer* vertexBuffer;
	ID3D11Buffer* indexBuffer;
	UINT vertexCount;
	UINT indexCount;

	LookBuffer* lookbuffer;

	WorldBuffer* worldBuffer;
	D3DXMATRIX world,view,proj,scale;

	D3DXVECTOR3	m_vtx[8];	/// 프러스텀을 구성할 정점 8개
	D3DXVECTOR3	m_vPos;		/// 현재 카메라의 월드좌표
	D3DXPLANE	m_plane[6];	/// 프러스텀을 구성하는 6개의 평면

};

