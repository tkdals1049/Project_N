#include "../stdafx.h"
#include "Player.h"
#include "Model.h"
#include "ModelAnimation.h"
#include "ModelAnimationController.h"

Player::Player():dir(0), degree(0), degree_goal(0), speed(20.0f),
mode("idle"),Premode(""),isControl(true),isEquip(false),way(D3DXVECTOR3(0,0,0)),weaponNum(-1),equipName("AL-03"), unequipName("NK-00")
{
}

Player::Player(string file) : dir(0), degree(0), degree_goal(0), speed(20.0f),
mode("idle"), Premode(""), isControl(true), isEquip(false), way(D3DXVECTOR3(0, 0, 0)), weaponNum(-1), equipName("AL-03"), unequipName("NK-00")
{
		MoLoader::LoadBinary(file, &model);

		model->Reset();
		model->SetAdjust(D3DXVECTOR3(0, 0.78f, 0));
		model->SetAniPlay(1, 0, 1.5f);
		model->AniChange("idle");
}

Player::~Player()
{
	for (size_t i = 0; i <weapons.size(); i++)
		SAFE_DELETE(weapons[i].second);
}

void Player::PreUpdate(D3DXVECTOR3 origin, D3DXVECTOR3 direction)
{
	if (Keyboard::Get()->Down('P'))
	{
		SetPlayer();
	}
	if (model != NULL)
	{
		Control();
	}
}

void Player::Update()
{
	if(model!=NULL)
	{	
		Notify();
		Actor::Update();
		
		for each(Weapon temp in weapons)
		{
			temp.second->Update();

			if(isEquip)temp.second->SetWorld(temp.second->GetGeometricOffset()*model->GetWeaponWorld(temp.first));
			else temp.second->SetWorld(temp.second->GetGeometricOffset()*model->GetWeaponWorld(unequipName));
		}
	}
}

void Player::Render()
{
	if (model != NULL)
	{
		Actor::Render();

		for each(Weapon temp in weapons)
		{
			temp.second->Render();
		}
	}
}


void Player::Control()
{
	int degree_sum=0;

	D3DXMATRIX axis;
	D3DXMatrixRotationY(&axis, dir*(float)D3DX_PI / 180);

	D3DXVECTOR3 forward, right;
	D3DXVec3TransformCoord(&forward, &D3DXVECTOR3(0, 0, 1), &axis);
	D3DXVec3TransformCoord(&right, &D3DXVECTOR3(1, 0, 0), &axis);

	D3DXVECTOR3 move = model->GetPosition();
	D3DXVECTOR3 rotate = model->GetRotate();


	if (Keyboard::Get()->Down('D'))
	{
		isEquip=!isEquip;
		Input("w1_equip");
	}
	if (Keyboard::Get()->Down(VK_SPACE))
	{
		model->SetAniPlay(1,0,3.0f);
		if(mode=="rush")
		{
			Input("rush_jump");
			speed = 35.0f;
		}
		else {Input("jump");speed = 30.0f;}
		isControl=false;
	}
	if(isControl==true)
	{
		way = D3DXVECTOR3(0, 0, 0);

		if (Keyboard::Get()->Press(VK_UP))		  { way.z = 1; degree_sum+=0;}
		else if (Keyboard::Get()->Press(VK_DOWN)) { way.z = -1;degree_sum+=180;}
		if (Keyboard::Get()->Press(VK_LEFT))	  { way.x = -1;degree_sum+=270; }
		else if (Keyboard::Get()->Press(VK_RIGHT)){ way.x = 1; degree_sum+=90;}
		
		if (Keyboard::Get()->Press('A'))	dir--;
		else if (Keyboard::Get()->Press('S')) dir++;
		
		if(way.z != 0 || way.x != 0)
		{	
			if (way.z != 0 && way.x != 0)
			{
				if(way.z==1&&way.x==-1)degree_sum+=360;
				degree_sum/=2;
			}
			degree_goal=degree_sum;

			if(degree<0)degree+=360;
			else if(degree>=360)degree-=360;

			if (abs(degree_goal-degree)<=180)
			{
				if (degree_goal < degree)      degree-=abs(degree_goal - degree) / 20 + 1;		
				else if (degree_goal > degree) degree+=abs(degree_goal - degree) / 20 + 1;
			}
			else 
			{
				if (degree_goal < degree)     degree += abs(degree_goal - degree) / 20 + 1;
				else if (degree_goal > degree)degree -= abs(degree_goal - degree) / 20 + 1;
			}
		}
		
		if (way == D3DXVECTOR3(0, 0, 0)){Input("idle");speed=0.0f;}
		else
		{
			if (Keyboard::Get()->Press(VK_SHIFT)){Input("rush");speed=30.0f;}
			else {Input("run");speed = 20.0f;}
		}
	}

	model->SetPosition(move + speed * (way.z*forward + way.x*right)*Time::Get()->Delta());
	model->SetRotate(D3DXVECTOR3(rotate.x, (float)degree+ (float)dir-90, rotate.z));

}

void Player::Notify()
{
	ModelAnimationController* ani= model->GetAnimationController();
	int current = ani->GetCurrentKeyFrame();
	int end = ani->GetCurrentAnimation()->GetKeyFrames();
	if (mode == "run")
	{
		if (current >= end - 1|| Premode == "rush") ani->SetCurrentKeyFrame(27);
	}
	else if (mode == "jump"||mode == "rush_jump_end")
	{
		if (current >= end - 1) 
		{
		Input("idle");
		isControl=true;
		speed = 0.0f;
		model->SetAniPlay(1, 0, 1.5f);
		}
	}
	else if (mode == "rush_jump")
	{
		if (current >= end - 1) { Input("rush_jump_dive");}
	}
	else if (mode == "rush_jump_dive")
	{
		if (current >= end - 1) { Input("rush_jump_end"); }
	}
	else if (mode == "w1_equip")
	{
		if (current >= end - 1) { Input("w1_idle"); }
	}

}
void Player::Input(string mode)
{
	if (this->mode != mode)
	{
		model->AniChange(mode);
	}
	Premode = this->mode;
	this->mode = mode;
}

void Player::SetPlayer()
{
	if(model!=NULL) return;
	MoLoader::LoadBinary("../_Contents/BinModels/Actor_Player.model", &model);
	Model* weapon;
	MoLoader::LoadBinary("../_Contents/BinModels/Actor_Sword2.model", &weapon);
	weapon->Reset();
	weapon->SetAdjust(D3DXVECTOR3(0.5f, -0.5f, 0));
	AddWeaponVector(equipName,weapon);
	model->Reset();
	model->SetAdjust(D3DXVECTOR3(0, 0.78f, 0));
	model->SetAniPlay(1, 0, 1.5f);
	model->AniChange("idle");

}

void Player::ClearWeapon()
{
	for each(Weapon temp in weapons)
	{
		SAFE_DELETE(temp.second);
	}
	weapons.clear();
}

void Player::DeleteWeapon()
{
	for each(Weapon temp in weapons)	SAFE_DELETE(temp.second);
	weapons.clear();
}


void Player::AddWeaponVector(string weaponName, Model * weaponFile)
{
	weapons.push_back(Weapon(weaponName,weaponFile));
}