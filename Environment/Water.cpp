#include "stdafx.h"
#include "Water.h"
#include "WaveBuffer.h"

Water::Water(UINT width, UINT height, float depth)
	: wave(0), width(width), height(height),depth(depth), normal (D3DXVECTOR3(0, 1, 0)),size(D3DXVECTOR3(1,1,1))
{

	shader = new Shader(Shaders + L"Water.hlsl");
	worldBuffer = new WorldBuffer();
	waveBuffer = new WaveBuffer();
	reflectBuffer = new ReflectBuffer();

	D3DXMatrixIdentity(&world);
	CreateBuffer();

	wstring waveTexture= Contents + L"Textures/Diffuse/wave0.png";
	HRESULT hr = D3DX11CreateShaderResourceViewFromFile
	(
		D3D::GetDevice()
		, waveTexture.c_str()
		, NULL
		, NULL
		, &normalMap
		, NULL
	);
	assert(SUCCEEDED(hr));
}

Water::~Water()
{
	SAFE_DELETE(shader);

	SAFE_DELETE(worldBuffer);
	SAFE_DELETE(waveBuffer);
	SAFE_DELETE(reflectBuffer);
	SAFE_RELEASE(normalMap);

	SAFE_DELETE_ARRAY(vertex);
	SAFE_DELETE_ARRAY(index);
	
	SAFE_RELEASE(indexBuffer);
	SAFE_RELEASE(vertexBuffer);
}

void Water::CreateBuffer()
{
	size.x = (float)width;
	size.z = (float)height;
	vertexCount = 4;
	indexCount = 6;

	vertex = new VertexType[vertexCount];

	vertex[0].position = D3DXVECTOR3(-0.5f, depth, -0.5f);
	vertex[1].position = D3DXVECTOR3(-0.5f, depth, 0.5f);
	vertex[2].position = D3DXVECTOR3(0.5f, depth, -0.5f);
	vertex[3].position = D3DXVECTOR3(0.5f, depth, 0.5f);

	vertex[0].uv = D3DXVECTOR2(0, 0);
	vertex[1].uv = D3DXVECTOR2(0, 1);
	vertex[2].uv = D3DXVECTOR2(1, 0);
	vertex[3].uv = D3DXVECTOR2(1, 1);
	
	index = new UINT[6]{0,1,2,2,1,3};

	HRESULT hr;

	D3D11_BUFFER_DESC vertexDesc = { 0 };
	vertexDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexDesc.ByteWidth = sizeof(VertexType) * vertexCount;
	vertexDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

	D3D11_SUBRESOURCE_DATA vertexSubResource = { 0 };
	vertexSubResource.pSysMem = vertex;

	hr = D3D::GetDevice()->CreateBuffer(&vertexDesc, &vertexSubResource, &vertexBuffer);
	assert(SUCCEEDED(hr));


	D3D11_BUFFER_DESC indexDesc = { 0 };
	indexDesc.Usage = D3D11_USAGE_DEFAULT;
	indexDesc.ByteWidth = sizeof(UINT) * indexCount;
	indexDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;

	D3D11_SUBRESOURCE_DATA indexSubResource = { 0 };
	indexSubResource.pSysMem = index;

	hr = D3D::GetDevice()->CreateBuffer(&indexDesc, &indexSubResource, &indexBuffer);
	assert(SUCCEEDED(hr));

	D3DXMatrixScaling(&world,size.x,size.y,size.z);
	worldBuffer->SetMatrix(world);
}

void Water::Update()
{
	wave += 0.01f*Time::Get()->Delta();
	if (wave > 1.0f)
	{
		wave -= 1.0f;
	}
	waveBuffer->SetWave(wave);

}

void Water::Render()
{
	reflectBuffer->SetMatrix(ShaderManager::Get()->GetMirrorView());

	ID3D11DeviceContext* dc = D3D::GetDC();

	UINT stride = sizeof(VertexType);
	UINT offset = 0;

	dc->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
	dc->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);
	dc->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	dc->PSSetShaderResources(0, 1, ShaderManager::Get()->GetResourceView(reflect));
	dc->PSSetShaderResources(1, 1, ShaderManager::Get()->GetResourceView(reflact));
	dc->PSSetShaderResources(2, 1, &normalMap);

	worldBuffer->SetVSBuffer(1);
	reflectBuffer->SetVSBuffer(3);
	waveBuffer->SetPSBuffer(1);
	shader->Render();

	dc->DrawIndexed(indexCount, 0, 0);
}

void Water::SetSize(UINT width, UINT height)
{
	this->width=width;
	this->height=height;
	CreateBuffer();
}
