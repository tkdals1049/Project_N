#include "stdafx.h"
#include "Sky.h"
#include "Cloud.h"
#include "../Utilities/Mesh.h"

Sky* Sky::instance = NULL;
Sky * Sky::Get()
{
	if (instance == NULL)
		instance = new Sky();

	return instance;
}

void Sky::Delete()
{
	SAFE_DELETE(instance);
}
Sky::Sky()
	: meshFile(Contents + L"Meshes/Sphere.data")
	, shaderFile(Shaders + L"Sky.hlsl")
	, shader(NULL)
{
	VertexType* vertex = NULL;
	UINT* index = NULL;

	Mesh mesh;
	mesh.Open(meshFile);
	mesh.CopyVertex(&vertex, &vertexCount);
	mesh.CopyIndex(&index, &indexCount);
	
	
	D3D11_BUFFER_DESC desc = { 0 };
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.ByteWidth = sizeof(VertexType) * vertexCount;
	desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

	D3D11_SUBRESOURCE_DATA data = { 0 };
	data.pSysMem = vertex;

	HRESULT hr;
	hr = D3D::GetDevice()->CreateBuffer(&desc, &data, &vertexBuffer);
	assert(SUCCEEDED(hr));

	desc = { 0 };
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.ByteWidth = sizeof(UINT) * indexCount;
	desc.BindFlags = D3D11_BIND_INDEX_BUFFER;

	data = { 0 };
	data.pSysMem = index;

	hr = D3D::GetDevice()->CreateBuffer(&desc, &data, &indexBuffer);
	assert(SUCCEEDED(hr));

	SAFE_DELETE_ARRAY(vertex);
	SAFE_DELETE_ARRAY(index);

	D3DXMatrixIdentity(&world);
	
	skyBuffer = new SkyBuffer();
	worldBuffer = new WorldBuffer();

	shader = new Shader(shaderFile);
	cloud = new Cloud();
}

Sky::~Sky()
{
	SAFE_DELETE(cloud);
	SAFE_DELETE(worldBuffer);
	SAFE_DELETE(skyBuffer);
	SAFE_DELETE(shader);

	SAFE_RELEASE(indexBuffer);
	SAFE_RELEASE(vertexBuffer);
}

void Sky::Update()
{
	D3DXVECTOR3 position = CameraManager::Get()->GetPosition();
	D3DXMatrixTranslation(&world, position.x, position.y, position.z);
	worldBuffer->SetMatrix(world);

	cloud->Update();
}

void Sky::Render()
{
	UINT stride = sizeof(VertexType);
	UINT offset = 0;

	D3D::GetDC()->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
	D3D::GetDC()->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);
	D3D::GetDC()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	worldBuffer->SetVSBuffer(1);

	if (ShaderManager::Get()->GetOther() != depth && ShaderManager::Get()->GetOther() != shadow)
	{
		skyBuffer->SetPSBuffer(1);
		shader->Render();

		States::SetRasterizerNone();
		States::SetDepthStencilOff();
		{
			D3D::GetDC()->DrawIndexed(indexCount, 0, 0);
			cloud->Render();
		}
		States::SetDepthStencilDefault();
		States::SetRasterizerDefault();
	}

}

void Sky::PostRender(bool& isUse)
{
	ImGui::Begin("Enviroment",&isUse); 
	if (ImGui::CollapsingHeader("Sky"))
	{
		ImGui::ColorEdit3("Center", (float *)&skyBuffer->Data.Center);
		ImGui::ColorEdit3("Apex", (float *)&skyBuffer->Data.Apex);
	}

	ImGui::End();
}
