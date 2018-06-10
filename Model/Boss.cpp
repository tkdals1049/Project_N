#include "../stdafx.h"
#include "Boss.h"
#include "ModelMaterial.h"
#include "ModelSkeleton.h"
#include "ModelSkeletonBone.h"
#include "ModelAnimationController.h"
#include "Model.h"
#include "../Content/BinModel.h"
#include "../Content/FbxModel.h"
#include "../Content/Texture.h"

Boss::Boss() :different(false), dot(D3DXVECTOR2(0, 0)), a(0.0f)
{
	SaveFile = "";
	skeletonList = NULL;
}

Boss::~Boss()
{
		SAFE_DELETE(model);
	for (size_t i = 0; i <weapon.size(); i++)
		SAFE_DELETE(weapon[i].second);
}

void Boss::PreUpdate(D3DXVECTOR3 origin, D3DXVECTOR3 direction)
{

}
void Boss::Update()
{
		model->Update();

		for each(Weapon temp in weapon)
		{
			temp.second->Update();

			temp.second->SetWorld(model->GetWeaponWorld(temp.first));
		}
	
}
void Boss::PostRender(bool& isUse)
{
}
void Boss::Render()
{
	model->Render();

	for each(Weapon temp in weapon)
	{
		temp.second->Render();
	}
}

void Boss::SetModel(string file)
{
	SAFE_DELETE(model);
	MoLoader::LoadBinary(file, &model);
	model->Reset();
}

void Boss::SetModel(Model* another)
{
	model=another;
}

Model * Boss::GetModel()
{
	return model;
}

void Boss::AddWeaponVector(Model * model, string weaponName, Model * weaponFile)
{
	weapon.push_back(Weapon(weaponName,weaponFile));
}

void Boss::Check(D3DXVECTOR3 origin, D3DXVECTOR3 direction)
{
		if (model->Check(origin, direction))
		{
				if (model->GetSkeleton() != NULL)
				{
					int num = model->GetSkeleton()->GetBoneCount();
					skeletonList = new const char*[num];
					for (size_t i = 0; (int)i < num; i++)
					{
						ModelSkeletonBone* bone = model->GetSkeleton()->GetSkeletonBone(i);
						skeletonList[i] = new char[bone->GetName().size() + 1];
						strcpy_s((char*)skeletonList[i], bone->GetName().size() + 1, bone->GetName().c_str());
					}
				}
				different = true;
		}
}