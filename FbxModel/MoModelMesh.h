#pragma once

class MoModel;
class MoMaterial;
class Model;
class ModelMesh;
class MoBoneWeights;
class MoModelMesh
{
public:
	MoMaterial* GetMaterial() { return material; }

private:
	friend class MoLoader;
	friend class MoModel;

	MoModelMesh(MoModel* model, MoMaterial* material);
	~MoModelMesh();

	void PushVertex(D3DXVECTOR3& position, D3DXVECTOR3& normal, D3DXVECTOR2& uv, const MoBoneWeights& boneWeights);
	
	void Write(BinaryWriter* w);

	typedef VertexTextureNormalTangentBlend VertexType;

	MoModel* model;
	MoMaterial* material;

	vector<VertexType> vertices;
	vector<MoBoneWeights> boneWeights;
	vector<UINT> indices;
};
