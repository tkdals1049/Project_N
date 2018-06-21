#pragma once
#include "Actor.h"

class Player:public Actor
{
public:
	Player();
	Player(string file);
	~Player();

	void PreUpdate(D3DXVECTOR3 origin, D3DXVECTOR3 direction);

	void Update();
	void Render();
	void Input(string mode);
	void SetPlayer();
	void ClearWeapon();
	void DeleteWeapon();
	void AddWeaponVector(string weaponName, Model * weaponFile);

	float SetHeight();
private:
	void Control();
	void Notify();

	typedef pair<string, Model*> Weapon;
	vector<Weapon> weapons;
	D3DXMATRIX weaponWorld;
	int dir,degree,degree_goal,weaponNum;
	float speed;
	bool isControl,isEquip,isHeight;
	string mode,Premode,equipName, equipName2, unequipName;
	D3DXVECTOR3 way;

};
