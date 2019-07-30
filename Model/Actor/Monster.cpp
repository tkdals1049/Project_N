#include "../../stdafx.h"
#include "Monster.h"
#include "Player.h"
#include "../Etc/Look.h"
#include "../BinModel/Model.h"
#include "../BinModel/ModelSkeleton.h"
#include "../BinModel/ModelAnimation.h"
#include "../BinModel/ModelAnimationController.h"

Monster::Monster() :Actor(), dir(0), degree(0), degree_goal(0), speed(20.0f),
mode("idle"), Premode(""), isControl(true), isEquip(false), isHeight(false), way(D3DXVECTOR3(0, 0, 0)), weaponNum(-1), isLoaded(false), loadThread(NULL)
{
	look = new Look();
}

Monster::Monster(string file) : Actor(), dir(0), degree(0), degree_goal(0), speed(20.0f),
mode("idle"), Premode(""), isControl(true), isEquip(false), isHeight(false), way(D3DXVECTOR3(0, 0, 0)), weaponNum(-1), isLoaded(false), loadThread(NULL)
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
	if (loadThread != NULL && isLoaded)
	{
		loadThread->join();
		SAFE_DELETE(loadThread);
	}

	if (model != NULL && isLoaded)
	{
		Notify();
		Actor::Update();
		look->SetWorld(model->GetWeaponWorld("HD_01"));
		if(look->IsIn(&player->GetPosition()))	model->isDamage();
	}
}

void Monster::Render()
{
	if (model != NULL && isLoaded )
	{
		Actor::Render();
		look->Render();
	}
}

void Monster::Notify()
{
	if (model != NULL) return;
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

void Monster::SetMonster(string Name)
{

	if (model != NULL) return;

	loadThread = new thread([&]()
	{
		isLoaded = false;

		MoLoader::LoadBinary(Name, &model);
		model->Reset();
		model->SetAniPlay(1, 0, 1.5f);
		model->AniChange("idle");

		isLoaded = true;
	});

}