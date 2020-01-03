#include "stdafx.h"
#include "Boss.h"
#include "Player.h"
#include "../Etc/Look.h"

Boss::Boss() :Actor(),dir(0), degree(0), degree_goal(0), speed(20.0f),
mode("idle"), Premode(""), isControl(false), isEquip(false), isHeight(false), way(D3DXVECTOR3(0, 0, 0)), weaponNum(-1), isLoaded(false), loadThread(NULL)
{
	look = new Look();
}

Boss::~Boss()
{
	SAFE_DELETE(look);
	SAFE_DELETE(loadThread);
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
		look->SetWorld(model->GetWeaponWorld("HD_00"));
		if (look->IsIn(&player->GetPosition()))
		{
			look->ChangeColor();
			if (!isControl)Input("angry");
		}
	}
}

void Boss::Render()
{
	if (model != NULL && isLoaded)
	{
		Actor::Render();
		look->Render();
	}
}
void Boss::Moving(D3DXVECTOR3 playerPos)
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
				model->SetAniPlay(1, 1, 3.0f);
				Input("walk");
				D3DXVec3Normalize(&movePoint, &(movePoint));
				float degree = atan2(position.z - playerPos.z, playerPos.x - position.x);
				int ans = (int)(degree / radian);

				model->SetPosition(position + D3DXVECTOR3(movePoint.x, 0, movePoint.z)*move*Time::Get()->Delta());
				model->SetRotate(D3DXVECTOR3(-(float) 90, degree / radian, rotate.z));
			}
		}
		else
		{
			if (mode != "angry") Input("threat");
		}
	}
}
void Boss::Notify()
{
	if (!isLoaded)return;

	ModelAnimationController* ani = model->GetAnimationController();
	int current = ani->GetCurrentKeyFrame();
	int end = ani->GetCurrentAnimation()->GetKeyFrames();

	if (mode == "angry")
	{
		if (current >= end - 1) isControl = true;
	}
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
		Input("threat");
		isLoaded = true;
	});

}