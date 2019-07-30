#include "../../stdafx.h"
#include "Look.h"
#include "../../Utilities/Mesh.h"

Look::Look()
{

	shader = new Shader(Shaders + L"Look.hlsl");
	worldBuffer = new WorldBuffer();
	D3DXMatrixIdentity(&world);
	D3DXMatrixScaling(&scale, 500, 500, 500);
	worldBuffer->SetMatrix(scale* world);
	D3DXMatrixIdentity(&view);
	D3DXMatrixLookAtLH(&view, &D3DXVECTOR3(0,0,0), &D3DXVECTOR3(0,0,1), &D3DXVECTOR3(0, 1, 0));
	D3DXMatrixPerspectiveFovLH(&proj, (float)D3DX_PI /3, 1.0f, 0.1f, 100.0f);
	Make();
	CreateBuffer();
	ZeroMemory(m_vtx, sizeof(m_vtx[0]) * 8);
	ZeroMemory(m_plane, sizeof(m_plane[0]) * 6);
}

Look::~Look()
{
	SAFE_DELETE(index);
	SAFE_DELETE(vertex);
}

void Look::PreUpdate()
{
}

void Look::Update()
{

}

void Look::Render()
{
	ID3D11DeviceContext* dc = D3D::GetDC();

	UINT stride = sizeof(VertexType);
	UINT offset = 0;

	dc->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
	dc->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);
	dc->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	worldBuffer->SetVSBuffer(1);

	shader->Render();

		dc->DrawIndexed(indexCount, 0, 0);

}
void Look::SetWorld(D3DXMATRIX world)
{
	memcpy(&this->world, &world, sizeof(D3DXMATRIX));
	worldBuffer->SetMatrix(scale*world);
	D3DXVECTOR3 pos,forward;
	D3DXVec3TransformCoord(&pos, &D3DXVECTOR3(0, 0, 0), &world);
	D3DXVec3TransformCoord(&forward, &D3DXVECTOR3(1, 0, 0), &world);
	D3DXMatrixLookAtLH(&view, &pos, &forward, &D3DXVECTOR3(0, 1, 0));
	Make();
}

// 카메라(view) * 프로젝션(projection)행렬을 입력받아 6개의 평면을 만든다.
BOOL Look::Make()
{
	int				i;
	D3DXMATRIX	matInv;

	// 투영행렬까지 거치면 모든 3차원 월드좌표의 점은 (-1,-1,0) ~ (1,1,1)사이의 값으로 바뀐다.
	// m_vtx에 이 동차공간의 경계값을 넣어둔다.
	m_vtx[0].x = -1.0f;	m_vtx[0].y = -1.0f;	m_vtx[0].z = 0.0f;
	m_vtx[1].x = 1.0f;	m_vtx[1].y = -1.0f;	m_vtx[1].z = 0.0f;
	m_vtx[2].x = 1.0f;	m_vtx[2].y = -1.0f;	m_vtx[2].z = 1.0f;
	m_vtx[3].x = -1.0f;	m_vtx[3].y = -1.0f;	m_vtx[3].z = 1.0f;
	m_vtx[4].x = -1.0f;	m_vtx[4].y = 1.0f;	m_vtx[4].z = 0.0f;
	m_vtx[5].x = 1.0f;	m_vtx[5].y = 1.0f;	m_vtx[5].z = 0.0f;
	m_vtx[6].x = 1.0f;	m_vtx[6].y = 1.0f;	m_vtx[6].z = 1.0f;
	m_vtx[7].x = -1.0f;	m_vtx[7].y = 1.0f;	m_vtx[7].z = 1.0f;

	// view * proj의 역행렬을 구한다.
	D3DXMATRIX dummy = view * proj;
	D3DXMatrixInverse(&matInv, NULL, &dummy);

	for (i = 0; i < 8; i++)
		D3DXVec3TransformCoord(&m_vtx[i], &m_vtx[i], &matInv);

	// 0번과 5번은 프러스텀중 near평면의 좌측상단과 우측하단이므로, 둘의 좌표를 더해서 2로 나누면
	// 카메라의 좌표를 얻을 수 있다.(정확히 일치하는 것은 아니다.)
	m_vPos = (m_vtx[0] + m_vtx[5]) / 2.0f;

	// 얻어진 월드좌표로 프러스텀 평면을 만든다
	// 벡터가 프러스텀 안쪽에서 바깥쪽으로 나가는 평면들이다.
	D3DXPlaneFromPoints(&m_plane[0], m_vtx + 4, m_vtx + 7, m_vtx + 6);	// 상 평면(top)
	D3DXPlaneFromPoints(&m_plane[1], m_vtx, m_vtx + 1, m_vtx + 2);	// 하 평면(bottom)
	D3DXPlaneFromPoints(&m_plane[2], m_vtx, m_vtx + 4, m_vtx + 5);	// 근 평면(near)
	D3DXPlaneFromPoints(&m_plane[3], m_vtx + 2, m_vtx + 6, m_vtx + 7);	// 원 평면(far)
	D3DXPlaneFromPoints(&m_plane[4], m_vtx, m_vtx + 3, m_vtx + 7);	// 좌 평면(left)
	D3DXPlaneFromPoints(&m_plane[5], m_vtx + 1, m_vtx + 5, m_vtx + 6);	// 우 평면(right)

	return TRUE;
}

/// 한점 v가 프러스텀안에 있으면 TRUE를 반환, 아니면 FALSE를 반환한다.
BOOL Look::IsIn(const D3DXVECTOR3* pv)
{

	float		fDist;
	for (int i = 0; i < 6; i++)
	{
		fDist = D3DXPlaneDotCoord(&m_plane[i], pv);
		if (fDist > PLANE_EPSILON) return FALSE;
	}
	return TRUE;
}

BOOL Look::IsInSphere(const D3DXVECTOR3* pv, float radius)
{
	float		fDist;

	for (int i = 0; i < 6; i++)
	{
		fDist = D3DXPlaneDotCoord(&m_plane[i], pv);
		if (fDist > (radius + PLANE_EPSILON)) return FALSE;
	}

	return TRUE;
}
void Look::CreateBuffer()
{
	float height=(float)sqrt(3);
	vertexCount = 5;
	vertex = new VertexType[vertexCount];
	vertex[0].position = D3DXVECTOR3(0, 0, 0);
	vertex[1].position = D3DXVECTOR3(height, -1, -1);
	vertex[2].position = D3DXVECTOR3(height, 1, -1);
	vertex[3].position = D3DXVECTOR3(height, -1, 1);
	vertex[4].position = D3DXVECTOR3(height, 1, 1);

	for (int i = 0; i < (int)vertexCount; i++) vertex[i].color = D3DXCOLOR(0, 0, 0, 1);

	indexCount = (vertexCount + 1) * 3;
	index = new UINT[indexCount]{0,1,2,0,2,4,0,4,3,0,3,1,1,3,2,2,3,4};

	D3D11_BUFFER_DESC desc = { 0 };
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.ByteWidth = sizeof(VertexType) * vertexCount;
	desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

	D3D11_SUBRESOURCE_DATA data = { 0 };
	data.pSysMem = vertex;

	HRESULT hr;
	hr = D3D::GetDevice()->CreateBuffer(&desc, &data, &vertexBuffer);
	assert(SUCCEEDED(hr));


	desc = { 0 };
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.ByteWidth = sizeof(UINT) * indexCount;
	desc.BindFlags = D3D11_BIND_INDEX_BUFFER;

	data = { 0 };
	data.pSysMem = index;

	hr = D3D::GetDevice()->CreateBuffer(&desc, &data, &indexBuffer);
	assert(SUCCEEDED(hr));
}