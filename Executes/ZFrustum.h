#ifndef _ZFRUSTUM_H_
#define _ZFRUSTUM_H_

#define PLANE_EPSILON	5.0f

class ZFrustum
{
	D3DXVECTOR3	m_vtx[8];	/// ���������� ������ ���� 8��
	D3DXVECTOR3	m_vPos;		/// ���� ī�޶��� ������ǥ
	D3DXPLANE	m_plane[6];	/// ���������� �����ϴ� 6���� ���

public:
	static ZFrustum* Get();
	static void Delete();
	
	/// ī�޶�(view) * ��������(projection)����� �Է¹޾� 6���� ����� �����.
	BOOL	Make( D3DXMATRIX* pmatViewProj );

	/// ���� v�� �������Ҿȿ� ������ TRUE�� ��ȯ, �ƴϸ� FALSE�� ��ȯ�Ѵ�.
	BOOL	IsIn( D3DXVECTOR3* pv );

	/** �߽�(v)�� ������(radius)�� ���� ��豸(bounding sphere)�� �������Ҿȿ� ������
	 *  TRUE�� ��ȯ, �ƴϸ� FALSE�� ��ȯ�Ѵ�.
	 */
	BOOL	IsInSphere( D3DXVECTOR3* pv, float radius );

	/// ���� ī�޶��� ������ǥ�� ����ش�.
	D3DXVECTOR3*	GetPos() { return &m_vPos; }

private:
	static ZFrustum* instance;

	ZFrustum();
};

#endif // _ZFRUSTUM_H_