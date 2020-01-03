#include "stdafx.h"
#include "Monster.h"
#include "Player.h"
#include "../Etc/Look.h"

Monster::Monster() :Actor(), dir(0), degree(0), degree_goal(0), speed(20.0f),
mode("idle"), Premode(""), isControl(false), isEquip(false), isHeight(false), way(D3DXVECTOR3(0, 0, 0)), weaponNum(-1), isLoaded(false), loadThread(NULL)
{
	look = new Look();
}

Monster::Monster(string file) : Actor(), dir(0), degree(0), degree_goal(0), speed(20.0f),
mode("idle"), Premode(""), isControl(false), isEquip(false), isHeight(false), way(D3DXVECTOR3(0, 0, 0)), weaponNum(-1), isLoaded(false), loadThread(NULL)
{
	MoLoader::LoadBinary(file, &model);

	model->Reset();
	model->AniChange("idle");
	look = new Look();
}

Monster::~Monster()
{
	SAFE_DELETE(look);
	SAFE_DELETE(loadThread);
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
		if (look->IsIn(&player->GetPosition()))
		{
			look->ChangeColor();
			if(!isControl)Input("crew");
		}
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
void Monster::Moving(D3DXVECTOR3 playerPos)
{
	if (model != NULL && isLoaded)
	{
		if (isControl)
		{
			float move = 4;

			D3DXVECTOR3 position = model->GetPosition();
			D3DXVECTOR3 rotate = model->GetRotate();
			D3DXVECTOR3 movePoint = playerPos - position;
			float distance = D3DXVec3Length(&(position - playerPos));

			if (distance < 10)
			{
				Input("bite");
			}
			else
			{
				Input("rush");
				D3DXVec3Normalize(&movePoint, &(movePoint));
				float degree = atan2(position.z - playerPos.z, playerPos.x - position.x);
				int ans = (int)(degree / radian);

				model->SetPosition(position + movePoint*move*Time::Get()->Delta());
				model->SetRotate(D3DXVECTOR3(90, (float)degree / radian, rotate.z));
			}
		}
		else
		{
			if(mode != "crew") Input("idle");
		}
	}
}
void Monster::Notify()
{
	if (model == NULL) return;
	ModelAnimationController* ani = model->GetAnimationController();
	int current = ani->GetCurrentKeyFrame();
	int end = ani->GetCurrentAnimation()->GetKeyFrames();
	if (mode == "crew")
	{
		if (current >= end - 1) isControl = true;
	}
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