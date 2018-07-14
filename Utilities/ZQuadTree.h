#ifndef _ZQUADTREE_H_
#define _ZQUADTREE_H_

#include "ZFrustum.h"

class ZQuadTree
{
	/// ����Ʈ���� �����Ǵ� 4���� �ڳʰ��� ���� �����
	enum			CornerType { CORNER_TL, CORNER_TR, CORNER_BL, CORNER_BR };

	/// �̿���� ó���� �����
	enum { EDGE_UP, EDGE_DN, EDGE_LT, EDGE_RT };

	/// ����Ʈ���� �������Ұ��� ����
	enum			QuadLocation { FRUSTUM_OUT = 0,				/// �������ҿ��� �������
								   FRUSTUM_PARTIALLY_IN = 1,	/// �������ҿ� �κ�����
								   FRUSTUM_COMPLETELY_IN = 2,	/// �������ҿ� ��������
								   FRUSTUM_UNKNOWN = -1 };		/// �𸣰���(^^;)

private:

	typedef VertexLerpColorTextureNormal VertexType;

	ZQuadTree*	m_pChild[4];		/// QuadTree�� 4���� �ڽĳ��

	ZQuadTree*	m_pParent;			/// Triangle Crack(Popping)�� �������ؼ� ����Ѵ�.
	ZQuadTree*	m_pNeighbor[4];		/// Triangle Crack(Popping)�� �������ؼ� ����Ѵ�.

	int			m_nCenter;			/// QuadTree�� ������ ù��° ��
	int			m_nCorner[4];		/// QuadTree�� ������ �ι�° ��)
	BOOL		m_bCulled;			/// �������ҿ��� �ø��� ����ΰ�?
	float		m_fRadius;			/// ��带 ���δ� ��豸(bounding sphere)�� ������
	
private:
	/// �ڽ� ��带 �߰��Ѵ�.
	ZQuadTree*	_AddChild( int nCornerTL, int nCornerTR, int nCornerBL, int nCornerBR );

	/// 4���� �ڳʰ��� �����Ѵ�.
	BOOL		_SetCorners( int nCornerTL, int nCornerTR, int nCornerBL, int nCornerBR );

	/// Quadtree�� 4���� ���� Ʈ���� �κк���(subdivide)�Ѵ�.
	BOOL		_SubDivide();		// Quadtree�� subdivide�Ѵ�.

	/// ���� ��尡 ����� ������ ����ΰ�?
	BOOL		_IsVisible() { return ( m_nCorner[CORNER_TR] - m_nCorner[CORNER_TL] <= 1 ); }

	/// ����� �������� �ε����� �����Ѵ�.
	int			_GenTriIndex( int nTris, UINT* pIndex, VertexType* pHeightMap);

	/// �޸𸮿��� ����Ʈ���� �����Ѵ�.
	void		_Destroy();

	void _AllInFrustum();

	/// �����尡 �������ҿ� ���ԵǴ°�?
	int			_IsInFrustum( VertexType* pHeightMap);

	/// _IsInFrustum()�Լ��� ����� ���� �������� �ø� ����
	void		_FrustumCull( VertexType* pHeightMap);

	int _GetNodeIndex(int ed, int cx, int & _0, int & _1, int & _2, int & _3);

	ZQuadTree * _FindNode(VertexType * pHeightMap, int _0, int _1, int _2, int _3);

	void _BuildNeighborNode(ZQuadTree * pRoot, VertexType * pHeightMap, int cx);

	BOOL _BuildQuadTree(VertexType * pHeightMap);

	float		_GetDistance(D3DXVECTOR3* pv1, D3DXVECTOR3* pv2)
	{
		return D3DXVec3Length(&(*pv2 - *pv1));
	}

	/// ī�޶�� ���� ������ �Ÿ����� �������� LOD���� ���Ѵ�.
	int			_GetLODLevel(VertexType* pHeightMap, D3DXVECTOR3* pCamera)
	{
		float d = _GetDistance((D3DXVECTOR3*)(pHeightMap + m_nCenter), pCamera);
		return max((int)(d * 0.1f), 1);
	}

	/// ���� ��尡 LOD������� ����  ����� ������ ����ΰ�?
	BOOL		_IsVisible(VertexType* pHeightMap, D3DXVECTOR3* pCamera)
	{
		return ((m_nCorner[CORNER_TR] - m_nCorner[CORNER_TL]) <= _GetLODLevel(pHeightMap, pCamera));
	}

	BOOL IsInRect(RECT* rc, POINT pt)
	{
		if ((rc->left <= pt.x) && (pt.x <= rc->right) &&
			(rc->bottom <= pt.y) && (pt.y <= rc->top))
			return TRUE;

		return FALSE;
	}

	int rootCorner(int num);
public:

				/// ���� ��Ʈ��� ������
				ZQuadTree( int cx, int cy );

				/// ���� �ڽĳ�� ������
				ZQuadTree( ZQuadTree* pParent );

				/// �Ҹ���
				~ZQuadTree();

	/// QuadTree�� �����Ѵ�.
	BOOL		Build( VertexType* pHeightMap );

	///	�ﰢ���� �ε����� �����, ����� �ﰢ���� ������ ��ȯ�Ѵ�.
	int			GenerateIndex(UINT* pIndex, VertexType* pHeightMap);
};

#endif // _ZQUADTREE_H_
