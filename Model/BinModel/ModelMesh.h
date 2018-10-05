#pragma once

class Model;
class ModelMaterial;
class ModelMesh
{
public:
	void SetMaterial(ModelMaterial* material) { this->material = material; }
	bool ClickCheck(D3DXMATRIX & world, D3DXVECTOR3 origin, D3DXVECTOR3 direction);
	void GetMinMax(D3DXVECTOR3& minp, D3DXVECTOR3& maxp){minp=this->minp;maxp=this->maxp;}
	void Reset();

	static void Write(BinaryWriter * w, ModelMesh * modelMesh);
	static void Read(BinaryReader * r, Model * model, ModelMesh * modelMesh);

private:
	friend class MoModel;
	friend class MoModelMesh;
	friend class Model;
	typedef VertexTextureNormalTangentBlend VertexType;

	ModelMesh(Model* model);
	~ModelMesh();

	void Update();
	void Render();

	void CreateNormalData();

	void CreateBuffer();

	Model* model;
	ModelMaterial* material;

	VertexType* vertexData;
	UINT* indexData;

	ID3D11Buffer* vertexBuffer, *indexBuffer;
	UINT vertexCount, indexCount;

	D3DXMATRIX matGeometricOffset;
	WorldBuffer* worldBuffer;

	D3DXMATRIX world;
	D3DXVECTOR3 minp, maxp;

};
