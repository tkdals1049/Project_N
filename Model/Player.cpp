#include "../stdafx.h"
#include "Player.h"
#include "Actor.h"
#include "Model.h"
#include "ModelSkeleton.h"
#include "ModelAnimation.h"
#include "ModelAnimationController.h"

Player::Player():dir(0), degree(0), degree_goal(0), speed(20.0f),
mode("idle"),Premode(""),isControl(true),isEquip(false), isHeight(false),way(D3DXVECTOR3(0,0,0)),weaponNum(-1),equipName("AL-03"), equipName2("AR-03"), unequipName("NK-00")
{
}

Player::Player(string file) : dir(0), degree(0), degree_goal(0), speed(20.0f),
mode("idle"), Premode(""), isControl(true), isEquip(false), isHeight(false), way(D3DXVECTOR3(0, 0, 0)), weaponNum(-1), equipName("AL-03"), unequipName("NK-00")
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

	if (Keyboard::Get()->Down('F'))
	{
		if(weaponNum<weapons.size()) weaponNum++;
		else weaponNum=0;
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

		if (weaponNum >= 0 && weaponNum<weapons.size())
		{
			D3DXMATRIX world,scale,rotate;
			string worldName;
			
			weapons[weaponNum].second->Update();
			//무기 장착시와 아닐시 고정 위치 변 
			if(isEquip)
			{
				if (mode == "w1_equip"|| mode == "w1_equip_end" || mode == "w1_equip_walk_end")
				{
					if(mode!=Premode)
					{
					weapons[weaponNum].second->SetRotate(D3DXVECTOR3(90,0,0));
					weapons[weaponNum].second->SetAdjust(D3DXVECTOR3(0, 0, 0));
					}
					worldName = equipName2;
				}
				else
				{
					if (mode != Premode)
					{
					weapons[weaponNum].second->SetRotate(D3DXVECTOR3(-90, 0, 0));
					weapons[weaponNum].second->SetAdjust(D3DXVECTOR3(0.4f, 1.0f, 0));
					}
					worldName = equipName;
				}
			}
			else 
			{
				if (mode != Premode|| mode=="idle")
				{
				weapons[weaponNum].second->SetRotate(D3DXVECTOR3(-90,90,0));
				weapons[weaponNum].second->SetAdjust(D3DXVECTOR3(-0.12f, 0,1.5f));
				}
				worldName = unequipName;
			}


			D3DXVECTOR3 max= weapons[weaponNum].second->GetMaxP(),min = weapons[weaponNum].second->GetMinP();
			D3DXVECTOR3 size = weapons[weaponNum].second->GetScale(), another = model->GetScale();
			D3DXVECTOR3 center = max-min, position=(max+min);

			D3DXMatrixScaling(&scale, center.x/size.x , center.y / size.y , center.z / size.z );
			D3DXMatrixTranslation(&world, position.x, 0, 0);
			size.x *= another.x;
			size.y *= another.y;
			size.z *= another.z;

			weapons[weaponNum].second->SetWorld(model->GetWeaponWorld(worldName)*model->GetWorld());
			weaponWorld = world* weapons[weaponNum].second->GetGeometricOffset()*model->GetWeaponWorld(worldName)*model->GetWorld();
			ST_OBB* test = new ST_OBB();
			CalMatrix(test, max,min, size, weaponWorld);
			weaponWorld = scale * weaponWorld;
		}
	}
}

void Player::Render()
{
	if (model != NULL)
	{
		Actor::Render();

		if(weaponNum>=0&&weaponNum<weapons.size())
		weapons[weaponNum].second->Render();
		

		//Cube::Get()->Update(weaponWorld);
		//Cube::Get()->Render();
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

	//이동 및 조작

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

		if (Keyboard::Get()->Down('D'))
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
				speed = 35.0f;
			}
			else { Input("jump");speed = 30.0f; }
			isControl = false;
			isHeight = true;
		}

		//공격

		if (Keyboard::Get()->Down('Z')&&isEquip)
		{
			Input("w1_attack1_set");
			isControl = false;
		}
	}
	else
	{

		way=D3DXVECTOR3(0,0,0);
	}

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

void Player::Notify()
{ 
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
		if (degree >= end - 1) { Input("w1_attack1"); }
	}
	else if (mode == "w1_attack1")
	{
		if (degree >= end - 5) { isControl = true;}
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
	MoLoader::LoadBinary("../_Contents/BinModels/Actor_Boss4.model", &model);
	model->Reset();
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