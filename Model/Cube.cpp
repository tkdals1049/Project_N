#include "../stdafx.h"
#include "Cube.h"

Cube* Cube::instance = NULL;
Cube * Cube::Get()
{
	if (instance == NULL)
		instance = new Cube();

	return instance;
}

void Cube::Delete()
{
	SAFE_DELETE(instance);
}

Cube::Cube()
	: other(10.0f)
{
	shader = new Shader(Shaders + L"Domain.hlsl");
	worldBuffer = new WorldBuffer();
	CreateBuffer();
}

Cube::~Cube()
{
	SAFE_DELETE_ARRAY(vertex);
	SAFE_RELEASE(vertexBuffer);
	SAFE_RELEASE(indexBuffer);
}

void Cube::Update(D3DXMATRIX& world)
{
	worldBuffer->SetMatrix(world);
}

void Cube::Render()
{
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


void Cube::ColorChange(D3DXVECTOR4 color)
{
	D3DXCOLOR diffuse=(D3DXCOLOR)color;
	for (UINT i = 0;i < vertexCount;i++)
	{
		vertex[i].color=diffuse; 
	}
	ID3D11DeviceContext* dc = D3D::GetDC();
	dc->UpdateSubresource(vertexBuffer,0,NULL,vertex,0,0);
}

void Cube::CreateBuffer()
{
	HRESULT hr;

	vertexCount=8;
	vertex = NULL;
	vertex=new VertexColor[vertexCount];
	D3DXCOLOR color=D3DXCOLOR(0,0,0,1);

	vertex[0].position = D3DXVECTOR3(-0.5f, -0.5f, -0.5f);
	vertex[1].position = D3DXVECTOR3(-0.5f, -0.5f, 0.5f);
	vertex[2].position = D3DXVECTOR3(0.5f, -0.5f, -0.5f);
	vertex[3].position = D3DXVECTOR3(0.5f, -0.5f, 0.5f);
	vertex[4].position = D3DXVECTOR3(-0.5f, 0.5f, -0.5f);
	vertex[5].position = D3DXVECTOR3(-0.5f, 0.5f, 0.5f);
	vertex[6].position = D3DXVECTOR3(0.5f, 0.5f, -0.5f);
	vertex[7].position = D3DXVECTOR3(0.5f, 0.5f, 0.5f);

	for(UINT i=0;i<vertexCount;i++)
	{
	vertex[i].color=color;
	}
	
	UINT index[] = 
	{
	 0, 1, 1, 3, 2, 3, 2, 0,
	 4, 5, 5, 7, 6, 7, 6, 4,
	 0, 4, 1, 5, 2, 6, 3, 7
	};
	indexCount=vertexCount*3;

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
