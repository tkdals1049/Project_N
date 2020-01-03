#include "stdafx.h"
#include "UiPart.h"


UiPart::UiPart(wstring file)
{
	shader = new Shader(Shaders + L"Ui.hlsl");
	worldBuffer = new WorldBuffer();

	CreateBuffer();

	fileName = Contents + file;
	HRESULT hr = D3DX11CreateShaderResourceViewFromFile
	(
		D3D::GetDevice()
		, fileName.c_str() 
		, NULL
		, NULL
		, &texture
		, NULL
	);
	assert(SUCCEEDED(hr));
}

UiPart::~UiPart()
{
	SAFE_DELETE(texture);
	SAFE_DELETE(worldBuffer);
	SAFE_DELETE(shader);

	SAFE_RELEASE(vertexBuffer);
	SAFE_RELEASE(indexBuffer);

	SAFE_DELETE_ARRAY(index);
	SAFE_DELETE_ARRAY(vertex);
}

void UiPart::Update()
{

}

void UiPart::Render(D3DXVECTOR4 uv, D3DXVECTOR2 position, D3DXVECTOR2 size )
{
	ID3D11DeviceContext* dc = D3D::GetDC();

	vertex[0].uv = D3DXVECTOR2(uv.x, uv.w);
	vertex[1].uv = D3DXVECTOR2(uv.x, uv.y);
	vertex[2].uv = D3DXVECTOR2(uv.z, uv.w);
	vertex[3].uv = D3DXVECTOR2(uv.z, uv.y);
	dc->UpdateSubresource(vertexBuffer, 0, NULL, vertex, sizeof(VertexType)*vertexCount, 0);

	D3DXMatrixTranslation(&world, position.x, position.y, 0);
	D3DXMatrixScaling(&scale, size.x, size.y, 0);
	worldBuffer->SetMatrix(scale*world);

	UINT stride = sizeof(VertexType);
	UINT offset = 0;

	dc->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
	dc->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);
	dc->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	worldBuffer->SetVSBuffer(1);

	shader->Render();

	D3D::GetDC()->PSSetShaderResources(0, 1, &texture);

	States::SetDepthStencilMaskZero();
	States::SetBlendOn();
	{
		dc->DrawIndexed(6, 0, 0);
	}
	States::SetBlendOff();
	States::SetDepthStencilDefault();

}

void UiPart::CreateBuffer()
{
	HRESULT hr;

	vertex = NULL;
	vertexCount = 4;
	vertex = new VertexType[vertexCount];
	vertex[0].position = D3DXVECTOR3( -1, -1, 0);
	vertex[1].position = D3DXVECTOR3( -1,  1, 0);
	vertex[2].position = D3DXVECTOR3(  1, -1, 0);
	vertex[3].position = D3DXVECTOR3(  1,  1, 0);
	vertex[0].uv = D3DXVECTOR2(0,0);
	vertex[1].uv = D3DXVECTOR2(0,0);
	vertex[2].uv = D3DXVECTOR2(0,0);
	vertex[3].uv = D3DXVECTOR2(0,0);

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