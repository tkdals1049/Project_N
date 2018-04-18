#include "../stdafx.h"
#include "MoModel.h"
#include "MoMaterial.h"
#include "MoModelMesh.h"
#include "MoSkeleton.h"
#include "MoBoneWeights.h"
#include "MoanimationController.h"
#include "MoKeyFrame.h"
#include "../Model/Model.h"
#include "../Model/ModelSkeleton.h"
#include "../Model/ModelMaterial.h"
#include "../Model/ModelanimationController.h"
#include "../Model/ModelMesh.h"

MoModel::MoModel(string file)
	: file(file), skeleton(NULL)
{
	D3DXMatrixIdentity(&geometricOffset);
	animationController = new MoAnimationController();
}

MoModel::~MoModel()
{
	for (MoMaterial* material : materials)
		SAFE_DELETE(material);
}

void MoModel::PushMaterial(MoMaterial * material)
{
	materials.push_back(material);
}

void MoModel::PushVertex(MoMaterial * material, D3DXVECTOR3 & position, D3DXVECTOR3 & normal, D3DXVECTOR2 & uv, const MoBoneWeights& boneWeights)
{
	bool bNew = true;

	for (MoModelMesh* mesh : meshes)
	{
		if (material == mesh->GetMaterial())
		{
			mesh->PushVertex(position, normal, uv, boneWeights);
			bNew = false;
		}
	}

	if (bNew == true)
	{
		MoModelMesh* mesh = new MoModelMesh(this, material);
		mesh->PushVertex(position, normal, uv, boneWeights);

		meshes.push_back(mesh);
	}
}

MoSkeleton * MoModel::GetSkeleton()
{
	if (skeleton == NULL) skeleton = new MoSkeleton();
	return skeleton;
}

void MoModel::AddAnimationKeyFrames(MoKeyFrame * animationKeyFrames)
{
	this->animationKeyFrames.push_back(Pair(animationKeyFrames->GetAnimationName(), animationKeyFrames));
}

void MoModel::Write(string file)
{
	BinaryWriter* w = new BinaryWriter();
	wstring temp = String::StringToWString(file);

	w->Open(temp);
	{
		w->Matrix(geometricOffset);

		w->UInt(materials.size());
		for (MoMaterial* material : materials)
			material->Write(w);

		w->UInt(meshes.size());
		for (MoModelMesh* mesh : meshes)
			mesh->Write(w);

		animationController->Wirte(w);

		if (skeleton != NULL)
		{
		w->Bool(true);
		skeleton->Write(w);
		}
		else 
		w->Bool(false);

	}
	w->Close();

	SAFE_DELETE(w);
}

void MoModel::Write(string file, Model * model)
{
	BinaryWriter* w = new BinaryWriter();
	wstring temp = String::StringToWString(file);

	w->Open(temp);
	{
		w->Matrix(model->matGeometricOffset);

		w->UInt(model->materials.size());
		for (ModelMaterial* material : model->materials)
			ModelMaterial::Write(w, material);

		w->UInt(model->meshes.size());
		for (ModelMesh* mesh : model->meshes)
			ModelMesh::Write(w, mesh);

		ModelAnimationController::Write(w, model->animationController);

		if (model->skeleton != NULL)
		{
			w->Bool(true);
			ModelSkeleton::Write(w,model->skeleton);
		}
		else
			w->Bool(false);


	}
	w->Close();

	SAFE_DELETE(w);
}


void MoModel::Read(string file, Model ** model)
{
	BinaryReader* r = new BinaryReader();
	wstring temp = String::StringToWString(file);
	
	UINT count = 0;
	r->Open(temp);
	{
		*model = new Model();
		(*model)->matGeometricOffset = r->Matrix();
		(*model)->file=file;

		count = r->UInt();
		for (UINT i = 0; i < count; i++)
		{		
			ModelMaterial* material = new ModelMaterial();
			ModelMaterial::Read(r, material);

			(*model)->materials.push_back(material);
		}

		count = r->UInt();
		for (UINT i = 0; i < count; i++)
		{
			ModelMesh* mesh = new ModelMesh(*model);
			ModelMesh::Read(r, *model, mesh);

			(*model)->meshes.push_back(mesh);
		}

		ModelAnimationController* animationController = new ModelAnimationController();
		ModelAnimationController::Read(r, animationController);
		(*model)->animationController=animationController;

		bool check = r->Bool();
		if(check)
		{
		ModelSkeleton* skeleton = new ModelSkeleton();
		ModelSkeleton::Read(r,skeleton);
		(*model)->skeleton=skeleton;
		}



	}
	r->Close();

	SAFE_DELETE(r);
}