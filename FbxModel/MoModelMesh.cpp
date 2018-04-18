#include "../stdafx.h"
#include "MoModelMesh.h"
#include "MoModel.h"
#include "MoMaterial.h"
#include "MoBoneWeights.h"
#include "../Model/Model.h"
#include "../Model/ModelMesh.h"
#include "../Model/ModelMaterial.h"

MoModelMesh::MoModelMesh(MoModel * model, MoMaterial * material)
	: model(model), material(material)
{

}

MoModelMesh::~MoModelMesh()
{
}

void MoModelMesh::PushVertex(D3DXVECTOR3 & position, D3DXVECTOR3 & normal, D3DXVECTOR2 & uv, const MoBoneWeights& boneWeights)
{
	this->boneWeights.push_back(boneWeights);

	VertexType vertex;
	vertex.position = position;
	vertex.uv = uv;
	vertex.normal = normal;
	
	MoBoneWeights another = this->boneWeights[vertices.size()];
	MoBlendWeights weight = another.GetBlendWeights();
	vertex.blendIndices = weight.BlendIndices;
	vertex.blendWeights = weight.BlendWeights;

	vertices.push_back(vertex);

	indices.push_back((UINT)indices.size());

}

void MoModelMesh::Write(BinaryWriter * w)
{
	UINT materialNumber = material->GetNumber();
	w->UInt(materialNumber);

	w->UInt(vertices.size());
	w->Byte(&vertices[0], sizeof(VertexType) * vertices.size());

	w->UInt(indices.size());
	w->Byte(&indices[0], sizeof(UINT) * indices.size());
}
