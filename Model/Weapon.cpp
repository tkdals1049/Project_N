#include "../stdafx.h"
#include "Weapon.h"
#include "Player.h"
#include "Model.h"

Weapon::Weapon(Player* player, string equipName, string equipName2, string unequipName)
:player(player),weaponNum(-1), equipName(equipName), equipName2(equipName2), unequipName(unequipName)
{
}

Weapon::~Weapon()
{
	for (size_t i = 0; i <weapons.size(); i++)
		SAFE_DELETE(weapons[i].second);
}
void Weapon::PreUpdate()
{
	if (Keyboard::Get()->Down('F') && player->GetMode() == "idle")
	{
		if ((unsigned int)weaponNum<weapons.size()) weaponNum++;
		else weaponNum = 0;
	}
}
void Weapon::Update()
{
	if (weaponNum >= 0 && (unsigned int)weaponNum<weapons.size())
	{
		D3DXMATRIX world, scale, rotate;
		string worldName;
		weapons[weaponNum].second->Update();
		//무기 장착시와 아닐시 고정 위치 변 
		if (player->GetisEquip())
		{
			if (player->GetMode().find("equip") != -1)
				worldName = WeaponSetting(D3DXVECTOR3(90, 0, 0), D3DXVECTOR3(0, 0, 0), equipName2);
			else
				worldName = WeaponSetting(D3DXVECTOR3(-90, 0, 0), D3DXVECTOR3(0.4f, 2.0f, 0), equipName);
		}
		else
		{
			worldName = WeaponSetting(D3DXVECTOR3(-90, 90, 0), D3DXVECTOR3(-0.12f, 0, 1.5f), unequipName, player->GetMode() == "idle");
		}
		//무기 영역 및 월드 계산 
		weapons[weaponNum].second->SetWorld(player->GetModel()->GetWeaponWorld(worldName));

		//무기 장비시의 월드 및 OBB 계산
		if (player->GetisAttack())
		{
			player->CalMatrix(player->GetAttackRange(), weaponsInfo[weaponNum]->maxp, weaponsInfo[weaponNum]->minp, weaponsInfo[weaponNum]->size,
				weaponsInfo[weaponNum]->lotate* weapons[weaponNum].second->GetGeometricOffset()*player->GetModel()->GetWeaponWorld(worldName));
		}
	}
}

void Weapon::Render()
{
	if (weaponNum >= 0 && (unsigned int)weaponNum<weapons.size())
		weapons[weaponNum].second->Render();
}
void Weapon::ClearWeapon()
{
	for each(WeaponVec temp in weapons)
	{
		SAFE_DELETE(temp.second);
	}
	weapons.clear();
}

//무기를 추가함
void Weapon::AddWeaponVector(Model * weaponFile, D3DXVECTOR3 scale)
{
	weapons.push_back(WeaponVec(equipName, weaponFile));
	WeaponInfo* info = new WeaponInfo(weaponFile, scale);
	weaponsInfo.push_back(info);
	weaponFile->Reset();
}
//무기의 각도와 오차를 조정하고 모델의 장착부위를 출력함
string Weapon::WeaponSetting(D3DXVECTOR3 rotate, D3DXVECTOR3 adjust, string equipName, bool idleType)
{
	if (player->GetMode() != player->GetPremode() || idleType)
	{
		weapons[weaponNum].second->SetRotate(rotate);
		weapons[weaponNum].second->SetAdjust(adjust);
	}
	return equipName;
}
