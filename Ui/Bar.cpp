#include "stdafx.h"
#include "Bar.h"


Bar::Bar(D3DXCOLOR color) :dmg(0)
{
	shader = new Shader(Shaders + L"Color.hlsl");
	worldBuffer = new WorldBuffer();
	barbuffer = new BarBuffer();
	barbuffer->SetColor(color);

	CreateBuffer();
}

Bar::~Bar()
{
	SAFE_DELETE(barbuffer);
	SAFE_DELETE(worldBuffer);
	SAFE_DELETE(shader);

	SAFE_RELEASE(vertexBuffer);
	SAFE_RELEASE(indexBuffer);

	SAFE_DELETE_ARRAY(index);
	SAFE_DELETE_ARRAY(vertex);
}

void Bar::Update()
{

}

void Bar::Render(D3DXVECTOR2 position, D3DXVECTOR2 size)
{
	if (dmg != 0)
	{
		if (barbuffer->GageCompare()) barbuffer->SetGageX(-(float)dmg / 100);
		else 
		{
			barbuffer->SetGageY(-(float)1 / 100); dmg--;
		}
	}

	D3DXMatrixTranslation(&world, position.x, position.y, 0);
	D3DXMatrixScaling(&scale, size.x, size.y, 0);
	worldBuffer->SetMatrix(scale*world);
	
	ID3D11DeviceContext* dc = D3D::GetDC();

	UINT stride = sizeof(VertexType);
	UINT offset = 0;

	dc->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
	dc->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);
	dc->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	worldBuffer->SetVSBuffer(1);
	barbuffer->SetPSBuffer(0);

	shader->Render();

	States::SetDepthStencilMaskZero();
	States::SetBlendOn();
	{
		dc->DrawIndexed(6, 0, 0);
	}
	States::SetBlendOff();
	States::SetDepthStencilDefault();
}

void Bar::CreateBuffer()
{
	HRESULT hr;

	vertex = NULL;
	vertexCount = 4;
	vertex = new VertexType[vertexCount];
	vertex[0].position = D3DXVECTOR3(-1, -1, 0);
	vertex[1].position = D3DXVECTOR3(-1, 1, 0);
	vertex[2].position = D3DXVECTOR3(1, -1, 0);
	vertex[3].position = D3DXVECTOR3(1, 1, 0);
	vertex[0].uv = D3DXVECTOR2(0,0);
	vertex[1].uv = D3DXVECTOR2(0,1);
	vertex[2].uv = D3DXVECTOR2(1,0);
	vertex[3].uv = D3DXVECTOR2(1,1);

	index = NULL;
	indexCount = 6;
	index = new UINT[indexCount]{ 0,1,2,2,1,3 };

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
void Bar::Damage(UINT dmg)
{
	this->dmg = dmg;
}