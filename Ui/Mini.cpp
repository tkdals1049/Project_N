#include "stdafx.h"
#include "Mini.h"

Mini::Mini()
	: translation(0, 0), scale(1, 1)
{
	shader = new Shader(Shaders + L"MiniMap.hlsl");
	worldBuffer = new WorldBuffer();

	CreateBlock();
	CreateCircle();

	SetScale(scale);
	SetTranslation(translation);

}

Mini::~Mini()
{
	SAFE_DELETE(worldBuffer);
	SAFE_DELETE(shader);

	SAFE_RELEASE(vertexBuffer);
	SAFE_RELEASE(indexBuffer);
	SAFE_RELEASE(vertexBuffer2);
	SAFE_RELEASE(indexBuffer2);

}

void Mini::Update()
{
}

void Mini::Render(D3DXVECTOR2 scale, D3DXVECTOR2 position, RenderMode mode=none)
{
	static int form = 0;
	ID3D11Buffer* vertex;
	ID3D11Buffer* index;
	if (mode==none) return;
	
	ID3D11DeviceContext* dc = D3D::GetDC();

	UINT stride = sizeof(VertexType);
	UINT offset = 0;

	if (mode != minimap) { vertex = vertexBuffer; index = indexBuffer; form = 6; }
	else { vertex = vertexBuffer2; index = indexBuffer2; form = 360 * 3; }

	dc->IASetVertexBuffers(0, 1, &vertex, &stride, &offset);
	dc->IASetIndexBuffer(index, DXGI_FORMAT_R32_UINT, 0);
	dc->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	shader->Render();

	dc->PSSetShaderResources(0, 1, ShaderManager::Get()->GetResourceView(mode));

	SetScale(scale);
	SetTranslation(position);

	worldBuffer->SetMatrix(world);
	worldBuffer->SetVSBuffer(1);

	States::SetDepthStencilOff();
	{
		dc->DrawIndexed(form, 0, 0);
	}
	States::SetDepthStencilDefault();
}

void Mini::CreateBlock()
{
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


	HRESULT hr;
	D3D11_BUFFER_DESC desc;
	D3D11_SUBRESOURCE_DATA data;

	//1. Vertex Buffer
	ZeroMemory(&desc, sizeof(D3D11_BUFFER_DESC));
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.ByteWidth = sizeof(VertexType) * 4;
	desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

	ZeroMemory(&data, sizeof(D3D11_SUBRESOURCE_DATA));
	data.pSysMem = vertex;

	hr = D3D::GetDevice()->CreateBuffer(&desc, &data, &vertexBuffer);
	assert(SUCCEEDED(hr));

	//2. Index Buffer
	ZeroMemory(&desc, sizeof(D3D11_BUFFER_DESC));
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.ByteWidth = sizeof(UINT) * 6;
	desc.BindFlags = D3D11_BIND_INDEX_BUFFER;

	ZeroMemory(&data, sizeof(D3D11_SUBRESOURCE_DATA));
	data.pSysMem = index;

	hr = D3D::GetDevice()->CreateBuffer(&desc, &data, &indexBuffer);
	assert(SUCCEEDED(hr));

	SAFE_DELETE_ARRAY(vertex);
	SAFE_DELETE_ARRAY(index);
}
void Mini::CreateCircle()
{
	int size = 360;
	float mag = 360 / (float)size;
	VertexType* vertex = new VertexType[size+1];
	vertex[0].position = D3DXVECTOR3(0, 0, 0);
	vertex[0].uv = D3DXVECTOR2(0.5f,0.5f);
	for (int i = 1; i < size+1; i++)
	{
		float value = (float)radian * (i-1)*mag;

		vertex[i].position = D3DXVECTOR3(cos(value), sin(value), 0);
		vertex[i].uv = D3DXVECTOR2((float)(cos(value)+1)/2, (float)(1-sin(value))/2);
	}

	UINT* index = new UINT[size*3];
	for (int j = 0; j < size; j++)
	{
		index[j * 3] = 0;
		index[j * 3 + 1] = j==359?1:(2 + j);
		index[j * 3 + 2] = 1 + j;

	}

	HRESULT hr;
	D3D11_BUFFER_DESC desc;
	D3D11_SUBRESOURCE_DATA data;

	//1. Vertex Buffer
	ZeroMemory(&desc, sizeof(D3D11_BUFFER_DESC));
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.ByteWidth = sizeof(VertexType) * (size+1);
	desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

	ZeroMemory(&data, sizeof(D3D11_SUBRESOURCE_DATA));
	data.pSysMem = vertex;

	hr = D3D::GetDevice()->CreateBuffer(&desc, &data, &vertexBuffer2);
	assert(SUCCEEDED(hr));

	//2. Index Buffer
	ZeroMemory(&desc, sizeof(D3D11_BUFFER_DESC));
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.ByteWidth = sizeof(UINT) * size*3;
	desc.BindFlags = D3D11_BIND_INDEX_BUFFER;

	ZeroMemory(&data, sizeof(D3D11_SUBRESOURCE_DATA));
	data.pSysMem = index;

	hr = D3D::GetDevice()->CreateBuffer(&desc, &data, &indexBuffer2);
	assert(SUCCEEDED(hr));

	SAFE_DELETE_ARRAY(vertex);
	SAFE_DELETE_ARRAY(index);
}