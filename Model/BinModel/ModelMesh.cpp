#include "../../stdafx.h"
#include "ModelMesh.h"
#include "Model.h"
#include "ModelMaterial.h"

ModelMesh::ModelMesh(Model * model)
	: model(model)
	, material(NULL)
	, vertexData(NULL), indexData(NULL), vertexBuffer(NULL), indexBuffer(NULL)
	, vertexCount(0), indexCount(0), minp(D3DXVECTOR3(0, 0, 0)), maxp(D3DXVECTOR3(0, 0, 0))
{
	worldBuffer = new WorldBuffer();
	D3DXMatrixIdentity(&world);
	D3DXMatrixIdentity(&matGeometricOffset);
}

ModelMesh::~ModelMesh()
{
	SAFE_DELETE_ARRAY(vertexData);
	SAFE_DELETE_ARRAY(indexData);

	SAFE_DELETE(worldBuffer);

	SAFE_RELEASE(vertexBuffer);
	SAFE_RELEASE(indexBuffer);
}

void ModelMesh::Update()
{
	matGeometricOffset = model->GetGeometricOffset();
	world = model->GetWorld();
	worldBuffer->SetMatrix(matGeometricOffset*world);
	//CreateNormalData();
}

void ModelMesh::Render()
{
	if (vertexBuffer == NULL) return;
	if (indexBuffer == NULL) return;
	if (material == NULL) return;
	if (material->CanRender() == false) return;

	UINT stride = sizeof(VertexType);
	UINT offset = 0;

	D3D::GetDC()->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
	D3D::GetDC()->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);
	D3D::GetDC()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	material->SetPSBuffer();
	worldBuffer->SetVSBuffer(1);

	if (ShaderManager::Get()->GetOther() != depth && ShaderManager::Get()->GetOther() != shadow)
	{

		D3D::GetDC()->DrawIndexed(indexCount, 0, 0);
	}
	else
	{
		D3D::GetDC()->PSSetShaderResources(0, 1, ShaderManager::Get()->GetResourceView(0));
		ShaderManager::Get()->Render(indexCount,true);
	}
}
void ModelMesh::CreateNormalData()
{

	for (UINT i = 0; i < vertexCount; i++)
		vertexData[i].normal = D3DXVECTOR3(0, 0, 0);

	for (UINT i = 0; i < (indexCount / 3); i++)
	{
		UINT index0 = indexData[i * 3 + 0];
		UINT index1 = indexData[i * 3 + 1];
		UINT index2 = indexData[i * 3 + 2];

		VertexTextureNormalTangentBlend v0 = vertexData[index0];
		VertexTextureNormalTangentBlend v1 = vertexData[index1];
		VertexTextureNormalTangentBlend v2 = vertexData[index2];

		D3DXVECTOR3 deltaPos1 = v1.position - v0.position;
		D3DXVECTOR3 deltaPos2 = v2.position - v0.position;

		D3DXVECTOR2 deltaUV1 = v1.uv - v0.uv;
		D3DXVECTOR2 deltaUV2 = v2.uv - v0.uv;

		D3DXVECTOR3 normal;
		D3DXVec3Cross(&normal, &deltaPos1, &deltaPos2);

		vertexData[index0].normal += normal;
		vertexData[index1].normal += normal;
		vertexData[index2].normal += normal;

		float det = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV1.y * deltaUV2.x);
		vertexData[index0].tangent = (deltaPos1 * deltaUV2.y - deltaPos2 * deltaUV1.y)*det;
		vertexData[index1].tangent = (deltaPos1 * deltaUV2.y - deltaPos2 * deltaUV1.y)*det;
		vertexData[index2].tangent = (deltaPos1 * deltaUV2.y - deltaPos2 * deltaUV1.y)*det;
	}

	for (UINT i = 0; i < vertexCount; i++)
	{
		D3DXVec3Normalize(&vertexData[i].normal, &vertexData[i].normal);
		D3DXVec3Normalize(&vertexData[i].tangent, &vertexData[i].tangent);
	}
	D3D::GetDC()->UpdateSubresource(vertexBuffer, 0, NULL, vertexData, sizeof(VertexType)*vertexCount, 0);
}
void ModelMesh::CreateBuffer()
{
	HRESULT hr;
	D3D11_BUFFER_DESC desc;
	D3D11_SUBRESOURCE_DATA data;

	//1. Vertex Buffer
	if (vertexBuffer != NULL)
		SAFE_RELEASE(vertexBuffer);

	ZeroMemory(&desc, sizeof(D3D11_BUFFER_DESC));
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.ByteWidth = sizeof(VertexType) * vertexCount;
	desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

	ZeroMemory(&data, sizeof(D3D11_SUBRESOURCE_DATA));
	data.pSysMem = vertexData;

	hr = D3D::GetDevice()->CreateBuffer(&desc, &data, &vertexBuffer);
	assert(SUCCEEDED(hr));


	//2. Index Buffer
	if (indexBuffer != NULL)
		SAFE_RELEASE(indexBuffer);

	ZeroMemory(&desc, sizeof(D3D11_BUFFER_DESC));
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.ByteWidth = sizeof(UINT) * indexCount;
	desc.BindFlags = D3D11_BIND_INDEX_BUFFER;

	ZeroMemory(&data, sizeof(D3D11_SUBRESOURCE_DATA));
	data.pSysMem = indexData;

	hr = D3D::GetDevice()->CreateBuffer(&desc, &data, &indexBuffer);
	assert(SUCCEEDED(hr));
}

bool ModelMesh::ClickCheck(D3DXMATRIX& world, D3DXVECTOR3 origin, D3DXVECTOR3 direction)
{
	for (UINT i = 0; i < (indexCount / 3); i++)
	{
		UINT index0 = i * 3 + 0;
		UINT index1 = i * 3 + 1;
		UINT index2 = i * 3 + 2;

		D3DXVECTOR3 v0 = vertexData[index0].position;
		D3DXVECTOR3 v1 = vertexData[index1].position;
		D3DXVECTOR3 v2 = vertexData[index2].position;

		D3DXVec3TransformCoord(&v0, &v0, &world);
		D3DXVec3TransformCoord(&v1, &v1, &world);
		D3DXVec3TransformCoord(&v2, &v2, &world);

		float u, v, distance;
		BOOL check = D3DXIntersectTri(&v0, &v1, &v2, &origin, &direction, &u, &v, &distance);
		if (check)	return true;
	}
	return false;
}

void ModelMesh::Reset()
{
	for (UINT i = 0; i < vertexCount; i++)
	{
		if (i == 0)
		{
			maxp.x = vertexData[i].position.x;
			maxp.y = vertexData[i].position.y;
			maxp.z = vertexData[i].position.z;
			minp.x = vertexData[i].position.x;
			minp.y = vertexData[i].position.y;
			minp.z = vertexData[i].position.z;
		}
		else 
		{
			if (maxp.x<vertexData[i].position.x) maxp.x = vertexData[i].position.x;
			if (maxp.y<vertexData[i].position.y) maxp.y = vertexData[i].position.y;
			if (maxp.z<vertexData[i].position.z) maxp.z = vertexData[i].position.z;
			if (minp.x>vertexData[i].position.x) minp.x = vertexData[i].position.x;
			if (minp.y>vertexData[i].position.y) minp.y = vertexData[i].position.y;
			if (minp.z>vertexData[i].position.z) minp.z = vertexData[i].position.z;
		}
	}
}

////////////////////////////////////////////////////////////////////////////////////


void ModelMesh::Write(BinaryWriter * w, ModelMesh * modelMesh)
{
	UINT materialNumber = modelMesh->material->GetNumber();
	w->UInt(materialNumber);

	w->UInt(modelMesh->vertexCount);
	w->Byte(modelMesh->vertexData, sizeof(VertexType) * modelMesh->vertexCount);

	w->UInt(modelMesh->indexCount);
	w->Byte(modelMesh->indexData, sizeof(UINT) * modelMesh->indexCount);
}

void ModelMesh::Read(BinaryReader * r, Model * model, ModelMesh * modelMesh)
{
	UINT materialNumber = r->UInt();
	ModelMaterial* material = model->GetMatchMaterial(materialNumber);

	modelMesh->SetMaterial(material);


	modelMesh->vertexCount = r->UInt();
	modelMesh->vertexData = new VertexType[modelMesh->vertexCount];

	r->Byte((void **)&modelMesh->vertexData, sizeof(VertexType) * modelMesh->vertexCount);


	modelMesh->indexCount = r->UInt();
	modelMesh->indexData = new UINT[modelMesh->indexCount];
	r->Byte((void **)&modelMesh->indexData, sizeof(UINT) * modelMesh->indexCount);

	modelMesh->CreateBuffer();
}

////////////////////////////////////////////////////////////////////////////////////