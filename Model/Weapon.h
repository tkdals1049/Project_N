#pragma once
#include "Player.h"

struct WeaponInfo
{
	D3DXVECTOR3 maxp, minp;
	D3DXVECTOR3 size;
	D3DXVECTOR3 center, position;
	D3DXMATRIX world, lotate, scale;

	WeaponInfo()
	{
		maxp = minp = size = center = position = D3DXVECTOR3(0, 0, 0);
		D3DXMatrixIdentity(&lotate);
		D3DXMatrixIdentity(&scale);
		D3DXMatrixIdentity(&world);
	}
	WeaponInfo(Model* weapon, D3DXVECTOR3 another)
	{
		maxp = weapon->GetMaxP();
		minp = weapon->GetMinP();
		size = weapon->GetScale();
		center = maxp - minp;
		position = (maxp + minp);

		D3DXMatrixScaling(&scale, center.x / size.x, center.y / size.y, center.z / size.z);
		D3DXMatrixTranslation(&lotate, position.x, 0, 0);
		size.x *= another.x;size.y *= another.y;size.z *= another.z;
	}
};

class Weapon
{
public:
	Weapon(Player* player,string equipName= "AL-03",string equipName2 = "AR-03",string unequipName = "NK-00");
	~Weapon();

	void PreUpdate();
	void Update();
	void Render();

	void ClearWeapon();
	void AddWeaponVector(Model * weaponFile, D3DXVECTOR3 scale);
	string WeaponSetting(D3DXVECTOR3 rotate, D3DXVECTOR3 adjust, string equipName, bool idleType=false);
	bool isSetting(){return (unsigned int)weaponNum<weapons.size();}
private:
	Player * player;

	typedef pair<string, Model*> WeaponVec;
	vector<WeaponVec> weapons;
	vector<WeaponInfo*> weaponsInfo;

	string equipName, equipName2, unequipName;

	D3DXMATRIX weaponWorld;
	int weaponNum;
};