#include "../../stdafx.h"
#include "Boss.h"
#include "Player.h"
#include "../Etc/Look.h"
#include "../BinModel/Model.h"
#include "../BinModel/ModelSkeleton.h"
#include "../BinModel/ModelAnimation.h"
#include "../BinModel/ModelAnimationController.h"

Boss::Boss() :Actor(),dir(0), degree(0), degree_goal(0), speed(20.0f),
mode("idle"), Premode(""), isControl(true), isEquip(false), isHeight(false), way(D3DXVECTOR3(0, 0, 0)), weaponNum(-1), isLoaded(false), loadThread(NULL)
{
	look = new Look();
}

Boss::~Boss()
{
		SAFE_DELETE(model);
}

void Boss::Update()
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
	}
}

void Boss::Render()
{
	if (model != NULL && isLoaded)
	{
		Actor::Render();
		look->SetWorld(model->GetWeaponWorld("HD_00"));
		look->Render();
	}
}

void Boss::Notify()
{
	if (!isLoaded)return;

	ModelAnimationController* ani = model->GetAnimationController();
	int current = ani->GetCurrentKeyFrame();
	int end = ani->GetCurrentAnimation()->GetKeyFrames();
}
void Boss::Input(string mode)
{
	if (this->mode != mode)
	{
		model->AniChange(mode);
	}
	Premode = this->mode;
	this->mode = mode;
}

void Boss::SetBoss(string Name)
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