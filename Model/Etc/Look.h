#pragma once

#define PLANE_EPSILON	5.0f
class Look
{
public:
	Look();
	~Look();
	/// ī�޶�(view) * ��������(projection)����� �Է¹޾� 6���� ����� �����.
	BOOL	Make();

	/// ���� v�� �������Ҿȿ� ������ TRUE�� ��ȯ, �ƴϸ� FALSE�� ��ȯ�Ѵ�.
	BOOL	IsIn(const D3DXVECTOR3* pv);

	/** �߽�(v)�� ������(radius)�� ���� ��豸(bounding sphere)�� �������Ҿȿ� ������
	 *  TRUE�� ��ȯ, �ƴϸ� FALSE�� ��ȯ�Ѵ�.
	 */
	BOOL	IsInSphere(const D3DXVECTOR3* pv, float radius);

	void PreUpdate();
	void Update();
	void Render();;
	void CreateBuffer();

	void SetWorld(D3DXMATRIX world);
private:
	//���� ����Ʈ ���̴�
	typedef VertexColor VertexType;
	Shader* shader;

	VertexType* vertex;
	UINT* index;
	
	ID3D11Buffer* vertexBuffer;
	ID3D11Buffer* indexBuffer;
	UINT vertexCount;
	UINT indexCount;

	WorldBuffer* worldBuffer;
	D3DXMATRIX world,view,proj,scale;

	D3DXVECTOR3	m_vtx[8];	/// ���������� ������ ���� 8��
	D3DXVECTOR3	m_vPos;		/// ���� ī�޶��� ������ǥ
	D3DXPLANE	m_plane[6];	/// ���������� �����ϴ� 6���� ���

};

