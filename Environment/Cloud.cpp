#include "../stdafx.h"
#include "Cloud.h"
#include "../Utilities/PerlinNoise.h"
#include "../Utilities/Mesh.h"

Cloud::Cloud()
{
	CloudResolution = 10;
	CloudWidth = 10.0f;
	CloudTop = 1.0f;
	CloudBottom = -1.0f;
	CloudDepth = sqrt(pow(CloudWidth, 2)) /2;

	cloudData.Data.scale = 0.3f;
	cloudData.Data.translation = 0.0f;

	speed = 0.0001f;

	Initialize();
}

Cloud::~Cloud()
{



	SAFE_DELETE_ARRAY(vertexData);
	SAFE_DELETE_ARRAY(indexData);
	SAFE_RELEASE(vertexBuffer);
	SAFE_RELEASE(indexBuffer);
}

void Cloud::Initialize()
{
	CreateVertexData();
	CreateIndexData();
	CreateBuffer();

	perlinNoise = new PerlinNoise();
	shader = new Shader(Shaders + L"Cloud.hlsl");

	wstring fileName = Contents + L"Textures/cloud001.dds";
	HRESULT hr = D3DX11CreateShaderResourceViewFromFile(D3D::GetDevice(), fileName.c_str(), nullptr, nullptr, &diffuse, nullptr);

	assert(SUCCEEDED(hr));

	MakeCloudPerlin();

}

void Cloud::Update()
{
	D3DXVECTOR3 position;
	position=CameraManager::Get()->GetPosition();
	D3DXMatrixTranslation(&world, position.x, position.y, position.z);

}

void Cloud::Render()
{
	cloudData.Data.translation += speed / 4;
	if (cloudData.Data.translation >= 1.0f) {
		cloudData.Data.translation -= 1.0f;
	}

	UINT stride = sizeof(VertexTexture);
	UINT offset = 0;

	D3D::GetDC()->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
	D3D::GetDC()->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);
	D3D::GetDC()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	if (ShaderManager::Get()->GetOther()==none)
	{
		shader->Render();
		cloudData.SetPSBuffer(1);

		D3D::GetDC()->PSSetShaderResources(0, 1, &diffuse);
		D3D::GetDC()->PSSetShaderResources(1, 1, &perlin);

		States::SetBlendCloud();
		{
			D3D::GetDC()->DrawIndexed(indexCount, 0, 0);
		}

		States::SetBlendOff();
	}
}

void Cloud::MakeCloudPerlin()
{
	perlinNoise->MakePerlinNoise();

	perlin = *perlinNoise->GetPerlinNoise();

	D3DXMatrixIdentity(&world);
}

void Cloud::CreateVertexData()
{

	vertexCount = (CloudResolution + 1) * (CloudResolution + 1);

	float positionX, positionZ;

	vertexData = new VertexTexture[vertexCount];
	for (UINT z = 0; z <= CloudResolution; z++)
	{
		for (UINT x = 0; x <= CloudResolution; x++)
		{
			int index = (CloudResolution + 1) * z + x;

			positionX = (-0.5f * CloudWidth) + ((float)x * CloudWidth / CloudResolution);
			positionZ = (-0.5f * CloudWidth) + ((float)z * CloudWidth / CloudResolution);

			vertexData[index].position.x = positionX;
			vertexData[index].position.z = positionZ;
			vertexData[index].position.y = CloudTop - (CloudTop - CloudBottom) * sqrt((positionX * positionX) + (positionZ * positionZ))/CloudDepth ;

			vertexData[index].uv.x = (float)(x) / CloudResolution;
			vertexData[index].uv.y = (float)(z) / CloudResolution;
		}
	}
}

void Cloud::CreateIndexData()
{
	indexCount = CloudResolution * CloudResolution * 6;

	indexData = new UINT[indexCount];
	UINT count = 0;
	for (UINT z = 0; z < CloudResolution; z++) 
	{
		for (UINT x = 0; x < CloudResolution; x++) 
		{
			indexData[count++]=(CloudResolution+1) * z + x;
			indexData[count++]=(CloudResolution+1) * z + x + 1;
			indexData[count++]=(CloudResolution+1) * (z + 1) + x;
			indexData[count++]=(CloudResolution+1) * z + x + 1;
			indexData[count++]=(CloudResolution+1) * (z + 1) + (x + 1);
			indexData[count++]=(CloudResolution+1) * (z + 1) + x;
		}//for(x)
	}//for(z)
}


void Cloud::CreateBuffer()
{
	HRESULT hr;

	D3D11_BUFFER_DESC desc;
	D3D11_SUBRESOURCE_DATA data;


	//1. Vertex Buffer
	ZeroMemory(&desc, sizeof(D3D11_BUFFER_DESC));
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.ByteWidth = sizeof(VertexTexture) * vertexCount;
	desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

	ZeroMemory(&data, sizeof(D3D11_SUBRESOURCE_DATA));
	data.pSysMem = vertexData;

	hr = D3D::GetDevice()->CreateBuffer(&desc, &data, &vertexBuffer);
	assert(SUCCEEDED(hr));



	//2. Index Buffer
	ZeroMemory(&desc, sizeof(D3D11_BUFFER_DESC));
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.ByteWidth = sizeof(UINT) * indexCount;
	desc.BindFlags = D3D11_BIND_INDEX_BUFFER;

	ZeroMemory(&data, sizeof(D3D11_SUBRESOURCE_DATA));
	data.pSysMem = indexData;

	hr = D3D::GetDevice()->CreateBuffer(&desc, &data, &indexBuffer);
	assert(SUCCEEDED(hr));
}
