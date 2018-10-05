#include "../stdafx.h"
#include "Mini.h"

Mini::Mini()
	: translation(0, 0), scale(1, 1)
{
	shader = new Shader(Shaders + L"MiniMap.hlsl");
	worldBuffer = new WorldBuffer();

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


	D3DXVECTOR2 half = D3DXVECTOR2(0.5f, 0.5f);
	D3DXMatrixTranslation(&center, half.x, half.y, 0);
	D3DXMatrixInverse(&invCenter, NULL, &center);

	SetScale(scale);
	SetTranslation(translation);


	D3DDesc Windesc;
	D3D::GetDesc(&Windesc);

	D3DXMatrixOrthoOffCenterLH(
		&ortho
		,0, (float)Windesc.Width
		,0, (float)Windesc.Height
		, 0, 1
	);
	D3DXMatrixIdentity(&view);
}

Mini::~Mini()
{
	SAFE_RELEASE(vertexBuffer);
	SAFE_RELEASE(indexBuffer);
}

void Mini::Update()
{
}

void Mini::Render()
{

	CameraManager::Get()->SetViewProjection(view,ortho);

	ID3D11DeviceContext* dc = D3D::GetDC();

	UINT stride = sizeof(VertexType);
	UINT offset = 0;

	dc->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
	dc->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);
	dc->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	shader->Render();

	dc->PSSetShaderResources(0, 1, ShaderManager::Get()->GetResourceView(0));

	D3DXMATRIX T2;
	D3DXMatrixTranslation(&T2, 360, translation.y, 0);
	world = S * T;
	worldBuffer->SetMatrix(world);
	worldBuffer->SetVSBuffer(1);

	States::SetDepthStencilOff();
	{
		dc->DrawIndexed(6, 0, 0);
	}
	States::SetDepthStencilDefault();

	world = S * T*T2;
	worldBuffer->SetMatrix(world);
	worldBuffer->SetVSBuffer(1);

	dc->PSSetShaderResources(0, 1, ShaderManager::Get()->GetResourceView(1));

	States::SetDepthStencilOff();
	{
		dc->DrawIndexed(6, 0, 0);
	}
	States::SetDepthStencilDefault();

	CameraManager::Get()->DefaultCamera();
}
