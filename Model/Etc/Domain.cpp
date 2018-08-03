#include "../../stdafx.h"
#include "Domain.h"

Domain* Domain::instance = NULL;
Domain * Domain::Get()
{
	if (instance == NULL)
		instance = new Domain();

	return instance;
}

void Domain::Delete()
{
	SAFE_DELETE(instance);
}

Domain::Domain()
	: other(10.0f)
{
	shader = new Shader(Shaders + L"Domain.hlsl");
	worldBuffer = new WorldBuffer();
	CreateBuffer();
}

Domain::~Domain()
{
	SAFE_DELETE_ARRAY(vertex);
	SAFE_RELEASE(vertexBuffer);
	SAFE_RELEASE(indexBuffer);
}


void Domain::Render(D3DXMATRIX* world=NULL)
{
	if(world=NULL) D3DXMatrixIdentity(world);
	worldBuffer->SetMatrix(*world);
	ID3D11DeviceContext* dc = D3D::GetDC();

	UINT stride = sizeof(VertexColor);
	UINT offset = 0;

	dc->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
	dc->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);
	dc->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);

	worldBuffer->SetVSBuffer(1);

	shader->Render();
	{
		dc->DrawIndexed(indexCount, 0, 0);
	}
}


void Domain::ColorChange(D3DXVECTOR4 color)
{
	D3DXCOLOR diffuse=(D3DXCOLOR)color;
	for (UINT i = 0;i < vertexCount;i++)
	{
		vertex[i].color=diffuse; 
	}
	ID3D11DeviceContext* dc = D3D::GetDC();
	dc->UpdateSubresource(vertexBuffer,0,NULL,vertex,0,0);
}

void Domain::CreateBuffer()
{
	HRESULT hr;

	int count= 180;
	vertexCount=count*3;
	vertex = NULL;
	vertex=new VertexColor[vertexCount];
	D3DXCOLOR color=D3DXCOLOR(0,0,0,1);
	for(UINT i=0;i<vertexCount;i++)
	{
	float degree= i*(360/count)*(float)D3DX_PI / 180;
	int a=i/count;
	switch(a)
	{
	case 0:	vertex[i].position = D3DXVECTOR3(cos(degree), 0, sin(degree));break;
	case 1:	vertex[i].position = D3DXVECTOR3(cos(degree), sin(degree),0);break;
	case 2:	vertex[i].position = D3DXVECTOR3(0,cos(degree), sin(degree));break;
	}
	vertex[i].color=color;
	}

	index = NULL;
	indexCount=vertexCount*2;
	index=new UINT[indexCount];
	for (UINT i = 0; i < indexCount / 2;i++)
	{
		index[2 * i] = i;
		index[2 * i+1] = i+1;
		if(i%count==count-1)index[2 * i + 1]=i-count+1;
	}

	D3D11_BUFFER_DESC desc;
	D3D11_SUBRESOURCE_DATA data;

	//1. Vertex Buffer
	ZeroMemory(&desc, sizeof(D3D11_BUFFER_DESC));
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.ByteWidth = sizeof(VertexColor) * vertexCount;
	desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

	ZeroMemory(&data, sizeof(D3D11_SUBRESOURCE_DATA));
	data.pSysMem = vertex;

	hr = D3D::GetDevice()->CreateBuffer(&desc, &data, &vertexBuffer);
	assert(SUCCEEDED(hr));

	//2. Index Buffer
	ZeroMemory(&desc, sizeof(D3D11_BUFFER_DESC));
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.ByteWidth = sizeof(UINT) * indexCount;
	desc.BindFlags = D3D11_BIND_INDEX_BUFFER;

	ZeroMemory(&data, sizeof(D3D11_SUBRESOURCE_DATA));
	data.pSysMem = index;

	hr = D3D::GetDevice()->CreateBuffer(&desc, &data, &indexBuffer);
	assert(SUCCEEDED(hr));
}
 