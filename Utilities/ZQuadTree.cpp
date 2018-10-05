#include "stdafx.h"
#include "ZQuadTree.h"

int ZQuadTree::rootCorner(int num)
{
	if (m_pParent != NULL) 
	{
	int cx= m_pParent->rootCorner(num);
	return m_pParent->rootCorner(num);
	}
	else
	return m_nCorner[num];
}

// ���� ��Ʈ��� ������
ZQuadTree::ZQuadTree(int cx, int cy )
{
	int		i;
	m_nCenter = 0;
	m_pParent = NULL;

	for( i = 0 ; i < 4 ; i++ )
	{
		m_pChild[i] = NULL;
		m_pNeighbor[i] = NULL;
	}

	// ��Ʈ����� 4�� �ڳʰ� ����
	m_nCorner[CORNER_BL] = 0;
	m_nCorner[CORNER_BR] = cx;
	m_nCorner[CORNER_TL] = (cx + 1) * cy ;
	m_nCorner[CORNER_TR] = (cx + 1) * cy + cx;

	m_nCenter=(m_nCorner[CORNER_TL]+m_nCorner[CORNER_TR]+m_nCorner[CORNER_BL]+m_nCorner[CORNER_BR])/4;

	m_fRadius = 0.0f;
	m_bCulled = FALSE;
	m_fRadius = 0.0f;

}

// ���� �ڽĳ�� ������
ZQuadTree::ZQuadTree( ZQuadTree* pParent )
{
	int		i;
	m_pParent = pParent;
	m_nCenter = 0;
	for( i = 0 ; i < 4 ; i++ )
	{
		m_pChild[i] = NULL;
		m_pNeighbor[i] = NULL;
		m_nCorner[i] = 0;
	}

	m_bCulled = FALSE;
	m_fRadius = 0.0f;
}

// �Ҹ���
ZQuadTree::~ZQuadTree()
{
	_Destroy();
}

// �޸𸮿��� ����Ʈ���� �����Ѵ�.
void	ZQuadTree::_Destroy()
{
	// �ڽ� ������ �Ҹ� ��Ų��.
	for( int i = 0 ; i < 4 ; i++ ) SAFE_DELETE( m_pChild[i] );
}


// 4���� �ڳʰ��� �����Ѵ�.
BOOL	ZQuadTree::_SetCorners( int nCornerTL, int nCornerTR, int nCornerBL, int nCornerBR )
{
	m_nCorner[CORNER_TL] = nCornerTL;
	m_nCorner[CORNER_TR] = nCornerTR;
	m_nCorner[CORNER_BL] = nCornerBL;
	m_nCorner[CORNER_BR] = nCornerBR;
	m_nCenter			 = ( m_nCorner[CORNER_TL] + m_nCorner[CORNER_TR] + 
							 m_nCorner[CORNER_BL] + m_nCorner[CORNER_BR] ) / 4;

	return TRUE;
}


// �ڽ� ��带 �߰��Ѵ�.
ZQuadTree*	ZQuadTree::_AddChild( int nCornerTL, int nCornerTR, int nCornerBL, int nCornerBR )
{
	ZQuadTree*	pChild;

	pChild = new ZQuadTree( this );
	pChild->_SetCorners( nCornerTL, nCornerTR, nCornerBL, nCornerBR );

	return pChild;
}

// Quadtree�� 4���� �ڽ� Ʈ���� �κк���(subdivide)�Ѵ�.
BOOL	ZQuadTree::_SubDivide()
{
	int		nTopEdgeCenter;
	int		nBottomEdgeCenter;
	int		nLeftEdgeCenter;
	int		nRightEdgeCenter;
	int		nCentralPoint;

	// ��ܺ� ���
	nTopEdgeCenter		= ( m_nCorner[CORNER_TL] + m_nCorner[CORNER_TR] ) / 2;
	// �ϴܺ� ��� 
	nBottomEdgeCenter	= ( m_nCorner[CORNER_BL] + m_nCorner[CORNER_BR] ) / 2;
	// ������ ���
	nLeftEdgeCenter		= ( m_nCorner[CORNER_TL] + m_nCorner[CORNER_BL] ) / 2;
	// ������ ���
	nRightEdgeCenter	= ( m_nCorner[CORNER_TR] + m_nCorner[CORNER_BR] ) / 2;
	// �Ѱ��
	nCentralPoint		= ( m_nCorner[CORNER_TL] + m_nCorner[CORNER_TR] + 
							m_nCorner[CORNER_BL] + m_nCorner[CORNER_BR] ) / 4;

	// ���̻� ������ �Ұ����Ѱ�? �׷��ٸ� SubDivide() ����
	if( (m_nCorner[CORNER_TR] - m_nCorner[CORNER_TL]) <= 1 )
	{
		return FALSE;
	}

	// 4���� �ڽĳ�� �߰�
	m_pChild[CORNER_TL] = _AddChild( m_nCorner[CORNER_TL], nTopEdgeCenter, nLeftEdgeCenter, nCentralPoint );
	m_pChild[CORNER_TR] = _AddChild( nTopEdgeCenter, m_nCorner[CORNER_TR], nCentralPoint, nRightEdgeCenter );
	m_pChild[CORNER_BL] = _AddChild( nLeftEdgeCenter, nCentralPoint, m_nCorner[CORNER_BL], nBottomEdgeCenter );
	m_pChild[CORNER_BR] = _AddChild( nCentralPoint, nRightEdgeCenter, nBottomEdgeCenter, m_nCorner[CORNER_BR] );
	
	return TRUE;
}

// ����� �������� �ε����� �����Ѵ�.
int		ZQuadTree::_GenTriIndex( int nTris, UINT* pIndex, const VertexType* pHeightMap, bool isCull)
{
	if( m_bCulled &&isCull)
	{
		m_bCulled = FALSE;
		return nTris;
	}

	// ���� ��尡 ��µǾ�� �ϴ°�?
	//if( _IsVisible() )
	if (_IsVisible(pHeightMap,ZFrustum::Get()->GetPos()))
	{
		if (m_nCorner[CORNER_TR] - m_nCorner[CORNER_TL] <= 1)
		{
			pIndex[nTris++] = m_nCorner[CORNER_BL];
			pIndex[nTris++] = m_nCorner[CORNER_TL];
			pIndex[nTris++] = m_nCorner[CORNER_BR];
			pIndex[nTris++] = m_nCorner[CORNER_BR];
			pIndex[nTris++] = m_nCorner[CORNER_TL];
			pIndex[nTris++] = m_nCorner[CORNER_TR];
			return nTris;
		}

		BOOL	b[4] = { 0,0,0,0 };
		// ��� �̿����(neightbor node)�� ��°����Ѱ�?
		if (m_pNeighbor[EDGE_UP]) b[EDGE_UP] = m_pNeighbor[EDGE_UP]->_IsVisible(pHeightMap,ZFrustum::Get()->GetPos());
		// �ϴ� �̿����(neightbor node)�� ��°����Ѱ�?
		if (m_pNeighbor[EDGE_DN]) b[EDGE_DN] = m_pNeighbor[EDGE_DN]->_IsVisible(pHeightMap,ZFrustum::Get()->GetPos());
		// ���� �̿����(neightbor node)�� ��°����Ѱ�?
		if (m_pNeighbor[EDGE_LT]) b[EDGE_LT] = m_pNeighbor[EDGE_LT]->_IsVisible(pHeightMap,ZFrustum::Get()->GetPos());
		// ���� �̿����(neightbor node)�� ��°����Ѱ�?
		if (m_pNeighbor[EDGE_RT]) b[EDGE_RT] = m_pNeighbor[EDGE_RT]->_IsVisible(pHeightMap,ZFrustum::Get()->GetPos());

		// �̿������� ��δ� ��°����ϴٸ� ������� �̿������� ���� LOD�̹Ƿ� 
		// �κк����� �ʿ����.

		if ((b[EDGE_UP] && b[EDGE_DN] && b[EDGE_LT] && b[EDGE_RT]))
		{
			pIndex[nTris++] = m_nCorner[CORNER_BL];
			pIndex[nTris++] = m_nCorner[CORNER_TL];
			pIndex[nTris++] = m_nCorner[CORNER_BR];
			pIndex[nTris++] = m_nCorner[CORNER_BR];
			pIndex[nTris++] = m_nCorner[CORNER_TL];
			pIndex[nTris++] = m_nCorner[CORNER_TR];
			return nTris;
		}

		int		n;
		
		if (!b[EDGE_UP]) // ��� �κк����� �ʿ��Ѱ�?
		{
			n = (m_nCorner[CORNER_TL] + m_nCorner[CORNER_TR]) / 2;
			pIndex[nTris++] = m_nCenter; pIndex[nTris++] = m_nCorner[CORNER_TL]; pIndex[nTris++] = n;
			pIndex[nTris++] = m_nCenter; pIndex[nTris++] = n; pIndex[nTris++] = m_nCorner[CORNER_TR];
		}
		else	// ��� �κк����� �ʿ���� ���
		{
			pIndex[nTris++] = m_nCenter; pIndex[nTris++] = m_nCorner[CORNER_TL]; pIndex[nTris++] = m_nCorner[CORNER_TR];
		}
		
		if (!b[EDGE_DN]) // �ϴ� �κк����� �ʿ��Ѱ�?
		{
			n = (m_nCorner[CORNER_BL] + m_nCorner[CORNER_BR]) / 2;
			pIndex[nTris++] = m_nCenter; pIndex[nTris++] = m_nCorner[CORNER_BR]; pIndex[nTris++] = n;
			pIndex[nTris++] = m_nCenter; pIndex[nTris++] = n; pIndex[nTris++] = m_nCorner[CORNER_BL];
		}
		else	// �ϴ� �κк����� �ʿ���� ���
		{
			pIndex[nTris++] = m_nCenter; pIndex[nTris++] = m_nCorner[CORNER_BR]; pIndex[nTris++] = m_nCorner[CORNER_BL];
		}
		
		if (!b[EDGE_LT]) // ���� �κк����� �ʿ��Ѱ�?
		{
			n = (m_nCorner[CORNER_TL] + m_nCorner[CORNER_BL]) / 2;
			pIndex[nTris++] = m_nCenter; pIndex[nTris++] = m_nCorner[CORNER_BL]; pIndex[nTris++] = n;
			pIndex[nTris++] = m_nCenter; pIndex[nTris++] = n; pIndex[nTris++] = m_nCorner[CORNER_TL];
		}
		else	// ���� �κк����� �ʿ���� ���
		{
			pIndex[nTris++] = m_nCenter; pIndex[nTris++] = m_nCorner[CORNER_BL]; pIndex[nTris++] = m_nCorner[CORNER_TL];
		}
		
		if (!b[EDGE_RT]) // ���� �κк����� �ʿ��Ѱ�?
		{
			n = (m_nCorner[CORNER_TR] + m_nCorner[CORNER_BR]) / 2;
			pIndex[nTris++] = m_nCenter; pIndex[nTris++] = m_nCorner[CORNER_TR]; pIndex[nTris++] = n;
			pIndex[nTris++] = m_nCenter; pIndex[nTris++] = n; pIndex[nTris++] = m_nCorner[CORNER_BR];
		}
		else	// ���� �κк����� �ʿ���� ���
		{
			pIndex[nTris++] = m_nCenter; pIndex[nTris++] = m_nCorner[CORNER_TR]; pIndex[nTris++] = m_nCorner[CORNER_BR];
		}


		return nTris;
	}

	// �ڽ� ���� �˻�
	if( m_pChild[CORNER_TL] ) nTris = m_pChild[CORNER_TL]->_GenTriIndex( nTris, pIndex,pHeightMap,isCull);
	if( m_pChild[CORNER_TR] ) nTris = m_pChild[CORNER_TR]->_GenTriIndex( nTris, pIndex,pHeightMap,isCull);
	if( m_pChild[CORNER_BL] ) nTris = m_pChild[CORNER_BL]->_GenTriIndex( nTris, pIndex,pHeightMap,isCull);
	if( m_pChild[CORNER_BR] ) nTris = m_pChild[CORNER_BR]->_GenTriIndex( nTris, pIndex,pHeightMap,isCull);

	return nTris;
}

// ��� �ڽĳ���� m_bCulled���� FALSE�� �Ѵ�
void	ZQuadTree::_AllInFrustum()
{
	m_bCulled = FALSE;
	if (!m_pChild[0]) return;
	m_pChild[0]->_AllInFrustum();
	m_pChild[1]->_AllInFrustum();
	m_pChild[2]->_AllInFrustum();
	m_pChild[3]->_AllInFrustum();
}

int ZQuadTree::_IsInFrustum( const VertexType* pHeightMap)
{
	BOOL	b[4];
	BOOL	bInSphere;

	bInSphere = ZFrustum::Get()->IsInSphere(&pHeightMap[m_nCenter].position, m_fRadius );
	if( !bInSphere ) return FRUSTUM_OUT;	

	b[0] =  ZFrustum::Get()->IsIn( &pHeightMap[m_nCorner[0]].position );
	b[1] =  ZFrustum::Get()->IsIn( &pHeightMap[m_nCorner[1]].position );
	b[2] =  ZFrustum::Get()->IsIn( &pHeightMap[m_nCorner[2]].position );
	b[3] =  ZFrustum::Get()->IsIn( &pHeightMap[m_nCorner[3]].position );

	// 4����� �������� �ȿ� ����
	if( (b[0] + b[1] + b[2] + b[3]) == 4 ) return FRUSTUM_COMPLETELY_IN;

	// �Ϻκ��� �������ҿ� �ִ� ���
	return FRUSTUM_PARTIALLY_IN;
}

// _IsInFrustum()�Լ��� ����� ���� �������� �ø� ����
void	ZQuadTree::_FrustumCull( const VertexType* pHeightMap )
{
	int ret;

	ret = _IsInFrustum( pHeightMap );
	switch( ret )
	{
		case FRUSTUM_COMPLETELY_IN :	// �������ҿ� ��������, ������� �˻� �ʿ����
			_AllInFrustum();
			return;
		case FRUSTUM_PARTIALLY_IN :		// �������ҿ� �Ϻ�����, ������� �˻� �ʿ���
			m_bCulled = FALSE;
			break;
		case FRUSTUM_OUT :				// �������ҿ��� �������, ������� �˻� �ʿ����
			m_bCulled = TRUE;
			return;
	}
	if( m_pChild[0] ) m_pChild[0]->_FrustumCull( pHeightMap);
	if( m_pChild[1] ) m_pChild[1]->_FrustumCull( pHeightMap);
	if( m_pChild[2] ) m_pChild[2]->_FrustumCull( pHeightMap);
	if( m_pChild[3] ) m_pChild[3]->_FrustumCull( pHeightMap);
}

// ����Ʈ���� �����.
// 4�� ����(���,�ϴ�,����,����)�� �̿���� �ε����� ���Ѵ�.
int	ZQuadTree::_GetNodeIndex(int ed, int cx, int& _0, int& _1, int& _2, int& _3)
{
	int		n, _a, _b, _c, _d, gap;

	_a = _0;
	_b = _1;
	_c = _2;
	_d = _3;
	gap = _b - _a;	// ���� ����� �¿�����

	switch (ed)
	{
		case EDGE_UP:	// ���� ���� �̿������ �ε���
			_0 = _a + cx * gap;
			_1 = _b + cx * gap;
			_2 = _a;
			_3 = _b;
			break;
		case EDGE_DN:	// �Ʒ� ���� �̿������ �ε���
			_0 = _c;
			_1 = _d;
			_2 = _c - cx * gap;
			_3 = _d - cx * gap;
			break;
		case EDGE_LT:	// ���� ���� �̿������ �ε���
			_0 = _a - gap;
			_1 = _a;
			_2 = _c - gap;
			_3 = _c;
			if ((_0 / cx) != (_a / cx)) return -1;
			if ((_1 / cx) != (_b / cx)) return -1;
			break;
		case EDGE_RT:	// ���� ���� �̿������ �ε���
			_0 = _b;
			_1 = _b + gap;
			_2 = _d;
			_3 = _d + gap;
			if ((_0 / cx) != (_a / cx)) return -1;
			if ((_1 / cx) != (_b / cx)) return -1;
			break;
	}

	n = (_0 + _1 + _2 + _3) / 4;	// ��� �ε���
	if (!IS_IN_RANGE(n, 0, cx * cx - 1)) return -1;
	return n;
}

// ����Ʈ���� �˻��ؼ� 4�� �ڳʰ��� ��ġ�ϴ� ��带 ã�´�.
ZQuadTree* ZQuadTree::_FindNode(const VertexType* pHeightMap, int _0, int _1, int _2, int _3)
{
	ZQuadTree*	p = NULL;
	// ��ġ�ϴ� ����� ��尪�� ����
	if ((m_nCorner[0] == _0) && (m_nCorner[1] == _1) && (m_nCorner[2] == _2) && (m_nCorner[3] == _3))
		return this;

	// �ڽ� ��尡 �ְ�?
	if (m_pChild[0])
	{
		RECT	rc;
		POINT	pt;
		int n = (_0 + _1 + _2 + _3) / 4;

		// ���� �ʻ󿡼��� ��ġ
		pt.x = (int)pHeightMap[n].position.x;
		pt.y = (int)pHeightMap[n].position.z;

		// 4���� �ڳʰ��� �������� �ڽĳ���� �� ���������� ��´�.
		SetRect(&rc, (int)pHeightMap[m_pChild[0]->m_nCorner[CORNER_TL]].position.x,
			(int)pHeightMap[m_pChild[0]->m_nCorner[CORNER_TL]].position.z,
			(int)pHeightMap[m_pChild[0]->m_nCorner[CORNER_BR]].position.x,
			(int)pHeightMap[m_pChild[0]->m_nCorner[CORNER_BR]].position.z);
		// pt���� ���������ȿ� �ִٸ� �ڽĳ��� ����.
		if (IsInRect(&rc, pt))
			return m_pChild[0]->_FindNode(pHeightMap, _0, _1, _2, _3);

		SetRect(&rc, (int)pHeightMap[m_pChild[1]->m_nCorner[CORNER_TL]].position.x,
			(int)pHeightMap[m_pChild[1]->m_nCorner[CORNER_TL]].position.z,
			(int)pHeightMap[m_pChild[1]->m_nCorner[CORNER_BR]].position.x,
			(int)pHeightMap[m_pChild[1]->m_nCorner[CORNER_BR]].position.z);
		if (IsInRect(&rc, pt))
			return m_pChild[1]->_FindNode(pHeightMap, _0, _1, _2, _3);

		SetRect(&rc, (int)pHeightMap[m_pChild[2]->m_nCorner[CORNER_TL]].position.x,
			(int)pHeightMap[m_pChild[2]->m_nCorner[CORNER_TL]].position.z,
			(int)pHeightMap[m_pChild[2]->m_nCorner[CORNER_BR]].position.x,
			(int)pHeightMap[m_pChild[2]->m_nCorner[CORNER_BR]].position.z);
		if (IsInRect(&rc, pt))
			return m_pChild[2]->_FindNode(pHeightMap, _0, _1, _2, _3);

		SetRect(&rc, (int)pHeightMap[m_pChild[3]->m_nCorner[CORNER_TL]].position.x,
			(int)pHeightMap[m_pChild[3]->m_nCorner[CORNER_TL]].position.z,
			(int)pHeightMap[m_pChild[3]->m_nCorner[CORNER_BR]].position.x,
			(int)pHeightMap[m_pChild[3]->m_nCorner[CORNER_BR]].position.z);
		if (IsInRect(&rc, pt))
			return m_pChild[3]->_FindNode(pHeightMap, _0, _1, _2, _3);
	}

	return NULL;
}

// �̿���带 �����.(�ﰢ�� ������ ������)
void	ZQuadTree::_BuildNeighborNode(ZQuadTree* pRoot, const VertexType* pHeightMap, int cx)
{
	int				n;
	int				_0, _1, _2, _3;

	for (int i = 0; i<4; i++)
	{
		_0 = m_nCorner[0];
		_1 = m_nCorner[1];
		_2 = m_nCorner[2];
		_3 = m_nCorner[3];
		// �̿������ 4�� �ڳʰ��� ��´�.
		n = _GetNodeIndex(i, cx, _0, _1, _2, _3);
		// �ڳʰ����� �̿������ �����͸� ���´�.
		if (n >= 0) m_pNeighbor[i] = pRoot->_FindNode(pHeightMap, _0, _1, _2, _3);
	}

	// �ڽĳ��� ���ȣ��
	if (m_pChild[0])
	{
		m_pChild[0]->_BuildNeighborNode(pRoot, pHeightMap, cx);
		m_pChild[1]->_BuildNeighborNode(pRoot, pHeightMap, cx);
		m_pChild[2]->_BuildNeighborNode(pRoot, pHeightMap, cx);
		m_pChild[3]->_BuildNeighborNode(pRoot, pHeightMap, cx);
	}
}

// ����Ʈ���� �����.(Build()�Լ����� �Ҹ���)
BOOL	ZQuadTree::_BuildQuadTree(const VertexType* pHeightMap)
{
	if (_SubDivide())
	{
		// ������ܰ�, ���� �ϴ��� �Ÿ��� ���Ѵ�.
		D3DXVECTOR3 v = pHeightMap[m_nCorner[CORNER_TL]].position - pHeightMap[m_nCorner[CORNER_BR]].position;
		// v�� �Ÿ����� �� ��带 ���δ� ��豸�� �����̹Ƿ�, 
		// 2�� ������ �������� ���Ѵ�.
		float a = CORNER_TL + CORNER_BR;
		m_fRadius = D3DXVec3Length(&v) / 2.0f;
		m_pChild[CORNER_TL]->Build(pHeightMap);
		m_pChild[CORNER_TR]->Build(pHeightMap);
		m_pChild[CORNER_BL]->Build(pHeightMap);
		m_pChild[CORNER_BR]->Build(pHeightMap);
	}
	return TRUE;
}


// QuadTree�� �����Ѵ�.
BOOL	ZQuadTree::Build(const VertexType* pHeightMap)
{
	// ����Ʈ�� ����
	_BuildQuadTree(pHeightMap);
	// �̿���� ����
	_BuildNeighborNode(this, pHeightMap, rootCorner(CORNER_BR) + 1);
	return TRUE;
}

//	�ﰢ���� �ε����� �����, ����� �ﰢ���� ������ ��ȯ�Ѵ�.
int	ZQuadTree::GenerateIndex( UINT* pIndex,const VertexType* pHeightMap, bool isCull)
{
	if(isCull)_FrustumCull( pHeightMap);
	return _GenTriIndex( 0, pIndex, pHeightMap,isCull);
}
