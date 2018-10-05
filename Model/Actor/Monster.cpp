#include "../../stdafx.h"
#include "Monster.h"
#include "Player.h"
#include "../Etc/Look.h"
#include "../BinModel/Model.h"
#include "../BinModel/ModelSkeleton.h"
#include "../BinModel/ModelAnimation.h"
#include "../BinModel/ModelAnimationController.h"

Monster::Monster() :dir(0), degree(0), degree_goal(0), speed(20.0f),
mode("idle"), Premode(""), isControl(true), isEquip(false), isHeight(false), way(D3DXVECTOR3(0, 0, 0)), weaponNum(-1)
{
	look = new Look();
}

Monster::Monster(string file) : dir(0), degree(0), degree_goal(0), speed(20.0f),
mode("idle"), Premode(""), isControl(true), isEquip(false), isHeight(false), way(D3DXVECTOR3(0, 0, 0)), weaponNum(-1)
{
	MoLoader::LoadBinary(file, &model);

	model->Reset();
	model->AniChange("idle");
	look = new Look();
}

Monster::~Monster()
{
}

void Monster::PreUpdate(D3DXVECTOR3 origin, D3DXVECTOR3 direction)
{
}

void Monster::Update()
{
	if (model != NULL )
	{
		Notify();
		Actor::Update();
	}
}

void Monster::Render()
{
	if (model != NULL)
	{
		Actor::Render();
		look->SetWorld(model->GetWeaponWorld("HD_02"));
		//look->Render();
	}
}

void Monster::Notify()
{
	ModelAnimationController* ani = model->GetAnimationController();
	int current = ani->GetCurrentKeyFrame();
	int end = ani->GetCurrentAnimation()->GetKeyFrames();
}
void Monster::Input(string mode)
{
	if (this->mode != mode)
	{
		model->AniChange(mode);
	}
	Premode = this->mode;
	this->mode = mode;
}

void Monster::SetMonster()
{
	if (model != NULL) return;
	MoLoader::LoadBinary("../_Contents/BinModels/Actor_Monster.model", &model);
	model->Reset();
	model->SetAniPlay(1, 0, 1.5f);
	model->AniChange("idle");

}