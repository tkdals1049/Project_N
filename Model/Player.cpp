#include "../stdafx.h"
#include "Player.h"
#include "Actor.h"
#include "Model.h"
#include "Weapon.h"
#include "ModelSkeleton.h"
#include "ModelAnimation.h"
#include "ModelAnimationController.h"

Player::Player()
:dir(0), degree(0), degree_goal(0), speed(20.0f),way(D3DXVECTOR3(0,0,0)), way2(D3DXVECTOR3(0, 0, 1)),
mode("idle"),Premode(""),isControl(true),isEquip(false), isHeight(false), isAttack(false),
attackRange(NULL),isLoaded(false),loadThread(NULL)
{
	weapon=new Weapon(this);
	attackRange=new ST_OBB();
	SetPlayer();
}

Player::~Player()
{
}

void Player::PreUpdate(D3DXVECTOR3 origin, D3DXVECTOR3 direction)
{
	if (model != NULL && isLoaded)
	{
		Control();
		weapon->PreUpdate();
	}
}

void Player::Update()
{
	if (loadThread != NULL&&isLoaded)
	{
		loadThread->join();
		SAFE_DELETE(loadThread);
	}

	if(model!=NULL)
	{	
		Notify();
		Actor::Update();
		weapon->Update();
	}
}

void Player::Render()
{
	if (model != NULL)
	{
		Actor::Render();
		weapon->Render();
	}
}


void Player::AddWeapon(Model * weaponFile, D3DXVECTOR3 scale)
{
	weapon->AddWeaponVector(weaponFile, scale);
}

void Player::ClearWeapon()
{
	weapon->ClearWeapon();
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

	//이동 및 조작
	if(isControl)
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
			if(degree<0)degree=360;
			else if(degree>360)degree=0;

			if (way.z == 1 && way.x == -1)degree_sum += 360;
			if (way.z != 0 && way.x != 0)degree_sum/=2;
			degree_goal=degree_sum;

			int size= abs(degree_goal - (degree)) / 20 + 1;
			
			if (180 <  degree)
			{
				if (degree_goal<degree&&degree_goal >= degree - 180) degree -= size;
				else
				{
					if (degree_goal<180) degree += abs(degree_goal + 360 - degree) / 20 + 1;
					else degree += size;
				}
			}
			else
			{
				if (degree_goal >= degree && degree_goal<degree + 180)	degree += size;
				else
				{
					if (degree_goal>180) degree -= abs(degree_goal - (degree + 360)) / 20 + 1;
					else degree -= size;
				}
			}
			way2=way;
		}

		//default 애니메이션

		if(isEquip)
		{
			if (way == D3DXVECTOR3(0, 0, 0)) { Input("w1_idle");speed = 0.0f; }
			else
			{
				{ Input("w1_walk");speed = 20.0f; }
			}
		}
		else 
		{ 
			if (way == D3DXVECTOR3(0, 0, 0)){Input("idle");speed=0.0f;}
			else
			{
				if (Keyboard::Get()->Press(VK_SHIFT)){Input("rush");speed=30.0f;}
				else {Input("run");speed = 20.0f;}
			}
		}
		//장비

		if (Keyboard::Get()->Down('D')&&weapon->isSetting())
		{
			if (isEquip)
			{
				if (mode == "w1_idle")Input("w1_equip_end");
				else Input("w1_equip_walk_end");
				isHeight = false;
			}
			else
			{
				isEquip=true;
				isHeight=true;
				Input("w1_equip");
			}
			isControl = false;
		}

		//점프

		if (Keyboard::Get()->Down(VK_SPACE))
		{
			model->SetAniPlay(1, 0, 3.0f);
			if (mode == "rush")
			{
				Input("rush_jump");
				speed = 50.0f;
			}
			else { Input("jump");speed = 40.0f; }

			if (way == D3DXVECTOR3(0, 0, 0)) way=way2;
			isControl = false;
			isHeight = true;
		}

		//공격
		if (Keyboard::Get()->Down('Z')&&isEquip)
		{
			Input("w1_attack1_set");
			isControl = false;
		}
		if (Keyboard::Get()->Down('X') && isEquip)
		{
			Input("w1_attack2");
			isControl = false;
			isAttack = true;
		}
		if (Keyboard::Get()->Down('C') && isEquip)
		{
			Input("w1_attack3");
			isControl = false;
			isAttack = true;
		}
	}
	else
	{
		if(mode.find("jump")==-1)
		way=D3DXVECTOR3(0,0,0);
	}
	
	//이동의 결과를 모델에 입력
	model->SetPosition(move + speed * (way.z*forward + way.x*right)*Time::Get()->Delta());
	model->SetRotate(D3DXVECTOR3(rotate.x, (float)degree+ (float)dir-90, rotate.z));

	//애니메이션 높이 조정
	D3DXMATRIX world;
	if(isHeight)
	{
		D3DXMatrixTranslation(&world,0,SetHeight(),0);
		model->SetWorld(world);
		model->SetAdjust(D3DXVECTOR3(0,0.05f,0));
	}
	else
	{
		D3DXMatrixTranslation(&world, 0, 0, 0);
		model->SetWorld(world);
		model->SetAdjust(D3DXVECTOR3(0, 0.42f, 0));
	}
}

//애니메이션 노티파이
void Player::Notify()
{ 
	if(isLoaded==false)return;

	ModelAnimationController* ani= model->GetAnimationController();
	int degree = ani->GetCurrentKeyFrame();
	int end = ani->GetCurrentAnimation()->GetKeyFrames();

	if (mode == "run")
	{
		if (degree >= end - 1|| Premode == "rush") ani->SetCurrentKeyFrame(27);
	}
	else if (mode == "jump"||mode == "rush_jump_end")
	{
		if (degree >= end - 1) 
		{
		Input("idle");
		isControl=true;
		speed = 0.0f;
		model->SetAniPlay(1, 0, 1.5f);
		isHeight = false;
		}
	}
	else if (mode == "rush_jump")
	{
		if (degree >= end - 1) { Input("rush_jump_dive");}
	}
	else if (mode == "rush_jump_dive")
	{
		if (degree >= end - 1) { Input("rush_jump_end"); }
	}
	else if (mode == "w1_equip")
	{
		if (degree >= end - 1) { Input("w1_idle"); isControl=true;}
	}
	else if (mode == "w1_equip_end"|| mode == "w1_equip_walk_end")
	{
		if (degree >= end - 10) { Input("idle"); isControl = true; isEquip=false;}
	}
	else if (mode == "w1_attack1_set")
	{
		if (degree >= end - 1) { Input("w1_attack1");isAttack = true;}
	}
	else if (mode == "w1_attack1")
	{
		if (degree >= end - 5) 
		{ isControl = true;
		isAttack = false;
		}
	}
	else if (mode == "w1_attack2")
	{
		if (degree >= end - 5) 
		{ isControl = true;
		isAttack = false;
		}
	}
	else if (mode == "w1_attack3")
	{
		if (degree >= end - 5) 
		{ isControl = true;
		isAttack = false;
		}
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


	loadThread = new thread([&]()
	{
		isLoaded = false;

		MoLoader::LoadBinary("../_Contents/BinModels/Actor_Player.model", &model);
		Model* wp,*wp2;
		MoLoader::LoadBinary("../_Contents/BinModels/Actor_Sword1.model", &wp);
		wp->Reset();
		weapon->AddWeaponVector(wp, model->GetScale());
		MoLoader::LoadBinary("../_Contents/BinModels/Actor_Sword2.model", &wp2);
		wp2->Reset();
		weapon->AddWeaponVector(wp2, model->GetScale());

		model->Reset();
		model->SetAniPlay(1, 0, 1.5f);
		model->AniChange("idle");

		isLoaded = true;
	});
}


//루트 조정으로 부족한 애니메이션 원점 조정으로 동작을 자연스럽게 함
float Player::SetHeight()
{
	ModelSkeleton* sk= model->GetSkeleton();
	float min=0,origin=0;
	for(int i = 0;i< sk->GetBoneCount();i++)
	{
		D3DXVECTOR3 temp(0,0,0);
		D3DXVec3TransformCoord(&temp,&temp,&model->GetWeaponWorld(sk->GetWeaponName(i)));
		if(i==0) {origin=min=temp.y;}
		else if(min>temp.y)min=temp.y;
	}
	return origin-min;
}

