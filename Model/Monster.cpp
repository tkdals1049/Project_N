#include "../stdafx.h"
#include "Monster.h"
#include "Model.h"
#include "ModelSkeleton.h"
#include "ModelAnimation.h"
#include "ModelAnimationController.h"

Monster::Monster() :dir(0), degree(0), degree_goal(0), speed(20.0f),
mode("idle"), Premode(""), isControl(true), isEquip(false), isHeight(false), way(D3DXVECTOR3(0, 0, 0)), weaponNum(-1)
{
}

Monster::Monster(string file) : dir(0), degree(0), degree_goal(0), speed(20.0f),
mode("idle"), Premode(""), isControl(true), isEquip(false), isHeight(false), way(D3DXVECTOR3(0, 0, 0)), weaponNum(-1)
{
	MoLoader::LoadBinary(file, &model);

	model->Reset();
	model->SetAdjust(D3DXVECTOR3(0, 0.78f, 0));
	model->SetAniPlay(1, 0, 1.5f);
	model->AniChange("idle");
}

Monster::~Monster()
{
}

void Monster::PreUpdate(D3DXVECTOR3 origin, D3DXVECTOR3 direction)
{
	if (Keyboard::Get()->Down('O'))
	{
		SetMonster();
	}
	if (model != NULL)
	{
		Control();
	}
}

void Monster::Update()
{
	if (model != NULL)
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
	}
}


void Monster::Control()
{
	int degree_sum = 0;

	D3DXMATRIX axis;
	D3DXMatrixRotationY(&axis, dir*(float)D3DX_PI / 180);

	D3DXVECTOR3 forward, right;
	D3DXVec3TransformCoord(&forward, &D3DXVECTOR3(0, 0, 1), &axis);
	D3DXVec3TransformCoord(&right, &D3DXVECTOR3(1, 0, 0), &axis);

	D3DXVECTOR3 move = model->GetPosition();
	D3DXVECTOR3 rotate = model->GetRotate();

	//이동 및 조작

	if (isControl == true)
	{
		way = D3DXVECTOR3(0, 0, 0);

		if (Keyboard::Get()->Press(VK_UP)) { way.z = 1; degree_sum += 0; }
		else if (Keyboard::Get()->Press(VK_DOWN)) { way.z = -1;degree_sum += 180; }
		if (Keyboard::Get()->Press(VK_LEFT)) { way.x = -1;degree_sum += 270; }
		else if (Keyboard::Get()->Press(VK_RIGHT)) { way.x = 1; degree_sum += 90; }

		if (Keyboard::Get()->Press('A'))	dir--;
		else if (Keyboard::Get()->Press('S')) dir++;

		if (way.z != 0 || way.x != 0)
		{
			if (way.z != 0 && way.x != 0)
			{
				if (way.z == 1 && way.x == -1)degree_sum += 360;
				degree_sum /= 2;
			}
			degree_goal = degree_sum;

			if (degree<0)degree += 360;
			else if (degree >= 360)degree -= 360;

			if (abs(degree_goal - degree) <= 180)
			{
				if (degree_goal < degree)      degree -= abs(degree_goal - degree) / 20 + 1;
				else if (degree_goal > degree) degree += abs(degree_goal - degree) / 20 + 1;
			}
			else
			{
				if (degree_goal < degree)     degree += abs(degree_goal - degree) / 20 + 1;
				else if (degree_goal > degree)degree -= abs(degree_goal - degree) / 20 + 1;
			}
		}

		//default 애니메이션

		if (isEquip)
		{
			if (way == D3DXVECTOR3(0, 0, 0)) { Input("w1_idle");speed = 0.0f; }
			else
			{
				{ Input("w1_walk");speed = 20.0f; }
			}
		}
		else
		{
			if (way == D3DXVECTOR3(0, 0, 0)) { Input("idle");speed = 0.0f; }
			else
			{
				if (Keyboard::Get()->Press(VK_SHIFT)) { Input("rush");speed = 30.0f; }
				else { Input("run");speed = 20.0f; }
			}
		}
	}
	model->SetPosition(move + speed * (way.z*forward + way.x*right)*Time::Get()->Delta());
	model->SetRotate(D3DXVECTOR3(rotate.x, (float)degree + (float)dir - 90, rotate.z));
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

float Monster::SetHeight()
{
	ModelSkeleton* sk = model->GetSkeleton();
	float min = 0, origin = 0;
	for (int i = 0;i< sk->GetBoneCount();i++)
	{
		D3DXVECTOR3 temp(0, 0, 0);
		D3DXVec3TransformCoord(&temp, &temp, &model->GetWeaponWorld(sk->GetWeaponName(i)));
		if (i == 0) { origin = min = temp.y; }
		else if (min>temp.y)min = temp.y;
	}
	return origin - min;
}