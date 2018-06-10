#include "../stdafx.h"
#include "Actor.h"
#include "ModelMaterial.h"
#include "ModelSkeleton.h"
#include "ModelSkeletonBone.h"
#include "ModelAnimationController.h"
#include "Model.h"
#include "../Content/BinModel.h"
#include "../Content/FbxModel.h"
#include "../Content/Texture.h"

Actor::Actor() :model(NULL),different(false), dot(D3DXVECTOR2(0, 0)), a(0.0f)
{
	SaveFile = "";
}

Actor::~Actor()
{
		SAFE_DELETE(model);
}

void Actor::PreUpdate(D3DXVECTOR3 origin, D3DXVECTOR3 direction)
{

}
void Actor::Update()
{
		if (model != NULL)
		{
			model->Update();
		}
}
void Actor::PostRender(bool& isUse)
{
}
void Actor::Render()
{

	if(model!=NULL)
	{
		model->Render();
	}
}

void Actor::SetModel(string file)
{
	if (model != NULL)SAFE_DELETE(model);
	MoLoader::LoadBinary(file, &model);
	model->Reset();
}

void Actor::SetModel(Model* another)
{
	if(another==NULL){ model=NULL; return;}
	if(model!=NULL)SAFE_DELETE(model);
	model = new Model();
	model=another;
}

Model * Actor::GetModel()
{
	return model;
}

void Actor::AniChange(string mode)
{
	model->AniChange(mode);
}

void Actor::Check(D3DXVECTOR3 origin, D3DXVECTOR3 direction)
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

void Actor::SetAdjust(D3DXVECTOR3 adjust)
{
	model->SetAdjust(adjust);
}

void Actor::SetAniPlay(D3DXVECTOR3 ani)
{
	model->SetAniPlay((int)ani.x,(int)ani.y,ani.z);
}

