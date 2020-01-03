#include "stdafx.h"
#include "Screen.h"
#include "UiPart.h"

Screen::Screen()
{
	shader = new Shader(Shaders + L"Screen.hlsl");
	worldBuffer = new WorldBuffer();
	screenbuffer = new ScreenBuffer();
	part = new UiPart(L"Textures/Ui/blood.png");
	CreateBuffer();
}

Screen::~Screen()
{

	SAFE_DELETE(part);
	SAFE_DELETE(screenbuffer);
	SAFE_DELETE(worldBuffer);
	SAFE_DELETE(shader);

	SAFE_RELEASE(vertexBuffer);
	SAFE_RELEASE(indexBuffer);
}

void Screen::Update()
{

}

void Screen::Render(D3DXVECTOR2 position, D3DXVECTOR2 size)
{
	ID3D11DeviceContext* dc = D3D::GetDC();

	UINT stride = sizeof(VertexType);
	UINT offset = 0;

	dc->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
	dc->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);
	dc->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	D3DXMatrixTranslation(&world, position.x, position.y, 0);
	D3DXMatrixScaling(&scale, size.x, size.y, 0);
	worldBuffer->SetMatrix(scale*world);
	worldBuffer->SetVSBuffer(1);

	screenbuffer->SetSize(size);
	screenbuffer->SetPSBuffer(0);
	shader->Render();

	D3D::GetDC()->PSSetShaderResources(0, 1, ShaderManager::Get()->GetResourceView(screen));

	States::SetDepthStencilOff();
	{
		dc->DrawIndexed(6, 0, 0);
	}
	States::SetDepthStencilDefault();

	if(screenbuffer->GetMode()==1)part->Render(D3DXVECTOR4(-1,-1,1,1), position, size);

}

void Screen::CreateBuffer()
{
	VertexType* vertex = new VertexType[4];
	vertex[0].position = D3DXVECTOR3(0, 0, 0);
	vertex[0].uv = D3DXVECTOR2(0, 1);

	vertex[1].position = D3DXVECTOR3(0, 1, 0);
	vertex[1].uv = D3DXVECTOR2(0, 0);

	vertex[2].position = D3DXVECTOR3(1, 0, 0);
	vertex[2].uv = D3DXVECTOR2(1, 1);

	vertex[3].position = D3DXVECTOR3(1, 1, 0);
	vertex[3].uv = D3DXVECTOR2(1, 0);

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

void Screen::SetMode()
{
	static float mode = 0;
	screenbuffer->SetMode(++mode >= 3 ? mode = 0 : mode);
}