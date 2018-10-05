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

// 최초 루트노드 생성자
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

	// 루트노드의 4개 코너값 설정
	m_nCorner[CORNER_BL] = 0;
	m_nCorner[CORNER_BR] = cx;
	m_nCorner[CORNER_TL] = (cx + 1) * cy ;
	m_nCorner[CORNER_TR] = (cx + 1) * cy + cx;

	m_nCenter=(m_nCorner[CORNER_TL]+m_nCorner[CORNER_TR]+m_nCorner[CORNER_BL]+m_nCorner[CORNER_BR])/4;

	m_fRadius = 0.0f;
	m_bCulled = FALSE;
	m_fRadius = 0.0f;

}

// 하위 자식노드 생성자
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

// 소멸자
ZQuadTree::~ZQuadTree()
{
	_Destroy();
}

// 메모리에서 쿼드트리를 삭제한다.
void	ZQuadTree::_Destroy()
{
	// 자식 노드들을 소멸 시킨다.
	for( int i = 0 ; i < 4 ; i++ ) SAFE_DELETE( m_pChild[i] );
}


// 4개의 코너값을 셋팅한다.
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


// 자식 노드를 추가한다.
ZQuadTree*	ZQuadTree::_AddChild( int nCornerTL, int nCornerTR, int nCornerBL, int nCornerBR )
{
	ZQuadTree*	pChild;

	pChild = new ZQuadTree( this );
	pChild->_SetCorners( nCornerTL, nCornerTR, nCornerBL, nCornerBR );

	return pChild;
}

// Quadtree를 4개의 자식 트리로 부분분할(subdivide)한다.
BOOL	ZQuadTree::_SubDivide()
{
	int		nTopEdgeCenter;
	int		nBottomEdgeCenter;
	int		nLeftEdgeCenter;
	int		nRightEdgeCenter;
	int		nCentralPoint;

	// 상단변 가운데
	nTopEdgeCenter		= ( m_nCorner[CORNER_TL] + m_nCorner[CORNER_TR] ) / 2;
	// 하단변 가운데 
	nBottomEdgeCenter	= ( m_nCorner[CORNER_BL] + m_nCorner[CORNER_BR] ) / 2;
	// 좌측변 가운데
	nLeftEdgeCenter		= ( m_nCorner[CORNER_TL] + m_nCorner[CORNER_BL] ) / 2;
	// 우측변 가운데
	nRightEdgeCenter	= ( m_nCorner[CORNER_TR] + m_nCorner[CORNER_BR] ) / 2;
	// 한가운데
	nCentralPoint		= ( m_nCorner[CORNER_TL] + m_nCorner[CORNER_TR] + 
							m_nCorner[CORNER_BL] + m_nCorner[CORNER_BR] ) / 4;

	// 더이상 분할이 불가능한가? 그렇다면 SubDivide() 종료
	if( (m_nCorner[CORNER_TR] - m_nCorner[CORNER_TL]) <= 1 )
	{
		return FALSE;
	}

	// 4개의 자식노드 추가
	m_pChild[CORNER_TL] = _AddChild( m_nCorner[CORNER_TL], nTopEdgeCenter, nLeftEdgeCenter, nCentralPoint );
	m_pChild[CORNER_TR] = _AddChild( nTopEdgeCenter, m_nCorner[CORNER_TR], nCentralPoint, nRightEdgeCenter );
	m_pChild[CORNER_BL] = _AddChild( nLeftEdgeCenter, nCentralPoint, m_nCorner[CORNER_BL], nBottomEdgeCenter );
	m_pChild[CORNER_BR] = _AddChild( nCentralPoint, nRightEdgeCenter, nBottomEdgeCenter, m_nCorner[CORNER_BR] );
	
	return TRUE;
}

// 출력할 폴리곤의 인덱스를 생성한다.
int		ZQuadTree::_GenTriIndex( int nTris, UINT* pIndex, const VertexType* pHeightMap, bool isCull)
{
	if( m_bCulled &&isCull)
	{
		m_bCulled = FALSE;
		return nTris;
	}

	// 현재 노드가 출력되어야 하는가?
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
		// 상단 이웃노드(neightbor node)가 출력가능한가?
		if (m_pNeighbor[EDGE_UP]) b[EDGE_UP] = m_pNeighbor[EDGE_UP]->_IsVisible(pHeightMap,ZFrustum::Get()->GetPos());
		// 하단 이웃노드(neightbor node)가 출력가능한가?
		if (m_pNeighbor[EDGE_DN]) b[EDGE_DN] = m_pNeighbor[EDGE_DN]->_IsVisible(pHeightMap,ZFrustum::Get()->GetPos());
		// 좌측 이웃노드(neightbor node)가 출력가능한가?
		if (m_pNeighbor[EDGE_LT]) b[EDGE_LT] = m_pNeighbor[EDGE_LT]->_IsVisible(pHeightMap,ZFrustum::Get()->GetPos());
		// 우측 이웃노드(neightbor node)가 출력가능한가?
		if (m_pNeighbor[EDGE_RT]) b[EDGE_RT] = m_pNeighbor[EDGE_RT]->_IsVisible(pHeightMap,ZFrustum::Get()->GetPos());

		// 이웃노드들이 모두다 출력가능하다면 현재노드와 이웃노드들이 같은 LOD이므로 
		// 부분분할이 필요없다.

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
		
		if (!b[EDGE_UP]) // 상단 부분분할이 필요한가?
		{
			n = (m_nCorner[CORNER_TL] + m_nCorner[CORNER_TR]) / 2;
			pIndex[nTris++] = m_nCenter; pIndex[nTris++] = m_nCorner[CORNER_TL]; pIndex[nTris++] = n;
			pIndex[nTris++] = m_nCenter; pIndex[nTris++] = n; pIndex[nTris++] = m_nCorner[CORNER_TR];
		}
		else	// 상단 부분분할이 필요없을 경우
		{
			pIndex[nTris++] = m_nCenter; pIndex[nTris++] = m_nCorner[CORNER_TL]; pIndex[nTris++] = m_nCorner[CORNER_TR];
		}
		
		if (!b[EDGE_DN]) // 하단 부분분할이 필요한가?
		{
			n = (m_nCorner[CORNER_BL] + m_nCorner[CORNER_BR]) / 2;
			pIndex[nTris++] = m_nCenter; pIndex[nTris++] = m_nCorner[CORNER_BR]; pIndex[nTris++] = n;
			pIndex[nTris++] = m_nCenter; pIndex[nTris++] = n; pIndex[nTris++] = m_nCorner[CORNER_BL];
		}
		else	// 하단 부분분할이 필요없을 경우
		{
			pIndex[nTris++] = m_nCenter; pIndex[nTris++] = m_nCorner[CORNER_BR]; pIndex[nTris++] = m_nCorner[CORNER_BL];
		}
		
		if (!b[EDGE_LT]) // 좌측 부분분할이 필요한가?
		{
			n = (m_nCorner[CORNER_TL] + m_nCorner[CORNER_BL]) / 2;
			pIndex[nTris++] = m_nCenter; pIndex[nTris++] = m_nCorner[CORNER_BL]; pIndex[nTris++] = n;
			pIndex[nTris++] = m_nCenter; pIndex[nTris++] = n; pIndex[nTris++] = m_nCorner[CORNER_TL];
		}
		else	// 좌측 부분분할이 필요없을 경우
		{
			pIndex[nTris++] = m_nCenter; pIndex[nTris++] = m_nCorner[CORNER_BL]; pIndex[nTris++] = m_nCorner[CORNER_TL];
		}
		
		if (!b[EDGE_RT]) // 우측 부분분할이 필요한가?
		{
			n = (m_nCorner[CORNER_TR] + m_nCorner[CORNER_BR]) / 2;
			pIndex[nTris++] = m_nCenter; pIndex[nTris++] = m_nCorner[CORNER_TR]; pIndex[nTris++] = n;
			pIndex[nTris++] = m_nCenter; pIndex[nTris++] = n; pIndex[nTris++] = m_nCorner[CORNER_BR];
		}
		else	// 우측 부분분할이 필요없을 경우
		{
			pIndex[nTris++] = m_nCenter; pIndex[nTris++] = m_nCorner[CORNER_TR]; pIndex[nTris++] = m_nCorner[CORNER_BR];
		}


		return nTris;
	}

	// 자식 노드들 검색
	if( m_pChild[CORNER_TL] ) nTris = m_pChild[CORNER_TL]->_GenTriIndex( nTris, pIndex,pHeightMap,isCull);
	if( m_pChild[CORNER_TR] ) nTris = m_pChild[CORNER_TR]->_GenTriIndex( nTris, pIndex,pHeightMap,isCull);
	if( m_pChild[CORNER_BL] ) nTris = m_pChild[CORNER_BL]->_GenTriIndex( nTris, pIndex,pHeightMap,isCull);
	if( m_pChild[CORNER_BR] ) nTris = m_pChild[CORNER_BR]->_GenTriIndex( nTris, pIndex,pHeightMap,isCull);

	return nTris;
}

// 모든 자식노드의 m_bCulled값을 FALSE로 한다
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

	// 4개모두 프러스텀 안에 있음
	if( (b[0] + b[1] + b[2] + b[3]) == 4 ) return FRUSTUM_COMPLETELY_IN;

	// 일부분이 프러스텀에 있는 경우
	return FRUSTUM_PARTIALLY_IN;
}

// _IsInFrustum()함수의 결과에 따라 프러스텀 컬링 수행
void	ZQuadTree::_FrustumCull( const VertexType* pHeightMap )
{
	int ret;

	ret = _IsInFrustum( pHeightMap );
	switch( ret )
	{
		case FRUSTUM_COMPLETELY_IN :	// 프러스텀에 완전포함, 하위노드 검색 필요없음
			_AllInFrustum();
			return;
		case FRUSTUM_PARTIALLY_IN :		// 프러스텀에 일부포함, 하위노드 검색 필요함
			m_bCulled = FALSE;
			break;
		case FRUSTUM_OUT :				// 프러스텀에서 완전벗어남, 하위노드 검색 필요없음
			m_bCulled = TRUE;
			return;
	}
	if( m_pChild[0] ) m_pChild[0]->_FrustumCull( pHeightMap);
	if( m_pChild[1] ) m_pChild[1]->_FrustumCull( pHeightMap);
	if( m_pChild[2] ) m_pChild[2]->_FrustumCull( pHeightMap);
	if( m_pChild[3] ) m_pChild[3]->_FrustumCull( pHeightMap);
}

// 쿼드트리를 만든다.
// 4개 방향(상단,하단,좌측,우측)의 이웃노드 인덱스를 구한다.
int	ZQuadTree::_GetNodeIndex(int ed, int cx, int& _0, int& _1, int& _2, int& _3)
{
	int		n, _a, _b, _c, _d, gap;

	_a = _0;
	_b = _1;
	_c = _2;
	_d = _3;
	gap = _b - _a;	// 현재 노드의 좌우폭값

	switch (ed)
	{
		case EDGE_UP:	// 위쪽 방향 이웃노드의 인덱스
			_0 = _a + cx * gap;
			_1 = _b + cx * gap;
			_2 = _a;
			_3 = _b;
			break;
		case EDGE_DN:	// 아래 방향 이웃노드의 인덱스
			_0 = _c;
			_1 = _d;
			_2 = _c - cx * gap;
			_3 = _d - cx * gap;
			break;
		case EDGE_LT:	// 좌측 방향 이웃노드의 인덱스
			_0 = _a - gap;
			_1 = _a;
			_2 = _c - gap;
			_3 = _c;
			if ((_0 / cx) != (_a / cx)) return -1;
			if ((_1 / cx) != (_b / cx)) return -1;
			break;
		case EDGE_RT:	// 우측 방향 이웃노드의 인덱스
			_0 = _b;
			_1 = _b + gap;
			_2 = _d;
			_3 = _d + gap;
			if ((_0 / cx) != (_a / cx)) return -1;
			if ((_1 / cx) != (_b / cx)) return -1;
			break;
	}

	n = (_0 + _1 + _2 + _3) / 4;	// 가운데 인덱스
	if (!IS_IN_RANGE(n, 0, cx * cx - 1)) return -1;
	return n;
}

// 쿼드트리를 검색해서 4개 코너값과 일치하는 노드를 찾는다.
ZQuadTree* ZQuadTree::_FindNode(const VertexType* pHeightMap, int _0, int _1, int _2, int _3)
{
	ZQuadTree*	p = NULL;
	// 일치하는 노드라면 노드값을 리턴
	if ((m_nCorner[0] == _0) && (m_nCorner[1] == _1) && (m_nCorner[2] == _2) && (m_nCorner[3] == _3))
		return this;

	// 자식 노드가 있가?
	if (m_pChild[0])
	{
		RECT	rc;
		POINT	pt;
		int n = (_0 + _1 + _2 + _3) / 4;

		// 현재 맵상에서의 위치
		pt.x = (int)pHeightMap[n].position.x;
		pt.y = (int)pHeightMap[n].position.z;

		// 4개의 코너값을 기준으로 자식노드의 맵 점유범위를 얻는다.
		SetRect(&rc, (int)pHeightMap[m_pChild[0]->m_nCorner[CORNER_TL]].position.x,
			(int)pHeightMap[m_pChild[0]->m_nCorner[CORNER_TL]].position.z,
			(int)pHeightMap[m_pChild[0]->m_nCorner[CORNER_BR]].position.x,
			(int)pHeightMap[m_pChild[0]->m_nCorner[CORNER_BR]].position.z);
		// pt값이 점유범위안에 있다면 자식노드로 들어간다.
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

// 이웃노드를 만든다.(삼각형 찢어짐 방지용)
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
		// 이웃노드의 4개 코너값을 얻는다.
		n = _GetNodeIndex(i, cx, _0, _1, _2, _3);
		// 코너값으로 이웃노드의 포인터를 얻어온다.
		if (n >= 0) m_pNeighbor[i] = pRoot->_FindNode(pHeightMap, _0, _1, _2, _3);
	}

	// 자식노드로 재귀호출
	if (m_pChild[0])
	{
		m_pChild[0]->_BuildNeighborNode(pRoot, pHeightMap, cx);
		m_pChild[1]->_BuildNeighborNode(pRoot, pHeightMap, cx);
		m_pChild[2]->_BuildNeighborNode(pRoot, pHeightMap, cx);
		m_pChild[3]->_BuildNeighborNode(pRoot, pHeightMap, cx);
	}
}

// 쿼드트리를 만든다.(Build()함수에서 불린다)
BOOL	ZQuadTree::_BuildQuadTree(const VertexType* pHeightMap)
{
	if (_SubDivide())
	{
		// 좌측상단과, 우측 하단의 거리를 구한다.
		D3DXVECTOR3 v = pHeightMap[m_nCorner[CORNER_TL]].position - pHeightMap[m_nCorner[CORNER_BR]].position;
		// v의 거리값이 이 노드를 감싸는 경계구의 지름이므로, 
		// 2로 나누어 반지름을 구한다.
		float a = CORNER_TL + CORNER_BR;
		m_fRadius = D3DXVec3Length(&v) / 2.0f;
		m_pChild[CORNER_TL]->Build(pHeightMap);
		m_pChild[CORNER_TR]->Build(pHeightMap);
		m_pChild[CORNER_BL]->Build(pHeightMap);
		m_pChild[CORNER_BR]->Build(pHeightMap);
	}
	return TRUE;
}


// QuadTree를 구축한다.
BOOL	ZQuadTree::Build(const VertexType* pHeightMap)
{
	// 쿼드트리 구축
	_BuildQuadTree(pHeightMap);
	// 이웃노드 구축
	_BuildNeighborNode(this, pHeightMap, rootCorner(CORNER_BR) + 1);
	return TRUE;
}

//	삼각형의 인덱스를 만들고, 출력할 삼각형의 개수를 반환한다.
int	ZQuadTree::GenerateIndex( UINT* pIndex,const VertexType* pHeightMap, bool isCull)
{
	if(isCull)_FrustumCull( pHeightMap);
	return _GenTriIndex( 0, pIndex, pHeightMap,isCull);
}
