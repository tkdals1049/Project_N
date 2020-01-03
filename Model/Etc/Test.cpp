#include "stdafx.h"
#include "Test.h"

Test::Test()
{
	shader = new Shader(Shaders + L"Texture.hlsl");
	worldBuffer = new WorldBuffer();
		
	CreateBuffer();

	D3DXMATRIX S, T;
	D3DXMatrixScaling(&S, 10, 10, 1);
	D3DXMatrixTranslation(&T, 0,0, 0);
	worldBuffer->SetMatrix(S*T);
}

Test::~Test()
{
	SAFE_DELETE(worldBuffer);
	SAFE_DELETE(shader);

	SAFE_RELEASE(texture);
	SAFE_RELEASE(vertexBuffer);
	SAFE_RELEASE(indexBuffer);

	SAFE_DELETE_ARRAY(index);
	SAFE_DELETE_ARRAY(vertex);
}

void Test::PreUpdate()
{
}

void Test::Update()
{

}

void Test::Render()
{

	ID3D11DeviceContext* dc = D3D::GetDC();

	UINT stride = sizeof(VertexType);
	UINT offset = 0;

	dc->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
	dc->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);
	dc->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	worldBuffer->SetVSBuffer(1);

	shader->Render();

	D3D::GetDC()->PSSetShaderResources(0, 1, &texture);

	dc->DrawIndexed(6, 0, 0);

}

void Test::CreateBuffer()
{
	HRESULT hr;

	VertexType* vertex = new VertexType[4];
	vertex[0].position = D3DXVECTOR3(0, 0, 0);
	vertex[0].uv = D3DXVECTOR2(0, 0);

	vertex[1].position = D3DXVECTOR3(0, 1, 0);
	vertex[1].uv = D3DXVECTOR2(0, 1);

	vertex[2].position = D3DXVECTOR3(1, 0, 0);
	vertex[2].uv = D3DXVECTOR2(1, 0);

	vertex[3].position = D3DXVECTOR3(1, 1, 0);
	vertex[3].uv = D3DXVECTOR2(1, 1);

	UINT* index = new UINT[6]{ 0, 1, 2, 2, 1, 3 };


	D3D11_BUFFER_DESC desc;
	D3D11_SUBRESOURCE_DATA data;

	//1. Vertex Buffer
	ZeroMemory(&desc, sizeof(D3D11_BUFFER_DESC));
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.ByteWidth = sizeof(VertexType) * vertexCount;
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